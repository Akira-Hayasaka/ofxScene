//
//  Material.h
//
/*
 http://www.yaldex.com/open-gl/toc.html
 */

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
            string chunck;
            
            chunck += "void pointLight(";
            chunck += "in int i, ";
            chunck += "in vec3 eye, ";
            chunck += "in vec3 ecPosition3, ";
            chunck += "inout vec3 normal, ";
            chunck += "inout vec3 ambient, ";
            chunck += "inout vec3 diffuse";
            if(useSpecular) chunck += ", inout vec3 specular ";
            chunck += "){\n";
            
            chunck += "    float nDotVP,attenuation,d;\n";
            chunck += "    vec3 VP;\n";
            
            chunck += "    vec3 lightPos = POINT_LIGHTS[i][0];\n";
            chunck += "    vec3 lightColor = POINT_LIGHTS[i][1];\n";
            chunck += "    vec3 lightAtt = POINT_LIGHTS[i][2];\n";
            
            chunck += "    VP = lightPos - ecPosition3;\n";
            chunck += "    d = length(VP);\n";
            chunck += "    VP = normalize(VP);\n";
            chunck += "    attenuation = 1./(lightAtt.x + lightAtt.y*d + lightAtt.z*d*d);\n";
            
            //diffuse
            chunck += "    nDotVP = max(0.0, dot(normal, VP));\n";
            chunck += "    if(nDotVP != 0.){\n";
            chunck += "        diffuse += lightColor * nDotVP * attenuation;\n";
            
            //specular
            if( useSpecular ){
                chunck += "        vec3 halfVector = normalize(VP + eye);\n";
                chunck += "        float nDotHV = max(0.0, dot(normal, halfVector));\n";
                chunck += "        float pf = (nDotVP == 0.0)? 0. : pow(nDotHV, Shininess );\n";
                chunck += "        specular += lightColor * pf * attenuation;\n";
            }
            
            
            chunck += "    }\n}\n";
            
            return chunck;
        }
        
        string directionalLightChunck(bool useSpecular = true ){
            
            string chunck;
            
            chunck += "void directionalLight(in int i,\n";
            chunck += "                      in vec3 normal,\n";
            chunck += "                      in vec3 eye,\n";
            chunck += "                      inout vec3 ambient,\n";
            chunck += "                      inout vec3 diffuse";
            if(useSpecular)  chunck += "                      , inout vec3 specular";
            chunck += "){\n";
            
            chunck += "    vec3 lightDir = DIRECTIONAL_LIGHTS[i][0];\n";
            chunck += "    vec3 lightColor = DIRECTIONAL_LIGHTS[i][1];\n";

            chunck += "float nDotVP = max(0.0, dot(normal, lightDir ) );\n";
            chunck += "diffuse  += lightColor * nDotVP;\n";
            
            if(useSpecular){
                chunck += "float nDotHV = max(0.0, dot(normal, normalize(lightDir+eye)));\n\n";
                chunck += "float pf = (nDotVP == 0.0)? 0. : pow( nDotHV, Shininess );\n";
                chunck += "specular += lightColor * pf;\n";
            }
            chunck += "}\n";
            
            return chunck;
        }
        
        string spotLightChunck( bool useSpecular = true ){
            string chunck;
            chunck += "void spotLight(\n";
            chunck += "in int i, ";
            chunck += "in vec3 eye, ";
            chunck += "in vec3 ecPosition3, ";
            chunck += "inout vec3 normal, ";
            chunck += "inout vec3 ambient, ";
            chunck += "inout vec3 diffuse";
            if(useSpecular) chunck += ", inout vec3 specular ";
            chunck += "){\n";
            
            chunck += "    float nDotVP;           // normal . light direction\n";
            chunck += "    float spotDot;          // cosine of angle between spotlight\n";
            chunck += "    float spotAttenuation;  // spotlight attenuation factor\n";
            chunck += "    float attenuation;      // computed attenuation factor\n";
            chunck += "    float d;                // distance from surface to light source\n";
            chunck += "    vec3 VP;                // direction from surface to light position\n";
            
            chunck += "    vec3 lightPos   = SPOT_LIGHTS[i][0].xyz;\n";
            chunck += "    vec3 lightColor = SPOT_LIGHTS[i][1].xyz;\n";
            chunck += "    vec3 lightAtt   = SPOT_LIGHTS[i][2].xyz;\n";
            chunck += "    vec3 lightDir   = SPOT_LIGHTS[i][3].xyz;\n";
            chunck += "    float cutoff    = SPOT_LIGHTS[i][0].w;\n";
            chunck += "    float exponent  = SPOT_LIGHTS[i][1].w;\n";
            
            chunck += "    VP = lightPos - ecPosition3;\n";
            chunck += "    d = length(VP);\n";
            chunck += "    VP = normalize(VP);\n";
            chunck += "    attenuation = 1.0 / (lightAtt.x + lightAtt.y*d + lightAtt.z * d * d);\n";
            
            chunck += "    spotDot = dot(-VP, normalize(lightDir));\n";
            chunck += "    spotAttenuation = (spotDot < cutoff)? 0.0 : pow(spotDot, exponent);\n";
            chunck += "    if(spotAttenuation != 0.){;\n";
            chunck += "        attenuation *= spotAttenuation;\n";
            chunck += "        nDotVP = max(0.0, dot(normal, VP));\n";
            
            //            chunck += "    ambient  += gl_LightSource[i].ambient * attenuation;\n";
            chunck += "        diffuse  += lightColor * nDotVP * attenuation;\n";
            
            if(useSpecular){
                chunck += "        vec3 halfVector = normalize(VP + eye);\n";
                chunck += "        float nDotHV = max(0.0, dot(normal, halfVector));\n";
                chunck += "        float pf = (nDotVP == 0.0)? 0.0 : pow(nDotHV, Shininess);\n";
                chunck += "        specular += lightColor * pf * attenuation;\n";
            }
            
            chunck += "    }\n";
            chunck += "}\n\n";
            
            
            return chunck;
        }
        
        string lightsChunck( bool useSpecular=true){
            string chunck;
            
            chunck += "    for(int i=0; i<NUM_POINT_LIGHTS; i++){\n";
            chunck += "        pointLight( i, eye, ecPosition3, normal, ambient, diffuse";
            if(useSpecular) chunck += " , specular";
            chunck += ");\n";
            chunck += "    }\n";
            
            chunck += "    for(int i=0; i<NUM_DIRECTIONAL_LIGHTS; i++){\n";
            chunck += "        directionalLight( i, normal, eye, ambient, diffuse";
            if(useSpecular) chunck += " , specular";
            chunck += ");\n";
            chunck += "    }\n";
            
            chunck += "    for(int i=0; i<NUM_SPOT_LIGHTS; i++){\n";
            chunck += "        spotLight( i, eye, ecPosition3, normal, ambient, diffuse";
            if(useSpecular) chunck += " , specular";
            chunck += ");\n";
            chunck += "    }\n";
            
            return chunck;
        }
        
        string randomFunc(){
            string chunck;
            chunck += "float random(float seed) {\n";
            chunck += "    return fract(sin(dot(gl_FragCoord.xyz + seed, vec3(12.9898, 78.233, 151.7182))) * 43758.5453 + seed);\n";
            chunck += "}\n\n";
            
            chunck += "float random(int seed) {\n";
            chunck += "    return random(float( seed ));\n";
            chunck += "}\n\n";
            
            return chunck;
        }
        
        string snoise3fFunc(){
            
            string outstring;
            
            outstring += "vec4 permute( vec4 x ) {\n";
            outstring += "	return mod( ( ( x * 34.0 ) + 1.0 ) * x, 289.0 );\n";
            outstring += "}\n";
            
            outstring += "vec4 taylorInvSqrt( vec4 r ) {\n";
            outstring += "	return 1.79284291400159 - 0.85373472095314 * r;\n";
            outstring += "}\n";
            
            outstring += "float snoise( vec3 v ) {\n";
            outstring += "	const vec2 C = vec2( 1.0 / 6.0, 1.0 / 3.0 );\n";
            outstring += "	const vec4 D = vec4( 0.0, 0.5, 1.0, 2.0 );\n";
            
            outstring += "	// First corner\n";
            outstring += "	vec3 i  = floor( v + dot( v, C.yyy ) );\n";
            outstring += "	vec3 x0 = v - i + dot( i, C.xxx );\n";
            
            outstring += "	// Other corners\n";
            outstring += "	vec3 g = step( x0.yzx, x0.xyz );\n";
            outstring += "	vec3 l = 1.0 - g;\n";
            outstring += "	vec3 i1 = min( g.xyz, l.zxy );\n";
            outstring += "	vec3 i2 = max( g.xyz, l.zxy );\n";
            
            outstring += "	vec3 x1 = x0 - i1 + 1.0 * C.xxx;\n";
            outstring += "	vec3 x2 = x0 - i2 + 2.0 * C.xxx;\n";
            outstring += "	vec3 x3 = x0 - 1. + 3.0 * C.xxx;\n";
            
            outstring += "	// Permutations\n";
            outstring += "	i = mod( i, 289.0 );\n";
            outstring += "	vec4 p = permute( permute( permute(\n";
            outstring += " 	i.z + vec4( 0.0, i1.z, i2.z, 1.0 ) )\n";
            outstring += "		   + i.y + vec4( 0.0, i1.y, i2.y, 1.0 ) )\n";
            outstring += "		   + i.x + vec4( 0.0, i1.x, i2.x, 1.0 ) );\n";
            
            outstring += "	// Gradients\n";
            outstring += "	// ( N*N points uniformly over a square, mapped onto an octahedron.)\n";
            outstring += "	float n_ = 1.0 / 7.0; // N=7\n";
            outstring += "	vec3 ns = n_ * D.wyz - D.xzx;\n";
            outstring += "	vec4 j = p - 49.0 * floor( p * ns.z *ns.z );  //  mod(p,N*N)\n";
            
            outstring += "	vec4 x_ = floor( j * ns.z );\n";
            outstring += "	vec4 y_ = floor( j - 7.0 * x_ );    // mod(j,N)\n";
            
            outstring += "	vec4 x = x_ *ns.x + ns.yyyy;\n";
            outstring += "	vec4 y = y_ *ns.x + ns.yyyy;\n";
            outstring += "	vec4 h = 1.0 - abs( x ) - abs( y );\n";
            
            outstring += "	vec4 b0 = vec4( x.xy, y.xy );\n";
            outstring += "	vec4 b1 = vec4( x.zw, y.zw );\n";
            
            outstring += "	vec4 s0 = floor( b0 ) * 2.0 + 1.0;\n";
            outstring += "	vec4 s1 = floor( b1 ) * 2.0 + 1.0;\n";
            outstring += "	vec4 sh = -step( h, vec4( 0.0 ) );\n";
            
            outstring += "	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;\n";
            outstring += "	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;\n";
            
            outstring += "	vec3 p0 = vec3( a0.xy, h.x );\n";
            outstring += "	vec3 p1 = vec3( a0.zw, h.y );\n";
            outstring += "	vec3 p2 = vec3( a1.xy, h.z );\n";
            outstring += "	vec3 p3 = vec3( a1.zw, h.w );\n";
            
            outstring += "	// Normalise gradients\n";
            outstring += "	vec4 norm = taylorInvSqrt( vec4( dot( p0, p0 ), dot( p1, p1 ), dot( p2, p2 ), dot( p3, p3 ) ) );\n";
            outstring += "	p0 *= norm.x;\n";
            outstring += "	p1 *= norm.y;\n";
            outstring += "	p2 *= norm.z;\n";
            outstring += "	p3 *= norm.w;\n";
            
            outstring += "	// Mix final noise value\n";
            outstring += "	vec4 m = max( 0.6 - vec4( dot( x0, x0 ), dot( x1, x1 ), dot( x2, x2 ), dot( x3, x3 ) ), 0.0 );\n";
            outstring += "	m = m * m;\n";
            outstring += "	return 42.0 * dot( m*m, vec4( dot( p0, x0 ), dot( p1, x1 ),\n";
            outstring += "					  dot( p2, x2 ), dot( p3, x3 ) ) );\n";
            
            outstring += "}\n\n";
            
            return outstring;
        }
        float gauss( float x, float sigma ){
            return exp( -(x*x)/(2.*sigma*sigma));
        }
        
        vector<ofVec3f> gaussKernel( int sizeX, int sizeY ){
            vector<ofVec3f> kernel;
            sizeX /= 2;
            sizeY /= 2;
            for(float i=-sizeX; i<=sizeX; i++){
                for(float j=-sizeY;j<=sizeY;j++){
                    kernel.push_back( ofVec3f( i, j, gauss(i,float(sizeX)) * gauss(j,float(sizeY))));
                }
            }
            
            float sum = 0.;
            for(int i=0; i<kernel.size(); i++){
                sum += kernel[i].z;
            }
            for(int i=0; i<kernel.size(); i++){
                kernel[i].z /= sum;
            }
            
            return kernel;
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
            frag += "uniform float Alpha;\n";
            frag += "uniform vec3 Diffuse;\n\n";
            if(texture){
                frag += "uniform sampler2DRect map;\n";
                frag += "varying vec2 vUv;\n\n";
            }
            
            frag += "void main(){\n";
            frag += "    gl_FragColor = vec4( Diffuse, Alpha );\n";
            if(texture) frag += "gl_FragColor *= texture2DRect( map, vUv);\n";
            frag += "}";
            
            
            //load shader
            setShaderFromStrings( vert, frag );
            
            //set uniforms if it compiled
            if( isCompiled ){
                setUniform("Diffuse", diffuse );
                setUniform("Alpha", alpha );
                if(texture){
                    setUniform("mapDim", texture->getWidth(), texture->getHeight() );
                    setUniform("map", *texture );
                }
            }else{
                loadDefault();
                cout << "FLAT MATERIAL DIDN'T COMPIILE LOADED A DEFAULT MATERIAL INSTEAD" << endl;
            }
            glUseProgram(0);
            
        }
    };
    
    class NormalMaterial : public Material{
    public:
        
        NormalMaterial( ofTexture* normalMap = NULL ){
            init( normalMap );
        }
        ~NormalMaterial(){}
        
        void init( ofTexture* normalMap = NULL ){
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

			frag += "//varying\n";
			frag += "varying vec3 vNorm;\n";

			frag += "void main(){\n";
			frag += "    vec3 diffuse = .5 + .5 * vNorm;\n";
			frag += "    gl_FragColor = vec4( diffuse, 1. );\n";

			
			frag += "}";
			
			
			//load shader
			setShaderFromStrings( vert, frag );
			
			//set uniforms if it compiled
			if( isCompiled ){
//				setUniform("color1", color1 );
//				setUniform("color2", color2 );
//				setUniform("alpha", alpha );
//				setUniform("exponent", exponent );
				
//				if(texture){
//					setUniform("mapDim", texture->getWidth(), texture->getHeight() );
//					setUniform("map", *texture );
//				}
			}else{
				loadDefault();
				cout << "Material didn't compiile loadinf defualt material instead" << endl;
			}
			glUseProgram(0);
            
            
        }
        
        /*
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
                cout << "Normal Material didn't compiile loading defualt material instead" << endl;
            }
            glUseProgram(0);
            
        }
         */
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
          glUseProgram(0);
          
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
                      float _shininess = 64.f,
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
          frag += "uniform mat4 SPOT_LIGHTS[8];\n";
          frag += "uniform int NUM_SPOT_LIGHTS;\n";
          
          frag += "uniform mat4 viewMatrix;\n";
          frag += "uniform vec3 Diffuse;\n";
          frag += "uniform vec3 Specular;\n";
          frag += "uniform vec3 Ambient;\n";
          frag += "uniform float Alpha;\n";
          frag += "uniform float Shininess;\n";
          
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
          frag += spotLightChunck();
          
          frag += "void main(){\n";
          frag += "    vec3 diffuse = vec3(0.);\n";
          frag += "    vec3 specular = vec3(0.);\n";
          frag += "    vec3 ambient = vec3(1.);\n";
          frag += "    vec3 normal = normalize( vNorm );\n";
          
          frag += lightsChunck();
          
          if(texture){
              frag += "    vec3 texColor = texture2DRect( map, vUv ).xyz;\n";
              frag += "    diffuse *= texColor * Diffuse;\n";
              frag += "    ambient *= texColor * Ambient;\n";
          }
          else{
              frag += "    diffuse *= Diffuse;\n";
              frag += "    ambient *= Ambient;\n";
          }
          
          
          frag += "    gl_FragColor = vec4( diffuse + specular * Specular + ambient, 1.);\n";
          
          
          frag += "}\n";
          
          //load shader
          setShaderFromStrings( vert, frag );
          
          //set uniforms if it compiled
          if( isCompiled ){
              setUniform( "Diffuse", diffuse);
              setUniform( "Specular", specular);
              setUniform( "Ambient", ambient);
              setUniform( "Alpha", 1.f );
              setUniform( "Shininess", shininess );
              
              if(texture){
                  setUniform( "mapDim", texture->getWidth(), texture->getHeight() );
                  setUniform( "map", *texture );
              }
          }
          
          else{
              loadDefault();
              cout << "Phong Material didn't compiile. loaded defualt material instead" << endl;
          }
          glUseProgram(0);
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
          frag += "uniform mat4 SPOT_LIGHTS[8];\n";
          frag += "uniform int NUM_SPOT_LIGHTS;\n";
          
          frag += "uniform mat4 viewMatrix;\n";
          frag += "uniform vec3 Diffuse;\n";
          frag += "uniform vec3 Specular;\n";
          frag += "uniform vec3 Ambient;\n";
          frag += "uniform float Alpha;\n";
          frag += "uniform float Shininess;\n";
          
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
          frag += spotLightChunck( false );//false - don't need specular
          
          frag += "void main(){\n";
          
          frag += "    vec3 diffuse = vec3(0.);\n";
          frag += "    vec3 ambient = vec3(1.);\n";
          frag += "    vec3 normal = normalize( vNorm );\n";
          
          frag += lightsChunck(false);
          
          if(texture){
              frag += "    vec3 texColor = texture2DRect( map, vUv ).xyz;\n";
              frag += "    diffuse *= texColor * Diffuse;\n";
              frag += "    ambient *= texColor * Ambient;\n";
          }
          else{
              frag += "    diffuse *= Diffuse;\n";
              frag += "    ambient *= Ambient;\n";
          }
          
          frag += "    gl_FragColor = vec4( diffuse + ambient, 1.);\n";
          
          
          frag += "}\n";
          
          //load shader
          setShaderFromStrings( vert, frag );
          
          //set uniforms if it compiled
          if( isCompiled ){
              setUniform( "Diffuse", diffuse);
              setUniform( "Ambient", ambient);
              setUniform( "Alpha", 1.f );
              
              if(texture){
                  setUniform( "mapDim", texture->getWidth(), texture->getHeight() );
                  setUniform( "map", *texture );
              }
          }
          
          else{
              loadDefault();
              cout << "Lambert Material didn't compiile. loaded defualt material instead" << endl;
          }
          glUseProgram(0);
        }
    };
    
}
