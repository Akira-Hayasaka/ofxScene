#pragma once

#include "ofMain.h"
#include "ofxScene.h"


class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void exit();
    
    ofImage oleImage;
    ofImage globeImage;
    ofVec3f bgColor;
    
    ofEasyCam camera;
    
    ofxScene::Scene scene;

    ofxScene::LambertMaterial lambertMaterial;
    
    ofxScene::Mesh plane;
    ofxScene::Mesh cube;
//    ofxScene::Mesh sphere;
    ofxScene::Mesh facingRatioMesh;
    ofxScene::Mesh normalMesh;
    ofxScene::Mesh dynamicMesh;
    
    vector<ofxScene::Mesh> nestedMeshes;
    
    
    ofxScene::SphereGeometry dynamicSphere;
    vector<ofVec3f> orginalPositions;
    vector<ofVec3f> orginalNormals;
    
    ofxScene::Mesh waltHead;
    ofxScene::Mesh waltHead1;
    ofxScene::Mesh waltHead2;
    
    ofxScene::SphereGeometry sphereGeometry;
    vector<ofxScene::PointLight> pointLights;
    vector<ofxScene::Mesh> lightMeshes;
    
    ofxScene::SpotLight spotLight;
    ofxScene::Mesh spotLightMesh;
    ofVec3f spotLightColor;
    
    ofxScene::Mesh debugMesh;
    ofxScene::NormalDisplayMesh normalDisplayMesh;
    
};
