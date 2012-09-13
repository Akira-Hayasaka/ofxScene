//
//  PointLight.h
//


#pragma once
#include "ofMain.h"

namespace ofxScene{
    class DirectionalLight : public ofMatrix3x3{
    public:
        DirectionalLight( ofVec3f direction=ofVec3f(0,1,0), ofVec3f color=ofVec3f(1) ){
            setDirection( direction );
            setColor( color );
        }
        void setup( ofVec3f _direction, ofVec3f _color ){
            setDirection( _direction );
            setColor( _color );
        }
        
        ofVec3f getColor(){
            return ofVec3f( d,e,f );
        }
        ofVec3f getDirection(){
            return ofVec3f( a,b,c );
        }
        ofVec3f getPosition(){
            return ofVec3f(g,h,i);
        }
        
        void setDirection( ofVec3f _dir ){
            _dir.normalize();
            a = _dir.x;
            b = _dir.y;
            c = _dir.z;
        }
        void setDirection( float _x, float _y, float _z ){
            ofVec3f dir( _x, _y, _z);
            dir.normalize();
            a = dir.x;
            b = dir.y;
            c = dir.z;
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
        
        //we don't really need/use position bu it might come in handy and we have room for it
        void setPosition( ofVec3f _pos ){
            g = _pos.x;
            h = _pos.y;
            i = _pos.z;
        }
        void setPosition( float _x=0.f, float _y=0.f, float _z=0.f ){
            g = _x;
            h = _y;
            i = _z;
        }
        
    };
}