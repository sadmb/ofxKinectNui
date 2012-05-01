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
	
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = false;
	initSetting.grabDepth = true;
	initSetting.grabAudio = false;
	initSetting.grabLabel = true;
	initSetting.grabSkeleton = false;
	initSetting.grabCalibratedVideo = false;
	initSetting.grabLabelCv = true;
	kinect.init(initSetting);
//	kinect.init(false, true, false, true, false, false, true, false);	/// only use depth capturing and separated label capturing
	kinect.open();

	ofSetVerticalSync(true);

	angle = kinect.getCurrentAngle();
	
	depthImage.allocate(320, 240);
	thresholdedImage.allocate(320, 240);
	threshold = 20;
	
	colorImage.allocate(320, 240, OF_IMAGE_COLOR_ALPHA);
	labelImages = new ofxCvGrayscaleImage[ofxKinectNui::KINECT_PLAYERS_INDEX_NUM];
	for(int i = 0; i < ofxKinectNui::KINECT_PLAYERS_INDEX_NUM; i++){
		labelImages[i].allocate(320, 240);
	}
	contourFinders = new ofxCvContourFinder[ofxKinectNui::KINECT_PLAYERS_INDEX_NUM - 1]; /// we get 7 players in maximum.
	
	settings.loadFile("settings.xml");
	nearClipping = settings.getValue("KINECT:CLIPPING:NEAR", kinect.getNearClippingDistance());
	farClipping = settings.getValue("KINECT:CLIPPING:FAR", kinect.getFarClippingDistance());
	kinect.setFarClippingDistance(farClipping);
	kinect.setNearClippingDistance(nearClipping);

	ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::update() {
	kinect.update();
	if(kinect.isOpened()){
		depthImage.setFromPixels(kinect.getDepthPixels());
		thresholdedImage = depthImage;
		thresholdedImage.threshold(threshold);
		contourFinderDepth.findContours(thresholdedImage, 20, (320*240)/3, 10, true);

		////// You can skip copying to thresholdedImage if you don't need depth draw
		// depthImage.threshold(threshold);
		// contourFinderDepth.findContours(depthImage, 20, (320*240)/3, 10, true);
		
		colorImage.setFromPixels(kinect.getLabelPixels());
		for(int i = 0; i < ofxKinectNui::KINECT_PLAYERS_INDEX_NUM; i++){
			labelImages[i].setFromPixels(kinect.getLabelPixelsCv(i));
			if(i > 0){
				contourFinders[i - 1].findContours(labelImages[i], 20, (320*240)/3, 10, true);
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	ofBackground(100, 100, 100);
	depthImage.draw(20, 20);		/// normal depth images
	thresholdedImage.draw(360, 20);	/// thresholded depth images

	ofSetColor(0);
	ofFill();
	ofRect(700, 20, 320, 240);
	ofRect(700, 280, 320, 240);
	ofRect(20, 280, 320, 240);
	ofSetColor(255);

	ofEnableAlphaBlending();
	colorImage.draw(20, 280);		/// normal label images
	ofDisableAlphaBlending();
	labelImages[0].draw(360, 280);	/// whole players' silhouette

	// contours from depth images
	for(int i = 0; i < contourFinderDepth.nBlobs; i++){
		contourFinderDepth.blobs[i].draw(700, 20);
	}

	// contours from label images
	for(int i = 0; i < ofxKinectNui::KINECT_PLAYERS_INDEX_NUM - 1; i++){
		for(int j = 0; j < contourFinders[i].nBlobs; j++){
			contourFinders[i].blobs[j].draw(700, 280);
		}
	}

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
	reportStream << " (press: < >), fps: " << ofGetFrameRate() << endl
				 << "press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << endl
				 << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				 << "press LEFT and RIGHT to change the far clipping distance: " << farClipping << " mm" << endl
				 << "press '+' and '-' to change the near clipping distance: " << nearClipping << " mm" << endl
				 << "press 's' to save the near/ far clipping distance to xml file." << endl;
	ofDrawBitmapString(reportStream.str(), 20, 652);
	
}


//--------------------------------------------------------------
void testApp::exit() {
	kinect.setAngle(0);
	kinect.close();
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
	case OF_KEY_LEFT:
		if(farClipping > nearClipping + 10){
			farClipping -= 10;
			kinect.setFarClippingDistance(farClipping);
		}
		break;
	case OF_KEY_RIGHT:
		if(farClipping < 4000){
			farClipping += 10;
			kinect.setFarClippingDistance(farClipping);
		}
		break;
	case '-':
		if(nearClipping >= 10){
			nearClipping -= 10;
			kinect.setNearClippingDistance(nearClipping);
		}
		break;
	case '+':
		if(nearClipping < farClipping - 10){
			nearClipping += 10;
			kinect.setNearClippingDistance(nearClipping);
		}
		break;
	case 's':
		settings.setValue("KINECT:CLIPPING:NEAR", nearClipping);
		settings.setValue("KINECT:CLIPPING:FAR", farClipping);
		settings.saveFile("settings.xml");
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

