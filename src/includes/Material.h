//
//  Material.h
//  ofxSceneExample
//
//  Created by lars berg on 8/18/12.
//
/*
 http://www.yaldex.com/open-gl/toc.html
 */



//

#pragma once
#include "Shader.h"

namespace ofxScene{
    
    
    class Material : public Shader{
    public:
        Material(){};
        ~Material(){};
        
        void load( string vertLoc, string fragLoc ){
            
            setShaderFromStrings( ofBufferFromFile( vertLoc ).getText(), ofBufferFromFile( fragLoc ).getText() );
            
            if( !isCompiled ){
                cout << "DEFAULT DIDN'T COMPILE!!!" << endl;
                loadDefault();
            }
        }
        
        void loadDefault(){
            
            unload();
            
            
            vert.clear();
            frag.clear();
            
            //vertex shader
            vert += "//uniform matrices\n";
            vert += "uniform mat4 projectionMatrix;\n";
            vert += "uniform mat4 modelViewMatrix;\n";
            
            vert += "//vertex attributes\n";
            vert += "attribute vec3 position;\n";
            
            vert += "void main(){\n";
            vert += "   gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.);\n";
            vert += "}\n";
            
            //fragment shader
            frag += "void main(){\n";
            frag += "    gl_FragColor = vec4( 1., 0., 0., 1.);\n";
            frag += "}";
            
            
            //load shader
            setShaderFromStrings( vert, frag );
            if( !isCompiled ){
                cout << "DEFAULT DIDN'T COMPILE!!!" << endl;
            }
        }
        
        void setPointLights( vector<PointLight>& pointLights ){
            setUniform( "NUM_POINT_LIGHTS", int( pointLights.size() ) );
            setUniform( "POINT_LIGHTS[0]", &pointLights );
        }
        
        string pointLightChunck(bool useSpecular = true ){
            string PLChunck;
            
            PLChunck += "void pointLight(";
            PLChunck += "in int i, ";
            PLChunck += "in vec3 eye, ";
            PLChunck += "in vec3 ecPosition3, ";
            PLChunck += "inout vec3 normal, ";
            PLChunck += "inout vec3 ambient, ";
            PLChunck += "inout vec3 diffuse";
            if(useSpecular) PLChunck += ", inout vec3 specular ";
            PLChunck += "){\n";
            
            PLChunck += "    float nDotVP,attenuation,d;\n";
            PLChunck += "    vec3 VP;\n";
            
            PLChunck += "    vec3 lightPos = POINT_LIGHTS[i][0];\n";
            PLChunck += "    vec3 lightColor = POINT_LIGHTS[i][1];\n";
            PLChunck += "    vec3 lightAtt = POINT_LIGHTS[i][2];\n";
            
            // Compute vector from surface to light position
            PLChunck += "    VP = lightPos - ecPosition3;\n";
//            PLChunck += "    VP = (viewMatrix * vec4(lightPos,1.)).xyz - ecPosition3;\n";
            
            // Compute distance between surface and light position
            PLChunck += "    d = length(VP);\n";
            
            // Normalize the vector from surface to light position
            PLChunck += "    VP = normalize(VP);\n";
            
            // Compute attenuation
            PLChunck += "    attenuation = 1./(lightAtt.x + lightAtt.y*d + lightAtt.z*d*d);\n";
            
            //diffuse
            PLChunck += "    nDotVP = max(0.0, dot(normal, VP));\n";
            PLChunck += "    diffuse += POINT_LIGHTS[i][1] * nDotVP * attenuation;\n";
            
            //specular
            if( useSpecular ){
                PLChunck += "    vec3 halfVector = normalize(VP + eye);\n";
                PLChunck += "    float nDotHV = max(0.0, dot(normal, halfVector));\n";
                PLChunck += "    float pf = (nDotVP == 0.0)? 0. : pow(nDotHV, SHININESS );\n";
                PLChunck += "    specular += POINT_LIGHTS[i][1] * pf * attenuation;\n";
            }
            
            
            PLChunck += "}\n";
            
            return PLChunck;
        }
        
