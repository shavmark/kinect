#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	comms.setup();
	kinect.setup(comms);

	bodies = std::make_shared<Software2552::KinectBody>(&kinect);
	faces = std::make_shared<Software2552::KinectFaces>(&kinect);
	audio = std::make_shared<Software2552::KinectAudio>(&kinect);

	if (bodies && faces && audio) {
		audio->setup();
		faces->setup();
		bodies->useFaces(faces);
		bodies->useAudio(audio);
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	if (bodies) {
		bodies->update(comms);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	//faces.draw();
	//bodies.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
