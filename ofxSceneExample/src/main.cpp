#include "testApp.h"
#include "ofAppGlutWindow.h"

//--------------------------------------------------------------
int main(){
    ofAppGlutWindow window;
	window.setGlutDisplayString("rgba double samples>=4 depth");
	ofSetupOpenGL(&window, 1080,720, OF_WINDOW);			// <-------- setup the GL context
    
	ofRunApp(new testApp()); // start the app
}
