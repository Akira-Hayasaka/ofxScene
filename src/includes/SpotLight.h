//
//  SpotLight.h
//


#pragma once
#include "ofMain.h"

namespace ofxScene{
    class SpotLight : public ofMatrix4x4{
    public:
        
        /*
         the spotlight will be passed to the ahder as a mat4
         
         mat4 = {
             vec4( position.xyz, cutoff ),
             vec4( color.xyz, exponent ),
             vec4( attenuation(constaint, linear, quadratic), 0),
             vec4( direction.xyz, 0)
         }
         */
        
        
        
        SpotLight(ofVec3f direction=ofVec3f(0,1,0),
                  ofVec3f color=ofVec3f(1),
                  ofVec3f attenuation=ofVec3f(1,0,0),
                  float cutoff=0.,
                  float exponent = 2)
        {
          setDirection( direction );
          setColor( color );
          setAttenuation( attenuation );
          setCutoff(cutoff);
          setExponent(exponent);
        }
        
        void setPosition( ofVec3f position=ofVec3f(1)){
            _mat[0][0] = position[0];
            _mat[0][1] = position[1];
            _mat[0][2] = position[2];
        }
        void setPosition( float x, float y, float z){
            _mat[0][0] = x;
            _mat[0][1] = y;
            _mat[0][2] = z;
        }
        
        void setColor( ofVec3f color=ofVec3f(1)){
            _mat[1][0] = color[0];
            _mat[1][1] = color[1];
            _mat[1][2] = color[2];
        }
        
        void setColor( float r, float g, float b ){
            _mat[1][0] = r;
            _mat[1][1] = g;
            _mat[1][2] = b;
        }
        
        void setAttenuation( ofVec3f attenuation=ofVec3f(0)){
            _mat[2][0] = attenuation[0];
            _mat[2][1] = attenuation[1];
            _mat[2][2] = attenuation[2];
        }
        
        void setDirection( ofVec3f direction=ofVec3f(0,1,0)){
            direction.normalize();
            _mat[3][0] = direction[0];
            _mat[3][1] = direction[1];
            _mat[3][2] = direction[2];
        }
        
        void setDirection( float x, float y, float z){
            ofVec3f direction(x,y,z);
            direction.normalize();
            _mat[3][0] = direction[0];
            _mat[3][1] = direction[1];
            _mat[3][2] = direction[2];
        }
        
        void setCutoff( float cutOff ){
            _mat[0][3] = cutOff;
        }
        
        void setExponent( float exponent ){
            _mat[1][3] = exponent;
        }
        
        ofVec3f getPosition(){
            return ofVec3f(_mat[0][0],_mat[0][1],_mat[0][2]);
        }
        
        ofVec3f getColor(){
            return ofVec3f(_mat[1][0],_mat[1][1],_mat[1][2]);
        }
        
        ofVec3f getAttenuation(){
            return ofVec3f(_mat[2][0],_mat[2][1],_mat[2][2]);
        }
        
        ofVec3f getDirection(){
            return ofVec3f(_mat[3][0],_mat[3][1],_mat[3][2]);
        }
        
        float getCutoff(){
            return _mat[0][3];
        }
        
        float getExponent(){
            return _mat[1][3];
        }
    };
}