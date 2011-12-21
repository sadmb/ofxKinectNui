/******************************************************************/
/**
 * @file	ofxKinectNui.cpp
 * @brief	Kinect Official Sensor wrapper for oF
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#include "ofxKinectNui.h"

const UINT color[8] = {	0x00FFFFFF,
						0xFF0000FF,
						0xFF00FF00,
						0xFFFF0000,
						0xFF00FFFF,
						0xFFFF00FF,
						0xFFFFFF00,
						0xFF6600FF};

//---------------------------------------------------------------------------
ofxKinectNui::ofxKinectNui(){

	ofLog(OF_LOG_VERBOSE, "ofxKinect: Creating ofxKinect");
	
	videoPixels = NULL;
	depthPixelsRaw = NULL;
	labelPixels = NULL;
	calibratedRGBPixels = NULL;
	skeletonPoints = NULL;
	bIsInited = false;
	bIsOpened = false;

	addKinectListener(this, &ofxKinectNui::pluggedFunc, &ofxKinectNui::unpluggedFunc);
}

//---------------------------------------------------------------------------
ofxKinectNui::~ofxKinectNui(){
	close();

	if(videoTexture.bAllocated()){
		videoTexture.clear();
	}
	if(depthTexture.bAllocated()){
		depthTexture.clear();
	}
	if(labelTexture.bAllocated()){
		labelTexture.clear();
	}
	if(videoPixels != NULL){
		delete[] videoPixels;
		videoPixels = NULL;
	}
	if(depthPixelsRaw != NULL){
		delete[] depthPixelsRaw;
		depthPixelsRaw = NULL;
	}
	if(labelPixels != NULL){
		delete[] labelPixels;
		labelPixels = NULL;
	}
	if(calibratedRGBPixels != NULL){
		delete[] calibratedRGBPixels;
		calibratedRGBPixels = NULL;
	}

	calibration.clear();

	if(skeletonPoints != NULL){
		delete[] skeletonPoints[0];
		skeletonPoints[0] = NULL;
		delete[] skeletonPoints;
		skeletonPoints = NULL;
	}

	removeKinectListener(this);

	bIsInited = false;
	bIsOpened = false;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::init(bool grabVideo /*= true*/,
						bool grabDepth /*= true*/,
						bool grabLabel /*= false*/,
						bool grabSkeleton /*= false*/,
						bool useTexture /*= true*/,
						NUI_IMAGE_RESOLUTION videoResolution /*= NUI_IMAGE_RESOLUTION_640x480*/,
						NUI_IMAGE_RESOLUTION depthResolution /*=NUI_IMAGE_RESOLUTION_320x240*/){

	bGrabsVideo = grabVideo;
	bGrabsDepth = grabDepth;
	bGrabsLabel = grabLabel;
	bGrabsSkeleton = grabSkeleton;
	bUsesTexture = useTexture;
	mVideoResolution = videoResolution;
	mDepthResolution = depthResolution;

	bIsInited = true;
	
	if(!kinect.IsConnected()){
		string error = "Check the kinect connection.";
		ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
		return false;
	}

	if(isOpened()){
		string error = "Do not call init() while stream is opened.";
		ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
		return false;
	}

	if(grabLabel){
		grabDepth = true; // grabDepth when grabLabel
	}
	// set video resolution
	switch(videoResolution){
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
		if(grabVideo){
			string error = "Invalid video resolution: select 1280x1024 or 640x480.";
			ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
			return false;
		}else{
			width = 640;
			height = 480;
			videoResolution = NUI_IMAGE_RESOLUTION_640x480;
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Video resolution has changed to 640x480 automatically.");
			break;
		}
	}
	
	// set depth resolution
	switch(depthResolution){
	case NUI_IMAGE_RESOLUTION_640x480:
		if(grabLabel){
			string error = "Invalid depth resolution: select 320x240, 80x60 or you must disable grabLabel when you select 640x480.";
			ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
			return false;
		}else{
			depthWidth = 640;
			depthHeight = 480;
		}
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
		if(grabDepth){
			string error = "Invalid depth resolution: select 320x240, 80x60 or you must disable grabLabel when you select 640x480.";
			ofLog(OF_LOG_ERROR, "ofxKinectNui: " + error);
			return false;
		}else{
			depthWidth = 320;
			depthHeight = 240;
			depthResolution = NUI_IMAGE_RESOLUTION_320x240;
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Depth resolution has changed to 320x240 automatically.");
			break;
		}
	}

	calibration.init(mVideoResolution, mDepthResolution);

	DWORD dwFlags = 0x00000000;
	if(bGrabsVideo){
		dwFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;
		if(bUsesTexture){
			videoTexture.allocate(width, height, GL_RGB);
		}

		int length = width * height;
		if(videoPixels == NULL){
			videoPixels = new unsigned char[length * 3];
		}
		pixels.setFromExternalPixels(videoPixels, width, height, OF_IMAGE_COLOR);

		memset(videoPixels, 0, length * 3 * sizeof(unsigned char));
	}
	if(bGrabsDepth){
		int length = depthWidth * depthHeight;
		if(depthPixelsRaw == NULL){
			depthPixelsRaw = new unsigned short[length];
		}
		memset(depthPixelsRaw, 0, length * sizeof(unsigned short));

		if(bGrabsVideo){
			if(calibratedRGBPixels == NULL){
				calibratedRGBPixels = new unsigned char[length * 3];
			}
			memset(calibratedRGBPixels, 0, length * 3 * sizeof(unsigned char));
		}

		if(bGrabsLabel){
			if(labelPixels == NULL){
				labelPixels = new unsigned char[length * 4];
			}
			memset(labelPixels, 0, length * 4 * sizeof(unsigned char));

			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
			if(bUsesTexture){
				depthTexture.allocate(depthWidth, depthHeight, GL_LUMINANCE);
				labelTexture.allocate(depthWidth, depthHeight, GL_RGBA);
			}
		}else{
			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
			if(bUsesTexture){
				depthTexture.allocate(depthWidth, depthHeight, GL_LUMINANCE);
			}
		}
	}
	if(bGrabsSkeleton){
		if(skeletonPoints == NULL){
			skeletonPoints = new ofPoint*[kinect::nui::SkeletonFrame::SKELETON_COUNT];
			skeletonPoints[0] = new ofPoint[kinect::nui::SkeletonFrame::SKELETON_COUNT * kinect::nui::SkeletonData::POSITION_COUNT];
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; i++){
				skeletonPoints[i] = skeletonPoints[0] + i * kinect::nui::SkeletonData::POSITION_COUNT;
			}
		}
		dwFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
	}

    kinect.Initialize(dwFlags);

	if(!kinect.IsInited()){
		ofLog(OF_LOG_ERROR, "ofxKinectNui: Initialization failed.");
	}
	
	return kinect.IsInited();
}

