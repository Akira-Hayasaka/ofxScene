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

void main(){
    vNorm = normalize( normalMatrix * normal ) * .5 + .5;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.);
}
