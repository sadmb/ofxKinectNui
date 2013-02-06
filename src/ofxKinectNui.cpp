#include "ofxKinectNui.h"
#include "ofxKinectNuiDraw.h"

#define VIDEO_BPP_RGB			3
#define VIDEO_BPP_INFRARED		1

const UINT color[ofxKinectNui::KINECT_PLAYERS_INDEX_NUM] = {
	0x00FFFFFF,	/// no user
	0xFF0000FF,
	0xFF00FF00,
	0xFFFF0000,
	0xFF00FFFF,
	0xFFFF00FF,
	0xFFFFFF00,
	0xFF6600FF
};

//---------------------------------------------------------------------------
ofxKinectNui::ofxKinectNui(){

	ofLog(OF_LOG_VERBOSE, "ofxKinect: Creating ofxKinect");
	
	bIsInited = false;
	bIsOpened = false;
	bIsNearmode = false;
	bGrabsVideo = false;
	bGrabsDepth = false;
	bGrabsLabel = false;
	bGrabsSkeleton = false;
	bGrabsCalibratedVideo = false;
	bIsFrameNew = false;

	bIsFoundSkeleton = false;
	updateFlagDefault_ = UPDATE_FLAG_NONE;

	mVideoBpp = VIDEO_BPP_RGB;

	videoDraw_ = NULL;
	depthDraw_ = NULL;
	labelDraw_ = NULL;
	skeletonDraw_ = NULL;

	addKinectListener(this, &ofxKinectNui::pluggedFunc, &ofxKinectNui::unpluggedFunc);
}

//---------------------------------------------------------------------------
ofxKinectNui::~ofxKinectNui(){
	close();

	// clear pixels data
	if(videoPixels.isAllocated()){
		videoPixels.clear();
	}
	if(depthPixels.isAllocated()){
		depthPixels.clear();
	}
	if(distancePixels.isAllocated()){
		distancePixels.clear();
	}
	if(labelPixels.isAllocated()){
		labelPixels.clear();
	}
	if(calibratedVideoPixels.isAllocated()){
		calibratedVideoPixels.clear();
	}
	for(int i = 0; i < ofxKinectNui::KINECT_PLAYERS_INDEX_NUM; ++i) {
		if(labelPixelsCv[i].isAllocated()) {
			labelPixelsCv[i].clear();
		}
	}

	removeKinectListener(this);

	bIsInited = false;
	bIsOpened = false;
	bIsNearmode = false;
	bGrabsVideo = false;
	bGrabsDepth = false;
	bGrabsLabel = false;
	bGrabsSkeleton = false;
	bGrabsCalibratedVideo = false;
	bIsFrameNew = false;
	bIsFoundSkeleton = false;
}

//---------------------------------------------------------------------------
bool ofxKinectNui::init(){
	InitSetting setting;
	return init(setting);
}
bool ofxKinectNui::init(const InitSetting& setting){
	return init(setting.grabVideo,
				setting.grabDepth,
				setting.grabAudio,
				setting.grabLabel,
				setting.grabSkeleton,
				setting.grabCalibratedVideo,
				setting.grabLabelCv,
				setting.videoImageType,
				setting.videoResolution,
				setting.depthResolution);
}




/**
	@brief	initialize kinect sensor
	@param	grabVideo			set false to disable video capture
	@param	grabDepth			set false to disable depth capture
	@param	grabAudio			set true to enable audio capture
	@param	grabLabel			set true to enable label capture, only the first kinect sensor can capture.
	@param	grabSkeleton		set true to enable skeleton capture, only the first kinect sensor can capture. 
	@param	grabCalibratedVideo	set true to enable calibratedVideo capture
	@param	grabLabelCv			set true to enable separated label capture for cv use, only the first kinect sensor can capture.
	@param	videoResolution		default is 640x480
	@param	depthResolution		default is 320x240
	*/
