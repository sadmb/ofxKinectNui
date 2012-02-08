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
	
	bool init(	bool grabVideo = true,
				bool grabDepth = true,
				bool grabAudio = false,
				bool grabLabel = false,
				bool grabSkeleton = false,
				bool grabCalibratedVideo = false,
				bool grabLabelCv = false,
				bool useTexture = true,
				NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480,
				NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);

	bool open(bool nearmode = false);
	void close();
	void update();

	void pluggedFunc();
	void unpluggedFunc();

	void draw(float x, float y, float w, float h);
	void draw(float x, float y);
	void draw(const ofPoint& point);
	void draw(const ofPoint& point, float w, float h);
	void draw(const ofRectangle& rect);

	void drawDepth(float x, float y, float w, float h);
	void drawDepth(float x, float y);
	void drawDepth(const ofPoint& point);
	void drawDepth(const ofPoint& point, float w, float h);
	void drawDepth(const ofRectangle& rect);

	void drawSkeleton(float x, float y, float w, float h);
	void drawSkeleton(float x, float y);
	void drawSkeleton(const ofPoint& point);
	void drawSkeleton(const ofPoint& point, float w, float h);
	void drawSkeleton(const ofRectangle& rect);
	
	void drawLabel(float x, float y, float w, float h);
	void drawLabel(float x, float y);
	void drawLabel(const ofPoint& point);
	void drawLabel(const ofPoint& point, float w, float h);
	void drawLabel(const ofRectangle& rect);
	
	void setAngle(int angleInDegrees);
	int getCurrentAngle();
	int getTargetAngle();

	void setUsesTexture(bool bUse);

	ofPixels& getVideoPixels();
	ofPixels& getDepthPixels();
	ofPixels& getLabelPixels();
	ofPixels& getCalibratedVideoPixels();
	ofPixels& getLabelPixelsCv(int playerId);
	ofPixels* getLabelPixelsCvArray();
	ofShortPixels& getDistancePixels();
	std::vector<BYTE> getSoundBuffer();

	ofTexture& getVideoTextureReference();
	ofTexture& getDepthTextureReference();
	ofTexture& getLabelTextureReference();
	
	ofPoint** getSkeletonPoints();
	
	ofColor getColorAt(int x, int y);
	ofColor getColorAt(const ofPoint& point);

	ofColor getCalibratedColorAt(int depthX, int depthY);
	ofColor getCalibratedColorAt(const ofPoint& depthPoint);

	ofVec3f getWorldCoordinateFor(int depthX, int depthY);

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

	bool grabsVideo();
	bool grabsDepth();
	bool grabsLabel();
	bool grabsSkeleton();
	bool grabsAudio();
	bool grabsCalibratedVideo();
	bool grabsLabelCv();

	bool usesTexture();

	NUI_IMAGE_RESOLUTION getVideoResolution();
	NUI_IMAGE_RESOLUTION getDepthResolution();

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

protected:
	ofPoint calculateScaledSkeletonPoint(const ofPoint& point, float width, float height);

public:
	const static int KINECT_PLAYERS_INDEX_NUM = 8;

protected:
	kinect::nui::Kinect kinect;	///< kinect instance

	int width;		///<	width of video stream
	int height;		///<	height of depth stream
	int depthWidth;		///<	width of depth stream
	int depthHeight;	///<	height of depth stream

	ofPixels videoPixels;			///<	video pixels
	ofPixels depthPixels;			///<	depth pixels
	ofShortPixels distancePixels;	///<	distance pixels (raw depth pixels data from sensor)
	ofPixels labelPixels;			///<	label pixels
	ofPixels calibratedVideoPixels;	///<	video pixels adjusted to depth pixels
	ofPixels* labelPixelsCv;		///<	separated label pixels for cv use, labelPixelsCv[0] contains whole players silhouette. labelPixelsCv[playerId] contains each players silhouette.
	std::vector<BYTE> soundBuffer;	///<	audio buffer
	float audioBeamAngle, audioAngle, audioAngleConfidence;	///< for audio

	ofTexture videoTexture;				///< video texture
	ofTexture depthTexture;				///< depth texture
	ofTexture labelTexture;				///< label texture

	ofPoint** skeletonPoints;	///< joint points of all skeletons

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
	bool bUsesTexture;				///< uses texture?
	bool bIsFrameNew;				///< frame updated?

	NUI_IMAGE_RESOLUTION mVideoResolution;	///< video resolution flag
	NUI_IMAGE_RESOLUTION mDepthResolution;	///< depth resolution flag

};
#endif // OFX_KINECT_NUI_H