//
//  Mesh.h
//


#pragma once
#include "ofMain.h"
#include "Node3D.h"
#include "Shader.h"
#include "Geometry.h"

namespace ofxScene{
    class Mesh;
    class Mesh : public Node3D {
    public:
        
        Mesh(){
            lastFrame = -1;
            parent = NULL;
            wireframe = false;
            cull = GL_BACK;
            renderType = GL_TRIANGLES;
            depthTest = true;
            doubleSided = false;
            deallocateShader = NULL;
            deallocateGeometry = NULL;
        };
        ~Mesh(){

            removeAllocatedShader();
            removeAllocatedGeometry();
        };
        void removeAllocatedShader(){
            if( deallocateShader != NULL ){
                deallocateShader->useCount--;
                if(deallocateShader->useCount <= 0){
                    cout << "deleting shader" << endl;
                    delete deallocateShader;
                }
            }
        }
        void removeAllocatedGeometry(){
            if(deallocateGeometry){
                deallocateGeometry->useCount--;
                if(deallocateGeometry->useCount <= 0){
                    cout << "deleting geometry" << endl;
                    delete deallocateGeometry;
                }
            }
        }
        
        void allocateShader(Shader* _shader = NULL){
            removeAllocatedShader();
            
            deallocateShader = _shader;
            _shader->useCount++;
        }
        void allocateGeometry(Geometry* _geometry = NULL){
            removeAllocatedGeometry();
            
            deallocateGeometry = _geometry;
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
        
        void draw( ofMatrix4x4 viewMatrix, ofMatrix4x4 projectionMatrix ){

            if( shader->getProgram() != 0 ){
                updateMatrices();
                
                ofMatrix4x4 modelViewMatrix = worldMatrix * viewMatrix;
                
                shader->setUniform( "viewMatrix", viewMatrix );
                shader->setUniform( "modelViewMatrix", modelViewMatrix );
                shader->setUniform( "projectionMatrix", projectionMatrix );
                shader->setUniform( "normalMatrix", *setNormalMatrix( modelViewMatrix ) );
                
                (depthTest)? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                (doubleSided)? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
                glCullFace( cull );
                
                if(wireframe){
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
                }
                
                shader->draw( *geometry, renderType );
                if(wireframe){
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
                }
                
                if(cull != GL_BACK){
                    glCullFace( GL_BACK );
                }
            }
        }
        
        void setWireframe( bool _wireframe = true ){
            wireframe = _wireframe;
        }

        
//    private:
        Shader* shader;
        Geometry* geometry;
        Shader* deallocateShader;
        Geometry* deallocateGeometry;
        
    public:
        
        int lastFrame;
        bool wireframe, depthTest, doubleSided;
        GLenum cull, renderType;
    };
}