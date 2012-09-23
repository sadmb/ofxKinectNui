#include "ofxKinectNuiPlayer.h"

//---------------------------------------------------------------------------
/**
	@brief	Constructor
*/
ofxKinectNuiPlayer::ofxKinectNuiPlayer(){
	f = NULL;
	filename = "";
	soundBuffer.clear();
	fps = 30;

	bPlay = false;
	bLoop = false;
	bVideo = false;
	bDepth = false;
	bLabel = false;
	bSkeleton = false;
	bAudio = false;
	bUsesTexture = false;
}

//---------------------------------------------------------------------------
/**
	@brief	Destructor
*/
ofxKinectNuiPlayer::~ofxKinectNuiPlayer() {
	close();

	if(videoTexture.isAllocated()){
		videoTexture.clear();
	}
	if(depthTexture.isAllocated()){
		depthTexture.clear();
	}
	if(labelTexture.isAllocated()){
		labelTexture.clear();
	}

	if(videoPixels.isAllocated()){
		videoPixels.clear();
	}
	if(distancePixels.isAllocated()){
		distancePixels.clear();
	}
	if(depthPixels.isAllocated()){
		depthPixels.clear();
	}
	if(labelPixels.isAllocated()){
		labelPixels.clear();
	}
	if(calibratedVideoPixels.isAllocated()){
		calibratedVideoPixels.clear();
	}

	if(!soundBuffer.empty()){
		soundBuffer.clear();
	}

	bPlay = false;
	bLoop = false;
	bVideo = false;
	bDepth = false;
	bCalibratedVideo = false;
	bLabel = false;
	bSkeleton = false;
	bAudio = false;
	bUsesTexture = false;
}

//---------------------------------------------------------------------------
/**
	@brief	Setup the player to read
	@param	file		filepath
	@param	useTexture	set false if you want to get pixels directly
*/
void ofxKinectNuiPlayer::setup(	const string & file, bool useTexture /*= true*/){
	bUsesTexture = useTexture;
	f = fopen(ofToDataPath(file).c_str(), "rb");
	filename = file;

	unsigned char dst = 0;
	fread(&dst, sizeof(char), 1, f);

	bVideo = (bool)((dst >> 5) & 0x1);
	bDepth = (bool)((dst >> 4) & 0x1);
	bCalibratedVideo = (bool)((dst >> 3) & 0x1);
	bLabel = (bool)((dst >> 2) & 0x1);
	bSkeleton = (bool)((dst >> 1) & 0x1);
	bAudio = (bool)((dst >> 0) & 0x1);
	

	NUI_IMAGE_RESOLUTION videoResolution;
	NUI_IMAGE_RESOLUTION depthResolution;

	fread(&videoResolution, sizeof(int), 1, f);
	fread(&depthResolution, sizeof(int), 1, f);
	
	fread(&nearClippingDistance, sizeof(unsigned short), 1, f);
	fread(&farClippingDistance, sizeof(unsigned short), 1, f);
	ofxBase3DVideo::initLookups(nearClippingDistance, farClippingDistance);

	switch(videoResolution){
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
		ofLog(OF_LOG_ERROR, "ofxKinectNuiPlayer: " + error);
		return;
	}
	switch(depthResolution){
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
		ofLog(OF_LOG_ERROR, "ofxKinectNuiPlayer: " + error);
		return;
	}
	
	if(bVideo){
		if(!videoPixels.isAllocated()){
			videoPixels.allocate(width, height, OF_PIXELS_RGB);
		}
//		memset(videoPixels.getPixels(), 0, width * height * 3);
	}
	if(bDepth){
		if(!distancePixels.isAllocated()){
			distancePixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
//		memset(distancePixels.getPixels(), 0, depthWidth * depthHeight);

		if(!depthPixels.isAllocated()){
			depthPixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
//		memset(depthPixels.getPixels(), 0, depthWidth * depthHeight);
	}
	if(bCalibratedVideo){
		if(!calibratedVideoPixels.isAllocated()){
			calibratedVideoPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGB);
		}
//		memset(calibratedVideoPixels.getPixels(), 0, depthWidth * depthHeight * 3);
	}
	if(bLabel){
		if(!labelPixels.isAllocated()){
			labelPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGBA);
		}
//		memset(labelPixels.getPixels(), 0, depthWidth * depthHeight * 4);
	}
	if(!videoTexture.isAllocated() && bUsesTexture && bVideo){
		videoTexture.allocate(width, height, GL_RGB);
	}
	if(!depthTexture.isAllocated() && bUsesTexture && bDepth){
		depthTexture.allocate(depthWidth, depthHeight, GL_LUMINANCE);
	}
	if(!labelTexture.isAllocated() && bUsesTexture && bLabel){
		labelTexture.allocate(depthWidth, depthHeight, GL_RGBA);
	}

	if(bSkeleton){
		for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
			skeletonPoints[i][0].z = -1;
		}
	}

	lastFrameTime = ofGetElapsedTimeMillis();

}

