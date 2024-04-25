#include "ofApp.h"
#include "OscTester.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofxGlobalContext::Manager::defaultManager().createContext<OscTester>();
    $Context(OscTester)->setup();
    gui.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    ofxGlobalContext::Manager::defaultManager().update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(20, 20);
    ofSetColor(255, 255, 0);
    ofDrawBitmapString(ofToString(ofGetElapsedTimef()) + " ms", 0, 0);
    ofTranslate(50, 50);
    ofSetColor(255);
    for(int i = $Context(OscTester)->logs.size() - 1; i >= 0; i--)
    {
        auto& log = $Context(OscTester)->logs[i];
        if(i == $Context(OscTester)->logs.size() - 1)
        {
            ofSetColor(255, 180, 180);
        }
        else
        {
            ofSetColor(180, 180, 180);
        }
        ofTranslate(0, 20);
        ofDrawBitmapString(log, 0, 0);
    }
    ofPopStyle();
    ofPopMatrix();
    
    gui.begin();
    $Context(OscTester)->drawGui();
    gui.end();
    
    gui.draw();
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
