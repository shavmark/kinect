#pragma once
#pragma once

#define Foundation_UnWindows_INCLUDED
// For M_PI and log definitions
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <memory>
#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
// Microsoft Speech Platform SDK 11
#include <sapi.h>
#include <sphelper.h> // SpFindBestToken()
#include <strsafe.h>
#include <intsafe.h>
#include <Synchapi.h>
#include <memory>

// keep all c+ code above main 
#include "ofMain.h"
#include "snappy.h"
#include "ofxJSON.h"
#include "networking.h"
#include "kinect2552.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		Software2552::Kinect2552 kinect;
		Software2552::WriteOsc comms;
	

		shared_ptr<Software2552::KinectBody> bodies=nullptr;
		shared_ptr<Software2552::KinectFaces> faces = nullptr;
		shared_ptr<Software2552::KinectAudio> audio = nullptr;

		ofImage image;
		ofImage imageir;
		ofPixels pixels;


};
