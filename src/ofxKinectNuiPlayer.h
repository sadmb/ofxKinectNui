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

#include "kinect/nui/Kinect.h" // this should be before ofMain.h
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
class ofxKinectNuiPlayer: public ofxBase3DVideo{
public:
	ofxKinectNuiPlayer();
	virtual ~ofxKinectNuiPlayer();
	
	void setup(	const string & file, bool useTexture = true);
	void close();
	void play();
	void stop();
	void pause();

	void update();
	
	void draw(float x, float y);
	void draw(float , float y, float w, float h);
	void draw(const ofPoint & point);
	void draw(const ofPoint & point, float w, float h);
	void draw(const ofRectangle & rect);

	void drawDepth(float x, float y, float w, float h);
	void drawDepth(float x, float y);
	void drawDepth(const ofPoint & point);
	void drawDepth(const ofPoint & point, float w, float h);
	void drawDepth(const ofRectangle & rect);

	void drawSkeleton(float x, float y, float w, float h);
	void drawSkeleton(float x, float y);
	void drawSkeleton(const ofPoint & point);
	void drawSkeleton(const ofPoint & point, float w, float h);
	void drawSkeleton(const ofRectangle & rect);

	void drawLabel(float x, float y);
	void drawLabel(float , float y, float w, float h);
	void drawLabel(const ofPoint & point);
	void drawLabel(const ofPoint & point, float w, float h);
	void drawLabel(const ofRectangle & rect);

	void drawAudioDirection(float x, float y);
	void drawAudioDirection(const ofPoint& point);
	
	ofPixels& getVideoPixels();
	ofPixels& getDepthPixels();
	ofPixels& getLabelPixels();
	ofPixels& getCalibratedVideoPixels();
	ofShortPixels& getDistancePixels();
	std::vector<BYTE> getSoundBuffer();
	
	ofTexture & getVideoTextureReference();
	ofTexture & getDepthTextureReference();
	ofTexture & getLabelTextureReference();
	ofTexture& getCalibratedVideoTextureReference();

	void setUsesTexture(bool bUse);

	float getWidth();
	float getHeight();
	float getDepthWidth();
	float getDepthHeight();

	void loop()				{bLoop = true;}
	void noLoop()			{bLoop = false;}
	void setLoop(bool loop)	{bLoop = loop;}
	bool isLooping()		{return bLoop;}
	void setMute(bool mute)	{bMute = mute;}
	bool isMute()			{return bMute;}

	bool isFrameNew();
	
	float getDistanceAt(int x, int y);
	float getDistanceAt(ofPoint p);

	ofVec3f getWorldCoordinateFor(int x, int y);
	ofVec3f getWorldCoordinateFor(ofPoint p);

	float getAudioBeamAngle();
	float getAudioAngle();
	float getAudioAngleConfidence();


protected:
	ofPoint calcScaledSkeletonPoint(const ofPoint& skeletonPoint, float width, float height);

public:
	float fps; ///< fps when recorded

	int	width; ///< width
	int	height; ///< height
	int	depthWidth; ///< width for depth camera
	int	depthHeight; ///< height for depth camera

protected:
	FILE * f; ///< file for play
	string filename; ///< file name
	ofTexture depthTexture, videoTexture, labelTexture; ///< textures for play
	ofPixels videoPixels; ///< video pixel data
	ofPixels depthPixels; ///< video pixel data
	ofShortPixels distancePixels; ///< raw depth pixel data
	ofPixels labelPixels; ///< label pixel data
	ofPixels calibratedVideoPixels; ///< calibrated video data corresponding to depth camera
	float skeletons[kinect::nui::SkeletonFrame::SKELETON_COUNT][kinect::nui::SkeletonData::POSITION_COUNT][3]; ///< skeleton data
	ofPoint skeletonPoints[kinect::nui::SkeletonFrame::SKELETON_COUNT][kinect::nui::SkeletonData::POSITION_COUNT];	///< skeleton points
	std::vector<BYTE> soundBuffer;	///< audio
	
	float audioBeamAngle, audioAngle, audioAngleConfidence;

	bool bPlay; ///< is play?
	bool bLoop; ///< is loop?
	bool bVideo; ///< is video recorded?
	bool bDepth; ///< is depth recorded?
	bool bCalibratedVideo; ///< is depth recorded?
	bool bLabel; ///< is label recorded?
	bool bSkeleton; ///< is skeleton recorded?
	bool bAudio;	///< is audio recorded?
	bool bUsesTexture; ///< is using texture?
	bool bIsFrameNew; ///< is frame new?
	bool bMute;
	
	int lastFrameTime; ///< last frame time
};
#endif // OFX_KINECT_NUI_PLAYER_H
