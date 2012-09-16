//
//  Shader.cpp
//

#include "Shader.h"
using namespace ofxScene;

void Shader::setShaderFromStrings( string _vert, string _frag ){
        
    vert = _vert;
    frag = _frag;
     
    GLint vert_status = GL_FALSE;
    GLint frag_status = GL_FALSE;
    
    setupShaderFromSource( GL_VERTEX_SHADER, vert );
    glGetShaderiv(getShader(GL_VERTEX_SHADER), GL_COMPILE_STATUS, &vert_status);
    
    setupShaderFromSource( GL_FRAGMENT_SHADER, frag );
    glGetShaderiv(getShader(GL_FRAGMENT_SHADER), GL_COMPILE_STATUS, &frag_status);
    
    isCompiled = ( vert_status && frag_status );
    if( linkProgram() && isCompiled ){
        isCompiled = true;
        cacheAttributeLocations();
        cacheUniforms();
    }else{
        isCompiled = false;
        cout<<endl<< _vert <<endl<<endl<< _frag <<endl<<endl;
    }

}
bool Shader::checkShaderCompile(){
    GLint vert_status = GL_FALSE;
    GLint frag_status = GL_FALSE;
    glGetShaderiv(getShader(GL_VERTEX_SHADER), GL_COMPILE_STATUS, &vert_status);
    glGetShaderiv(getShader(GL_FRAGMENT_SHADER), GL_COMPILE_STATUS, &frag_status);
    
    isCompiled = ( vert_status && frag_status );
    return isCompiled;
}

