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

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	kinect.init(true, true, true, true); // enable all capture
//	kinect.init(true, true, false, false, true, NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_320x240); // default settings.
	kinect.open();
	
#ifdef USE_TWO_KINECTS
	// watch out that only the first kinect can grab label and skeleton.
	kinect2.init(true, true, false, false, true);
	kinect2.open();
#endif
	ofSetVerticalSync(true);

	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bRecord = false;
	bPlayback = false;

	bDrawCalibratedTexture = false;

	ofSetFrameRate(60);
	
	calibratedTexture.allocate(320, 240, GL_RGB);
}

//--------------------------------------------------------------
void testApp::update() {
	kinectSource->update();
	if(bRecord && kinectRecorder.isOpened()){
		kinectRecorder.newFrame(kinect.getSkeletonPoints(), kinect.getPixels(), kinect.getDepthPixelsRaw(), kinect.getCalibratedRGBPixels(), kinect.getLabelPixels());
	}
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(100, 100, 100);
	if(bDrawCalibratedTexture){
		ofPushMatrix();
		drawCalibratedTexture();
		ofPopMatrix();
	}else{
		if(!bPlayback){
			kinect.draw(20, 20, 400, 300);			// draw video images from kinect camera
			ofEnableAlphaBlending();
			kinect.drawDepth(20, 340, 400, 300);	// draw depth images from kinect depth sensor
			kinect.drawLabel(20, 340, 400, 300);		// draw players' label images on video images
			ofDisableAlphaBlending();
			kinect.drawSkeleton(20, 20, 400, 300);	// draw skeleton images on video images

#ifdef USE_TWO_KINECTS
			kinect2.draw(440, 20, 400, 300);
			kinect2.drawDepth(440, 340, 400, 300);
#endif
		}else{
			kinectPlayer.draw(20, 20, 400, 300);
			ofEnableAlphaBlending();
			kinectPlayer.drawDepth(20, 340, 400, 300);
			kinectPlayer.drawLabel(20, 340, 400, 300);
			ofDisableAlphaBlending();
			kinectPlayer.drawSkeleton(20, 20, 400, 300);
		}
	}

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

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << " (press: < >), fps: " << ofGetFrameRate() << endl
				 << "press c to close the stream and o to open it again, stream is: " << kinect.isOpened() << endl
				 << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 << "press r to record and q to playback, record is: " << bRecord << ", playback is: " << bPlayback;
	ofDrawBitmapString(reportStream.str(), 20, 652);

}

//--------------------------------------------------------------
void testApp::drawCalibratedTexture(){
	int offsetX = 0;
	int offsetY = 0;
	ofRotateY(50);
	calibratedTexture.loadData(kinect.getCalibratedRGBPixels(), 320, 240, GL_RGB);
	calibratedTexture.draw(offsetX, offsetY, 800, 600);
	for(int y = 0; y < 240; y+=2){
		for(int x = 0; x < 320; x+=2){
			float distance = kinect.getDistanceAt(x, y);
			if(distance > 500 && distance < 1500){
				glPushMatrix();
				float radius = (1600 - distance) / 100;
				radius = radius * radius;
				ofPoint p = ofPoint(x, y);
				ofSetColor(kinect.getCalibratedColorAt(p));
				glTranslatef(x * 2.5 + offsetX, y * 2.5 + offsetY, radius);
				ofCircle(0, 0, radius);
				glPopMatrix();
			}
		}
	}
}


//--------------------------------------------------------------
void testApp::exit() {
	if(calibratedTexture.bAllocated()){
		calibratedTexture.clear();
	}

	kinect.setAngle(0);
	kinect.close();
	kinectPlayer.close();
	kinectRecorder.close();


#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch(key){
	case 'q':
	case 'Q':
		bDrawCalibratedTexture = !bDrawCalibratedTexture;
		break;
	case 'o':
	case 'O':
		if(!kinect.isInited()){
			kinect.init();
		}
		if(kinect.isInited()){
			kinect.setAngle(angle);
			kinect.open();
		}
		break;
	case 'c':
	case 'C':
		if(kinect.isOpened()){
			kinect.setAngle(0);
			kinect.close();
		}
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
	mRotationY = x;
	mRotationX = y;
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
void testApp::startRecording(){
	if(!bRecord){
		// stop playback if running
		stopPlayback();

		kinectRecorder.init(ofToDataPath("recording.dat"), kinect.getVideoResolution(), kinect.getDepthResolution());
		bRecord = true;
	}
}

//--------------------------------------------------------------
void testApp::stopRecording(){
	if(bRecord){
		kinectRecorder.close();
		bRecord = false;
	}
}

//--------------------------------------------------------------
void testApp::startPlayback(){
	if(!bPlayback){
		stopRecording();
		kinect.close();

		// set record file and source
		kinectPlayer.setup(ofToDataPath("recording.dat"), true, true, true, true, kinect.getVideoResolution(), kinect.getDepthResolution());
		kinectPlayer.loop();
		kinectSource = &kinectPlayer;
		bPlayback = true;
	}
}

//--------------------------------------------------------------
void testApp::stopPlayback(){
	if(bPlayback){
		kinectPlayer.close();
		kinect.open();
		kinectSource = &kinect;
		bPlayback = false;
	}
}
