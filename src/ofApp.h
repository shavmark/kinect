#pragma once
#include "ofxJSON.h"
#include "kinect2552.h"
#include "networking.h"

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
		Software2552::KinectBodies bodies;
		Software2552::KinectFaces faces;
		Software2552::KinectAudio audio;
		Software2552::Kinect2552 kinect;
		Software2552::WriteComms comms;
		
};