bool ofxKinectNui::init(bool grabVideo /*= true*/,
						bool grabDepth /*= true*/,
						bool grabAudio /*= false*/,
						bool grabLabel /*= false*/,
						bool grabSkeleton /*= false*/,
						bool grabCalibratedVideo /*= false*/,
						bool grabLabelCv /*= false*/,
						NUI_IMAGE_TYPE videoImageType /*= NUI_IMAGE_TYPE_COLOR*/,
						NUI_IMAGE_RESOLUTION videoResolution /*= NUI_IMAGE_RESOLUTION_640x480*/,
						NUI_IMAGE_RESOLUTION depthResolution /*=NUI_IMAGE_RESOLUTION_320x240*/){

	if(grabLabel){
		if(!grabDepth){
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Please set grabDepth TRUE when grab label.");
			grabDepth = true; // grabDepth when grabLabel
		}
	}
	if(grabCalibratedVideo){
		if(!grabDepth || !grabVideo){
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Please set grabVideo and grabDepth TRUE when grab calibrated video.");
			grabDepth = true;
			grabVideo = true;
		}
	}
	bGrabsVideo = grabVideo;
	bGrabsDepth = grabDepth;
	bGrabsAudio = grabAudio;
	bGrabsLabel = grabLabel;
	bGrabsSkeleton = grabSkeleton;
	bGrabsCalibratedVideo = grabCalibratedVideo;
	bGrabsLabelCv = grabLabelCv;
	mVideoImageType = videoImageType;
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

	// set video resolution
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

	DWORD dwFlags = 0x00000000;
	updateFlagDefault_ = UPDATE_FLAG_NONE;
#ifdef NUI_IMAGE_TYPE_COLOR_INFRARED
	if (mVideoImageType == NUI_IMAGE_TYPE_COLOR_INFRARED)
		mVideoBpp = VIDEO_BPP_INFRARED;
#endif

	if(bGrabsVideo){
		int length = width * height;
		if(!videoPixels.isAllocated()){
			ofPixelFormat format = OF_PIXELS_RGB;
#ifdef NUI_IMAGE_TYPE_COLOR_INFRARED
			if (mVideoImageType == NUI_IMAGE_TYPE_COLOR_INFRARED)
				format = OF_PIXELS_MONO;
#endif
			videoPixels.allocate(width, height, format);
		}
		updateFlagDefault_ |= UPDATE_FLAG_VIDEO;

		dwFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;
	}
	if(bGrabsDepth){
		int length = depthWidth * depthHeight;
		if(!depthPixels.isAllocated()){
			depthPixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
		updateFlagDefault_ |= UPDATE_FLAG_DEPTH;
		if(!distancePixels.isAllocated()){
			distancePixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
		updateFlagDefault_ |= UPDATE_FLAG_DISTANCE;

		if(bGrabsCalibratedVideo){
			if(!calibratedVideoPixels.isAllocated()){
				ofPixelFormat format = OF_PIXELS_RGB;
#ifdef NUI_IMAGE_TYPE_COLOR_INFRARED
				if (mVideoImageType == NUI_IMAGE_TYPE_COLOR_INFRARED)
					format = OF_PIXELS_MONO;
#endif
				calibratedVideoPixels.allocate(depthWidth, depthHeight, format);
			}
			updateFlagDefault_ |= UPDATE_FLAG_CALIBRATED_VIDEO;
		}

		if(bGrabsLabel || bGrabsLabelCv){
			if(bGrabsLabel){
				if(!labelPixels.isAllocated()){
					labelPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGBA);
				}
				updateFlagDefault_ |= UPDATE_FLAG_LABEL;
			}

			if(bGrabsLabelCv){
				for(int i = 0; i < KINECT_PLAYERS_INDEX_NUM; ++i){
					if(!labelPixelsCv[i].isAllocated()){
						labelPixelsCv[i].allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
					}
				}
				updateFlagDefault_ |= UPDATE_FLAG_LABEL_CV;
			}

			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
		}else{
			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
		}
	}
	if(bGrabsAudio){
		updateFlagDefault_ |= UPDATE_FLAG_AUDIO;
		dwFlags |= NUI_INITIALIZE_FLAG_USES_AUDIO;
	}
	if(bGrabsSkeleton){
		updateFlagDefault_ |= UPDATE_FLAG_SKELETON;
		dwFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;

		for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i) {
			// z==-1 is a condition of not tracked.
			skeletonPoints[i][0].z = -1;
			rawSkeletonPoints[i][0].z = -1;
		}
	}

    kinect.Initialize(dwFlags);

	if(!kinect.IsInited()){
		ofLog(OF_LOG_ERROR, "ofxKinectNui: Initialization failed.");
	}
	
	return kinect.IsInited();
}

