//uniforms

//varying
varying vec3 vNorm;

//fragment shader
void main(){
    gl_FragColor = vec4( vNorm, 1.);
}