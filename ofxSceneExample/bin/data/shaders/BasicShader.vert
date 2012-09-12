//uniform matrices
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;


//vertex attributes
attribute vec3 position;
attribute vec3 normal;

//varying
varying vec3 vNorm;
varying vec3 ecPosition3;
varying vec3 eye;

void main(){
    vec4 ecPosition = modelViewMatrix * vec4(position, 1.);
    
    ecPosition3 = ecPosition.xyz / ecPosition.w;
    gl_Position = projectionMatrix * ecPosition;
    eye = -normalize(ecPosition3);
    vNorm = normalize( normalMatrix * normal );

    
}
