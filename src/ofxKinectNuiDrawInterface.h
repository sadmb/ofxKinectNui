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
#include "ofRectangle.h"
#include "ofPoint.h"

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////

class IDrawPixels {
public:
	virtual void setDrawArea(const ofRectangle & r){}
	virtual void setDrawArea(const ofPoint & p, float w, float h){}
	virtual void setDrawArea(float x, float y, float w, float h){}
	virtual void setDrawArea(float x, float y, float z, float w, float h){}
	virtual void setDrawArea(const ofPoint & p){}
	virtual void setDrawArea(float x, float y){}
	virtual void setDrawArea(float x, float y, float z){}
	virtual void setDrawArea(const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, const ofPoint& p4){}
	virtual void setSource(ofPixels& src)=0;
	virtual void draw()=0;
};

class IDrawPoints {
public:
	virtual void draw(ofPoint* src)=0;
};

#endif // OFX_KINECT_NUI_DRAW_INTERFACE_H