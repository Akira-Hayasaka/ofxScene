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

            for(int i=0; i<meshes.size(); i++){
                if(pointLights.size()){
                    meshes[i]->shader->setUniform( "NUM_POINT_LIGHTS", int( pointLights.size() ) );
                    meshes[i]->shader->setUniform( "POINT_LIGHTS[0]", pointLightsTransformed );
                }
                if(directionalLights.size()){
                    meshes[i]->shader->setUniform( "NUM_DIRECTIONAL_LIGHTS", int( directionalLights.size() ) );
                    meshes[i]->shader->setUniform( "DIRECTIONAL_LIGHTS[0]", directionalLightsTransformed );
                }
                if(spotLights.size()){
                    meshes[i]->shader->setUniform( "NUM_SPOT_LIGHTS", int( spotLights.size() ) );
                    meshes[i]->shader->setUniform( "SPOT_LIGHTS[0]", spotLightsTransformed );
                }
                
                meshes[i]->draw( modelView, projection );
            }
            
            if(fbo){
                fbo->end();
            }
            
            camera.end();
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
        
        bool autoClearColor;
        bool autoClearDepth;
        
        vector <Mesh*> allocatedMeshes;
        vector<PointLight*> allocatedPointLights;
        vector<DirectionalLight*> allocatedDirectionalLights;
        vector<SpotLight*> allocatedSpotLights;
    };
}