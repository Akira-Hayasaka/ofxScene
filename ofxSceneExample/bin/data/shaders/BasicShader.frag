//uniforms
uniform mat4 viewMatrix;
uniform vec3 DIFFUSE;
uniform float ALPHA;
uniform float SHININESS;

uniform mat3 POINT_LIGHTS[16];
uniform float NUM_POINT_LIGHTS;

//varying
varying vec3 vNorm;
varying vec3 ecPosition3;
varying vec3 eye;


void pointLight(
in int i,
in vec3 eye,
in vec3 ecPosition3,
inout vec3 normal,
inout vec3 ambient,
inout vec3 diffuse,
inout vec3 specular
){
    
    float nDotVP;         // normal . light direction
    float nDotHV;         // normal . light half vector
    float pf;             // power factor
    float attenuation;    // computed attenuation factor
    float d;              // distance from surface to light source
    vec3  VP;             // direction from surface to light position
    vec3  halfVector;     // direction of maximum highlights
    
    vec3 lightPos = POINT_LIGHTS[i][0];
    vec3 lightColor = POINT_LIGHTS[i][1];
    vec3 lightAtt = POINT_LIGHTS[i][2];
    
    // Compute vector from surface to light position
    VP = (viewMatrix * vec4(lightPos,1.)).xyz - ecPosition3;
    
    // Compute distance between surface and light position
    d = length(VP);
    
    // Normalize the vector from surface to light position
    VP = normalize(VP);
    
    // Compute attenuation
    attenuation = 1./(lightAtt.x + lightAtt.y*d + lightAtt.z*d*d);
    
    halfVector = normalize(VP + eye);
    
    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));
    
    pf = (nDotVP == 0.0)? 0. : pow(nDotHV, SHININESS );
    
    diffuse += POINT_LIGHTS[i][1] * nDotVP * attenuation;
    specular += POINT_LIGHTS[i][1] * pf * attenuation;
}

void main(){
    vec3 diffuse = vec3(0.);
    vec3 specular = vec3(0.);
    vec3 ambient = vec3(0.);
    
    vec3 normal = normalize( vNorm );
    for(int i=0; i<int(NUM_POINT_LIGHTS); i++){
        pointLight( i, eye, ecPosition3, normal, ambient, diffuse, specular );
    }
    
    gl_FragColor = vec4( diffuse + specular, 1.);
}