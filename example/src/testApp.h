#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxKinectNuiPlayer.h"
#include "ofxKinectNuiRecorder.h"

// uncomment this to read from two kinects simultaneously
// #define USE_TWO_KINECTS


class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		void drawCalibratedTexture();
		void exit();
		void drawCircle3f(int n, int radius, ofVec3f cur);
		

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		void startRecording();
		void stopRecording();
		void startPlayback();
		void stopPlayback();

		ofxKinectNui kinect;

#ifdef USE_TWO_KINECTS
		ofxKinectNui kinect2;
#endif
		ofxKinectNuiPlayer kinectPlayer;
		ofxKinectNuiRecorder kinectRecorder;

		ofxBase3DVideo* kinectSource;

		ofTexture calibratedTexture;

		bool bRecord;
		bool bPlayback;
		bool bDrawCalibratedTexture;

		float nearClipping;
		float farClipping;
		int angle;
		
		int mRotationX, mRotationY;

};
