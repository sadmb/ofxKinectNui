#include "ofxKinectNui.h"

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
	labelPixelsCv = NULL;
	skeletonPoints = NULL;

	addKinectListener(this, &ofxKinectNui::pluggedFunc, &ofxKinectNui::unpluggedFunc);
}

//---------------------------------------------------------------------------
ofxKinectNui::~ofxKinectNui(){
	close();

	// clear textures
	if(videoTexture.isAllocated()){
		videoTexture.clear();
	}
	if(depthTexture.isAllocated()){
		depthTexture.clear();
	}
	if(labelTexture.isAllocated()){
		labelTexture.clear();
	}

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
	if(labelPixelsCv != NULL){
		delete[] labelPixelsCv;
		labelPixelsCv = NULL;
	}

	if(skeletonPoints != NULL){
		delete[] skeletonPoints[0];
		skeletonPoints[0] = NULL;
		delete[] skeletonPoints;
		skeletonPoints = NULL;
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
}

//---------------------------------------------------------------------------
/**
	@brief	initialize kinect sensor
	@param	grabVideo			set false to disable video capture
	@param	grabDepth			set false to disable depth capture
	@param	grabAudio			set true to enable audio capture
	@param	grabLabel			set true to enable label capture, only the first kinect sensor can capture.
	@param	grabSkeleton		set true to enable skeleton capture, only the first kinect sensor can capture. 
	@param	grabCalibratedVideo	set true to enable calibratedVideo capture
	@param	grabLabelCv			set true to enable separated label capture for cv use, only the first kinect sensor can capture.
	@param	useTexture			set false when you don't need texture: you just want to get pixels and draw on openCV etc.
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
						bool useTexture /*= true*/,
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
	if(bGrabsVideo){
		if(bUsesTexture){
			videoTexture.allocate(width, height, GL_RGB);
		}

		int length = width * height;
		if(!videoPixels.isAllocated()){
			videoPixels.allocate(width, height, OF_PIXELS_RGB);
		}
		memset(videoPixels.getPixels(), 0, length * 3 * sizeof(unsigned char));

		dwFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;
	}
	if(bGrabsDepth){
		if(bUsesTexture){
			depthTexture.allocate(depthWidth, depthHeight, GL_LUMINANCE);
		}

		int length = depthWidth * depthHeight;
		if(!depthPixels.isAllocated()){
			depthPixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
		memset(depthPixels.getPixels(), 0, length * sizeof(unsigned char));
		if(!distancePixels.isAllocated()){
			distancePixels.allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
		}
		memset(distancePixels.getPixels(), 0, length * sizeof(unsigned short));

		if(bGrabsCalibratedVideo){
			if(!calibratedVideoPixels.isAllocated()){
				calibratedVideoPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGB);
			}
			memset(calibratedVideoPixels.getPixels(), 0, length * 3 * sizeof(unsigned char));
		}

		if(bGrabsLabel || bGrabsLabelCv){
			if(bGrabsLabel){
				if(bUsesTexture){
					labelTexture.allocate(depthWidth, depthHeight, GL_RGBA);
				}
				if(!labelPixels.isAllocated()){
					labelPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGBA);
				}
				memset(labelPixels.getPixels(), 0, length * 4 * sizeof(unsigned char));
			}

			if(bGrabsLabelCv){
				if(labelPixelsCv == NULL){
					labelPixelsCv = new ofPixels[KINECT_PLAYERS_INDEX_NUM];
				}
				for(int i = 0; i < KINECT_PLAYERS_INDEX_NUM; i++){
					if(!labelPixelsCv[i].isAllocated()){
						labelPixelsCv[i].allocate(depthWidth, depthHeight, OF_PIXELS_MONO);
					}
					memset(labelPixelsCv[i].getPixels(), 0, length * sizeof(unsigned char));
				}
			}

			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
		}else{
			dwFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
		}
	}
	if(bGrabsAudio){
		dwFlags |= NUI_INITIALIZE_FLAG_USES_AUDIO;
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
/**
	@brief	open stream
*/
bool ofxKinectNui::open(bool nearmode /*= false */){
	if(kinect.IsConnected() && !isOpened()){
		bIsNearmode = nearmode;
		if(isInited()){
			if(!kinect.IsInited()){
				init(bGrabsVideo, bGrabsDepth, bGrabsLabel, bGrabsSkeleton, bUsesTexture, mVideoResolution, mDepthResolution);
			}
		}
		if(bGrabsVideo){
			kinect.VideoStream().Open(NUI_IMAGE_TYPE_COLOR, mVideoResolution);
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
				memcpy(videoPixels.getPixels() + (video.Width() * y + x) * 3, &videobit, sizeof(char) * 3);
			}
		}
	}

	if(bGrabsDepth){
		// Get the depth data of next frame
		kinect::nui::DepthFrame depth(kinect.DepthStream());
		unsigned short depthbit;
		unsigned short playerLabel;
		for(int x = 0; x < depth.Width(); x++){
			for(int y = 0; y < depth.Height(); y++){
				if(bGrabsLabel || bGrabsLabelCv){
					depthbit = depth(x, y) >> 3;
					playerLabel = depth(x, y) & 0x7;
					if(bGrabsLabel){
						memcpy(labelPixels.getPixels() + (depth.Width() * y + x) * 4, &color[playerLabel], sizeof(char) * 4);
					}
					if(bGrabsLabelCv){
						for(int i = 0; i < KINECT_PLAYERS_INDEX_NUM; i++){
							unsigned short lb;
							if((i == 0 && playerLabel > 0) || (i > 0 && playerLabel == i)){
								lb = 0xFF;
								memcpy(labelPixelsCv[i].getPixels() + (depth.Width() * y + x), &lb, sizeof(char));
							}else{
								lb = 0x00;
								memcpy(labelPixelsCv[i].getPixels() + (depth.Width() * y + x), &lb, sizeof(char));
							}
						}
					}
				}else{
					depthbit = depth(depth.Width() - x, y);
				}
				memcpy(distancePixels.getPixels() + (depth.Width() * y + x), &depthbit, sizeof(short));
				if(bIsDepthNearValueWhite){
					depthPixels[depth.Width() * y + x] = depthPixelsLookupNearWhite[depthbit];
				}else{
					depthPixels[depth.Width() * y + x] = depthPixelsLookupFarWhite[depthbit];
				}
				if(bGrabsCalibratedVideo){
					int depthIndex = depth.Width() * y + x;
					long vindex = kinect.GetColorPixelCoordinatesFromDepthPixel(depth.Width() * y + x, 0) * 3;
					for(int i = 0; i < 3; i++){
						unsigned char vbit;
						if(vindex + i < 0 || vindex + i > width * height * 3){
							vbit = 0;
						}else{
							vbit = videoPixels[vindex + i];
						}
						memcpy(calibratedVideoPixels.getPixels() + (depth.Width() * y + x) * 3 + i, &vbit, sizeof(char));
					}
				}
			}
		}
	}

	if(bGrabsAudio){
		soundBuffer = kinect.AudioStream().Read();
		audioBeamAngle = (float)kinect.AudioStream().GetAudioBeamAngle();
		audioAngle = (float)kinect.AudioStream().GetAudioAngle();
		audioAngleConfidence = (float)kinect.AudioStream().GetAudioAngleConfidence();
	}

	if(bUsesTexture){
		if(bGrabsVideo){
			videoTexture.loadData(videoPixels);
		}
		if(bGrabsDepth){
			depthTexture.loadData(depthPixels);
		}
		if(bGrabsLabel){
			labelTexture.loadData(labelPixels);
		}
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
/**
	@brief	Draw video images from camera
	@param	x	X position to draw
	@param	y	Y position to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
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
/**
	@brief	Draw video images from camera
	@param	x	X position to draw
	@param	y	Y position to draw
*/
void ofxKinectNui::draw(float x, float y){
	draw(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video images from camera
	@param	point	The point to draw
*/
void ofxKinectNui::draw(const ofPoint& point){
	draw(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video images from camera
	@param	point	The point to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
void ofxKinectNui::draw(const ofPoint& point, float w, float h){
	draw(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw video images from camera
	@param	rect	The rectangle area to draw
*/
void ofxKinectNui::draw(const ofRectangle& rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth images
	@param	x	X position to draw
	@param	y	Y position to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
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
/**
	@brief	Draw depth images
	@param	x	X position to draw
	@param	y	Y position to draw
*/
void ofxKinectNui::drawDepth(float x, float y){
	draw(x, y, depthWidth, depthHeight);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth images
	@param	point	The point to draw
*/
void ofxKinectNui::drawDepth(const ofPoint& point){
	draw(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth images
	@param	point	The point to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
void ofxKinectNui::drawDepth(const ofPoint& point, float w, float h){
	draw(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw depth images
	@param	rect	The rectangle area to draw
*/
void ofxKinectNui::drawDepth(const ofRectangle& rect){
	draw(rect.x, rect.y, rect.width, rect.height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw skeleton images
	@param	x	X position to draw
	@param	y	Y position to draw
*/
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
						kinect::nui::SkeletonData::SkeletonPoint p = skeleton[i].TransformSkeletonToDepthImage(j);
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
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SPINE], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SPINE], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HEAD], w, h));
		
					// BODY_LEFT
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_LEFT], w, h));

					// BODY_RIGHT
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_SHOULDER_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ELBOW_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_WRIST_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HAND_RIGHT], w, h));
		
					// LEG_LEFT
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_LEFT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_LEFT], w, h));

					// LEG_RIGHT
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_CENTER], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_HIP_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_KNEE_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT], w, h));
					ofLine(calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_ANKLE_RIGHT], w, h), calculateScaledSkeletonPoint(skeletonPoints[i][NUI_SKELETON_POSITION_FOOT_RIGHT], w, h));
		
					ofSetColor(255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i + 1), 255 * (int)pow(-1.0, i));
					ofSetLineWidth(0);
					ofFill();
					for(int k = 0; k < kinect::nui::SkeletonData::POSITION_COUNT; k++){
						ofCircle(calculateScaledSkeletonPoint(skeletonPoints[i][k], w, h), 5);
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
	@brief	Draw label images
	@param	x	X position to draw
	@param	y	Y position to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
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
/**
	@brief	Draw label images
	@param	x	X position to draw
	@param	y	Y position to draw
*/
void ofxKinectNui::drawLabel(float x, float y){
	drawLabel(x, y, width, height);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label images
	@param	point	The point to draw
*/
void ofxKinectNui::drawLabel(const ofPoint& point){
	drawLabel(point.x, point.y);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label images
	@param	point	The point to draw
	@param	w	width of drawing area
	@param	h	height of drawing area
*/
void ofxKinectNui::drawLabel(const ofPoint& point, float w, float h){
	drawLabel(point.x, point.y, w, h);
}

//---------------------------------------------------------------------------
/**
	@brief	Draw label images
	@param	rect	The rectangle area to draw
*/
void ofxKinectNui::drawLabel(const ofRectangle& rect){
	drawLabel(rect.x, rect.y, rect.width, rect.height);
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
	@brief	Get video texture
	@return	Video texture
*/
ofTexture& ofxKinectNui::getVideoTextureReference(){
	if(!videoTexture.isAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getTextureReference - video texture is not allocated");
	}
	return videoTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Get depth texture
	@return	Dideo texture
*/
ofTexture& ofxKinectNui::getDepthTextureReference(){
	if(!depthTexture.isAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getDepthTextureReference - depth texture is not allocated");
	}
	return depthTexture;
}

//---------------------------------------------------------------------------
/**
	@brief	Get label texture
	@return	Label texture
*/
ofTexture& ofxKinectNui::getLabelTextureReference(){
	if(!labelTexture.isAllocated()){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getLabelTextureReference - label texture is not allocated");
	}
	return labelTexture;
}


//---------------------------------------------------------------------------
/**
	@brief	skeleton point data
	@return	map data of playerId and its skeleton points
*/
ofPoint** ofxKinectNui::getSkeletonPoints(){
	if(!bGrabsSkeleton){
		ofLog(OF_LOG_WARNING, "ofxKinectNui: getSkeletonPoints - skeleton is not grabbed.");
	}
	return skeletonPoints;
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
		int index = (depthY * depthWidth + depthX) * 3;
		if(index < 0 || index > depthWidth * depthHeight * 3){
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
ofVec3f ofxKinectNui::getWorldCoordinateFor(int depthX, int depthY){
	const double depthZ = distancePixels[depthWidth * depthX + depthY]/1000.0;
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
	for(int i = 0; i < KINECT_PLAYERS_INDEX_NUM; i++){
		if(labelPixels[depthWidth * y + x] & color[i]){
			return i;
		}
	}
	return 0;
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
	@brief	Set uses texture for drawing
	@return	true when uses texture
*/
void ofxKinectNui::setUsesTexture(bool bUse){
	bUsesTexture = bUse;
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

//---------------------------------------------------------------------------
/**
	@brief	Calculate skeleton points
	@param	point	0.0-1.0 scaled point
	@param	width
	@param	height
	@return	Scaled point
*/
ofPoint ofxKinectNui::calculateScaledSkeletonPoint(const ofPoint& skeletonPoint, float width, float height){
	float px = skeletonPoint.x / (float)depthWidth * width;
	float py = skeletonPoint.y / (float)depthHeight * height;
	return ofPoint(px, py);
}
