
#include "ofApp.h"

//========================================================================
int main( ){
	
	float w = 1920;
	float h = 1080;

//	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	ofSetupOpenGL(w, h, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