        string directionalLightChunck(bool useSpecular = true ){
            
            string DLChunck;
            
            DLChunck += "void directionalLight(in int i,\n";
            DLChunck += "                      in vec3 normal,\n";
            DLChunck += "                      in vec3 eye,\n";
            DLChunck += "                      inout vec3 ambient,\n";
            DLChunck += "                      inout vec3 diffuse";
            if(useSpecular)  DLChunck += "                      , inout vec3 specular";
            DLChunck += "){\n";
            
            DLChunck += "    vec3 lightDir = DIRECTIONAL_LIGHTS[i][0];\n";
            DLChunck += "    vec3 lightColor = DIRECTIONAL_LIGHTS[i][1];\n";

            DLChunck += "float nDotVP = max(0.0, dot(normal, lightDir ) );\n";
            DLChunck += "diffuse  += lightColor * nDotVP;\n";
            
            if(useSpecular){
                DLChunck += "float nDotHV = max(0.0, dot(normal, normalize(lightDir+eye)));\n\n";
                DLChunck += "float pf = (nDotVP == 0.0)? 0. : pow( nDotHV, SHININESS );\n";
                DLChunck += "specular += lightColor * pf;\n";
            }
            DLChunck += "}\n";
            
            return DLChunck;
        }
        
        ofVec3f diffuse, specular, ambient;
        float alpha;
    };
    
    class FlatMaterial : public Material{
    public:
        FlatMaterial( ofVec3f _diffuse = ofVec3f(1,1,1), float _alpha = 1.f, ofTexture* texture = NULL ){
            init( _diffuse, _alpha, texture );
        }
        void init( ofVec3f _diffuse = ofVec3f(1,1,1), float _alpha = 1.f, ofTexture* texture = NULL ){
            diffuse = _diffuse;
            alpha = _alpha;
            
            unload();
            
            
            vert.clear();
            frag.clear();
            
            //vertex shader
            vert += "//uniform matrices\n";
            vert += "uniform mat4 projectionMatrix;\n";
            vert += "uniform mat4 modelViewMatrix;\n\n";
            
            vert += "//uniforms\n";
            if(texture){
                vert += "uniform vec2 mapDim;\n";
                vert += "attribute vec2 texCoord;\n";
                vert += "varying vec2 vUv;\n\n";
            }
            
            vert += "//vertex attributes\n";
            vert += "attribute vec3 position;\n\n";
            
            vert += "void main(){\n";
            if(texture)    vert += "vUv = texCoord * mapDim;\n";
            vert += "   gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.);\n";
            vert += "}\n";
            
            //fragment shader
            frag += "//uniforms\n";
            frag += "uniform float ALPHA;\n";
            frag += "uniform vec3 DIFFUSE;\n\n";
            if(texture){
                frag += "uniform sampler2DRect map;\n";
                frag += "varying vec2 vUv;\n\n";
            }
            
            frag += "void main(){\n";
            frag += "    gl_FragColor = vec4( DIFFUSE, ALPHA );\n";
            if(texture) frag += "gl_FragColor *= texture2DRect( map, vUv);\n";
            frag += "}";
            
            
            //load shader
            setShaderFromStrings( vert, frag );
            
            //set uniforms if it compiled
            if( isCompiled ){
                setUniform("DIFFUSE", diffuse );
                setUniform("ALPHA", alpha );
                if(texture){
                    setUniform("mapDim", texture->getWidth(), texture->getHeight() );
                    setUniform("map", *texture );
                }
            }else{
                loadDefault();
                cout << "FLAT MATERIAL DIDN'T COMPIILE LOADED A DEFAULT MATERIAL INSTEAD" << endl;
            }
            
        }
    };
    
    class NormalMaterial : public Material{
    public:
        
