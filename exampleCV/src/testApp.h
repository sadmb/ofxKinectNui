/******************************************************************/
/**
 * @file	testApp.h
 * @brief	Example for ofxKinectNui addon
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#pragma once

#include "ofxKinectNui.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp {
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
		
		ofxKinectNui kinect;

		ofImage colorImage;
		ofxCvGrayscaleImage* labelImages;
		ofxCvContourFinder* contourFinders;

		ofxCvGrayscaleImage depthImage;
		ofxCvGrayscaleImage thresholdedImage;
		ofxCvContourFinder contourFinderDepth;

		ofxXmlSettings settings;

		int threshold;

		unsigned short nearClipping;
		unsigned short farClipping;
		int angle;
		
};
