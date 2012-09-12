//
//  Shader.h
//  ofxSceneGraphExample
//
//  Created by lars berg on 7/16/12.
//


#pragma once
#include "ofMain.h"
#include "VBO.h"
#include "PointLight.h"
#include "DirectionalLight.h"


namespace ofxScene{
    class ShaderUniform{
    public:
        ShaderUniform(){};
        ~ShaderUniform(){};
        
        void set( string _name, GLint _loc, GLenum _type, GLint _count = 1 ){
            name = _name;
            loc = _loc;
            type = _type;
            count = _count;
            texloc = 0;
        };
        
        void copy( ShaderUniform& _uniform ){
            name = _uniform.name;
            loc = _uniform.loc;
            count = _uniform.count;
            type = _uniform.type;
            texloc = _uniform.texloc;
            tex = _uniform.tex;
        }
        
        string name;
        GLint loc, count, texloc;
        GLenum type;
        ofTexture* tex;
    };

    
    class Shader : public ofShader{
    public:
        Shader(){};
        ~Shader(){};
                
        //drawing methods        
        void draw( VBO& geometry, GLenum renderType = GL_TRIANGLES );
        
        
//        void load( string _vert, string _frag );
        void setShaderFromStrings( string _vert, string _frag );
        bool checkShaderCompile();
        
        void cacheAttributeLocations();
        void cacheUniforms();
        
        
        //niforms
        void printUniforms(){
            
            for(u_it = uniforms.begin(); u_it != uniforms.end(); u_it++){
                cout<< u_it->first << endl;
            }
        }
        
        ShaderUniform* getUniform( string name ){
            if(uniforms.count( name )){
                return &uniforms[ name ];
            }
            return NULL;
        }
        
        void setUniform( const char* name, int _value );
        void setUniform( const char* name, float _value );
        void setUniform( const char* name, ofVec2f _value );
        void setUniform( const char* name, ofVec3f _value );
        void setUniform( const char* name, ofVec4f _value );
        void setUniform( const char* name, ofMatrix3x3& _value, bool transpose = false );
        void setUniform( const char* name, ofMatrix4x4& _value, bool transpose = false );
        void setUniform( const char* name, float _v1, float _v2 );
        void setUniform( const char* name, float _v1, float _v2, float _v3 );
        void setUniform( const char* name, float _v1, float _v2, float _v3, float _v4 );
        
        void setUniform( string name, ofTexture* _tex, bool linkTexture = TRUE );
        void setUniform( string name, ofTexture& _tex );
        void setUniform( string name, ofImage& _img ){ setUniform( name, _img.getTextureReference() );}
        void setUniform( string name, ofFbo& _fbo ){ setUniform( name, _fbo.getTextureReference() );}
        
        void setUniform( string name, vector<float>& _value );
        void setUniform( string name, vector<ofVec2f>& _value );
        void setUniform( string name, vector<ofVec3f>& _value );
        void setUniform( string name, vector<ofVec4f>& _value );
        
        void setUniform( string name, vector<ofMatrix3x3>& _value, bool transpose = false );
        void setUniform( string name, vector<ofMatrix3x3>* _value, bool transpose = false );
        void setUniform( string name, vector<ofMatrix4x4>& _value, bool transpose = false  );
        
        /////////lights
        void setUniform( string name, vector<PointLight>& _value){
            setUniform( name, (vector<ofMatrix3x3>& )_value );
        };
        void setUniform( string name, vector<PointLight>* _value){
            setUniform( name, (vector<ofMatrix3x3>* )_value );
        }
        void setUniform( string name, vector<DirectionalLight>& _value){
            setUniform( name, (vector<ofMatrix3x3>& )_value );
        };
        void setUniform( string name, vector<DirectionalLight>* _value){
            setUniform( name, (vector<ofMatrix3x3>* )_value );
        }
        
        void setUniform( string name, vector<float>* _value );
        void setUniform( string name, vector<ofVec2f>* _value );
        void setUniform( string name, vector<ofVec3f>* _value );
        void setUniform( string name, vector<ofVec4f>* _value );
        
        void setUniform(const char* name, float* _value );
        void setUniform(const char* name, ofVec2f* _value );
        void setUniform(const char* name, ofVec3f* _value );
        void setUniform(const char* name, ofVec4f* _value );
        
        void updateDynamicUniforms();
        
        
        //attributes
        map< string, GLint >::iterator a_it;
        map< string, GLint > attributes;
        
        //uniform maps/iteraters
        map< string, ShaderUniform >::iterator u_it;
        map< string, ShaderUniform > uniforms;
        map< string, ShaderUniform* > dynamicUniforms;
        
        map< string, float* > dynamicUniforms1f;
        map< string, ofVec2f* > dynamicUniforms2f;
        map< string, ofVec3f* > dynamicUniforms3f;
        map< string, ofVec4f* > dynamicUniforms4f;
        map< string, ofMatrix3x3* > dynamicUniformsMatrices3f;
        map< string, ofMatrix4x4* > dynamicUniformsMatrices4f;
        
        map< string, vector<float>* > dynamicUniforms1fv;
        map< string, vector<ofVec2f>* > dynamicUniforms2fv;
        map< string, vector<ofVec3f>* > dynamicUniforms3fv;
        map< string, vector<ofVec4f>* > dynamicUniforms4fv;
        map< string, vector<ofMatrix3x3>* > dynamicUniformsMatrices3fv;
        map< string, vector<ofMatrix4x4>* > dynamicUniformsMatrices4fv;
        
        map< string, ShaderUniform >::iterator cu_it;
        map< string, ShaderUniform > connectedUniforms;
        
        GLint sampler2Dcount;
        
        bool isCompiled;
        
        string vert, frag;
        
        int useCount;
    };

}

