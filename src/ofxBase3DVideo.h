/*
 * of3DVideo.h
 *
 *  Created on: 16/12/2010
 *      Author: arturo
 *  Modified on: 23/12/2011
 *      Author: sadmb
 */

#pragma once

class ofxBase3DVideo: public ofBaseVideo{
public:
	// needs implementing
	virtual ofPixels& getVideoPixels() = 0;			///< rgb scaled values from video camera
	virtual ofPixels& getDepthPixels() = 0;			///< gray scaled values from depth camera
	virtual ofShortPixels& getDistancePixels() = 0;	///< disntance values (in mm) from depth camera 
	virtual ofPixels& getLabelPixels() = 0;			///< rgba scaled value of players label
	virtual ofPixels& getCalibratedVideoPixels() = 0;	///< rgb scaled value from video camera adjusted to depth images 
	
	// implemented
	ofPixels& getPixelsRef(){return getVideoPixels();} ///< of007 compatible
	unsigned char* getPixels(){return getVideoPixels().getPixels();} ///< of007 compatible

	/**
	 * @brief	constructor
	 */
	ofxBase3DVideo(): nearClippingDistance(0), farClippingDistance(4000), bIsDepthNearValueWhite(true), bCalculatesLookups(false) {
		calculateLookups();
	}

	void initLookups(unsigned short nearClippingDistance, unsigned short farClippingDistance){
		setNearClippingDistance(nearClippingDistance);
		setFarClippingDistance(farClippingDistance);
		enableDepthNearValueWhite(true);
		bCalculatesLookups = false;
		calculateLookups();
	}

	/**
	 * @brief	Enable depth near value white or not
	 * @param	enabled		true when enabled.
	 */
	void enableDepthNearValueWhite(bool enabled){
		bIsDepthNearValueWhite = enabled;
	}

	/**
	 * @brief	Is depth near value white?
	 * @return	true when depth near value white
	 */
	bool isDepthNearValueWhite(){
		return bIsDepthNearValueWhite;
	}

	/**
	 * @brief	To set near clipping distance and far clipping distance in mm.
	 * @param	clippingDistance	The nearest clipping distance from camera. default is 0.
	 */
	void setNearClippingDistance(unsigned short clippingDistance){
		nearClippingDistance = clippingDistance;
		bCalculatesLookups = false;
		calculateLookups();
	}

	/**
	 * @brief	To get the nearest clipping distance
	 * @return	The nearest clipping distance
	 */
	unsigned short getNearClippingDistance(){
		return nearClippingDistance;
	}

	/**
	 * @brief	To set far clipping distance and far clipping distance in mm.
	 * @param	clippingDistance	The farthest clipping distance from camera. default is 0.
	 */
	void setFarClippingDistance(unsigned short clippingDistance){
		farClippingDistance = clippingDistance;
		bCalculatesLookups = false;
		calculateLookups();
	}

	/**
	 * @brief	To get the farthest clipping distance
	 * @return	The farthest clipping distance
	 */
	unsigned short getFarClippingDistance(){
		return farClippingDistance;
	}

protected:
	/**
	 * @brief	To get world coordinate scale value from depth position
	 * @param	depthX	x position
	 * @param	depthY	y position
	 * @param	depthZ	distance at (x, y)
	 * @return	ofPoint
	 */
	ofPoint getWorldCoordinateFor(int depthX, int depthY, double depthZ){
		ofPoint pt3d;
		pt3d.x = (depthX - depthWidth/2.0f) * (float)(320.0f/depthWidth) * depthZ * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240;
		pt3d.y = (depthY - depthHeight/2.0f) * (float)(240.0f/depthHeight) * depthZ * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240;
		pt3d.z = depthZ;
		return pt3d;
	}

	/**
	 * @brief	To calculate lookups for depth pixels
	 */
	void calculateLookups(){
		if(!bCalculatesLookups) {
			bCalculatesLookups = true;
			ofLog(OF_LOG_VERBOSE, "Setting up LUT for distance and depth values.");
			
			for(int i = 0; i < USHRT_MAX; i++){
				if(i == (USHRT_MAX - 1)) {
					depthPixelsLookupNearWhite[i] = 0;
					depthPixelsLookupFarWhite[i] = 0;
				} else {
					depthPixelsLookupFarWhite[i] = ofMap(i, nearClippingDistance, farClippingDistance, 0, 255, true);
					depthPixelsLookupNearWhite[i] = 255 - depthPixelsLookupFarWhite[i];
					if(depthPixelsLookupFarWhite[i] == 255){
						depthPixelsLookupFarWhite[i] = 0;
					}
					if(depthPixelsLookupNearWhite[i] == 255){
						depthPixelsLookupNearWhite[i] = 0;
					}
				}
			}
		}
	}

	int width;		///<	width of video stream
	int height;		///<	height of depth stream
	int depthWidth;		///<	width of depth stream
	int depthHeight;	///<	height of depth stream

	bool bCalculatesLookups;		///< calculates lookups?
	bool bIsDepthNearValueWhite;	///< is near value of depth white?
	unsigned short nearClippingDistance, farClippingDistance;	///< clipping distance

	unsigned char depthPixelsLookupNearWhite[USHRT_MAX];	///< lookup for depth pixels when near white
	unsigned char depthPixelsLookupFarWhite[USHRT_MAX];		///< lookup for depth pixels when far white
};
