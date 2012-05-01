/******************************************************************/
/**
 * @file	ofxKinectNuiDrawInterface.h
 * @brief	draw interface (for ofxKinectNui)
 * @note
 * @todo
 * @bug	
 *
 * @author	nariakiiwatani
 * @date	Apl. 28, 2012
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_DRAW_INTERFACE_H
#define OFX_KINECT_NUI_DRAW_INTERFACE_H

#include "ofPixels.h"
#include "ofPoint.h"

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////

class IDrawPixels {
public:
	virtual void setSource(ofPixels& src)=0;
	virtual void draw()=0;
};

class IDrawPoints {
public:
	virtual void draw(ofPoint* src)=0;
};

#endif // OFX_KINECT_NUI_DRAW_INTERFACE_H