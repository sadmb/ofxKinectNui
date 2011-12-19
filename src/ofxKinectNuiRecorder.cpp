/******************************************************************/
/**
 * @file	ofxKinectNuiRecorder.cpp
 * @brief	Recorder for Kinect Official Sensor
 * @note
 * @todo
 * @bug	
 * @reference	ofxKinectRecorder.cpp created by arturo 03/01/2011
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#include "ofxKinectNuiRecorder.h"

//---------------------------------------------------------------------------
ofxKinectNuiRecorder::ofxKinectNuiRecorder() {
	f=NULL;
}

//---------------------------------------------------------------------------
ofxKinectNuiRecorder::~ofxKinectNuiRecorder() {
	close();
}

//---------------------------------------------------------------------------
void ofxKinectNuiRecorder::setup(ofxKinectNui& kinect, const string & filename){
	mKinect = &kinect;
	mVideoResolution = mKinect->getVideoResolution();
	mDepthResolution = mKinect->getDepthResolution();
	switch(mVideoResolution){
	case NUI_IMAGE_RESOLUTION_1280x1024:
		width = 1280;
		height = 1024;
		break;
	case NUI_IMAGE_RESOLUTION_640x480:
		width = 640;
		height = 480;
		break;
	case NUI_IMAGE_RESOLUTION_320x240:
	case NUI_IMAGE_RESOLUTION_80x60:
	default:
		string error = "Invalid video resolution: select 1280x1024 or 640x480.";
		ofLog(OF_LOG_ERROR, "ofxKinectNuiCalibration: " + error);
		return;
	}
	switch(mDepthResolution){
	case NUI_IMAGE_RESOLUTION_640x480:
		depthWidth = 640;
		depthHeight = 480;
		break;
	case NUI_IMAGE_RESOLUTION_320x240:
		depthWidth = 320;
		depthHeight = 240;
		break;
	case NUI_IMAGE_RESOLUTION_80x60:
		depthWidth = 80;
		depthHeight = 60;
		break;
	case NUI_IMAGE_RESOLUTION_1280x1024:
	default:
		string error = "Invalid depth resolution: select 320x240, 80x60 or you must disable grabLabel when you select 640x480.";
		ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
		return;
	}

	f = fopen(ofToDataPath(filename).c_str(),"wb");

	skeletons = new float[kinect::nui::SkeletonFrame::SKELETON_COUNT * kinect::nui::SkeletonData::POSITION_COUNT * 3];

	unsigned char bit = ((unsigned int)mKinect->grabsVideo() << 3) | ((unsigned int)mKinect->grabsDepth() << 2) | ((unsigned int)mKinect->grabsLabel() << 1) | ((unsigned int)mKinect->grabsSkeleton());
	fwrite(&bit, sizeof(char), 1, f);
	int bit2 = (int)mKinect->getVideoResolution();
	fwrite(&bit2, sizeof(int), 1, f);
	bit2 = (int)mKinect->getDepthResolution();
	fwrite(&bit2, sizeof(int), 1, f);

	float fps = ofGetFrameRate();
	fwrite(&fps, sizeof(float), 1, f);
}

//---------------------------------------------------------------------------
void ofxKinectNuiRecorder::close(){
	if(!f){
		return;
	}

	fclose(f);
	f = NULL;

	if(skeletons != NULL){
		delete[] skeletons;
		skeletons = NULL;
	}

}

//---------------------------------------------------------------------------
void ofxKinectNuiRecorder::update() {
	if(!f){
		return;
	}

	float fps = ofGetFrameRate();
	fwrite(&fps, sizeof(float), 1, f);

	unsigned char* videoPixels = mKinect->getPixels();
	if( videoPixels != NULL) {
		fwrite(videoPixels,  sizeof(char), width * height * 3, f);
	}

	unsigned short* depthPixelsRaw = mKinect->getDepthPixelsRaw();
	if(depthPixelsRaw != NULL){
		fwrite(depthPixelsRaw, sizeof(short), depthWidth * depthHeight, f);
	}

	unsigned char* calibratedRGBPixels = mKinect->getCalibratedRGBPixels();
	if(calibratedRGBPixels != NULL){
		fwrite(calibratedRGBPixels, sizeof(char), depthWidth * depthHeight * 3, f);
	}

	unsigned char* labelPixels = mKinect->getLabelPixels();
	if(labelPixels != NULL){
		fwrite(labelPixels, sizeof(char), depthWidth * depthHeight * 4, f);
	}
	
	ofPoint** skeletonPoints = mKinect->getSkeletonPoints();
	if(skeletonPoints != NULL){
		int ptr = 0;
		for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; i++){
			for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; j++){
				skeletons[(i * kinect::nui::SkeletonData::POSITION_COUNT + j) * 3] = skeletonPoints[i][j].x;
				skeletons[(i * kinect::nui::SkeletonData::POSITION_COUNT + j) * 3 + 1] = skeletonPoints[i][j].y;
				skeletons[(i * kinect::nui::SkeletonData::POSITION_COUNT + j) * 3 + 2] = skeletonPoints[i][j].z;
			}
		}
		fwrite(skeletons, sizeof(float), kinect::nui::SkeletonFrame::SKELETON_COUNT * kinect::nui::SkeletonData::POSITION_COUNT * 3, f);
	}
}

bool ofxKinectNuiRecorder::isActive(){
	return f;
}