        NormalMaterial( ofTexture* texture = NULL ){
            setup( texture );
        }
        ~NormalMaterial(){}
        
        
        void setup( ofTexture* texture = NULL ){
            
            unload();
            
            vert.clear();
            frag.clear();
            
            //vertex shader
            vert += "//uniform matrices\n";
            vert += "uniform mat4 projectionMatrix;\n";
            vert += "uniform mat4 modelViewMatrix;\n";
            vert += "uniform mat3 normalMatrix;\n\n";
            
            vert += "//uniforms\n";
            if(texture){
                vert += "uniform vec2 mapDim;\n";
                vert += "attribute vec2 texCoord;\n";
                vert += "varying vec2 vUv;\n\n";
            }
            
            vert += "//vertex attributes\n";
            vert += "attribute vec3 position;\n";
            vert += "attribute vec3 normal;\n";
            vert += "varying vec3 vNorm;\n\n";
            
            vert += "void main(){\n";
            if(texture)    vert += "vUv = texCoord * mapDim;\n";
            vert += "   vNorm = normalize( normalMatrix * normal);\n";
            vert += "   gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.);\n";
            vert += "}\n";
            
            //fragment shader
            if(texture){
                frag += "uniform sampler2DRect map;\n";
                frag += "varying vec2 vUv;\n\n";
            }
            
            frag += "varying vec3 vNorm;\n";
            frag += "void main(){\n";
            frag += "    gl_FragColor = vec4( vNorm * .5 + .5, 1.);\n";
            if(texture) frag += "gl_FragColor *= texture2DRect( map, vUv);\n";
            frag += "}";
            
            
            //load shader
            setShaderFromStrings( vert, frag );
            
            //set uniforms if it compiled
            if( isCompiled ){
                if(texture){
                    setUniform("mapDim", texture->getWidth(), texture->getHeight() );
                    setUniform("map", *texture );
                }
            }else{
                loadDefault();
                cout << "Material didn't compiile loadinf defualt material instead" << endl;
            }
            
        }
    };
    class FacingRatioMaterial : public Material{
    public:
        
        FacingRatioMaterial(ofVec3f _color1=ofVec3f(1),
                            ofVec3f _color2=ofVec3f(0),
                            float _alpha=1.f,
                            float _exponent=1.f,
                            ofTexture* texture = NULL )
        {
          init( _color1, _color2, _alpha, _exponent, texture );
        }
        ~FacingRatioMaterial(){}
        
        
        void init(ofVec3f _color1=ofVec3f(1),
                  ofVec3f _color2=ofVec3f(0),
                  float _alpha=1.f,
                  float _exponent=1.f,
                  ofTexture* texture = NULL)
        {
          color1 = _color1;
          color2 = _color2;
          alpha = _alpha;
          exponent = _exponent;
          
          unload();
          
          vert.clear();
          frag.clear();
          
          //vertex shader
          vert += "//uniform matrices\n";
          vert += "uniform mat4 projectionMatrix;\n";
          vert += "uniform mat4 modelViewMatrix;\n";
          vert += "uniform mat4 viewMatrix;\n";
          vert += "uniform mat3 normalMatrix;\n";
          
          vert += "//vertex attributes\n";
          vert += "attribute vec3 position;\n";
          vert += "attribute vec3 normal;\n";
          
          vert += "//varying\n";
          vert += "varying vec3 vNorm;\n";
          vert += "varying vec3 ecPosition3;\n";
          vert += "varying vec3 eye;\n";
          
          vert += "void main(){\n";
          vert += "    vec4 ecPosition = modelViewMatrix * vec4(position, 1.);\n";
          vert += "    ecPosition3 = ecPosition.xyz / ecPosition.w;\n";
          vert += "    gl_Position = projectionMatrix * ecPosition;\n";
          vert += "    eye = -normalize(ecPosition3);\n";
          vert += "    vNorm = normalize( normalMatrix * normal );\n";
          vert += "}\n";
          
          
          //fragment shader
          frag += "//uniforms\n";
          frag += "uniform float exponent;\n";
          frag += "uniform float alpha;\n";
          frag += "uniform vec3 color1;\n\n";
          frag += "uniform vec3 color2;\n\n";
          if(texture){
              frag += "uniform sampler2DRect map;\n";
              frag += "uniform vec2 mapDim;\n";
          }
          
          frag += "//varying\n";
          frag += "varying vec3 vNorm;\n";
          frag += "varying vec3 ecPosition3;\n";
          frag += "varying vec3 eye;\n";
          
          frag += "void main(){\n";
          frag += "    vec3 diffuse = vec3(0.);\n";
          frag += "    float nDotEye = dot(normalize(eye),normalize(vNorm));\n";
          frag += "    float facingRatio = pow( nDotEye, exponent );\n";
          frag += "    diffuse = mix( color2, color1, facingRatio );\n";
          frag += "    gl_FragColor = vec4( diffuse, alpha );\n";
          if(texture){
              frag += "gl_FragColor *= texture2DRect( map, mapDim * vec2(facingRatio) );\n";
          }
          
          frag += "}";
          
          
          //load shader
          setShaderFromStrings( vert, frag );
          
          //set uniforms if it compiled
          if( isCompiled ){
              setUniform("color1", color1 );
              setUniform("color2", color2 );
              setUniform("alpha", alpha );
              setUniform("exponent", exponent );
              
              if(texture){
                  setUniform("mapDim", texture->getWidth(), texture->getHeight() );
                  setUniform("map", *texture );
              }
          }else{
              loadDefault();
              cout << "Material didn't compiile loadinf defualt material instead" << endl;
          }
          
        }
        
