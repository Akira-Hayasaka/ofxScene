#ofxScene
ofxScene is a scene graph addon for openframeworks.

it's still pretty rough, but it easily generates and renders geometry using customizable materials. 

#ofxScene::Geometry
ofxScene::Geometry can be extended to create custom geometries and primitives( see ofxScene::CubeGeometry, ofxScene::ConeGeometry, ofxScene::SphereGeometry )

#ofxScene::Material
ofxScene::Material is an extension of ofShader that can be extended to to build custom materials( ofxScene::PhongMaterial, ofxScene::LambertMaterial )
any lights in the scene will be automatically updated at render time

There is still lots of work to do here... shadow mapping, post-processing

#ofxScene::Mesh

#ofxScene::Scene

#ofxScene::Lights

#setup
place the ofxScene in the addons folder

#dependencies
ofxAssimpModelLoader.h used for loading geometry from files. int the future it be nice to load animations and entire scenes