//---------------------------------------------------------------------------
/**
	@brief	open stream
*/
bool ofxKinectNui::open(bool nearmode /*= false */){
	if(kinect.IsConnected() && !isOpened()){
		bIsNearmode = nearmode;
		if(isInited()){
			if(!kinect.IsInited()){
				init(bGrabsVideo, bGrabsDepth, bGrabsLabel, bGrabsAudio, bGrabsLabel, bGrabsSkeleton, bGrabsLabelCv, mVideoImageType, mVideoResolution, mDepthResolution);
			}
		}
		if(bGrabsVideo){
			kinect.VideoStream().Open(mVideoImageType, mVideoResolution);
		}
		if(bGrabsDepth){
			if(bGrabsLabel || bGrabsLabelCv){
				kinect.DepthStream().Open(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, mDepthResolution, nearmode);
			}else{
				kinect.DepthStream().Open(NUI_IMAGE_TYPE_DEPTH, mDepthResolution, nearmode);
			}
		}
		if(bGrabsAudio){
			kinect.AudioStream().Open();
		}

		bIsOpened = true;
		return true;
	}
	ofLog(OF_LOG_ERROR, "ofxKinectNui: Cannot open stream.");
	return false;
}

void ofxKinectNui::setVideoDrawer(IDrawPixels* drawer){
	videoDraw_ = drawer;
}
void ofxKinectNui::setDepthDrawer(IDrawPixels* drawer){
	depthDraw_ = drawer;
}
void ofxKinectNui::setLabelDrawer(IDrawPixels* drawer){
	labelDraw_ = drawer;
}

void ofxKinectNui::setSkeletonDrawer(IDrawPoints* drawer){
	skeletonDraw_ = drawer;
}

//---------------------------------------------------------------------------
/**
	@brief	close stream
*/
void ofxKinectNui::close(){
	if(isOpened()){
		kinect.Shutdown();
		bIsOpened = false;
		bIsFrameNew = false;
	}
}

