/******************************************************************/
/**
 * @file	ofxKinectNui.h
 * @brief	kinect nui wrapper for oF
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_H
#define OFX_KINECT_NUI_H

#include "kinect/nui/Kinect.h" // this should be before ofMain.h
#include "kinect/nui/ImageFrame.h" // for VideoFrame and DepthFrame
#include "ofMain.h"
#include "ofxBase3DVideo.h"

class IDrawPixels;
class IDrawPoints;

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
/****************************************/
/**
 * @class	ofxKinectNui
 * @brief	kinect wrapper for oF
 * @note	
 * @author	sadmb
 * @date		Oct. 26, 2011
 */
/****************************************/
class ofxKinectNui: public ofxBase3DVideo{
public:
	ofxKinectNui();
	virtual ~ofxKinectNui();
	
	struct InitSetting{
		bool grabVideo:1;
		bool grabDepth:1;
		bool grabAudio:1;
		bool grabLabel:1;
		bool grabSkeleton:1;
		bool grabCalibratedVideo:1;
		bool grabLabelCv:1;
		NUI_IMAGE_TYPE videoImageType;
		NUI_IMAGE_RESOLUTION videoResolution;
		NUI_IMAGE_RESOLUTION depthResolution;
		InitSetting(){
			grabVideo = true;
			grabDepth = true;
			grabAudio = false;
			grabLabel = false;
			grabSkeleton = false;
			grabCalibratedVideo = false;
			grabLabelCv = false;
			videoImageType = NUI_IMAGE_TYPE_COLOR;
			videoResolution = NUI_IMAGE_RESOLUTION_640x480;
			depthResolution = NUI_IMAGE_RESOLUTION_320x240;
		}
	};

	enum {
		UPDATE_FLAG_NONE = 0x00000000,
		
		UPDATE_FLAG_VIDEO			= 0x00000001,
		
		UPDATE_FLAG_DEPTH			= 0x00000100,
		UPDATE_FLAG_LABEL			= 0x00000200,
		UPDATE_FLAG_LABEL_CV		= 0x00000400,
		UPDATE_FLAG_DISTANCE		= 0x00000800,
		UPDATE_FLAG_CALIBRATED_VIDEO= 0x00001000,
		
		UPDATE_FLAG_SKELETON		= 0x00010000,
		
		UPDATE_FLAG_AUDIO			= 0x01000000,

		UPDATE_FLAG_GROUP_VIDEO		= 0x000000FF,
		UPDATE_FLAG_GROUP_DEPTH		= 0x0000FF00,
		UPDATE_FLAG_GROUP_SKELETON	= 0x00FF0000,
		UPDATE_FLAG_GROUP_AUDIO		= 0xFF000000,

		UPDATE_FLAG_ALL				= 0xFFFFFFFF,
	};

	static const int SKELETON_COUNT = NUI_SKELETON_COUNT;
	static const int SKELETON_POSITION_COUNT = NUI_SKELETON_POSITION_COUNT;

	bool init();
	bool init(const InitSetting& setting);
	
	bool init(	bool grabVideo,
				bool grabDepth,
				bool grabAudio,
				bool grabLabel,
				bool grabSkeleton,
				bool grabCalibratedVideo,
				bool grabLabelCv,
				NUI_IMAGE_TYPE videoImageType = NUI_IMAGE_TYPE_COLOR,
				NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480,
				NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);

	bool open(bool nearmode = false);
	void close();
	void update(){update(UPDATE_FLAG_ALL);}
	void update(UINT flag);

	void setVideoDrawer(IDrawPixels* drawer);
	void setDepthDrawer(IDrawPixels* drawer);
	void setLabelDrawer(IDrawPixels* drawer);
	void setSkeletonDrawer(IDrawPoints* drawer);

	void drawVideo();
	void drawDepth();
	void drawLabel();
	void drawSkeleton();

	void drawVideo(int x, int y, int width, int height);
	void drawDepth(int x, int y, int width, int height);
	void drawLabel(int x, int y, int width, int height);

	void pluggedFunc();
	void unpluggedFunc();

	void drawSkeleton(float x, float y, float w, float h);
	void drawSkeleton(float x, float y);
	void drawSkeleton(const ofPoint& point);
	void drawSkeleton(const ofPoint& point, float w, float h);
	void drawSkeleton(const ofRectangle& rect);
	
	void setAngle(int angleInDegrees);
	int getCurrentAngle();
	int getTargetAngle();

	ofPixels& getVideoPixels();
	ofPixels& getDepthPixels();
	ofPixels& getLabelPixels();
	ofPixels& getCalibratedVideoPixels();
	ofPixels& getLabelPixelsCv(int playerId);
	ofPixels* getLabelPixelsCvArray();
	ofShortPixels& getDistancePixels();
	std::vector<BYTE> getSoundBuffer();
	
