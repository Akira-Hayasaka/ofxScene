//
//  Scene.h
//

#pragma once
#include "ofMain.h"

namespace ofxScene{
    
    class Scene{
    public:
        Scene(){
            autoClearColor = true;
            autoClearDepth = true;
        };
        ~Scene(){
            for(int i=0; i<allocatedMeshes.size(); i++){
                allocatedMeshes[i]->useCount--;
                if(allocatedMeshes[i]->useCount <= 0){
                    cout << "deleting mesh" << endl;
                    delete allocatedMeshes[i];
                    allocatedMeshes[i] = NULL;
                }
            }
            
            for(int i=0; i<allocatedPointLights.size(); i++){
                delete allocatedPointLights[i];
                allocatedPointLights[i] = NULL;
            }
            for(int i=0; i<allocatedDirectionalLights.size(); i++){
                delete allocatedDirectionalLights[i];
                allocatedDirectionalLights[i] = NULL;
            }
            for(int i=0; i<allocatedSpotLights.size(); i++){
                delete allocatedSpotLights[i];
                allocatedSpotLights[i] = NULL;
            }
        };
        
        void add( Mesh& mesh ){
            meshes.push_back( &mesh );
            
            if( materials.count( mesh.shader ) == 0 )   materials[ mesh.shader ] = 0;
            if( geometries.count( mesh.geometry ) == 0 )   geometries[ mesh.geometry ] = 0;
        }
        
        Mesh* add( Mesh* mesh ){
            allocatedMeshes.push_back( mesh );
            mesh->useCount++;
            add( *mesh );
            return mesh;
        }
        
        void add( PointLight& pointLight ){
            pointLights.push_back( &pointLight );
        }
        void add( DirectionalLight& directionalLight ){
            directionalLights.push_back( &directionalLight );
        }
        void add( SpotLight& spotLight ){
            spotLights.push_back( &spotLight );
        }
        PointLight* add( PointLight* pointLight ){
            allocatedPointLights.push_back( pointLight );
            add( *pointLight );
            return pointLight;
        }
        DirectionalLight* add( DirectionalLight* directionalLight ){
            allocatedDirectionalLights.push_back( directionalLight );
            add( *directionalLight );
            return directionalLight;
        }
        SpotLight* add( SpotLight* spotLight ){
            allocatedSpotLights.push_back( spotLight );
            add( *spotLight );
            return spotLight;
        }
        
        void transformPointLights( ofMatrix4x4& modelView ){
            pointLightsTransformed.resize( pointLights.size() );
            for(int i=0; i<pointLights.size(); i++){
                pointLightsTransformed[i] = *pointLights[i];
                pointLightsTransformed[i].setPosition( pointLightsTransformed[i].getPosition() * modelView );
            }
        }
        
