/******************************************************************/
/**
 * @file	ofxKinectNuiPlayer.h
 * @brief	kinect player for Kinect Official Sensor
 * @note
 * @todo
 * @bug	
 * @reference	ofxKinectPlayer.h created by arturo: 14/12/2010
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_PLAYER_H
#define OFX_KINECT_NUI_PLAYER_H

#include "ofxKinectNuiCalibration.h"
#include "ofMain.h"
#include "ofxBase3DVideo.h"

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
/****************************************/
/**
 * @class	ofxKinectNuiPlayer.h
 * @brief	kinect player for a kinect official device
 * @note	
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/****************************************/
class ofxKinectNuiPlayer: public ofxBase3DVideo {
public:
	/**
	 * @brief	Constructor
	 */
	ofxKinectNuiPlayer();

	/**
	 * @brief	Destructor
	 */
	virtual ~ofxKinectNuiPlayer();
	
	/**
	 * @brief	Setup the recording to read
	 * @note	Make sure you recorded the video/depth/skeleton frames or reading them will fail big time!
	 * @args	file				filepath
	 * @args	hasRecordedVideo	set false when not recording the video image
	 * @args	hasRecordedDepth	set false when not recording the depth image
	 * @args	hasRecordedLabel	set true when recording the label image
	 * @args	hasRecordedSkeleton	set true when recording the skeleton points
	 * @args	videoResolution		1280x1024 or 640x480, default is 640x480
	 * @args	depthResolution		640x480, 320x240 or 80x60, default is 320x240. set label false when choose 640x480
	 */
	void setup(	const string & file,
				bool hasRecordedVideo = true,
				bool hasRecordedDepth = true,
				bool hasRecordedLabel = false,
				bool hasRecordedSkeleton = false,
				NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480,
				NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);

	/**
	 * @brief	Update kinect player
	 */
	void update();
	
	/**
	 * @brief	Draw video player
	 */
	void draw(float x, float y);
	void draw(float , float y, float w, float h);
	void draw(const ofPoint & point);
	void draw(const ofPoint & point, float w, float h);
	void draw(const ofRectangle & rect);

	/**
	 * @brief	Draw depth player
	 */
	void drawDepth(float x, float y, float w, float h);
	void drawDepth(float x, float y);
	void drawDepth(const ofPoint & point);
	void drawDepth(const ofPoint & point, float w, float h);
	void drawDepth(const ofRectangle & rect);

	/**
	 * @brief	Draw skeleton player
	 */
	void drawSkeleton(float x, float y, float w, float h);
	void drawSkeleton(float x, float y);
	void drawSkeleton(const ofPoint & point);
	void drawSkeleton(const ofPoint & point, float w, float h);
	void drawSkeleton(const ofRectangle & rect);

	/**
	 * @brief	Draw video player
	 */
	void drawLabel(float x, float y);
	void drawLabel(float , float y, float w, float h);
	void drawLabel(const ofPoint & point);
	void drawLabel(const ofPoint & point, float w, float h);
	void drawLabel(const ofRectangle & rect);

	/**
	 * @brief	Get the reference of video texture
	 */
	ofTexture & getTextureReference();

	/**
	 * @brief	Get the reference of depth texture
	 */
	ofTexture & getDepthTextureReference();

	/**
	 * @brief	Get the reference of depth texture
	 */
	ofTexture & getLabelTextureReference();

	/**
	 * @brief	Set if use texture
	 * @args	bUseTexture:	true when use texture
	 */
	void setUsesTexture(bool bUse);

	/**
	 * @brief	Get the width of video
	 */
	float getWidth();

	/**
	 * @brief	Get the height of video
	 */
	float getHeight();

	/**
	 * @brief	Get the width of depth
	 */
	float getDepthWidth();

	/**
	 * @brief	Get the height of depth
	 */
	float getDepthHeight();

	void setAnchorPercent(float xPct, float yPct){};
    void setAnchorPoint(float x, float y){};
	void resetAnchor(){};

	/**
	 * @brief	Is frame updated?
	 * @return	true when updated
	 */
	bool isFrameNew();

	/**
	 * @brief	Close connection
	 */
	void close();
	
	/**
	 * @brief	enable looping
	 */
	void loop()				{bLoop = true;}

	/**
	 * @brief	disable looping
	 */
	void noLoop()			{bLoop = false;}

	/**
	 * @brief	enabled or disabled looping
	 */
	void setLoop(bool loop)	{bLoop = loop;}

	/**
	 * @brief	enabled or disabled looping
	 */
	bool isLooping()		{return bLoop;}
	

	/**
	 * @brief	Get pixels of video
	 */
	unsigned char* getPixels();

	/**
	 * @brief	Get pixels of depth
	 */
	unsigned char* getDepthPixels();

	/**
	 * @brief	Get pixels of depth
	 */
	unsigned char* getLabelPixels();

	/**
	 * @brief	Get pixels of distance data
	 */
	float* getDistancePixels();

	/**
	 * @brief	Get calibrated video pixels
	 */
	unsigned char* getCalibratedRGBPixels();
	
	/**
	 * @brief	for ver007 compatibility
	 */
	ofPixelsRef getPixelsRef();

	/**
	 * @brief	Get distance at the point
	 * @args	x
	 * @args	y
	 */
	float getDistanceAt(int x, int y) ;

	/**
	 * @brief	Get world coordinate for the point
	 * @args	x
	 * @args	y
	 */
	ofVec3f getWorldCoordinateFor(int x, int y);

	int fps; ///< fps when recorded

	int	width; ///< width
	int	height; ///< height
	int	depthWidth; ///< width for depth camera
	int	depthHeight; ///< height for depth camera
	
private:
	ofPoint calcScaledSkeletonPoint(const ofPoint& skeletonPoint, float width, float height);

	FILE * f; ///< file for play
	string filename; ///< file name
	ofTexture depthTexture, videoTexture, labelTexture; ///< textures for play
	bool bUsesTexture; ///< is using texture?
	unsigned short* depthPixelsRaw; ///< raw depth pixel data
	unsigned char* videoPixels; ///< video pixel data
	unsigned char* labelPixels; ///< label pixel data
	unsigned char* calibratedRGBPixels; ///< calibrated video data corresponding to depth camera
	float* skeletons; ///< skeleton data
	ofPoint** skeletonPoints; ///< skeleton points
	
	ofPixels pixels;

	bool bLoop; ///< is loop?
	bool bVideo; ///< is video recorded?
	bool bDepth; ///< is depth recorded?
	bool bLabel; ///< is label recorded?
	bool bSkeleton; ///< is skeleton recorded?

	bool bIsFrameNew; ///< is frame new?
	
	int lastFrameTime; ///< last frame time

	ofxKinectNuiCalibration calibration; ///< calibration
};
#endif // OFX_KINECT_NUI_PLAYER_H
