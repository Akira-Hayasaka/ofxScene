//
//  Node3D.h
//


#pragma once
#include "ofMain.h"
#include "Shader.h"
#include "Geometry.h"

namespace ofxScene{
    class Node3D;
    class Node3D{
    public:
        
        Node3D(){
            lastFrame = -1;
            parent = NULL;
            up.set(0,-1,0);
            scale.set(1,1,1);
        };
        ~Node3D(){};
        
        ofMatrix3x3* setNormalMatrix( ofMatrix4x4 modelView ){
            ofMatrix4x4 n = ofMatrix4x4::getOrthoNormalOf( modelView );
            normalMatrix.set(n._mat[0][0], n._mat[0][1], n._mat[0][2],
                             n._mat[1][0], n._mat[1][1], n._mat[1][2],
                             n._mat[2][0], n._mat[2][1], n._mat[2][2]);
            return &normalMatrix;
        }
        
        void updateMatrices( bool forceUpdate = false, int frame = ofGetFrameNum() ){
            if( forceUpdate || lastFrame < frame ){
                lastFrame = frame;
                
                localMatrix.makeScaleMatrix( scale );
                localMatrix.rotate( orientation );
                localMatrix.setTranslation( position );
                
                
                worldMatrix = localMatrix;
                if(parent != NULL ){//recursively calc the parents matrices
                    if(parent->lastFrame != frame){
                        parent->updateMatrices();
                    }
                    
                    worldMatrix *= parent->worldMatrix;
                }
            }
        }
        
        void rotate( float degrees, float x, float y, float z ){
            orientation.makeRotate( degrees, x, y, z );
        }
        
        void rotate( float x, float y, float z ){
            orientation.makeRotate(x, ofVec3f(1,0,0), y, ofVec3f(0,1,0), z, ofVec3f(0,0,1));
        }
        
        void lookAt( ofVec3f target ){
            target -= position;
            if(target.length() > .0001){
                orientation.makeRotate( target, up );
            }
        }
        
        void rotateTo( ofVec3f direction ){
            orientation.makeRotate( direction, up );
        }
        void addChild( Node3D& child ){
            children.push_back(&child);
            child.parent = this;
        }
        
    public:
        
        Node3D* parent;
        vector<Node3D*> children;
        
        ofVec3f position;
        ofQuaternion orientation;
        ofVec3f scale;
        ofVec3f up;
        
        ofMatrix3x3 normalMatrix;
        ofMatrix4x4 localMatrix, worldMatrix;
        
        int lastFrame;
        bool wireframe, depthTest, doubleSided;
        GLenum cull;
    };
}