//---------------------------------------------------------------------------
/**
	@brief	Stop and close
*/
void ofxKinectNuiPlayer::close(){
	if(!f){
		return;
	}
	fclose(f);
	f = NULL;
	filename = "";

	bUsesTexture = false;
	bPlay = false;
}

//---------------------------------------------------------------------------
/**
	@brief	Play
*/
void ofxKinectNuiPlayer::play(){
	bPlay = true;
	lastFrameTime = ofGetElapsedTimeMillis();
}

//---------------------------------------------------------------------------
/**
	@brief	Stop
*/
void ofxKinectNuiPlayer::stop(){
	bPlay = false;
	f = fopen(ofToDataPath(filename).c_str(), "rb");
	unsigned char dst = 0;
	fread(&dst, sizeof(char), 1, f);
	int dst2 = 0;
	fread(&dst2, sizeof(int), 1, f);
	fread(&dst2, sizeof(int), 1, f);
	unsigned short bit3 = 0;
	fread(&bit3, sizeof(unsigned short), 1, f);
	fread(&bit3, sizeof(unsigned short), 1, f);
}

//---------------------------------------------------------------------------
/**
	@brief	Pause
*/
void ofxKinectNuiPlayer::pause(){
	bPlay = false;
}

//---------------------------------------------------------------------------
/**
	@brief	Update kinect player
*/
void ofxKinectNuiPlayer::update(){
	if(!f || !bPlay){
		return;
	}

	int currentFrameTime = ofGetElapsedTimeMillis();
	if((currentFrameTime-lastFrameTime)<(1000./fps)){
		return;
	}
	lastFrameTime = currentFrameTime;

	fread(&fps, sizeof(float), 1, f);
	if(bVideo){
		fread(videoPixels.getPixels(), sizeof(unsigned char), width * height * 3, f);
	}
	if(bDepth){
		fread(distancePixels.getPixels(), sizeof(unsigned short), depthWidth * depthHeight, f);
	}
	if(bCalibratedVideo){
		fread(calibratedVideoPixels.getPixels(), sizeof(unsigned char), depthWidth * depthHeight * 3, f);
	}
	if(bLabel){
		fread(labelPixels.getPixels(), sizeof(unsigned char), depthWidth * depthHeight * 4, f);
	}
	if(bSkeleton){
		int validCount;
		fread(&validCount, sizeof(int), 1, f);
		if(validCount > 0) {
			fread(skeletons, sizeof(float), validCount * kinect::nui::SkeletonData::POSITION_COUNT * 3, f);
			for(int i = 0; i < validCount; ++i) {
				for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
					skeletonPoints[i][j].x = skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][0];
					skeletonPoints[i][j].y = skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][1];
					skeletonPoints[i][j].z = skeletons[i][kinect::nui::SkeletonData::POSITION_COUNT + j][2];
				}
			}
		}
	}

	int n = depthWidth * depthHeight;
	if(bIsDepthNearValueWhite){
		for(int i = 0; i < n; i++){
			depthPixels[i] = depthPixelsLookupNearWhite[distancePixels[i]];
		}
	}else{
		for(int i = 0; i < n; i++){
			depthPixels[i] = depthPixelsLookupFarWhite[distancePixels[i]];
		}
	}

	if(bUsesTexture){
		if(bVideo){
			videoTexture.loadData(videoPixels);
		}
		if(bDepth){
			depthTexture.loadData(getDepthPixels());
		}
		if(bLabel){
			labelTexture.loadData(labelPixels);
		}
	}

	if(bAudio){
		fread(&audioBeamAngle, sizeof(float), 1, f);
		fread(&audioAngle, sizeof(float), 1, f);
		fread(&audioAngleConfidence, sizeof(float), 1, f);
	}
	bIsFrameNew = true;

	// loop?
	if(bLoop && std::feof(f) > 0) {
		f = fopen(ofToDataPath(filename).c_str(), "rb");
		unsigned char dst = 0;
		fread(&dst, sizeof(char), 1, f);
		int dst2 = 0;
		fread(&dst2, sizeof(int), 1, f);
		fread(&dst2, sizeof(int), 1, f);
		unsigned short bit3 = 0;
		fread(&bit3, sizeof(unsigned short), 1, f);
		fread(&bit3, sizeof(unsigned short), 1, f);
	}
	
}


