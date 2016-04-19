#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>

#include <sapi.h>  
#include <sphelper.h>  
#include <Synchapi.h>
// keep all MS files above ofmain.h
#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
