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

#include <queue>

#include "kinect/nui/Kinect.h"
#include "ofMain.h"
#include "ofxKinectNui.h"
#include "ofxBase3DVideo.h"
#include "FreeImage.h"

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

	void setup(ofxKinectNui& kinect, const string& filename);

	void update();
	void close();

	bool isActive();

private:
	ofFile videoFile;
	ofFile depthFile;
	FILE* f;
	int width, height;
	int depthWidth, depthHeight;
	float skeletons[kinect::nui::SkeletonFrame::SKELETON_COUNT][kinect::nui::SkeletonData::POSITION_COUNT][3];

	ofxKinectNui* mKinect;
	NUI_IMAGE_RESOLUTION mVideoResolution;
	NUI_IMAGE_RESOLUTION mDepthResolution;
};
#endif // OFX_KINECT_NUI_RECORDER_H
