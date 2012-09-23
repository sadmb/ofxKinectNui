/******************************************************************/
/**
 * @file	ofxKinectNuiDraw.h
 * @brief	drawer for ofxKinectNui
 * @note
 * @todo
 * @bug	
 *
 * @author	nariakiiwatani
 * @date	Apl. 28, 2012
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_DRAW_H
#define OFX_KINECT_NUI_DRAW_H

#include <Windows.h>
#include <NuiApi.h>

#include "ofxKinectNuiDrawInterface.h"
#include "ofTexture.h"
#include "ofGraphics.h"

////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
class ofxKinectNuiDrawTexture : public IDrawPixels {
public:
	static ofxKinectNuiDrawTexture* createTextureForVideo(NUI_IMAGE_RESOLUTION resolution);
	static ofxKinectNuiDrawTexture* createTextureForDepth(NUI_IMAGE_RESOLUTION resolution);
	static ofxKinectNuiDrawTexture* createTextureForLabel(NUI_IMAGE_RESOLUTION resolution);
	virtual void destroy();
	virtual void setSource(ofPixels& src);

	void draw();

	void setDrawArea(const ofRectangle & r);
	void setDrawArea(const ofPoint & p, float w, float h);
	void setDrawArea(float x, float y, float w, float h);
	void setDrawArea(float x, float y, float z, float w, float h);
	void setDrawArea(const ofPoint & p);
	void setDrawArea(float x, float y);
	void setDrawArea(float x, float y, float z);
	void setDrawArea(const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, const ofPoint& p4);

private:
	void allocateTexture(NUI_IMAGE_RESOLUTION resolutio, int glInternalFormat);
	void allocateTexture(int w, int h, int glInternalFormat);
	ofxKinectNuiDrawTexture(){}
	~ofxKinectNuiDrawTexture(){}

private:
	ofTexture texture_;
	enum AreaType {
		AREA_TYPE_XYZWH,
		AREA_TYPE_POINTS,
	} drawType_;
	
	float x_,y_,z_,w_,h_;
	ofPoint p_[4];
};

class ofxKinectNuiDrawSkeleton : public IDrawPoints {
public:
	void setLineColor(const ofColor& color);
	void setPointColor(const ofColor& color);
	void draw(ofPoint* src);
private:
	
	ofColor lineColor_;
	ofColor pointColor_;
};


#endif // OFX_KINECT_NUI_DRAW_H