//---------------------------------------------------------------------------
bool ofxKinectNui::open(){
	if(kinect.IsConnected() && !isOpened()){
		if(isInited()){
			if(!kinect.IsInited()){
				init(bGrabsVideo, bGrabsDepth, bGrabsLabel, bGrabsSkeleton, bUsesTexture, mVideoResolution, mDepthResolution);
			}
		}
		if(bGrabsVideo){
			kinect.VideoStream().Open(NUI_IMAGE_TYPE_COLOR, mVideoResolution);
		}

		if(bGrabsDepth){
			if(bGrabsLabel){
				kinect.DepthStream().Open(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, mDepthResolution);
			}else{
				kinect.DepthStream().Open(NUI_IMAGE_TYPE_DEPTH, mDepthResolution);
			}
		}

		bIsOpened = true;

		return true;
	}
	ofLog(OF_LOG_ERROR, "ofxKinectNui: Cannot open stream.");
	return false;
}

//---------------------------------------------------------------------------
void ofxKinectNui::close(){
	if(isOpened()){
		kinect.Close();

		bIsOpened = false;
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::update(){

	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}

	// wait and update all data
	kinect.WaitAndUpdateAll();
		
	if(bGrabsVideo){
		// Get the video data of next frame
		kinect::nui::VideoFrame video(kinect.VideoStream());
		if( video.Pitch() == 0 ) {
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Buffer length of received image data is bogus");
			return;
		}
		unsigned int videobit;
		for(int x = 0; x < video.Width(); x++){
			for(int y = 0; y < video.Height(); y++){
				// windows native color set: ABGR to RGBA
				videobit = video(x, y) & 0x00FFFFFF;
				videobit = (videobit & 0x00FF0000) >> 16 | (videobit & 0x0000FF00) | (videobit & 0x000000FF) << 16; 
				memcpy(videoPixels + (video.Width() * y + x) * 3, &videobit, sizeof(char) * 3);
			}
		}
				
		pixels.setFromExternalPixels(videoPixels, width, height, OF_IMAGE_COLOR);
	}

	if(bGrabsDepth){
		// Get the depth data of next frame
		kinect::nui::DepthFrame depth(kinect.DepthStream());
		unsigned short depthbit;
		unsigned short playerLabel;
		for(int x = 0; x < depth.Width(); x++){
			for(int y = 0; y < depth.Height(); y++){
				if(bGrabsLabel){
					depthbit = depth(x, y) >> 3;
					playerLabel = depth(x, y) & 0x7;
					memcpy(labelPixels + (depth.Width() * y + x) * 4, &color[playerLabel], sizeof(char) * 4);
				}else{
					depthbit = depth(depth.Width() - x, y);
				}
				memcpy(depthPixelsRaw + (depth.Width() * y + x), &depthbit, sizeof(short));
				if(bGrabsVideo){
					int depthIndex = depth.Width() * y + x;
					long vindex = kinect.GetColorPixelCoordinatesFromDepthPixel(depth.Width() * y + x, 0) * 3;
					for(int i = 0; i < 3; i++){
						unsigned char vbit;
						if(vindex + i < 0 || vindex + i > width * height * 3){
							vbit = 0;
						}else{
							vbit = videoPixels[vindex + i];
						}
						memcpy(calibratedRGBPixels + (depth.Width() * y + x) * 3 + i, &vbit, sizeof(char));
					}
				}
			}
		}
		calibration.update(depthPixelsRaw);
	}

	if(bUsesTexture){
		if(bGrabsVideo){
			videoTexture.loadData(videoPixels, width, height, GL_RGB);
		}
		if(bGrabsDepth){
			depthTexture.loadData(getDepthPixels(), depthWidth, depthHeight, GL_LUMINANCE);
			if(bGrabsLabel){
				labelTexture.loadData(labelPixels, depthWidth, depthHeight, GL_RGBA);
			}
		}
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::pluggedFunc(){
}

//---------------------------------------------------------------------------
void ofxKinectNui::unpluggedFunc(){
	bIsOpened = false;
	std::cout << "outoutout" << std::endl;
}

//---------------------------------------------------------------------------
void ofxKinectNui::draw(float x, float y, float w, float h){
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}

	if(bUsesTexture && bGrabsVideo){
		videoTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set UsesTexture and GrabsVideo true");
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::draw(float x, float y){
	draw(x, y, width, height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::draw(const ofPoint& point){
	draw(point.x, point.y);
}

//---------------------------------------------------------------------------
void ofxKinectNui::draw(const ofPoint& point, float w, float h){
	draw(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
void ofxKinectNui::draw(const ofRectangle& rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(float x, float y, float w, float h){
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}

	if(bUsesTexture && bGrabsDepth){
		depthTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set UsesTexture and GrabsDepth true");
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(float x, float y){
	draw(x, y, depthWidth, depthHeight);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(const ofPoint& point){
	draw(point.x, point.y);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(const ofPoint& point, float w, float h){
	draw(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(const ofRectangle& rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(float x, float y, float w, float h){
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}

	if(bGrabsSkeleton){
		// Get the skeleton data of next frame
		kinect::nui::SkeletonFrame skeleton = kinect.Skeleton().GetNextFrame();
		if(skeleton.IsFoundSkeleton()){
			skeleton.TransformSmooth();
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; i++){
				if( skeleton[i].TrackingState() == NUI_SKELETON_TRACKED){
					for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; j++){
						kinect::nui::SkeletonData::Point p = skeleton[i].TransformSkeletonToDepthImage(j);
						skeletonPoints[i][j] = ofPoint(p.x, p.y, p.depth);
					}

					// push skeleton points data into vector in order to get them
					ofPushMatrix();
					ofTranslate(x, y + 15); // skeleton is a bit too high
					ofPushStyle();
					ofSetColor(255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i), 255 * (int)pow(-1.0, i + 1));
					ofNoFill();
					ofSetLineWidth(4);
					// HEAD
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SPINE], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SPINE], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HEAD], w, h));
		
					// BODY_LEFT
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_LEFT], w, h));

					// BODY_RIGHT
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_RIGHT], w, h));
		
					// LEG_LEFT
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_LEFT], w, h));

					// LEG_RIGHT
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT], w, h));
					ofLine(calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT], w, h), calcScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_RIGHT], w, h));
		
					ofSetColor(255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i));
					ofSetLineWidth(0);
					ofFill();
					for(int k = 0; k < kinect::nui::SkeletonData::POSITION_COUNT; k++){
						ofCircle(calcScaledSkeletonPoint(skeletonPoints[i][k], w, h), 5);
					}
					ofPopStyle();
					ofPopMatrix();

				}else{
					for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; j++){
						skeletonPoints[i][j] = ofPoint(-1, -1, -1);
					}
				}
			}
		}else{
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; i++){
				for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; j++){
					skeletonPoints[i][j] = ofPoint(-1, -1, -1);
				}
			}
		}
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set GrabsSkeleton true");
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(float x, float y){
	drawSkeleton(x, y, width, height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(const ofPoint& point){
	drawSkeleton(point.x, point.y);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(const ofPoint& point, float w, float h){
	drawSkeleton(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(const ofRectangle& rect){
	drawSkeleton(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(float x, float y, float w, float h){
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}

	if(bUsesTexture && bGrabsLabel){
		labelTexture.draw(x, y, w, h);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set UsesTexture and GrabsLabel true");
	}
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(float x, float y){
	drawLabel(x, y, width, height);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(const ofPoint& point){
	drawLabel(point.x, point.y);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(const ofPoint& point, float w, float h){
	drawLabel(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(const ofRectangle& rect){
	drawLabel(rect.x, rect.y, rect.width, rect.height);
}

void ofxKinectNui::setAngle(int angleInDegrees){
	if(kinect.IsConnected()){
		targetAngle = angleInDegrees;
		kinect.SetAngle(targetAngle);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: setAngle() is denied. Check Kinect connection.");
	}
}

int ofxKinectNui::getCurrentAngle(){
	if(kinect.IsConnected()){
		return (int)kinect.GetAngle();
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getAngle() is denied. Check Kinect connection.");
		return 0;
	}
}

int ofxKinectNui::getTargetAngle(){
	return targetAngle;
}

//---------------------------------------------------------------------------
unsigned char* ofxKinectNui::getPixels(){
	return videoPixels;
}

//---------------------------------------------------------------------------
unsigned char* ofxKinectNui::getDepthPixels(){
	return calibration.getDepthPixels();
}

//---------------------------------------------------------------------------
unsigned char* ofxKinectNui::getLabelPixels(){
	return labelPixels;
}

//---------------------------------------------------------------------------
unsigned short* ofxKinectNui::getDepthPixelsRaw(){
	return depthPixelsRaw;
}

//---------------------------------------------------------------------------
unsigned char* ofxKinectNui::getCalibratedRGBPixels(){
	return calibratedRGBPixels;
}

//---------------------------------------------------------------------------
float* ofxKinectNui::getDistancePixels(){
	return calibration.getDistancePixels();
}

//---------------------------------------------------------------------------
ofPixels& ofxKinectNui::getPixelsRef(){
	return pixels;
}

//---------------------------------------------------------------------------
ofTexture& ofxKinectNui::getTextureReference(){
	if(!videoTexture.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getTextureReference - video texture is not allocated");
	}
	return videoTexture;
}

//---------------------------------------------------------------------------
ofTexture& ofxKinectNui::getDepthTextureReference(){
	if(!depthTexture.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getDepthTextureReference - depth texture is not allocated");
	}
	return depthTexture;
}

//---------------------------------------------------------------------------
ofTexture& ofxKinectNui::getLabelTextureReference(){
	if(!labelTexture.bAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getLabelTextureReference - label texture is not allocated");
	}
	return labelTexture;
}

//---------------------------------------------------------------------------
ofPoint** ofxKinectNui::getSkeletonPoints(){
	if(!bGrabsSkeleton){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getSkeletonPoints - skeleton is not grabbed.");
	}
	return skeletonPoints;
}

//---------------------------------------------------------------------------
ofColor ofxKinectNui::getColorAt(int x, int y){
	ofColor c;
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Kinect stream is not opened");
		return c;
	}

	if(bGrabsVideo){
		int index = (y * width + x) * 3;
		if(index < 0 || index > width * height * 3){
			return c;
		}
		c.r = videoPixels[index + 0];
		c.g = videoPixels[index + 1];
		c.b = videoPixels[index + 2];
		c.a = 255;
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set GrabsVideo true");
	}
	return c;
}

//---------------------------------------------------------------------------
ofColor ofxKinectNui::getColorAt(const ofPoint& point){
	return getColorAt(point.x, point.y);
}

//---------------------------------------------------------------------------
ofColor ofxKinectNui::getCalibratedColorAt(int depthX, int depthY){
	ofColor c;
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Kinect stream is not opened");
		return c;
	}

	if(bGrabsVideo && bGrabsDepth){
		int index = (depthY * depthWidth + depthX) * 3;
		if(index < 0 || index > depthWidth * depthHeight * 3){
			return c;
		}
		c.r = calibratedRGBPixels[index + 0];
		c.g = calibratedRGBPixels[index + 1];
		c.b = calibratedRGBPixels[index + 2];
		c.a = 255;
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set GrabsVideo and GrabsDepth true");
	}
	return c;
}

//---------------------------------------------------------------------------
ofColor ofxKinectNui::getCalibratedColorAt(const ofPoint & depthPoint){
	return getCalibratedColorAt(depthPoint.x, depthPoint.y);
}

//---------------------------------------------------------------------------
ofVec3f ofxKinectNui::getWorldCoordinateFor(int depthX, int depthY) {
	return calibration.getWorldCoordinateFor(depthX, depthY);
}

//---------------------------------------------------------------------------
float ofxKinectNui::getDistanceAt(int depthX, int depthY){
	return calibration.getDistanceAt(depthX, depthY);
}

//---------------------------------------------------------------------------
float ofxKinectNui::getDistanceAt(const ofPoint& depthPoint){
	return calibration.getDistanceAt(depthPoint);
}

//---------------------------------------------------------------------------
int ofxKinectNui::getPlayerIndexAt(int x, int y) {
	return labelPixels[depthWidth * y + x];
}

//---------------------------------------------------------------------------
int ofxKinectNui::getPlayerIndexAt(const ofPoint& point){
	return getPlayerIndexAt(point.x, point.y);
}

//---------------------------------------------------------------------------
bool ofxKinectNui::isFrameNew(){
	return true;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::isInited(){
	return bIsInited;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::isConnected(){
	return kinect.IsConnected();
}

//---------------------------------------------------------------------------
bool ofxKinectNui::isOpened(){
	return bIsOpened;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::grabsVideo(){
	return bGrabsVideo;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::grabsDepth(){
	return bGrabsDepth;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::grabsSkeleton(){
	return bGrabsSkeleton;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::grabsLabel(){
	return bGrabsLabel;
}

//---------------------------------------------------------------------------
void ofxKinectNui::setUsesTexture(bool bUse){
	bUsesTexture = bUse;
}

//---------------------------------------------------------------------------
NUI_IMAGE_RESOLUTION ofxKinectNui::getVideoResolution(){
	return mVideoResolution;
}

//---------------------------------------------------------------------------
NUI_IMAGE_RESOLUTION ofxKinectNui::getDepthResolution(){
	return mDepthResolution;
}

//---------------------------------------------------------------------------
ofVec2f ofxKinectNui::getVideoSize(){
	DWORD x = 0;
	DWORD y = 0;
	::NuiImageResolutionToSize(mVideoResolution, x, y);
	ofVec2f resolution = ofVec2f((float)x, (float)y);
	return resolution;
}

//---------------------------------------------------------------------------
ofVec2f ofxKinectNui::getDepthSize(){
	DWORD x = 0;
	DWORD y = 0;
	::NuiImageResolutionToSize(mDepthResolution, x, y);
	ofVec2f resolution = ofVec2f((float)x, (float)y);
	return resolution;
}

//---------------------------------------------------------------------------
void ofxKinectNui::enableDepthNearValueWhite(bool bEnabled){
	calibration.enableDepthNearValueWhite(bEnabled);
}

//---------------------------------------------------------------------------
bool ofxKinectNui::isDepthNearValueWhite(){
	return calibration.isDepthNearValueWhite();
}

//---------------------------------------------------------------------------
void ofxKinectNui::setClipping(float nearClippingInMillimeters, float farClippingInMillimeters){
	calibration.setClippingInMilimeters(nearClippingInMillimeters, farClippingInMillimeters);
}

//---------------------------------------------------------------------------
float ofxKinectNui::getNearClipping(){
	return ofxKinectNuiCalibration::getNearClipping();
}

//---------------------------------------------------------------------------
float ofxKinectNui::getFarClipping(){
	return ofxKinectNuiCalibration::getFarClipping();
}

//---------------------------------------------------------------------------
ofxKinectNuiCalibration& ofxKinectNui::getCalibration(){
	return calibration;
}

//---------------------------------------------------------------------------
ofPoint ofxKinectNui::calcScaledSkeletonPoint(const ofPoint& skeletonPoint, float width, float height){
	float px = skeletonPoint.x * width;
	float py = skeletonPoint.y * height;
	return ofPoint(px, py);
}