//---------------------------------------------------------------------------
/**
	@brief	update stream data.
*/
void ofxKinectNui::update(UINT flag){
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		return;
	}
	flag &= updateFlagDefault_;

	// wait and update data
	UINT kinectUpdateFlag = kinect::nui::Kinect::UPDATE_FLAG_NONE;
	if(flag & UPDATE_FLAG_GROUP_VIDEO) {
		kinectUpdateFlag |= kinect::nui::Kinect::UPDATE_FLAG_VIDEO;
	}
	if(flag & UPDATE_FLAG_GROUP_DEPTH) {
		kinectUpdateFlag |= kinect::nui::Kinect::UPDATE_FLAG_DEPTH;
	}
	if(flag & UPDATE_FLAG_GROUP_AUDIO) {
		kinectUpdateFlag |= kinect::nui::Kinect::UPDATE_FLAG_AUDIO;
	}
	if(flag & UPDATE_FLAG_GROUP_SKELETON) {
		kinectUpdateFlag |= kinect::nui::Kinect::UPDATE_FLAG_SKELETON;
	}
	
	kinect.WaitAndUpdate(kinectUpdateFlag);
	
	if(flag & UPDATE_FLAG_GROUP_VIDEO){
		// Get the video data of next frame
		kinect::nui::VideoFrame video(kinect.VideoStream());
		if( video.Pitch() == 0 ) {
			ofLog(OF_LOG_WARNING, "ofxKinectNui: Buffer length of received image data is bogus");
			return;
		}
		unsigned int videobit;
		int w = video.Width();
		int h = video.Height();
		for(int y = 0; y < h; ++y){
			int offset = y*w;
			for(int x = 0; x < w; ++x){
				// windows native color set: ABGR to RGBA
				videobit = video(x, y) & 0x00FFFFFF;
				videobit = (videobit & 0x00FF0000) >> 16 | (videobit & 0x0000FF00) | (videobit & 0x000000FF) << 16; 
				memcpy(videoPixels.getPixels() + (offset + x) * mVideoBpp, &videobit, sizeof(char) * mVideoBpp);
			}
		}
		if(videoDraw_ && (flag & UPDATE_FLAG_VIDEO)) {
			videoDraw_->setSource(videoPixels);
		}
	}

	if(flag & UPDATE_FLAG_GROUP_DEPTH){
		// Get the depth data of next frame
		kinect::nui::DepthFrame depth(kinect.DepthStream());
		unsigned short depthbit;
		unsigned short playerLabel;
		int w = depth.Width();
		int h = depth.Height();
		for(int y = 0; y < h; ++y){
			int offset = y*w;
			for(int x = 0; x < w; ++x){
				int depthIndex = offset + x;
				depthbit = depth(x, y) >> 3;
				playerLabel = depth(x, y) & 0x7;
				if(flag & UPDATE_FLAG_LABEL){
					memcpy(labelPixels.getPixels() + depthIndex * 4, &color[playerLabel], sizeof(char) * 4);
				}
				if(flag & UPDATE_FLAG_LABEL_CV){
					for(int i = 0; i < KINECT_PLAYERS_INDEX_NUM; ++i){
						unsigned char lb;
						if((i == 0 && playerLabel > 0) || (i > 0 && playerLabel == i)){
							lb = 0xFF;
							memcpy(labelPixelsCv[i].getPixels() + depthIndex, &lb, sizeof(char));
						}else{
							lb = 0x00;
							memcpy(labelPixelsCv[i].getPixels() + depthIndex, &lb, sizeof(char));
						}
					}
				}
				if(flag & UPDATE_FLAG_DISTANCE) {
					memcpy(distancePixels.getPixels() + depthIndex, &depthbit, sizeof(short));
				}
				if(flag & UPDATE_FLAG_DEPTH) {
					if(bIsDepthNearValueWhite){
						depthPixels[depthIndex] = depthPixelsLookupNearWhite[depthbit];
					}else{
						depthPixels[depthIndex] = depthPixelsLookupFarWhite[depthbit];
					}
				}
				if(flag & UPDATE_FLAG_CALIBRATED_VIDEO) {
					long vindex = kinect.GetColorPixelCoordinatesFromDepthPixel(depthIndex, 0) * mVideoBpp;
					for(int i = 0; i < mVideoBpp; ++i){
						unsigned char vbit;
						if(vindex + i < 0 || vindex + i > width * height * mVideoBpp){
							vbit = 0;
						}else{
							vbit = videoPixels[vindex + i];
						}
						memcpy(calibratedVideoPixels.getPixels() + depthIndex * mVideoBpp + i, &vbit, sizeof(char));
					}
				}
			}
		}
		if(depthDraw_ && (flag & UPDATE_FLAG_DEPTH)) {
			depthDraw_->setSource(depthPixels);
		}
		if(labelDraw_ && (flag & UPDATE_FLAG_LABEL)) {
			labelDraw_->setSource(labelPixels);
		}
	}
	if(flag & UPDATE_FLAG_GROUP_SKELETON){
		// Get the skeleton data of next frame
		kinect::nui::SkeletonFrame skeleton = kinect.Skeleton().GetNextFrame();
		if(skeleton.IsFoundSkeleton()){
			bIsFoundSkeleton = true;
			skeleton.TransformSmooth();
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
				if( skeleton[i].TrackingState() == NUI_SKELETON_TRACKED){
					for(int j = 0; j < kinect::nui::SkeletonData::POSITION_COUNT; ++j){
						kinect::nui::SkeletonData::SkeletonPoint p = skeleton[i].TransformSkeletonToDepthImage(j, mDepthResolution);
						skeletonPoints[i][j] = ofPoint(p.x, p.y, p.depth);
						rawSkeletonPoints[i][j] = ofPoint(skeleton[i][j].x, skeleton[i][j].y, skeleton[i][j].z);            
					}
				}else{
					// if skeleton is not tracked, set top z data negative.
					skeletonPoints[i][0].z = -1;
					rawSkeletonPoints[i][0].z = -1;
					continue;
				}
			}
		}
		else {
			bIsFoundSkeleton = false;
			for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
				// if skeleton is not tracked, set top z data negative.
				skeletonPoints[i][0].z = -1;
				rawSkeletonPoints[i][0].z = -1;
			}
		}
	}else{
		bIsFoundSkeleton = false;
	}

	if(flag & UPDATE_FLAG_GROUP_AUDIO){
		soundBuffer = kinect.AudioStream().Read();
		audioBeamAngle = (float)kinect.AudioStream().GetAudioBeamAngle();
		audioAngle = (float)kinect.AudioStream().GetAudioAngle();
		audioAngleConfidence = (float)kinect.AudioStream().GetAudioAngleConfidence();
	}

	bIsFrameNew = true;
}

