#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	shared_ptr<Software2552::Router>router = std::make_shared<Software2552::Router>();
	if (!router) {
		return; //things would be really messed up...
	}
	router->setup();
	
	kinect.setup(router);

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
		bodies->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	//ofScale(ofGetWidth(), ofGetWidth());
	ofTranslate(0, 0); // kinect draws top left 
	kinect.kinect.draw();
	ofPushMatrix();
	ofTranslate(0, kinectHeightForDepth);
	kinect.kinect.face.draw();
	ofPopMatrix();

	if (kinect.imageir.isAllocated()) {
		ofSetColor(ofColor::white);
		ofPushMatrix();
		ofTranslate(0, 0); // kinect draws top left 
		ofScale(0.5f, 0.5f);
		kinect.imageir.draw(0, 0);
		ofPopMatrix();
	}
	if (kinect.imagebi.isAllocated()) {
		ofSetColor(ofColor::white);
		ofPushMatrix();
		ofTranslate(kinectWidthForDepth, 0); // kinect draws top left 
		ofScale(0.5f, 0.5f);
		kinect.imagebi.draw(0, 0);
		ofPopMatrix();
	}
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