        ofVec3f color1, color2;
        float alpha, exponent;
    };
        
    class PhongMaterial : public Material{
    public:
        PhongMaterial(
                      ofVec3f _diffuse = ofVec3f(1),
                      ofVec3f _specular = ofVec3f(.8f),
                      ofVec3f _ambient = ofVec3f(0),
                      float _alpha = 1.f,
                      float _shininess = 128.f,
                      ofTexture* texture = NULL )
        {
          init(_diffuse, _specular, _ambient, _alpha, _shininess, texture);
        }
        void init(ofVec3f _diffuse = ofVec3f(1),
                  ofVec3f _specular = ofVec3f(.8f),
                  ofVec3f _ambient = ofVec3f(0),
                  float _alpha = 1.f,
                  float _shininess = 128.f,
                  ofTexture* texture = NULL )
        {
          diffuse = _diffuse;
          specular = _specular;
          ambient = _ambient;
          alpha = _alpha;
          shininess = _shininess;
          
          unload();
          vert.clear();
          frag.clear();
          
          //vertex shader
          vert += "//uniform matrices\n";
          vert += "uniform mat4 projectionMatrix;\n";
          vert += "uniform mat4 modelViewMatrix;\n";
          vert += "uniform mat4 viewMatrix;\n";
          vert += "uniform mat3 normalMatrix;\n";
          
          vert += "//vertex attributes\n";
          vert += "attribute vec3 position;\n";
          vert += "attribute vec3 normal;\n";
          
          vert += "//varying\n";
          vert += "varying vec3 vNorm;\n";
          vert += "varying vec3 ecPosition3;\n";
          vert += "varying vec3 eye;\n";
          
          if(texture){
              vert += "uniform vec2 mapDim;\n";
              vert += "attribute vec2 texCoord;\n";
              vert += "varying vec2 vUv;\n\n";
          }
          
          vert += "void main(){\n";
          vert += "    vec4 ecPosition = modelViewMatrix * vec4(position, 1.);\n";
          vert += "    ecPosition3 = ecPosition.xyz / ecPosition.w;\n";
          vert += "    gl_Position = projectionMatrix * ecPosition;\n";
          vert += "    eye = -normalize(ecPosition3);\n";
          vert += "    vNorm = normalize( normalMatrix * normal );\n";
          
          if(texture)    vert += "vUv = texCoord * mapDim;\n";
          
          vert += "}\n";
          
          //fragment shader
          frag += "//uniforms\n";
          frag += "uniform mat3 POINT_LIGHTS[16];\n";
          frag += "uniform int NUM_POINT_LIGHTS;\n";
          frag += "uniform mat3 DIRECTIONAL_LIGHTS[16];\n";
          frag += "uniform int NUM_DIRECTIONAL_LIGHTS;\n";
          
          frag += "uniform mat4 viewMatrix;\n";
          frag += "uniform vec3 DIFFUSE;\n";
          frag += "uniform vec3 SPECULAR;\n";
          frag += "uniform vec3 AMBIENT;\n";
          frag += "uniform float ALPHA;\n";
          frag += "uniform float SHININESS;\n";
          
          frag += "//varying\n";
          frag += "varying vec3 vNorm;\n";
          frag += "varying vec3 ecPosition3;\n";
          frag += "varying vec3 eye;\n";
          
          if(texture){
              frag += "uniform sampler2DRect map;\n";
              frag += "varying vec2 vUv;\n\n";
          }
          
          frag += pointLightChunck();
          frag += directionalLightChunck();
          
          frag += "void main(){\n";
          frag += "    vec3 diffuse = vec3(0.);\n";
          frag += "    vec3 specular = vec3(0.);\n";
          frag += "    vec3 ambient = AMBIENT;\n";
          frag += "    vec3 normal = normalize( vNorm );\n";
          
          frag += "    for(int i=0; i<NUM_POINT_LIGHTS; i++){\n";
          frag += "        pointLight( i, eye, ecPosition3, normal, ambient, diffuse, specular );\n";
          frag += "    }\n";
          
          frag += "    for(int i=0; i<NUM_DIRECTIONAL_LIGHTS; i++){\n";
          frag += "        directionalLight( i, normal, eye, ambient, diffuse, specular );\n";
          frag += "    }\n";
          
          if(texture){
              frag += "    diffuse *= texture2DRect( map, vUv ).xyz * DIFFUSE;\n";
          }
          else{
              frag += "    diffuse *= DIFFUSE;\n";
          }
          
          frag += "    gl_FragColor = vec4( diffuse + specular * SPECULAR + ambient, 1.);\n";
          
          
          frag += "}\n";
          
          //load shader
          setShaderFromStrings( vert, frag );
          
          //set uniforms if it compiled
          if( isCompiled ){
              setUniform( "DIFFUSE", diffuse);
              setUniform( "SPECULAR", specular);
              setUniform( "AMBIENT", ambient);
              setUniform( "ALPHA", 1.f );
              setUniform( "SHININESS", shininess );
              
              if(texture){
                  setUniform( "mapDim", texture->getWidth(), texture->getHeight() );
                  setUniform( "map", *texture );
              }
          }
          
          else{
              loadDefault();
              cout << "Phong Material didn't compiile. loaded defualt material instead" << endl;
          }
        }
        
