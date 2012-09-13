//
//  Geometry.h
//

#pragma once
#include "ofMain.h"
#include "VertexAttribute.h"

namespace ofxScene{
    
    class VBO{
    public:
        VBO(){};
        ~VBO(){
            
            //delete all the buffers
            map< string, ofxScene::VertexAttributeBase* >::iterator vbo_it;
            for ( vbo_it = attributes.begin() ; vbo_it != attributes.end(); vbo_it++ ){
                glDeleteBuffers( 1, &vbo_it->second->bufferId );
            }
            
        };
        
        //attributes base methods
        VertexAttributeBase* getAttr( string name ){
            return ( attributes.count( name ) )? attributes[name] : NULL;
        }
        //        void updateAttribute( string name, vector<int> data ){
        //
        //        }
        
        void addIndices( vector <int>& _indices ){
            attr1i["indices"].init( _indices, &_indices[0], 1, GL_INT, GL_ELEMENT_ARRAY_BUFFER );
            attributes["indices"] = &attr1i["indices"];
        }
        void addAttribute( string name, vector<int>& data ){
            attr1i[name].init( data, &data[0], 1, GL_INT );
            attributes[name] = &attr1i[name];
        }
        void addAttribute( string name, vector<float>& data ){
            attr1f[name].init( data, &data[0], 1 );
            attributes[name] = &attr1f[name];
        }
        void addAttribute( string name, vector<ofVec2f>& data ){
            attr2f[name].init( data, &data[0][0], 2 );
            attributes[name] = &attr2f[name];
        }
        void addAttribute( string name, vector<ofVec3f>& data ){
            attr3f[name].init( data, &data[0][0], 3 );
            attributes[name] = &attr3f[name];
        }
        void addAttribute( string name, vector<ofVec4f>& data ){
            attr4f[name].init( data, &data[0][0], 4 );
            attributes[name] = &attr4f[name];
        }
        
        //attributes
        map<string, ofxScene::VertexAttributeBase* > attributes;
        
        map<string, ofxScene::VertexAttribute<int>      > attr1i;
        map<string, ofxScene::VertexAttribute<float>    > attr1f;
        map<string, ofxScene::VertexAttribute<ofVec2f>  > attr2f;
        map<string, ofxScene::VertexAttribute<ofVec3f>  > attr3f;
        map<string, ofxScene::VertexAttribute<ofVec4f>  > attr4f;
        
    };
}