void Shader::cacheAttributeLocations(){
    attributes.clear();
    
    //adapted from ofShader
    GLint numAttributes = 0;
    GLint attributeMaxLength = 0;
    GLint count = -1;
    GLenum type = 0;
    glGetProgramiv(getProgram(), GL_ACTIVE_ATTRIBUTES, &numAttributes);
    glGetProgramiv(getProgram(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength);
    
    GLchar* attributeName = new GLchar[attributeMaxLength];
    for(GLint i = 0; i < numAttributes; i++) {
        GLsizei length;
        glGetActiveAttrib( getProgram(), i, attributeMaxLength, &length, &count, &type, attributeName );
        attributes[ attributeName ] = getAttributeLocation( attributeName );
    }
    delete [] attributeName;
}

void Shader::cacheUniforms(){
    uniforms.clear();
    
    //adapted from ofShader
    GLint numUniforms = 0;
    GLint uniformMaxLength = 0;
    GLint count = -1;
    GLint loc;
    GLenum type = 0;
    glGetProgramiv(getProgram(), GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(getProgram(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);
    
    GLchar* uniformName = new GLchar[uniformMaxLength];
    for(GLint i = 0; i < numUniforms; i++) {
        GLsizei length;
        glGetActiveUniform(getProgram(), i, uniformMaxLength, &length, &count, &type, uniformName);
        loc = glGetUniformLocation( getProgram(), uniformName );
        uniforms[ uniformName ].set( uniformName, loc, type, count );
        
        if(type == GL_SAMPLER_2D){
            uniforms[ uniformName ].texloc = sampler2Dcount;
            sampler2Dcount++;
        }
    }
    delete [] uniformName;
    cout << endl;
}

void Shader::draw( VBO& geometry, GLenum renderType ){
    begin();
    
    updateDynamicUniforms();
    
    //bind textures
    for (u_it = uniforms.begin(); u_it != uniforms.end(); u_it++) {
        if(u_it->second.type == GL_SAMPLER_2D ){
            setUniformTexture(u_it->first.c_str(), *u_it->second.tex, u_it->second.texloc );
        }
    }
    map< string, ShaderUniform > uniforms;
    
    VertexAttributeBase* attr = NULL;
    
    //bind all the applicable vertex buffers
    for(a_it = attributes.begin(); a_it != attributes.end(); a_it++){
        attr = geometry.getAttr( a_it->first );
        if(attr != NULL ){
            attr->bind();
            
            glEnableVertexAttribArray( a_it->second );
            glVertexAttribPointer(a_it->second,
                                  attr->size,
                                  attr->type,
                                  GL_FALSE,
                                  attr->stride,
                                  (GLvoid *)0);
        }
    }
    
    if(geometry.getAttr("indices") == NULL ){
        glDrawArrays( renderType, 0, (attr)? attr->count : 0 );
    }else{
        attr = geometry.getAttr("indices");
        
        //bind and draw the indices
        attr->bind();
        glIndexPointer( GL_UNSIGNED_INT, 0, 0);
        glDrawElements( renderType, attr->count, GL_UNSIGNED_INT, 0);
        
        //unbind
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    //unbind all the buffers and attributes
    attr = NULL;
    for(a_it = attributes.begin(); a_it != attributes.end(); a_it++){
        glDisableVertexAttribArray( a_it->second );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    
    glUseProgram(0);
}


void Shader::setUniform( const char* name, int _value ){
    if(uniforms.count( name )){
        begin();
        setUniform1i( name, _value );
    }
}

void Shader::setUniform( const char* name, float _value ){
    if(uniforms.count( name )){
        begin();
        setUniform1f( name, _value );
    }
}
void Shader::setUniform( const char* name, float _v1, float _v2 ){
    if(uniforms.count( name )){
        begin();
        setUniform2f( name, _v1, _v2 );
    }
}
void Shader::setUniform( const char* name, float _v1, float _v2, float _v3 ){
    if(uniforms.count( name )){
        begin();
        setUniform3f( name, _v1, _v2, _v3 );
    }
}
void Shader::setUniform( const char* name, float _v1, float _v2, float _v3, float _v4 ){
    if(uniforms.count( name )){
        begin();
        setUniform4f( name, _v1, _v2, _v3, _v4 );
    }
}
void Shader::setUniform( const char* name, ofVec2f _value ){
    if(uniforms.count( name )){
        begin();
        setUniform2f( name, _value.x, _value.y );
    }
}
void Shader::setUniform( const char* name, ofVec3f _value ){
    if(uniforms.count( name )){
        begin();
        setUniform3f( name, _value.x, _value.y, _value.z );
    }
}
void Shader::setUniform( const char* name, ofVec4f _value ){
    if(uniforms.count( name )){
        begin();
        setUniform4f( name, _value.x, _value.y, _value.z, _value.w );
    }
}
void Shader::setUniform( const char* name, ofMatrix3x3& _value, bool transpose ){
    if(uniforms.count( name )){
        begin();
        glUniformMatrix3fv(uniforms[name].loc,1, transpose, &_value[0] );
    }
}
void Shader::setUniform( const char* name, ofMatrix4x4& _value, bool transpose ){
    if(uniforms.count( name )){
        begin();
        glUniformMatrix4fv(uniforms[name].loc,1, transpose, _value.getPtr() );
    }
}

void Shader::setUniform( string name, ofTexture& _tex ){
    setUniform( name, &_tex );
}


void Shader::setUniform( string name, ofTexture* _tex, bool linkTexture ){
    if(uniforms.count( name )){
        begin();
        setUniformTexture( name.c_str(), *_tex, uniforms[name].texloc );
        if(linkTexture){
            //this will bind the texture everytime the shader is used
            //
            uniforms[name].type = GL_SAMPLER_2D;
            uniforms[name].tex = _tex;
        }
    }
    else{
        cout << "Shader: couldn't find uniform named "<< name << endl;
    }
}

void Shader::setUniform( string name, vector<float>& _value ){
    if(uniforms.count( name )){
        begin();
        setUniform1fv( name.c_str(), &_value[0], _value.size() );
    }
}
void Shader::setUniform( string name, vector<ofVec2f>& _value ){
    if(uniforms.count( name )){
        begin();
        setUniform2fv( name.c_str(), &_value[0][0], _value.size() );
    }
}
void Shader::setUniform( string name, vector<ofVec3f>& _value ){
    if(uniforms.count( name )){
        begin();
        setUniform3fv( name.c_str(), &_value[0][0], _value.size() );
    }
}
void Shader::setUniform( string name, vector<ofVec4f>& _value ){
    if(uniforms.count( name )){
        begin();
        setUniform4fv( name.c_str(), &_value[0][0], _value.size() );
    }
}

void Shader::setUniform( string name, vector<ofMatrix3x3>& _value, bool transpose ){
    if(uniforms.count( name )){
        begin();
        glUniformMatrix3fv(uniforms[name].loc,
                           _value.size(),
                           transpose,
                           &_value[0][0]);
    }
};


void Shader::setUniform( string name, vector<ofMatrix3x3>* _value, bool transpose ){
    setUniform( name, *_value, transpose );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniformsMatrices3fv[ name ] = _value;
    }
};
void Shader::setUniform( string name, vector<ofMatrix4x4>& _value, bool transpose ){
    if(uniforms.count( name )){
        begin();
        glUniformMatrix4fv(uniforms[name].loc,
                           _value.size(),
                           transpose,
                           &_value[0]._mat[0][0]);
    }
};


void Shader::setUniform( string name, vector<ofMatrix4x4>* _value, bool transpose ){
    setUniform( name, *_value, transpose );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniformsMatrices4fv[ name ] = _value;
    }
};


void Shader::setUniform( string name, vector<float>* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms1fv[ name ] = _value;
    }
}
void Shader::setUniform( string name, vector<ofVec2f>* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms2fv[ name ] = _value;
    }
}
void Shader::setUniform( string name, vector<ofVec3f>* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms3fv[ name ] = _value;
    }
}
void Shader::setUniform( string name, vector<ofVec4f>* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms4fv[ name ] = _value;
    }
}