        //variables
        float shininess;
    };
    
    class LambertMaterial : public Material{
    public:
        LambertMaterial(ofVec3f _diffuse = ofVec3f(1,1,1),
                        ofVec3f _ambient = ofVec3f(0,0,0),
                        float _alpha = 1.f,
                        ofTexture* texture = NULL )
        {
          init( _diffuse, _ambient, _alpha, texture );

        }
        void init(ofVec3f _diffuse = ofVec3f(1,1,1),
                   ofVec3f _ambient = ofVec3f(0,0,0),
                   float _alpha = 1.f,
                   ofTexture* texture = NULL )
        {
          diffuse = _diffuse;
          ambient = _ambient;
          alpha = _alpha;
          
          unload();
          vert.clear();
          frag.clear();
          
          //vertex shader
          vert += "//uniform matrices\n";
          vert += "uniform mat4 projectionMatrix;\n";
          vert += "uniform mat4 modelViewMatrix;\n";
          vert += "uniform mat4 viewMatrix;\n";
          vert += "uniform mat3 normalMatrix;\n";
          
          vert += "//vertex attributes\n";
          vert += "attribute vec3 position;\n";
          vert += "attribute vec3 normal;\n";
          
          vert += "//varying\n";
          vert += "varying vec3 vNorm;\n";
          vert += "varying vec3 ecPosition3;\n";
          vert += "varying vec3 eye;\n";
          
          if(texture){
              vert += "uniform vec2 mapDim;\n";
              vert += "attribute vec2 texCoord;\n";
              vert += "varying vec2 vUv;\n\n";
          }
          
          vert += "void main(){\n";
          vert += "    vec4 ecPosition = modelViewMatrix * vec4(position, 1.);\n";
          vert += "    ecPosition3 = ecPosition.xyz / ecPosition.w;\n";
          vert += "    gl_Position = projectionMatrix * ecPosition;\n";
          vert += "    eye = -normalize(ecPosition3);\n";
          vert += "    vNorm = normalize( normalMatrix * normal );\n";
          
          if(texture)    vert += "vUv = texCoord * mapDim;\n";
          
          vert += "}\n";
          
          //fragment shader
          frag += "//uniforms\n";
          frag += "uniform mat3 POINT_LIGHTS[16];\n";
          frag += "uniform int NUM_POINT_LIGHTS;\n";
          frag += "uniform mat3 DIRECTIONAL_LIGHTS[16];\n";
          frag += "uniform int NUM_DIRECTIONAL_LIGHTS;\n";
          
          frag += "uniform mat4 viewMatrix;\n";
          frag += "uniform vec3 DIFFUSE;\n";
          frag += "uniform vec3 SPECULAR;\n";
          frag += "uniform vec3 AMBIENT;\n";
          frag += "uniform float ALPHA;\n";
          frag += "uniform float SHININESS;\n";
          
          frag += "//varying\n";
          frag += "varying vec3 vNorm;\n";
          frag += "varying vec3 ecPosition3;\n";
          frag += "varying vec3 eye;\n";
          
          if(texture){
              frag += "uniform sampler2DRect map;\n";
              frag += "varying vec2 vUv;\n\n";
          }
          
          frag += pointLightChunck( false );//false - don't need specular
          frag += directionalLightChunck( false );//false - don't need specular
          
          frag += "void main(){\n";
          
          frag += "    vec3 diffuse = vec3(0.);\n";
          frag += "    vec3 ambient = AMBIENT;\n";
          frag += "    vec3 normal = normalize( vNorm );\n";
          
          frag += "    for(int i=0; i<NUM_POINT_LIGHTS; i++){\n";
          frag += "        pointLight( i, eye, ecPosition3, normal, ambient, diffuse );\n";
          frag += "    }\n";
          
          frag += "    for(int i=0; i<NUM_DIRECTIONAL_LIGHTS; i++){\n";
          frag += "        directionalLight( i, normal, eye, ambient, diffuse );\n";
          frag += "    }\n";
          
          if(texture){
              frag += "    diffuse *= texture2DRect( map, vUv ).xyz * DIFFUSE;\n";
          }
          else{
              frag += "    diffuse *= DIFFUSE;\n";
          }
          
          frag += "    gl_FragColor = vec4( diffuse + ambient, 1.);\n";
          
          
          frag += "}\n";
          
          //load shader
          setShaderFromStrings( vert, frag );
          
          //set uniforms if it compiled
          if( isCompiled ){
              setUniform( "DIFFUSE", diffuse);
              setUniform( "AMBIENT", ambient);
              setUniform( "ALPHA", 1.f );
              
              if(texture){
                  setUniform( "mapDim", texture->getWidth(), texture->getHeight() );
                  setUniform( "map", *texture );
              }
          }
          
          else{
              loadDefault();
              cout << "Lambert Material didn't compiile. loaded defualt material instead" << endl;
          }
        }
    };
    
}
