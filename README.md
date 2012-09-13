#ofxScene
ofxScene is a scene graph addon for openframeworks.

it's still pretty rough, but it easily generates and renders geometry using customizable materials. 

#setup
put ofxScene in the addons folder

#dependencies
ofxAssimpModelLoader.h used for loading geometry from files. in the future it'd be nice to load entire scenes & animations

##ofxScene::Geometry
ofxScene::Geometry can be extended to create custom geometries and primitives( ofxScene::CubeGeometry, ofxScene::SphereGeometry, etc. )

##ofxScene::Material
ofxScene::Material is an extension of ofShader that can be extended to to build custom materials( ofxScene::PhongMaterial(), ofxScene::LambertMaterial )
any lights in the scene will be automatically updated at render time

There is still lots of work to do here... add spot lights, shadow mapping, post-processing

##ofxScene::Mesh

##ofxScene::Scene

##ofxScene::Lights


