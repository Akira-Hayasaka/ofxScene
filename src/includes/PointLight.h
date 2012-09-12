//
//  PointLight.h
//  ofxSceneExample
//
//  Created by lars berg on 8/29/12.
//
//


#pragma once
#include "ofMain.h"

namespace ofxScene{
    class PointLight : public ofMatrix3x3{
        public:
        
        PointLight( ofVec3f position=ofVec3f(0), ofVec3f color=ofVec3f(1), ofVec3f attenuation=ofVec3f(0) ){
            setPosition( position );
            setColor( color );
            setAttenuation( attenuation );
        }
        
        void setup( ofVec3f position, ofVec3f color, ofVec3f attenuation=ofVec3f(0) ){
            setPosition( position );
            setColor( color );
            setAttenuation( attenuation );
        }
        
        ofVec3f getColor(){
            return ofVec3f( d,e,f );
        }
        ofVec3f getPosition(){
            return ofVec3f( a,b,c );
        }
        ofVec3f getAttenuation(){
            return ofVec3f(g,h,i);
        }
        
        void setPosition( ofVec3f _pos ){
            a = _pos.x;
            b = _pos.y;
            c = _pos.z;
        }
        void setPosition( float _x, float _y, float _z ){
            a = _x;
            b = _y;
            c = _z;
        }
        
        void setColor( ofVec3f _color ){
            d = _color.x;
            e = _color.y;
            f = _color.z;
        }
        
        void setColor( float _r, float _g, float _b ){
            d = _r;
            e = _g;
            f = _b;
        }
        
        void setAttenuation( float _constant, float _linear, float _quadratic ){
            g = _constant;
            h = _linear;
            i = _quadratic;
        }
        
        void setAttenuation( ofVec3f _attenuation ){
            g = _attenuation.x;
            h = _attenuation.y;
            i = _attenuation.z;
        }
    };
}