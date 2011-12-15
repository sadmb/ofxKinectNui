/******************************************************************/
/**
 * @file	ofxKinectNuiCalibration.h
 * @brief	kinect depth calibration for oF
 * @note	ofxKinectCalibration.h created by arturo: 03/01/2011
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 28, 2011
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_CALIBRATION_H
#define OFX_KINECT_NUI_CALIBRATION_H

#include "kinect\nui\Kinect.h"
#include "ofMain.h"


//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
/****************************************/
/**
 * @class	ofxKinectNuiPlayer.h
 * @brief	kinect player for a kinect official device
 * @note	
 * @author	sadmb
 * @date		Oct. 26, 2011
 */
/****************************************/
class ofxKinectNuiCalibration {
public:
	ofxKinectNuiCalibration();
	virtual ~ofxKinectNuiCalibration();

	void init(NUI_IMAGE_RESOLUTION videoResolution = NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_320x240);
	void update(unsigned short * depth);
	void clear();

	/**
		set the near value of the pixels in the greyscale depth image to white?

		bEnabled = true : pixels close to the camera are brighter (default)
		bEnabled = false: pixels closer to the camera are darker
	**/
	void enableDepthNearValueWhite(bool bEnabled=true);
	bool isDepthNearValueWhite();


	/// calculates the coordinate in the world for the pixel (perspective calculation). Center  of image is (0.0)
	ofVec3f getWorldCoordinateFor(int x, int y);
	ofVec3f getWorldCoordinateFor(int x, int y, double z);


	float getDistanceAt(int x, int y);
	float getDistanceAt(const ofPoint & p);
	
	static void setClippingInMilimeters(float near, float far);
	static float getNearClipping();
	static float getFarClipping();

	unsigned char 	* getDepthPixels();		// grey scale values
	/// get the distance in milimeters to a given point
	float* getDistancePixels();

	int	width;
	int	height;
	int	depthWidth;
	int	depthHeight;
private:
	static void calculateLookups();
	static bool lookupsCalculated;
	static float distancePixelsLookup[65535];
	static unsigned char depthPixelsLookupNearWhite[65535];
	static unsigned char depthPixelsLookupFarWhite[65535];
	static double fx_d;
	static double fy_d;
	static float cx_d;
	static float cy_d;
	
	static float nearClipping, farClipping;
	
	static float rawToMilimeters(unsigned short raw);
	static unsigned short milimetersToRaw(float milimeters);
	
	float * 				distancePixels;
	unsigned char *			depthPixels;

	bool bDepthNearValueWhite;

};
#endif // OFX_KINECT_NUI_CALIBRATION_H
