/******************************************************************/
/**
 * @file	ofxKinectNuiRecorder.h
 * @brief	Recorder for Kinect Official Sensor
 * @note
 * @todo
 * @bug	
 * @reference	ofxKinectRecorder.h created by arturo 03/01/2011
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_RECORDER_H
#define OFX_KINECT_NUI_RECORDER_H

#include "kinect\nui\Kinect.h"
#include "ofMain.h"

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
/****************************************/
/**
 * @class	ofxKinectNuiRecorder
 * @brief	Recorder for a kinect official device
 * @note	
 * @author	sadmb
 * @date		Oct. 26, 2011
 */
/****************************************/
class ofxKinectNuiRecorder {
public:
	ofxKinectNuiRecorder();
	virtual ~ofxKinectNuiRecorder();

	void init(	const string & filename,
				NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480,
				NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);

	void newFrame(	ofPoint** skeletonPoints = NULL,
					unsigned char* videoPixels = NULL,
					unsigned short* depthPixelsRaw = NULL,
					unsigned char* calibratedRGBPixels = NULL,
					unsigned char* labelPixels = NULL);
	void close();

	bool isOpened();

private:
	FILE* f;
	int width, height;
	int depthWidth, depthHeight;
	float* skeletons;
	NUI_IMAGE_RESOLUTION mVideoResolution;
	NUI_IMAGE_RESOLUTION mDepthResolution;
};
#endif // OFX_KINECT_NUI_RECORDER_H