	int getSkeletonPoints(ofPoint* ret[]);
	int getRawSkeletonPoints(ofPoint* ret[]);

	ofColor getColorAt(int x, int y);
	ofColor getColorAt(const ofPoint& point);

	ofColor getCalibratedColorAt(int depthX, int depthY);
	ofColor getCalibratedColorAt(const ofPoint& depthPoint);

	ofPoint getWorldCoordinateFor(int depthX, int depthY);

	unsigned short getDistanceAt(int depthX, int depthY);
	unsigned short getDistanceAt(const ofPoint& depthPoint);

	int getPlayerIndexAt(int depthX, int depthY);
	int getPlayerIndexAt(const ofPoint& point);

	float getAudioBeamAngle();
	float getAudioAngle();
	float getAudioAngleConfidence();
	
	bool isFrameNew();
	bool isInited();
	bool isConnected();
	bool isOpened();
	bool isNearmode();
	bool isFoundSkeleton();
	bool isTrackedSkeleton(int id);

	bool grabsVideo();
	bool grabsDepth();
	bool grabsLabel();
	bool grabsSkeleton();
	bool grabsAudio();
	bool grabsCalibratedVideo();
	bool grabsLabelCv();

	NUI_IMAGE_RESOLUTION getVideoResolution();
	NUI_IMAGE_RESOLUTION getDepthResolution();

	NUI_IMAGE_TYPE getVideoImageType();
	int getVideoResolutionWidth();
	int getVideoResolutionHeight();
	int getDepthResolutionWidth();
	int getDepthResolutionHeight();

	void enableDepthNearValueWhite(bool bEnabled);
	bool isDepthNearValueWhite();

	template<class T> void addKinectListener(T* object, void(T::*pluggedFunction)(), void(T::*unpluggedFunction)())
	{
		kinect.AddKinectListener(object, pluggedFunction, unpluggedFunction);
	}

	template<class T> void removeKinectListener(T* object)
	{
		kinect.RemoveKinectListener(object);
	}

public:
	const static int KINECT_PLAYERS_INDEX_NUM = 8;

	static int getActiveCount(){
		return kinect::nui::Kinect::GetActiveCount();
	}
	static int getAvailableCount(){
		return kinect::nui::Kinect::GetAvailableCount();
	}
	static int getConnectedCount(){
		return kinect::nui::Kinect::GetConnectedCount();
	}

	static int getNextAvailableIndex(){
		return kinect::nui::Kinect::GetNextAvailableIndex();
	}

protected:
	kinect::nui::Kinect kinect;	///< kinect instance

	ofPixels videoPixels;			///<	video pixels
	ofPixels depthPixels;			///<	depth pixels
	ofShortPixels distancePixels;	///<	distance pixels (raw depth pixels data from sensor)
	ofPixels labelPixels;			///<	label pixels
	ofPixels calibratedVideoPixels;	///<	video pixels adjusted to depth pixels
	ofPixels labelPixelsCv[KINECT_PLAYERS_INDEX_NUM];		///<	separated label pixels for cv use, labelPixelsCv[0] contains whole players silhouette. labelPixelsCv[playerId] contains each players silhouette.
	std::vector<BYTE> soundBuffer;	///<	audio buffer
	float audioBeamAngle, audioAngle, audioAngleConfidence;	///< for audio

	ofPoint skeletonPoints[SKELETON_COUNT][SKELETON_POSITION_COUNT];	///< joint points of all skeletons
	ofPoint rawSkeletonPoints[SKELETON_COUNT][SKELETON_POSITION_COUNT];	///< joint points of all skeletons

	int targetAngle;	///< target angle of kinect tilt
	
	bool bIsOpened;					///< is stream opened?
	bool bIsInited;					///< is kinect initialized?
	bool bIsNearmode;				///< is kinect nearmode?
	bool bGrabsVideo;				///< grabs video?
	bool bGrabsDepth;				///< grabs depth?
	bool bGrabsAudio;				///< grabs audio?
	bool bGrabsLabel;				///< grabs label?
	bool bGrabsSkeleton;			///< grabs skeleton?
	bool bGrabsCalibratedVideo;		///< grabs calibrated video?
	bool bGrabsLabelCv;				///< grabs separated label for cv?
	bool bIsFrameNew;				///< frame updated?
	bool bIsFoundSkeleton;

	NUI_IMAGE_TYPE mVideoImageType;			///< video image type
	NUI_IMAGE_RESOLUTION mVideoResolution;	///< video resolution flag
	NUI_IMAGE_RESOLUTION mDepthResolution;	///< depth resolution flag

	int mVideoBpp;

	UINT updateFlagDefault_;
	IDrawPixels* videoDraw_;
	IDrawPixels* depthDraw_;
	IDrawPixels* labelDraw_;
	IDrawPoints* skeletonDraw_;
};
#endif // OFX_KINECT_NUI_H