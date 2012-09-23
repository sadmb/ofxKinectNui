#include "ofxKinectNuiRecorder.h"

//---------------------------------------------------------------------------
/**
	@brief	Constructor
*/
ofxKinectNuiRecorder::ofxKinectNuiRecorder() {
	f=NULL;
}

//---------------------------------------------------------------------------
/**
	@brief	Destructor
*/
ofxKinectNuiRecorder::~ofxKinectNuiRecorder() {
	close();
}

//---------------------------------------------------------------------------
/**
	@brief	Setup recorder
	@param	kinect		which kinect to record
	@param	filename	filename to output data
*/
void ofxKinectNuiRecorder::setup(ofxKinectNui& kinect, const string & filename){
	mKinect = &kinect;
	mVideoResolution = mKinect->getVideoResolution();
	mDepthResolution = mKinect->getDepthResolution();
	switch(mVideoResolution){
	case NUI_IMAGE_RESOLUTION_1280x960:
		width = 1280;
		height = 960;
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
	case NUI_IMAGE_RESOLUTION_1280x960:
	default:
		string error = "Invalid depth resolution: select 640x480, 320x240 or 80x60.";
		ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
		return;
	}

	f = fopen(ofToDataPath(filename).c_str(),"wb");

//	skeletons = new float[kinect::nui::SkeletonFrame::SKELETON_COUNT * kinect::nui::SkeletonData::POSITION_COUNT * 3];

	unsigned char bit = ((unsigned int)mKinect->grabsVideo() << 5) | ((unsigned int)mKinect->grabsDepth() << 4) |  ((unsigned int)mKinect->grabsCalibratedVideo() << 3) | ((unsigned int)mKinect->grabsLabel() << 2) | ((unsigned int)mKinect->grabsSkeleton() << 1) | ((unsigned int)mKinect->grabsAudio());
	fwrite(&bit, sizeof(char), 1, f);
	int bit2 = (int)mKinect->getVideoResolution();
	fwrite(&bit2, sizeof(int), 1, f);
	bit2 = (int)mKinect->getDepthResolution();
	fwrite(&bit2, sizeof(int), 1, f);
	unsigned short bit3 = (int)mKinect->getNearClippingDistance();
	fwrite(&bit3, sizeof(unsigned short), 1, f);
	bit3 = mKinect->getFarClippingDistance();
	fwrite(&bit3, sizeof(unsigned short), 1, f);
}

//---------------------------------------------------------------------------
/**
	@brief	Close the recorder
*/
void ofxKinectNuiRecorder::close(){
	if(!f){
		return;
	}

	fclose(f);
	f = NULL;
}

//---------------------------------------------------------------------------
/**
	@brief	Update recording
*/
void ofxKinectNuiRecorder::update() {
	if(!f){
		return;
	}

	float fps = ofGetFrameRate();
	fwrite(&fps, sizeof(float), 1, f);

	unsigned char* videoPixels = mKinect->getVideoPixels().getPixels();
	if( videoPixels != NULL) {
		fwrite(videoPixels,  sizeof(char), width * height * 3, f);
	}

	unsigned short* distancePixels = mKinect->getDistancePixels().getPixels();
	if(distancePixels != NULL){
		fwrite(distancePixels, sizeof(short), depthWidth * depthHeight, f);
	}

	unsigned char* calibratedVideoPixels = mKinect->getCalibratedVideoPixels().getPixels();
	if(calibratedVideoPixels != NULL){
		fwrite(calibratedVideoPixels, sizeof(char), depthWidth * depthHeight * 3, f);
	}

	unsigned char* labelPixels = mKinect->getLabelPixels().getPixels(); 
	if(labelPixels != NULL){
		fwrite(labelPixels, sizeof(char), depthWidth * depthHeight * 4, f);
	}

	ofPoint* skeletonPoints[kinect::nui::SkeletonFrame::SKELETON_COUNT];
	int validCount = mKinect->getSkeletonPoints(skeletonPoints);

	if(mKinect->grabsSkeleton()){
		for(int i = 0; i < validCount; i++){
			for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
				skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][0] = skeletonPoints[i][j].x;
				skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][1] = skeletonPoints[i][j].y;
				skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][2] = skeletonPoints[i][j].z;
			}
		}
		fwrite(&validCount, sizeof(int), 1, f);
		if(validCount > 0) {
			fwrite(skeletons, sizeof(float), validCount * kinect::nui::SkeletonData::POSITION_COUNT * 3, f);
		}
	}

	if(mKinect->grabsAudio()){
		float temp1 = mKinect->getAudioBeamAngle();
		fwrite(&temp1, sizeof(float), 1, f);
		float temp2 = mKinect->getAudioAngle();
		fwrite(&temp2, sizeof(float), 1, f);
		float temp3 = mKinect->getAudioAngleConfidence();
		fwrite(&temp3, sizeof(float), 1, f);
	}
}

//---------------------------------------------------------------------------
/**
	@brief		Is recording?
	@return		true if is recording 
*/
bool ofxKinectNuiRecorder::isActive(){
	return f;
}
