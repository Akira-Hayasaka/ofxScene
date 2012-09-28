//
//  Mesh.h
//


#pragma once
#include "ofMain.h"
#include "Node3D.h"
#include "Geometry.h"
#include "Material.h"

namespace ofxScene{
    class Mesh;
    class Mesh : public Node3D {
    public:
        
        Mesh(){
            setDefaultVars();
        };
        Mesh( Geometry& _geometry, Shader& _shader ){
            setDefaultVars();
            init( _geometry, _shader );
        }
        
        Mesh( Geometry& _geometry, Shader* _shader ){
            setDefaultVars();
            init( _geometry, _shader );
        }
        
        Mesh( Geometry* _geometry, Shader& _shader ){
            setDefaultVars();
            init( _geometry, _shader );
        }
        
        Mesh( Geometry* _geometry, Shader* _shader ){
            setDefaultVars();
            init( _geometry, _shader );
        }
        
        ~Mesh(){
            removeAllocatedShader();
            removeAllocatedGeometry();
        };
        
        void setDefaultVars(){
            lastFrame = -1;
            parent = NULL;
            wireframe = false;
            cull = GL_BACK;
            renderType = GL_TRIANGLES;
            depthTest = true;
            doubleSided = false;
            allocatedShader = NULL;
            allocatedGeometry = NULL;
            useCount = 0;
        }
        
        void removeAllocatedShader(){
            if( allocatedShader != NULL ){
                allocatedShader->useCount--;
                if(allocatedShader->useCount <= 0){
                    cout << "deleting shader" << endl;
                    delete allocatedShader;
                    allocatedShader = NULL;
                }
            }
        }
        void removeAllocatedGeometry(){
            if(allocatedGeometry){
                allocatedGeometry->useCount--;
                if(allocatedGeometry->useCount <= 0){
                    cout << "deleting geometry" << endl;
                    delete allocatedGeometry;
                    allocatedGeometry = NULL;
                }
            }
        }
        
        void allocateShader(Shader* _shader = NULL){
            removeAllocatedShader();
            
            allocatedShader = _shader;
            _shader->useCount++;
        }
        void allocateGeometry(Geometry* _geometry = NULL){
            removeAllocatedGeometry();
            
            allocatedGeometry = _geometry;
            _geometry->useCount++;
        }
        
        void init( Geometry& _geometry, Shader& _shader ){
            geometry = &_geometry;
            shader = &_shader;
            
            glEnable(GL_CULL_FACE);
            
            //TODO:: check that the shader and the geometry have matching attributes - vbos
        }
        
        void init( Geometry& _geometry, Shader* _shader ){
            allocateShader( _shader );
            init( _geometry, *_shader );
        }
        
        void init( Geometry* _geometry, Shader& _shader ){
            allocateGeometry( _geometry );
            init( *_geometry, _shader );
        }
                
        void init( Geometry* _geometry, Shader* _shader ){
            allocateGeometry( _geometry );
            allocateShader( _shader );
            init( *_geometry, *_shader );
        }
        
        
        Shader* getShader(){
            return shader;
        };
        Geometry* getGeometry(){
            return geometry;
        };
        
        void draw( ofCamera camera ){
            draw( camera.getModelViewMatrix(), camera.getProjectionMatrix() );
        }
        
        
        void draw( ofVec3f _pos, ofCamera camera ){
            ofVec3f tempPos = position;
            position = _pos;
            updateMatrices( true );
            draw( camera.getModelViewMatrix(), camera.getProjectionMatrix() );
            position = tempPos;
        }
        
        void renderSetup(){
            
        }
        
        void draw( ofMatrix4x4 viewMatrix, ofMatrix4x4 projectionMatrix ){
                
            if( geometry != NULL && shader != NULL && shader->getProgram() != 0 ){
                updateMatrices(false);
                
                ofMatrix4x4 modelViewMatrix = worldMatrix * viewMatrix;
                
                shader->setUniform( "viewMatrix", viewMatrix );
                shader->setUniform( "modelViewMatrix", modelViewMatrix );
                shader->setUniform( "projectionMatrix", projectionMatrix );
                shader->setUniform( "normalMatrix", *setNormalMatrix( modelViewMatrix ) );
                
                (depthTest)? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                (doubleSided)? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
                if(cull != GL_BACK) glCullFace( cull );
                if(wireframe)   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
                
                shader->bindGeometry( geometry );
                
                if(geometry->getAttr("indices") == NULL ){
                    VertexAttributeBase* attr = geometry->attributes.begin()->second;
                    glDrawArrays( renderType, 0, (attr)? attr->count : 0 );
                }
                else{
                    VertexAttributeBase* attr = geometry->getAttr("indices");
                    
                    //bind and draw the indices
                    attr->bind();
                    glIndexPointer( GL_UNSIGNED_INT, 0, 0);
                    glDrawElements( renderType, attr->count, GL_UNSIGNED_INT, 0);
                    
                    //unbind
                    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
                }
                shader->draw( *geometry, renderType );
                
                if(wireframe)   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
                if(cull != GL_BACK) glCullFace( GL_BACK );
            }
        }
        