//---------------------------------------------------------------------------
/**
	@brief	listener function when kinect is plugged.
*/
void ofxKinectNui::pluggedFunc(){
}

//---------------------------------------------------------------------------
/**
	@brief	listener function when kinect is unplugged.
*/
void ofxKinectNui::unpluggedFunc(){
	close();
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawVideo(int x, int y, int width, int height)
{
	videoDraw_->setDrawArea(x,y,width,height);
	drawVideo();
}

void ofxKinectNui::drawVideo(){
	videoDraw_->draw();
}
//---------------------------------------------------------------------------
void ofxKinectNui::drawDepth(int x, int y, int width, int height)
{
	depthDraw_->setDrawArea(x,y,width,height);
	drawDepth();
}
void ofxKinectNui::drawDepth(){
	depthDraw_->draw();
}
//---------------------------------------------------------------------------
void ofxKinectNui::drawLabel(int x, int y, int width, int height)
{
	labelDraw_->setDrawArea(x,y,width,height);
	drawLabel();
}
void ofxKinectNui::drawLabel(){
	labelDraw_->draw();
}

//---------------------------------------------------------------------------
void ofxKinectNui::drawSkeleton(){
	for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i){
		// if z is negative the skeleton is not tracked
		if(skeletonPoints[i][0].z < 0) {
			continue;
		}
		skeletonDraw_->draw(skeletonPoints[i]);
 	}
}
/**
	@brief	Draw skeleton images
	@param	x	X position to draw
	@param	y	Y position to draw
*/
void ofxKinectNui::drawSkeleton(float x, float y, float w, float h){
	ofPushMatrix();
	ofTranslate(x, y);
	ofScale(1.0f/(float)depthWidth * w, 1.0f/(float)depthHeight * h);
	
	drawSkeleton();
	ofPopMatrix();
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton images
	@param	x	X position to draw
	@param	y	Y position to draw
*/
void ofxKinectNui::drawSkeleton(float x, float y){
	drawSkeleton(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton images
	@param	point	The point to draw
*/
void ofxKinectNui::drawSkeleton(const ofPoint& point){
	drawSkeleton(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton images
	@param	point	The point to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
void ofxKinectNui::drawSkeleton(const ofPoint& point, float w, float h){
	drawSkeleton(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton images
	@param	rect	The rectangle area to draw
*/
void ofxKinectNui::drawSkeleton(const ofRectangle& rect){
	drawSkeleton(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Set angle of Kinect elevation
	@param	angleInDegrees	kinect tilt angle in degrees
*/
void ofxKinectNui::setAngle(int angleInDegrees){
	if(kinect.IsConnected()){
		targetAngle = angleInDegrees;
		kinect.SetAngle(targetAngle);
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: setAngle() is denied. Check Kinect connection.");
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Get current angle of Kinect
	@return	Current angle
*/
int ofxKinectNui::getCurrentAngle(){
	if(kinect.IsConnected()){
		return (int)kinect.GetAngle();
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getAngle() is denied. Check Kinect connection.");
		return 0;
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Get target angle of Kinect
	@return	Target angle
*/
int ofxKinectNui::getTargetAngle(){
	return targetAngle;
}

//---------------------------------------------------------------------------
/**
	@brief	Get video pixel data
	@return	Video pixel RGB data
*/
ofPixels& ofxKinectNui::getVideoPixels(){
	return videoPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get depth pixel data
	@return	Depth pixel gray scale data
*/
ofPixels& ofxKinectNui::getDepthPixels(){
	return depthPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get label pixel data
	@return	Label pixel RGBA data
*/
ofPixels& ofxKinectNui::getLabelPixels(){
	return labelPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get video pixel data adjusted to depth pixel data
	@return	Calibrated video pixel RGB data
*/
ofPixels& ofxKinectNui::getCalibratedVideoPixels(){
	return calibratedVideoPixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get label pixel data of player id
	@param	playerId	You can get whole players' silhouette when set 0, you can get each player's silhouette when set 1-7.
	@return	Label pixel thresholded data
*/
ofPixels& ofxKinectNui::getLabelPixelsCv(int playerId){
	if(playerId < 0 || playerId >= KINECT_PLAYERS_INDEX_NUM){
		ofLog(OF_LOG_ERROR, "ofxKinectNui: at getLabelPixelsCv(int playerId). please set 0-7 for playerId.");
		return labelPixelsCv[0];
	}else{
		return labelPixelsCv[playerId];
	}
}

//---------------------------------------------------------------------------
/**
	@brief	Get label pixel data of all players in array
	@return	Label pixel thresholded data array. labelPixelsCv[0] contains whole players' silhouette, labelPixelsCv[playerId] contains each player's silhouette
*/
ofPixels* ofxKinectNui::getLabelPixelsCvArray(){
	return labelPixelsCv;
}

//---------------------------------------------------------------------------
/**
	@brief	Get distance pixel data
	@return	Distance pixel data
*/
ofShortPixels& ofxKinectNui::getDistancePixels(){
	return distancePixels;
}

//---------------------------------------------------------------------------
/**
	@brief	Get sound buffer
	@return	Sound buffer
*/
std::vector<BYTE> ofxKinectNui::getSoundBuffer(){
	return soundBuffer;
}

//---------------------------------------------------------------------------
/**
	@brief	skeleton point data
	@return	valid skeleton count
*/
int ofxKinectNui::getSkeletonPoints(ofPoint* ret[]){
	if(!bGrabsSkeleton){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getSkeletonPoints - skeleton is not grabbed.");
	}
	int valid = 0;
	for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i) {
		if(skeletonPoints[i][0].z >= 0) {
			ret[i] = skeletonPoints[i];
			valid++;
		}
	}
	return valid;
}

//---------------------------------------------------------------------------
/**
	@brief	skeleton point data in xyz coordinate system (in meters)
	@return	map data of playerId and its skeleton points in xyz coordinate system (in meters)
*/
int ofxKinectNui::getRawSkeletonPoints(ofPoint* ret[]){
	if(!bGrabsSkeleton){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getRawSkeletonPoints - skeleton is not grabbed.");
	}
	int valid = 0;
	for(int i = 0; i < kinect::nui::SkeletonFrame::SKELETON_COUNT; ++i) {
		if(rawSkeletonPoints[i][0].z >= 0) {
			valid++;
		}
		ret[i] = rawSkeletonPoints[i];
	}
	return valid;
}

//---------------------------------------------------------------------------
/**
	@brief	Get color at the point 
	@param	x
	@param	y
	@return	ofColor of the point
*/
ofColor ofxKinectNui::getColorAt(int x, int y){
	ofColor c;
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Kinect stream is not opened");
		return c;
	}

	if(bGrabsVideo){
		int index = (y * width + x) * mVideoBpp;
		if(index < 0 || index > width * height * mVideoBpp){
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
/**
	@brief	Get color at the point 
	@param	point
	@return	ofColor of the point
*/
ofColor ofxKinectNui::getColorAt(const ofPoint& point){
	return getColorAt(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get color adjusted at the depth point 
	@param	depthX	x position on depth images
	@param	depthY	y position on depth images
	@return	ofColor of the point
*/
ofColor ofxKinectNui::getCalibratedColorAt(int depthX, int depthY){
	ofColor c;
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Kinect stream is not opened");
		return c;
	}

	if(bGrabsVideo && bGrabsDepth){
		int index = (depthY * depthWidth + depthX) * mVideoBpp;
		if(index < 0 || index > depthWidth * depthHeight * mVideoBpp){
			return c;
		}
		c.r = calibratedVideoPixels[index + 0];
		c.g = calibratedVideoPixels[index + 1];
		c.b = calibratedVideoPixels[index + 2];
		c.a = 255;
	}else{
		ofLog(OF_LOG_WARNING, "ofxKinectNui: You should set GrabsVideo and GrabsDepth true");
	}
	return c;
}

//---------------------------------------------------------------------------
/**
	@brief	Get color adjusted at the depth point 
	@param	depthPoint	position on depth images
	@return	ofColor of the point
*/
ofColor ofxKinectNui::getCalibratedColorAt(const ofPoint & depthPoint){
	return getCalibratedColorAt(depthPoint.x, depthPoint.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get real world scale at the depth point
	@param	depthX	x position on depth sensor
	@param	depthY	y position on depth sensor
	@return	ofVec3f	real world scale
*/
ofPoint ofxKinectNui::getWorldCoordinateFor(int depthX, int depthY){
	const double depthZ = distancePixels[depthWidth * depthY + depthX]/1000.0;
	return ofxBase3DVideo::getWorldCoordinateFor(depthX, depthY, depthZ);
}

//---------------------------------------------------------------------------
/**
	@brief	Get color at the point 
	@param	depthX	x position on depth images
	@param	depthY	y position on depth images
	@return	distance (mm)
*/
unsigned short ofxKinectNui::getDistanceAt(int depthX, int depthY){
	return distancePixels[depthY * depthWidth + depthX];
}

//---------------------------------------------------------------------------
/**
	@brief	Get color at the point 
	@param	depthPoint	position on depth images
	@return	distance (mm)
*/
unsigned short ofxKinectNui::getDistanceAt(const ofPoint& depthPoint){
	return getDistanceAt(depthPoint.x, depthPoint.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get player index at the point 
	@param	depthX	x position on depth sensor
	@param	depthY	y position on depth sensor
	@return	player index	0 when no player
*/
int ofxKinectNui::getPlayerIndexAt(int x, int y) {
	if(!kinect.IsInited() || !kinect.IsConnected() || !isOpened()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Kinect stream is not opened");
		return -1;
	}
	if(!bGrabsDepth || !bGrabsLabel){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: Please enable depth and label capture");
		return -1;
	}
	if(y >= depthHeight || x >= depthWidth){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getPlayerIndexAt out of range.");
		return -1;
	}
	int depthIndex = (depthWidth * y + x) * 4;
	UINT pix = (labelPixels[depthIndex + 3] << 24) | (labelPixels[depthIndex + 2] << 16) | (labelPixels[depthIndex + 1] << 8) | (labelPixels[depthIndex]);
	if(pix != color[0]){
		for(int i = 1; i < KINECT_PLAYERS_INDEX_NUM; i++){
			if(pix == color[i]){
				return i;
			}
		}
	}else{
		return 0;
	}
	return -1;
}

//---------------------------------------------------------------------------
/**
	@brief	Get player index at the point 
	@param	depthPoint	position on depth sensor
	@return	player index	0 when no player
*/
int ofxKinectNui::getPlayerIndexAt(const ofPoint& point){
	return getPlayerIndexAt(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Get player index at the point 
	@param	depthPoint	position on depth sensor
	@return	player index	0 when no player
*/
float ofxKinectNui::getAudioBeamAngle(){
	return audioBeamAngle;
}

//---------------------------------------------------------------------------
/**
	@brief	Get player index at the point 
	@param	depthPoint	position on depth sensor
	@return	player index	0 when no player
*/
float ofxKinectNui::getAudioAngle(){
	return audioAngle;
}

//---------------------------------------------------------------------------
/**
	@brief	Get player index at the point 
	@param	depthPoint	position on depth sensor
	@return	player index	0 when no player
*/
float ofxKinectNui::getAudioAngleConfidence(){
	return audioAngleConfidence;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the frame is updated
	@return	true when frame is updated
*/
bool ofxKinectNui::isFrameNew(){
	return bIsFrameNew;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether this kinect device is initialized
	@return	true when kinect is initialized
*/
bool ofxKinectNui::isInited(){
	return bIsInited;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether this kinect device is connected
	@return	true when kinect is recognized
*/
bool ofxKinectNui::isConnected(){
	return kinect.IsConnected();
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether this kinect stream is opened
	@return	true when stream is opened
*/
bool ofxKinectNui::isOpened(){
	return bIsOpened;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether this kinect depth sensor is nearmode
	@return	true when depth sensor is nearmode
*/
bool ofxKinectNui::isNearmode(){
	return bIsNearmode;
}

//---------------------------------------------------------------------------
/**
	@brief	is found skeleton
	@return	true when skeleton is found
*/
bool ofxKinectNui::isFoundSkeleton(){
	return bIsFoundSkeleton;
}

//---------------------------------------------------------------------------
/**
	@brief	whether the target skeleton is tracked
	@return	true when the skeleton is tracked
*/
bool ofxKinectNui::isTrackedSkeleton(int id){
	if(bGrabsSkeleton){
		if(skeletonPoints[id][0].z < 0){
			return false;
		}else{
			return true;
		}
	}else{
		return false;
	}
}


//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs video stream
	@return	true when grabs video
*/
bool ofxKinectNui::grabsVideo(){
	return bGrabsVideo;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs depth stream
	@return	true when grabs depth
*/
bool ofxKinectNui::grabsDepth(){
	return bGrabsDepth;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs skeleton
	@return	true when grabs skeleton
*/
bool ofxKinectNui::grabsSkeleton(){
	return bGrabsSkeleton;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs label data
	@return	true when grabs label
*/
bool ofxKinectNui::grabsLabel(){
	return bGrabsLabel;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs audio data
	@return	true when grabs audio
*/
bool ofxKinectNui::grabsAudio(){
	return bGrabsAudio;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs calibrated video stream
	@return	true when grabs calibrated video
*/
bool ofxKinectNui::grabsCalibratedVideo(){
	return bGrabsCalibratedVideo;
}

//---------------------------------------------------------------------------
/**
	@brief	Determines whether the kinect grabs separeted label data for cv
	 * @return	true when grabs separeted label for cv
*/
bool ofxKinectNui::grabsLabelCv(){
	return bGrabsLabelCv;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets video resolution
	@return	video image resolution
*/
NUI_IMAGE_TYPE ofxKinectNui::getVideoImageType(){
	return mVideoImageType;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets video resolution
	@return	video image resolution
*/
NUI_IMAGE_RESOLUTION ofxKinectNui::getVideoResolution(){
	return mVideoResolution;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets depth resolution
	@return	depth image resolution
*/
NUI_IMAGE_RESOLUTION ofxKinectNui::getDepthResolution(){
	return mDepthResolution;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets width of video resolution
	@return	width
*/
int ofxKinectNui::getVideoResolutionWidth(){
	return width;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets height of video resolution
	@return	height
*/
int ofxKinectNui::getVideoResolutionHeight(){
	return height;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets width of depth resolution
	@return	width
*/
int ofxKinectNui::getDepthResolutionWidth(){
	return depthWidth;
}

//---------------------------------------------------------------------------
/**
	@brief	Gets height of depth resolution
	@return	height
*/
int ofxKinectNui::getDepthResolutionHeight(){
	return depthHeight;
}