void Shader::setUniform(const char* name, float* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms1f[ name ] = _value;
    }
}
void Shader::setUniform(const char* name, ofVec2f* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms2f[ name ] = _value;
    }
}
void Shader::setUniform(const char* name, ofVec3f* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms3f[ name ] = _value;
    }
}
void Shader::setUniform(const char* name, ofVec4f* _value ){
    setUniform( name, *_value );
    ShaderUniform* uniformPointer = getUniform( name );
    if( uniformPointer ){
        dynamicUniforms[ name ] = uniformPointer;
        dynamicUniforms4f[ name ] = _value;
    }
}


void Shader::updateDynamicUniforms(){
    map< string, ShaderUniform* >::iterator d_it;
    for(d_it = dynamicUniforms.begin(); d_it != dynamicUniforms.end(); d_it++){
        
        switch (d_it->second->type) {
            case GL_FLOAT:
                if( d_it->second->count == 1 ){
                    setUniform( d_it->first.c_str(), *dynamicUniforms1f[ d_it->first ] );
                }
                else{
                    setUniform( d_it->first.c_str(), *dynamicUniforms1fv[ d_it->first ] );
                }
                break;
                
            case GL_FLOAT_VEC2:
                if( d_it->second->count == 1 ){
                    setUniform( d_it->first.c_str(), *dynamicUniforms2f[ d_it->first ] );
                }
                else{
                    setUniform( d_it->first.c_str(), *dynamicUniforms2fv[ d_it->first ] );
                }
                break;
                
            case GL_FLOAT_VEC3:
                if( d_it->second->count == 1 ){
                    setUniform( d_it->first.c_str(), *dynamicUniforms3f[ d_it->first ] );
                }
                else{
                    setUniform( d_it->first.c_str(), *dynamicUniforms3fv[ d_it->first ] );
                }
                break;
                
            case GL_FLOAT_VEC4:
                
                if( d_it->second->count == 1 ){
                    setUniform( d_it->first.c_str(), *dynamicUniforms4f[ d_it->first ] );
                }
                else{
                    setUniform( d_it->first.c_str(), *dynamicUniforms4fv[ d_it->first ] );
                }
                break;
                
            case GL_FLOAT_MAT3:
                setUniform( d_it->first.c_str(), *dynamicUniformsMatrices3fv[ d_it->first ] );
                break;
                
            default:
                break;
        }
    }
}