        void transformDirectionalLights( ofMatrix4x4& modelView  ){
            ofQuaternion rotQuat = modelView.getRotate();
            directionalLightsTransformed.resize( directionalLights.size() );
            for(int i=0; i<directionalLights.size(); i++){
                directionalLightsTransformed[i] =  *directionalLights[i];
                directionalLightsTransformed[i].setDirection( rotQuat * directionalLightsTransformed[i].getDirection() );
            }
        }
        
        
        void transformSpotLights( ofMatrix4x4& modelView  ){
            ofQuaternion rotQuat = modelView.getRotate();
            spotLightsTransformed.resize( spotLights.size() );
            for(int i=0; i<spotLights.size(); i++){
                spotLightsTransformed[i] =  *spotLights[i];
                spotLightsTransformed[i].setPosition( spotLightsTransformed[i].getPosition() * modelView );
                spotLightsTransformed[i].setDirection( rotQuat * spotLightsTransformed[i].getDirection() );
            }
        }
        
        
        void draw( ofCamera& camera, ofFbo* fbo=NULL ){
            if(fbo){
                fbo->begin();
                if(autoClearColor)  glClear( GL_COLOR_BUFFER_BIT );
                if(autoClearDepth)  glClear( GL_DEPTH_BUFFER_BIT );
            }
            camera.begin();//the camera clip planes and viewport are set here( privately )

            //set projection and modelview matrices
            projection = camera.getProjectionMatrix();
            modelView = camera.getModelViewMatrix();
            
            transformPointLights( modelView );
            transformDirectionalLights( modelView );
            transformSpotLights( modelView );
            
            map< Shader*, int >::iterator mat_it;
            for ( mat_it = materials.begin() ; mat_it != materials.end(); mat_it++ ){
                if(pointLights.size()){
                    mat_it->first->setUniform( "NUM_POINT_LIGHTS", int( pointLights.size() ) );
                    mat_it->first->setUniform( "POINT_LIGHTS[0]", pointLightsTransformed );
                }
                if(directionalLights.size()){
                    mat_it->first->setUniform( "NUM_DIRECTIONAL_LIGHTS", int( directionalLights.size() ) );
                    mat_it->first->setUniform( "DIRECTIONAL_LIGHTS[0]", directionalLightsTransformed );
                }
                if(spotLights.size()){
                    mat_it->first->setUniform( "NUM_SPOT_LIGHTS", int( spotLights.size() ) );
                    mat_it->first->setUniform( "SPOT_LIGHTS[0]", spotLightsTransformed );
                }
            }

            for(int i=0; i<meshes.size(); i++){
                
                if(currentMaterial != meshes[i]->shader ){
                    currentMaterial = meshes[i]->shader;
                    currentGeometry = NULL;
                }
                
                if(currentGeometry != meshes[i]->geometry ){
                    meshes[i]->shader->bindGeometry( meshes[i]->geometry );
                    currentGeometry = meshes[i]->geometry;
                }
                
                if( currentGeometry != NULL && currentMaterial != NULL && currentMaterial->getProgram() != 0 ){
                    
                    //transformation matrices
                    meshes[i]->updateMatrices(false);
                    ofMatrix4x4 modelViewMatrix = meshes[i]->worldMatrix * modelView;
                    meshes[i]->shader->setUniform( "viewMatrix", modelView );
                    meshes[i]->shader->setUniform( "modelViewMatrix", modelViewMatrix );
                    meshes[i]->shader->setUniform( "projectionMatrix", projection );
                    meshes[i]->shader->setUniform( "normalMatrix", *meshes[i]->setNormalMatrix( modelViewMatrix ) );
                    
                    //culling and depth testing
                    (meshes[i]->depthTest)? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                    (meshes[i]->doubleSided)? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
                    if(meshes[i]->cull != GL_BACK) glCullFace( meshes[i]->cull );
                    if(meshes[i]->wireframe)   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
                    
                    //drawing
                    VertexAttributeBase* indexAttr = meshes[i]->geometry->getAttr("indices");
                    if( indexAttr ){
                        glDrawElements( meshes[i]->renderType, indexAttr->count, GL_UNSIGNED_INT, 0);
                    }
                    else{
                        VertexAttributeBase* attr = meshes[i]->geometry->attributes.begin()->second;
                        glDrawArrays( meshes[i]->renderType, 0, (attr)? attr->count : 0 );
                    }
                    
                    //revert to defualt render settings
                    if(meshes[i]->wireframe)   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
                    if(meshes[i]->cull != GL_BACK) glCullFace( GL_BACK );
                }
                
                if(fbo){
                    fbo->end();
                    fbo->getTextureReference();// with multisampling we need to update() the texture which gets called here privately
                }
                
                camera.end();
            }
            
            currentGeometry = NULL;
            currentMaterial = NULL;
        }
        
        void setAutoClear( bool _color, bool _depth ){
            //only useful when drawing to a FBO
            autoClearColor = _color;
            autoClearDepth = _depth;
        }
        
        vector <Mesh*> meshes;
        vector <PointLight*> pointLights;
        vector <PointLight> pointLightsTransformed;
        
        vector <DirectionalLight*> directionalLights;
        vector <DirectionalLight> directionalLightsTransformed;
        
        vector <SpotLight*> spotLights;
        vector <SpotLight> spotLightsTransformed;
        vector <float> spotLightsFlattened;
        
        ofMatrix4x4 projection, modelView;
        
        bool autoClearColor, autoClearDepth;//only useful when drawing to a FBO
        
        vector <Mesh*> allocatedMeshes;
        map <Shader*, int> materials;
        map <Geometry*, int> geometries;
        
        Geometry* currentGeometry;
        Shader* currentMaterial;
        
        vector<PointLight*> allocatedPointLights;
        vector<DirectionalLight*> allocatedDirectionalLights;
        vector<SpotLight*> allocatedSpotLights;
        
        
    };
}