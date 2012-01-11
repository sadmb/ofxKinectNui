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
	
	/**
	 * @brief	initialize kinect sensor
	 * @param	grabVideo			set false to disable video capture
	 * @param	grabDepth			set false to disable depth capture
	 * @param	grabLabel			set true to enable label capture, only the first kinect sensor can capture.
	 * @param	grabSkeleton		set true to enable skeleton capture, only the first kinect sensor can capture. 
	 * @param	grabCalibratedVideo	set true to enable calibratedVideo capture
	 * @param	grabLabelCv			set true to enable separated label capture for cv use, only the first kinect sensor can capture.
	 * @param	useTexture			set false when you don't need texture: you just want to get pixels and draw on openCV etc.
	 * @param	videoResolution		default is 640x480
	 * @param	depthResolution		default is 320x240
	 */
	bool init(	bool grabVideo = true,
				bool grabDepth = true,
				bool grabLabel = false,
				bool grabSkeleton = false,
				bool grabCalibratedVideo = false,
				bool grabLabelCv = false,
				bool useTexture = true,
				NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480,
				NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);

	/**
	 * @brief	open stream
	 */
	bool open();

	/**
	 * @brief	close stream
	 */
	void close();

	/**
	 * @brief	update stream data.
	 */
	void update();

	/**
	 * @brief	listener function when kinect is plugged.
	 */
	void pluggedFunc();

	/**
	 * @brief	listener function when kinect is unplugged.
	 */
	void unpluggedFunc();

	/**
	 * @brief	Draw video images from camera
	 */
	void draw(float x, float y, float w, float h);
	void draw(float x, float y);
	void draw(const ofPoint& point);
	void draw(const ofPoint& point, float w, float h);
	void draw(const ofRectangle& rect);

	/**
	 * @brief	Draw depth images
	 */
	void drawDepth(float x, float y, float w, float h);
	void drawDepth(float x, float y);
	void drawDepth(const ofPoint& point);
	void drawDepth(const ofPoint& point, float w, float h);
	void drawDepth(const ofRectangle& rect);

	/**
	 * @brief	Draw skeleton images
	 */
	void drawSkeleton(float x, float y, float w, float h);
	void drawSkeleton(float x, float y);
	void drawSkeleton(const ofPoint& point);
	void drawSkeleton(const ofPoint& point, float w, float h);
	void drawSkeleton(const ofRectangle& rect);
	
	/**
	 * @brief	Draw label images
	 */
	void drawLabel(float x, float y, float w, float h);
	void drawLabel(float x, float y);
	void drawLabel(const ofPoint& point);
	void drawLabel(const ofPoint& point, float w, float h);
	void drawLabel(const ofRectangle& rect);
	
	/**
	 * @brief	Set angle of Kinect elevation
	 */
	void setAngle(int angleInDegrees);

	/**
	 * @brief	Get current angle of Kinect
	 * @return	Current angle
	 */
	int getCurrentAngle();

	/**
	 * @brief	Get target angle of Kinect
	 * @return	Target angle
	 */
	int getTargetAngle();

	/**
	 * @brief	Get target angle of Kinect
	 * @return	Target angle
	 */
	void setUsesTexture(bool bUse);

	/**
	 * @brief	Get video pixel data
	 * @return	Video pixel RGB data
	 */
	ofPixels& getVideoPixels();

	/**
	 * @brief	Get video pixel data adjusted to depth pixel data
	 * @return	Calibrated video pixel RGB data
	 */
	ofPixels& getCalibratedVideoPixels();

	/**
	 * @brief	Get depth pixel data
	 * @return	Depth pixel gray scale data
	 */
	ofPixels& getDepthPixels();

	/**
	 * @brief	Get label pixel data
	 * @return	Label pixel RGBA data
	 */
	ofPixels& getLabelPixels();

	/**
	 * @brief	Get label pixel data of player id
	 * @param	playerId	You can get whole players' silhouette when set 0, you can get each player's silhouette when set 1-7.
	 * @return	Label pixel thresholded data
	 */
	ofPixels& getLabelPixelsCv(int playerId);

	/**
	 * @brief	Get label pixel data of all players in array
	 * @return	Label pixel thresholded data array. labelPixelsCv[0] contains whole players' silhouette, labelPixelsCv[playerId] contains each player's silhouette
	 */
	ofPixels* getLabelPixelsCvArray();

	/**
	 * @brief	Get distance pixel data
	 * @return	Distance pixel data
	 */
	ofShortPixels& getDistancePixels();

	/**
	 * @brief	Get video texture
	 * @return	Video texture
	 */
	ofTexture& getVideoTextureReference();

	/**
	 * @brief	Get depth texture
	 * @return	Dideo texture
	 */
	ofTexture& getDepthTextureReference();

	/**
	 * @brief	Get label texture
	 * @return	Label texture
	 */
	ofTexture& getLabelTextureReference();
	
	/**
	 * @brief	skeleton point data
	 * @return	map data of playerId and its skeleton points
	 */
	ofPoint** getSkeletonPoints();
	
	/**
	 * @brief	Get color at the point 
	 * @param	x
	 * @param	y
	 * @return	ofColor of the point
	 */
	ofColor getColorAt(int x, int y);
	ofColor getColorAt(const ofPoint& point);

	/**
	 * @brief	Get color adjusted at the depth point 
	 * @param	depthX	x position on depth sensor
	 * @param	depthY	y position on depth sensor
	 * @return	ofColor of the point
	 */
	ofColor getCalibratedColorAt(int depthX, int depthY);
	ofColor getCalibratedColorAt(const ofPoint& depthPoint);

	/**
	 * @brief	Get real world scale at the depth point
	 * @param	depthX	x position on depth sensor
	 * @param	depthY	y position on depth sensor
	 * @return	ofVec3f	real world scale
	 */
	ofVec3f getWorldCoordinateFor(int depthX, int depthY);


	/**
	 * @brief	Get color at the point 
	 * @param	depthX	x position on depth sensor
	 * @param	depthY	y position on depth sensor
	 * @return	distance (mm)
	 */
	unsigned short getDistanceAt(int depthX, int depthY);
	unsigned short getDistanceAt(const ofPoint& depthPoint);

	/**
	 * @brief	Get player index at the point 
	 * @param	depthX	x position on depth sensor
	 * @param	depthY	y position on depth sensor
	 * @return	player index	0 when no player
	 */
	int getPlayerIndexAt(int depthX, int depthY);
	int getPlayerIndexAt(const ofPoint& point);
	
	bool isFrameNew();

	/**
	 * @brief	Is this kinect initialized?
	 * @return	true when kinect is initialized
	 */
	bool isInited();

	/**
	 * @brief	Is this kinect recognized?
	 * @return	true when kinect is recognized
	 */
	bool isConnected();

	/**
	 * @brief	Is this kinect stream opened?
	 * @return	true when stream is opened
	 */
	bool isOpened();

	/**
	 * @brief	Does this kinect grab video stream?
	 * @return	true when grabs video
	 */
	bool grabsVideo();

	/**
	 * @brief	Does this kinect grab depth stream?
	 * @return	true when grabs depth
	 */
	bool grabsDepth();

	/**
	 * @brief	Does this kinect grab label stream?
	 * @return	true when grabs label
	 */
	bool grabsLabel();

	/**
	 * @brief	Does this kinect grab skeleton?
	 * @return	true when grabs skeleton
	 */
	bool grabsSkeleton();

	/**
	 * @brief	Does this kinect grab calibrated video?
	 * @return	true when grabs calibrated video
	 */
	bool grabsCalibratedVideo();

	/**
	 * @brief	Does this kinect grab separated label for cv?
	 * @return	true when grabs separeted label for cv
	 */
	bool grabsLabelCv();

	/**
	 * @brief	Use texture for drawing?
	 * @return	true when uses texture
	 */
	bool usesTexture();

	/**
	 * @brief	Use texture for drawing?
	 */
	NUI_IMAGE_RESOLUTION getVideoResolution();
	NUI_IMAGE_RESOLUTION getDepthResolution();

	int getVideoResolutionWidth();
	int getVideoResolutionHeight();
	int getDepthResolutionWidth();
	int getDepthResolutionHeight();

	/**
	 * @brief	Set near value of depth frame white
	 * @param	bEnabled	Set false to set near value of depth frame black
	 */
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

	const static int KINECT_PLAYERS_INDEX_NUM = 8;

protected:
	/**
	 * @brief	Calculate skeleton points
	 * @param	point	0.0-1.0 scaled point
	 * @param	width
	 * @param	height
	 * @return	Scaled point
	 */
	ofPoint calculateScaledSkeletonPoint(const ofPoint& point, float width, float height);

	kinect::nui::Kinect kinect;

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

	ofTexture videoTexture;				///< video texture
	ofTexture depthTexture;				///< depth texture
	ofTexture labelTexture;				///< label texture

	ofPoint** skeletonPoints;	///< joint points of all skeletons

	int targetAngle;	///< target angle of kinect tilt
	
	bool bIsOpened;					///< is stream opened?
	bool bIsInited;					///< is kinect initialized?
	bool bGrabsVideo;				///< grabs video?
	bool bGrabsDepth;				///< grabs depth?
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