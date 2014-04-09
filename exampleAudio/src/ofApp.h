/******************************************************************/
/**
 * @file	ofApp.h
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo	If you want to use audio stream of Kinect, you should #define USES_KINECT_AUDIOSTREAM on ofxKinectNui.h
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#pragma once

#include "ofMain.h"
#include "ofxKinectNui.h"
#include "ofxKinectNuiSoundRecorder.h"


class ofxKinectNuiDrawTexture;

class ofApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();

		void exit();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void kinectPlugged();
		void kinectUnplugged();

		void audioIn(float* input, int vbufferSize, int nChannels);
		
		void startRecording();
		void stopRecording();
		void startPlayback();
		void stopPlayback();

	private:
		ofxKinectNui kinect;
		ofxKinectNuiSoundRecorder kinectSoundRecorder;
		ofSoundPlayer player;

		bool bRecord;
		bool bPlayback;
		bool bDrawCalibratedTexture;
		bool bPlugged;
		bool bUnplugged;
		
		int angle;

		vector<float> left;
		vector<float> right;
		vector<float> volHistory;

		float smoothedVol;
		float scaledVol;

		float beamAngle;
		float sourceAngle;
		float sourceConfidence;

		ofSoundStream soundStream;

};