//---------------------------------------------------------------------------
bool ofxKinectNuiPlayer::isFrameNew(){
	return bIsFrameNew;
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video player
	@param	x	X position of video player
	@param	x	X position of video player
	@param	w	Width of video player
	@param	h	Height of video player
*/
void ofxKinectNuiPlayer::draw(float x, float y, float w, float h){
	if(bUsesTexture && bVideo) {
		videoTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNuiPlayer: You should set UsesTexture and Video true");
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video player
	@param	x	X position of video player
	@param	x	X position of video player
	@param	w	Width of video player
	@param	h	Height of video player
*/
void ofxKinectNuiPlayer::draw(float x, float y){
	draw(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video player
	@param	x	X position of video player
	@param	x	X position of video player
	@param	w	Width of video player
	@param	h	Height of video player
*/
void ofxKinectNuiPlayer::draw(const ofPoint & point){
	draw(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video player
	@param	point	position of video player
	@param	w	Width of video player
	@param	h	Height of video player
*/
void ofxKinectNuiPlayer::draw(const ofPoint & point, float w, float h){
	draw(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video player
	@param	rect	Rectangle of video player area
*/
void ofxKinectNuiPlayer::draw(const ofRectangle & rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth player
	@param	x	X position of depth player
	@param	x	X position of depth player
	@param	w	Width of depth player
	@param	h	Height of depth player
*/
void ofxKinectNuiPlayer::drawDepth(float x, float y, float w, float h){
	if(bUsesTexture && bDepth) {
		depthTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNuiPlayer: You should set UsesTexture and Depth true");
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth player
	@param	x	X position of depth player
	@param	x	X position of depth player
*/
void ofxKinectNuiPlayer::drawDepth(float x, float y){
	depthTexture.draw(x, y, depthWidth, depthHeight);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth player
	@param	point	position of depth player
*/
void ofxKinectNuiPlayer::drawDepth(const ofPoint & point){
	drawDepth(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth player
	@param	point	position of depth player
	@param	w	Width of depth player
	@param	h	Height of depth player
*/
void ofxKinectNuiPlayer::drawDepth(const ofPoint & point, float w, float h){
	drawDepth(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth player
	@param	rect	Rectangle of depth player area
*/
void ofxKinectNuiPlayer::drawDepth(const ofRectangle & rect){
	drawDepth(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton player
	@param	x	X position of skeleton player
	@param	x	X position of skeleton player
	@param	w	Width of skeleton player
	@param	h	Height of skeleton player
*/
void ofxKinectNuiPlayer::drawSkeleton(float x, float y, float w, float h){
	if(bSkeleton){
		ofPushMatrix();
		ofTranslate(x, y);
		ofScale(1/(float)depthWidth * w, 1/(float)depthHeight * h);
		ofPushStyle();
		ofPolyline pLine;
		for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; i++){
			if(skeletonPoints[i][0].z < 0){
				continue;
			}
			ofSetColor(255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i), 255 * (int)pow(-1.0, i + 1));
			ofNoFill();
			ofSetLineWidth(4);
			// HEAD
			pLine.clear();
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SPINE].x, skeletonPoints[i][NUI_SKELETON_POSITION_SPINE].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HEAD].x, skeletonPoints[i][NUI_SKELETON_POSITION_HEAD].y);
			pLine.draw();
			
			// BODY_LEFT
			pLine.clear();
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_HAND_LEFT].y);
			pLine.draw();

			// BODY_RIGHT
			pLine.clear();
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_HAND_RIGHT].y);
			pLine.draw();
	
			// LEG_LEFT
			pLine.clear();
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_LEFT].x, skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_LEFT].y);
			pLine.draw();

			// LEG_RIGHT
			pLine.clear();
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].x, skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT].y);
			pLine.addVertex(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_RIGHT].x, skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_RIGHT].y);
			pLine.draw();
		
			ofSetColor(255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i));
			ofSetLineWidth(0);
			ofFill();
			for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; j++){
				ofCircle(skeletonPoints[i][j].x, skeletonPoints[i][j].y, 5);
			}
		}
		ofPopStyle();
		ofPopMatrix();
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton player
	@param	x	X position of skeleton player
	@param	x	X position of skeleton player
*/
void ofxKinectNuiPlayer::drawSkeleton(float x, float y){
	drawSkeleton(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton player
	@param	point	position of skeleton player
*/
void ofxKinectNuiPlayer::drawSkeleton(const ofPoint & point){
	drawSkeleton(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton player
	@param	x	X position of skeleton player
	@param	x	X position of skeleton player
	@param	w	Width of skeleton player
	@param	h	Height of skeleton player
*/
void ofxKinectNuiPlayer::drawSkeleton(const ofPoint & point, float w, float h){
	drawSkeleton(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton player
	@param	x	X position of skeleton player
	@param	x	X position of skeleton player
	@param	w	Width of skeleton player
	@param	h	Height of skeleton player
*/
void ofxKinectNuiPlayer::drawSkeleton(const ofRectangle & rect){
	drawSkeleton(rect.x, rect.y, rect.width, rect.height);
}


//---------------------------------------------------------------------------
/**
	@brief	Draw label player
	@param	x	X position of label player
	@param	x	X position of label player
	@param	w	Width of label player
	@param	h	Height of label player
*/
void ofxKinectNuiPlayer::drawLabel(float x, float y, float w, float h){
	if(bUsesTexture && bLabel) {
		labelTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNuiPlayer: You should set UsesTexture and Label true");
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label player
	@param	x	X position of label player
	@param	x	X position of label player
*/
void ofxKinectNuiPlayer::drawLabel(float x, float y){
	drawLabel(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label player
	@param	point position of label player
*/
void ofxKinectNuiPlayer::drawLabel(const ofPoint & point){
	drawLabel(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label player
	@param	point position of label player
	@param	w	Width of label player
	@param	h	Height of label player
*/
void ofxKinectNuiPlayer::drawLabel(const ofPoint & point, float w, float h){
	drawLabel(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label player
	@param	rect	Rectangle of label player area
*/
void ofxKinectNuiPlayer::drawLabel(const ofRectangle & rect){
	drawLabel(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw audio direction view
	@param	x	X position of drawing audio direction view
	@param	x	X position of drawing audio direction view
*/
void ofxKinectNuiPlayer::drawAudioDirection(float x, float y) {
	// TODO
}

//---------------------------------------------------------------------------
/**
	@brief	Draw audio direction view
	@param	point	position of drawing audio direction view
*/
void ofxKinectNuiPlayer::drawAudioDirection(const ofPoint & point) {
	drawAudioDirection(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get pixels of video
	@return	Video pixels
*/
ofPixels& ofxKinectNuiPlayer::getVideoPixels(){
	return videoPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get pixels of depth
	@return	Depth pixels
*/
ofPixels& ofxKinectNuiPlayer::getDepthPixels(){
	return depthPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get pixels of label
	@return	Label pixels
*/
ofPixels& ofxKinectNuiPlayer::getLabelPixels(){
	return labelPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get pixels of calibrated video
	@return	Calibrated video pixels
*/
ofPixels& ofxKinectNuiPlayer::getCalibratedVideoPixels(){
	return calibratedVideoPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get pixels of distance
	@return	Distance pixels
*/
ofShortPixels& ofxKinectNuiPlayer::getDistancePixels(){
	return distancePixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get sound buffer
	@return	Sound buffer
*/
std::vector<BYTE> ofxKinectNuiPlayer::getSoundBuffer(){
	return soundBuffer;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the reference of video texture
	@return	Video texture
*/
ofTexture& ofxKinectNuiPlayer::getVideoTextureReference(){
	return videoTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the reference of depth texture
	@return	Depth texture
*/
ofTexture & ofxKinectNuiPlayer::getDepthTextureReference(){
	return depthTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the reference of label texture
	@return	Label texture
*/
ofTexture & ofxKinectNuiPlayer::getLabelTextureReference(){
	return labelTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Get calibrated video texture
	@return	Calibrated video texture
*/
ofTexture & ofxKinectNuiPlayer::getCalibratedVideoTextureReference(){
	return labelTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Set if use texture
	@param	bUse	true when use texture
*/
void ofxKinectNuiPlayer::setUsesTexture(bool bUse){
	bUsesTexture = bUse;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the width of video
	@return	Width
*/
float ofxKinectNuiPlayer::getWidth(){
	return (float)width;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the height of video
	@return	Height
*/
float ofxKinectNuiPlayer::getHeight(){
	return (float)height;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the width of depth
	@return	Width
*/
float ofxKinectNuiPlayer::getDepthWidth(){
	return (float)depthWidth;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the height of depth
	@return	Height
*/
float ofxKinectNuiPlayer::getDepthHeight(){
	return (float)depthHeight;
}

//---------------------------------------------------------------------------
/**
	@brief	Get distance at the point
	@param	x
	@param	y
	@return	distance at (x, y)
*/
float ofxKinectNuiPlayer::getDistanceAt(int x, int y) {
	return distancePixels[y * depthWidth + x];
}

//---------------------------------------------------------------------------
/**
	@brief	Get distance at the point
	@param	p
	@return	distance at the point
*/
float ofxKinectNuiPlayer::getDistanceAt(ofPoint p) {
	return getDistanceAt((int)p.x, (int)p.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get world coordinate for the point
	@param	x
	@param	y
	@return	World vector
*/
ofVec3f ofxKinectNuiPlayer::getWorldCoordinateFor(int depthX, int depthY) {
	const double depthZ = distancePixels[depthWidth * depthY + depthX]/1000.0;
	return ofxBase3DVideo::getWorldCoordinateFor(depthX, depthY, depthZ);
}

//---------------------------------------------------------------------------
/**
	@brief	Get world coordinate for the point
	@param	p
	@return	World vector
*/
ofVec3f ofxKinectNuiPlayer::getWorldCoordinateFor(ofPoint p) {
	return getWorldCoordinateFor((int)p.x, (int)p.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get audio beam angle
	@return	Audio beam angle
*/
float ofxKinectNuiPlayer::getAudioBeamAngle() {
	return audioBeamAngle;
}

//---------------------------------------------------------------------------
/**
	@brief	Get estimate audio angle
	@return	Estimate Audio angle
*/
float ofxKinectNuiPlayer::getAudioAngle() {
	return audioAngle;
}

//---------------------------------------------------------------------------
/**
	@brief	Get the confidence of estimate audio angle
	@return	Confidence of estimate audio angle
*/
float ofxKinectNuiPlayer::getAudioAngleConfidence() {
	return audioAngleConfidence;
}


//---------------------------------------------------------------------------
/**
	@brief	Calculates the scaled point of skeleton joint
	@param	skeletonPoint
	@param	width
	@param	width
	@return	Scaled skeleton point
*/
ofPoint ofxKinectNuiPlayer::calcScaledSkeletonPoint(const ofPoint& skeletonPoint, float width, float height){
	float px = ofMap(skeletonPoint.x, 0, depthWidth, 0, width);
	float py = ofMap(skeletonPoint.y+15, 0, depthHeight, 0, height); // +15 - the skeletons are a little high
	return ofPoint(px, py);
}

