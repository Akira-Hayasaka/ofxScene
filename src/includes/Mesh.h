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
        Shader* allocatedShader;
        Geometry* allocatedGeometry;
        
    public:
        
        int lastFrame;
        bool wireframe, depthTest, doubleSided;
        GLenum cull, renderType;
        int useCount;
    };
}