        void setWireframe( bool _wireframe = true ){
            wireframe = _wireframe;
        }

        
//    private:
        Shader* shader;
        Geometry* geometry;
        Shader* allocatedShader;
        Geometry* allocatedGeometry;
        
    public:
        
        int lastFrame;
        bool wireframe, depthTest, doubleSided;//TODO:: move this stuff to the material
        GLenum cull, renderType;
        int useCount;
    };
    
    class NormalDisplayMesh : public Mesh{
    public:
        NormalDisplayMesh(){
            targetMesh = NULL;
            useTangents = false;
        };
        NormalDisplayMesh( Mesh& _targetMesh ){
            setup( _targetMesh );
        };
        ~NormalDisplayMesh(){};
        
        void setup( Mesh& _targetMesh ){
            targetMesh = &_targetMesh;
            
            init( new Geometry(), new NormalMaterial() );
            
            if( targetMesh->geometry->tangents.size() > 0 ){
                useTangents = true;
                geometry->vertices.resize( targetMesh->geometry->vertices.size() * 4 );
                geometry->normals.resize( targetMesh->geometry->vertices.size() * 4 );
                geometry->indices.resize( targetMesh->geometry->vertices.size()*6 );
                for(int i=0; i<targetMesh->geometry->vertices.size(); i++){
                    geometry->vertices[i*4  ] = targetMesh->geometry->vertices[i];
                    geometry->vertices[i*4+1] = targetMesh->geometry->vertices[i] + targetMesh->geometry->normals[i] * 10;
                    geometry->vertices[i*4+2] = targetMesh->geometry->vertices[i] + targetMesh->geometry->tangents[i] * 10;
                    geometry->vertices[i*4+3] = targetMesh->geometry->vertices[i] + targetMesh->geometry->binormals[i] * 10;
                    
                    geometry->normals[i*4  ] = targetMesh->geometry->normals[i];
                    geometry->normals[i*4+1] = targetMesh->geometry->normals[i];
                    geometry->normals[i*4+2] = targetMesh->geometry->tangents[i];
                    geometry->normals[i*4+3] = targetMesh->geometry->binormals[i];
                    
                    geometry->indices[i*6  ] = i*4;
                    geometry->indices[i*6+1] = i*4+1;
                    geometry->indices[i*6+2] = i*4;
                    geometry->indices[i*6+3] = i*4+2;
                    geometry->indices[i*6+4] = i*4;
                    geometry->indices[i*6+5] = i*4+3;
                }
            }
            else {
               geometry->vertices.resize( targetMesh->geometry->vertices.size() * 2 );
               geometry->normals.resize( targetMesh->geometry->vertices.size() * 2 );
               geometry->indices.resize( targetMesh->geometry->vertices.size()*2 );
               for(int i=0; i<targetMesh->geometry->vertices.size(); i++){
                   geometry->vertices[i*2  ] = targetMesh->geometry->vertices[i];
                   geometry->vertices[i*2+1] = targetMesh->geometry->vertices[i] + targetMesh->geometry->normals[i] * 10;
                   
                   geometry->normals[i*2  ] = targetMesh->geometry->normals[i];
                   geometry->normals[i*2+1] = targetMesh->geometry->normals[i];
                   
                   geometry->indices[i*2  ] = i*2;
                   geometry->indices[i*2+1] = i*2+1;
               }
           }
               
            geometry->addIndices( geometry->indices );
            geometry->addAttribute( "position", geometry->vertices );
            geometry->addAttribute( "normal", geometry->normals );
            renderType = GL_LINES;
        }
        
        void update(){
            if(targetMesh != NULL){
                if(useTangents){
                    for(int i=0; i<targetMesh->geometry->vertices.size(); i++){
                        geometry->vertices[i*4  ] = targetMesh->geometry->vertices[i];
                        geometry->vertices[i*4+1] = targetMesh->geometry->vertices[i] + targetMesh->geometry->normals[i] * 10;
                        geometry->vertices[i*4+2] = targetMesh->geometry->vertices[i] + targetMesh->geometry->tangents[i] * 10;
                        geometry->vertices[i*4+3] = targetMesh->geometry->vertices[i] + targetMesh->geometry->binormals[i] * 10;
                        
                        geometry->normals[i*4  ] = targetMesh->geometry->normals[i];
                        geometry->normals[i*4+1] = targetMesh->geometry->normals[i];
                        geometry->normals[i*4+2] = targetMesh->geometry->tangents[i];
                        geometry->normals[i*4+3] = targetMesh->geometry->binormals[i];
                    }
                }
                else{
                    for(int i=0; i<targetMesh->geometry->vertices.size(); i++){
                        geometry->vertices[i*2  ] = targetMesh->geometry->vertices[i];
                        geometry->vertices[i*2+1] = targetMesh->geometry->vertices[i] + targetMesh->geometry->normals[i] * 10;
                        
                        geometry->normals[i*2  ] = targetMesh->geometry->normals[i];
                        geometry->normals[i*2+1] = targetMesh->geometry->normals[i];
                    }
                }
                
                position = targetMesh->position;
                orientation = targetMesh->orientation;
                scale = targetMesh->scale;
                
                geometry->update();
   
            }
        }
        
        Mesh* targetMesh;
        bool useTangents;
    };
}