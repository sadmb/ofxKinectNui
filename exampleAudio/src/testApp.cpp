/******************************************************************/
/**
 * @file	testApp.cpp
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#include "testApp.h"
#include "ofxKinectNuiDraw.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = false;
	initSetting.grabDepth = false;
	initSetting.grabAudio = true;
	initSetting.grabLabel = false;
	initSetting.grabSkeleton = false;
	initSetting.grabCalibratedVideo = false;
	initSetting.grabLabelCv = false;
	kinect.init(initSetting);
	kinect.open();

	angle = kinect.getCurrentAngle();
	bRecord = false;
	bPlayback = false;
	bPlugged = kinect.isConnected();
	bUnplugged = false;

	kinect.addKinectListener(this, &testApp::kinectPlugged, &testApp::kinectUnplugged);
	ofSetFrameRate(60);

	soundStream.listDevices();
	
	//if you want to set a different device id 
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	smoothedVol     = 0.0;
	scaledVol		= 0.0;

	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);

}

//--------------------------------------------------------------
void testApp::update() {
	kinect.update();
	if(bRecord){
		kinectSoundRecorder.update();
	}
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
	volHistory.push_back(scaledVol);

	if(volHistory.size() >= 400){
		volHistory.erase(volHistory.begin(), volHistory.begin() +1);
	}

	beamAngle = kinect.getAudioBeamAngle();
	sourceAngle = kinect.getAudioAngle();
	sourceConfidence = kinect.getAudioAngleConfidence();

}

//--------------------------------------------------------------
void testApp::draw() {
	ofColor magenta(245, 58, 135);
	ofBackground(100, 100, 100);

	ofPushMatrix();
	ofTranslate(35, 35);
	ofFill();
	if(bRecord) {
		ofSetColor(255, 0, 0);
		ofCircle(0, 0, 10);
	}
	if(bPlayback) {
		ofSetColor(0, 255, 0);
		ofTriangle(-10, -10, -10, 10, 10, 0);
	}
	ofPopMatrix();

	ofSetColor(225);
	ofDrawBitmapString("KINECT AUDIO EXAMPLE", 32, 32);
	
	ofNoFill();
	
	// draw the left channel:
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(32, 170, 0);
			
	ofSetColor(225);
	ofDrawBitmapString("Left Channel", 4, 18);
		
	ofSetLineWidth(1);	
	ofRect(0, 0, 512, 200);

	ofSetColor(magenta);
	ofSetLineWidth(3);
	ofBeginShape();
	for (int i = 0; i < left.size(); i++){
		ofVertex(i*2, 100 -left[i]*180.0f);
	}
	ofEndShape(false);
	ofPopMatrix();
	ofPopStyle();

	// draw the right channel:
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(32, 370, 0);
			
	ofSetColor(225);
	ofDrawBitmapString("Right Channel", 4, 18);
		
	ofSetLineWidth(1);	
	ofRect(0, 0, 512, 200);

	ofSetColor(magenta);
	ofSetLineWidth(3);
	ofBeginShape();
	for (int i = 0; i < right.size(); i++){
		ofVertex(i*2, 100 -right[i]*180.0f);
	}
	ofEndShape(false);
	ofPopMatrix();
	ofPopStyle();
	
	// draw the average volume:
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(565, 170, 0);
			
	ofSetColor(225);
	ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
	ofRect(0, 0, 400, 400);

	ofPushMatrix();
	ofTranslate(200, 200);
	ofSetColor(magenta);
	ofFill();
	ofCircle(0, 0, scaledVol * 190.0f);
	ofSetLineWidth(5);
	int size = 179.0f * (1.0f - sourceConfidence) + 1;
	ofEnableAlphaBlending();
	for(int i = 0; i < size; i++){
		if(size == 1){
			magenta.a = 255;
		}else{
			magenta.a = 255.0f * sin(PI * i / (float)size);
		}
		ofSetColor(magenta);
		ofLine(160.0f * sin(sourceAngle + (i - size / 2) * PI / 180.0f), 160.0f * cos(sourceAngle + (i - size / 2) * PI / 180.0f), 170.0f * sin(sourceAngle + (i - size / 2) * PI / 180.0f), 170.0f * cos(sourceAngle + (i - size / 2) * PI / 180.0f));
	}
	ofDisableAlphaBlending();
	ofSetLineWidth(1);
	ofTranslate(150.0f * sin(beamAngle), 150.0f * cos(beamAngle));
	ofRotate(- beamAngle * 180.0f / PI);
	ofSetColor(245, 135, 58);
	ofTriangle(-5, 0, 5, 0, 0, 10);
	ofPopMatrix();

	
	magenta.a = 1;
	ofSetColor(magenta);
	//lets draw the volume history as a graph
	ofBeginShape();
	for (int i = 0; i < volHistory.size(); i++){
		if( i == 0 ) ofVertex(i, 400);

		ofVertex(i, 400 - volHistory[i] * 70);
			
		if( i == volHistory.size() -1 ) ofVertex(i, 400);
	}
	ofEndShape(false);
			
	ofPopMatrix();
	ofPopStyle();
	
	stringstream kinectReport;
	if(bPlugged && !kinect.isOpened() && !bPlayback){
		ofSetColor(0, 255, 0);
		kinectReport << "Kinect is plugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}else if(!bPlugged){
		ofSetColor(255, 0, 0);
		kinectReport << "Kinect is unplugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << " (press: < >), fps: " << ofGetFrameRate() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl
				 << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 << "press 'r' to record and 'p' to playback, record is: " << bRecord << ", playback is: " << bPlayback << endl;
	ofDrawBitmapString(reportStream.str(), 20, 652);
	
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
}


//--------------------------------------------------------------
void testApp::exit() {
	stopPlayback();
	kinect.setAngle(0);
	kinect.close();
	kinect.removeKinectListener(this);
	kinectSoundRecorder.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch(key){
	case 'o':
	case 'O':
		kinect.open();
		break;
	case 'c':
	case 'C':
		kinect.close();
		break;
	case 'r':
	case 'R':
		if(!bRecord){
			startRecording();
		}else{
			stopRecording();
		}
		break;
	case 'p':
	case 'P':
		if(!bPlayback){
			startPlayback();
		}else{
			stopPlayback();
		}
		break;
	case OF_KEY_UP:
		angle++;
		if(angle > 27){
			angle = 27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_DOWN:
		angle--;
		if(angle < -27){
			angle = -27;
		}
		kinect.setAngle(angle);
		break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void testApp::kinectPlugged(){
	bPlugged = true;
}

//--------------------------------------------------------------
void testApp::kinectUnplugged(){
	bPlugged = false;
}

//--------------------------------------------------------------
void testApp::startRecording(){
	if(!bRecord){
		// stop playback if running
		stopPlayback();

		kinectSoundRecorder.setup(kinect, "recording.wav");
		bRecord = true;
	}
}

//--------------------------------------------------------------
void testApp::stopRecording(){
	if(bRecord){
		kinectSoundRecorder.close();
		bRecord = false;
	}
}

//--------------------------------------------------------------
void testApp::startPlayback(){
	if(!bPlayback){
		stopRecording();
		kinect.close();

		// set record file and source
		player.loadSound("recording.wav");
		player.setLoop(true);
		player.play();
		bPlayback = true;
	}
}

//--------------------------------------------------------------
void testApp::stopPlayback(){
	if(bPlayback){
		player.stop();
		player.unloadSound();
		kinect.open();
		bPlayback = false;
	}
}
