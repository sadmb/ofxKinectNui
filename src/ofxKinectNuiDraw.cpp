
#include "ofxKinectNuiDraw.h"
#include "ofPolyline.h"

ofxKinectNuiDrawTexture* ofxKinectNuiDrawTexture::createTextureForVideo(NUI_IMAGE_RESOLUTION resolution){
	ofxKinectNuiDrawTexture* ret = new ofxKinectNuiDrawTexture();
	ret->allocateTexture(resolution, GL_RGB);
	return ret;
}
ofxKinectNuiDrawTexture* ofxKinectNuiDrawTexture::createTextureForDepth(NUI_IMAGE_RESOLUTION resolution){
	ofxKinectNuiDrawTexture* ret = new ofxKinectNuiDrawTexture();
	ret->allocateTexture(resolution, GL_LUMINANCE);
	return ret;
}
ofxKinectNuiDrawTexture* ofxKinectNuiDrawTexture::createTextureForLabel(NUI_IMAGE_RESOLUTION resolution){
	ofxKinectNuiDrawTexture* ret = new ofxKinectNuiDrawTexture();
	ret->allocateTexture(resolution, GL_RGBA);
	return ret;
}
void ofxKinectNuiDrawTexture::destroy(){
	texture_.clear();
	delete this;
}

void ofxKinectNuiDrawTexture::allocateTexture(NUI_IMAGE_RESOLUTION resolution, int glInternalFormat){
	switch(resolution) {
		case NUI_IMAGE_RESOLUTION_80x60:
			allocateTexture(80, 60, glInternalFormat);
			break;
		case NUI_IMAGE_RESOLUTION_320x240:
			allocateTexture(320, 240, glInternalFormat);
			break;
		case NUI_IMAGE_RESOLUTION_640x480:
			allocateTexture(640, 480, glInternalFormat);
			break;
		case NUI_IMAGE_RESOLUTION_1280x960:
			allocateTexture(1280, 960, glInternalFormat);
			break;
	}
}
void ofxKinectNuiDrawTexture::allocateTexture(int w, int h, int glInternalFormat){
	if(texture_.isAllocated()) {
		ofLog(OF_LOG_ERROR, "ofxKinectNuiDrawTexture: already allocated.");
	}
	texture_.allocate(w, h, glInternalFormat);
}


void ofxKinectNuiDrawTexture::draw(){
	switch(drawType_) {
		case AREA_TYPE_XYZWH:
			texture_.draw(x_, y_, z_, w_, h_);
			break;
		case AREA_TYPE_POINTS:
			texture_.draw(p_[0], p_[1], p_[2], p_[3]);
			break;
	}
}
void ofxKinectNuiDrawTexture::setSource(ofPixels& src){
	texture_.loadData(src);
}

void ofxKinectNuiDrawTexture::setDrawArea(const ofRectangle & r){
	setDrawArea(r.x, r.y, 0, r.width, r.height);
}
void ofxKinectNuiDrawTexture::setDrawArea(const ofPoint & p, float w, float h){
	setDrawArea(p.x, p.y, p.z, w, h);
}
void ofxKinectNuiDrawTexture::setDrawArea(float x, float y, float w, float h){
	setDrawArea(x, y, 0, w, h);
}
void ofxKinectNuiDrawTexture::setDrawArea(float x, float y, float z, float w, float h){
	drawType_ = AREA_TYPE_XYZWH;
	x_ = x;
	y_ = y;
	z_ = z;
	w_ = w;
	h_ = h;
}
void ofxKinectNuiDrawTexture::setDrawArea(const ofPoint & p){
	setDrawArea(p.x, p.y, p.z, texture_.getWidth(), texture_.getHeight());
}
void ofxKinectNuiDrawTexture::setDrawArea(float x, float y){
	setDrawArea(x, y, 0, texture_.getWidth(), texture_.getHeight());
}
void ofxKinectNuiDrawTexture::setDrawArea(float x, float y, float z){
	setDrawArea(x, y, z, texture_.getWidth(), texture_.getHeight());
}
void ofxKinectNuiDrawTexture::setDrawArea(const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, const ofPoint& p4){
	drawType_ = AREA_TYPE_POINTS;
	p_[0] = p1;
	p_[1] = p2;
	p_[2] = p3;
	p_[3] = p4;
}


//=========================================================================
//=========================================================================
//=========================================================================

void ofxKinectNuiDrawSkeleton::setLineColor(const ofColor& color){
	lineColor_ = color;
}
void ofxKinectNuiDrawSkeleton::setPointColor(const ofColor& color){
	pointColor_ = color;
}


void ofxKinectNuiDrawSkeleton::draw(ofPoint* src){
	ofPolyline pLine;
	ofPushStyle();
	ofSetColor(lineColor_);
	ofNoFill();
	ofSetLineWidth(4);
	// HEAD
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SPINE].x, src[NUI_SKELETON_POSITION_SPINE].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HEAD].x, src[NUI_SKELETON_POSITION_HEAD].y);
	pLine.draw();
	
	// BODY_LEFT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_LEFT].x, src[NUI_SKELETON_POSITION_SHOULDER_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ELBOW_LEFT].x, src[NUI_SKELETON_POSITION_ELBOW_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_WRIST_LEFT].x, src[NUI_SKELETON_POSITION_WRIST_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HAND_LEFT].x, src[NUI_SKELETON_POSITION_HAND_LEFT].y);
	pLine.draw();

	// BODY_RIGHT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_CENTER].x, src[NUI_SKELETON_POSITION_SHOULDER_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x, src[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ELBOW_RIGHT].x, src[NUI_SKELETON_POSITION_ELBOW_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_WRIST_RIGHT].x, src[NUI_SKELETON_POSITION_WRIST_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HAND_RIGHT].x, src[NUI_SKELETON_POSITION_HAND_RIGHT].y);
	pLine.draw();

	// LEG_LEFT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_LEFT].x, src[NUI_SKELETON_POSITION_HIP_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_KNEE_LEFT].x, src[NUI_SKELETON_POSITION_KNEE_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ANKLE_LEFT].x, src[NUI_SKELETON_POSITION_ANKLE_LEFT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_FOOT_LEFT].x, src[NUI_SKELETON_POSITION_FOOT_LEFT].y);
	pLine.draw();

	// LEG_RIGHT
	pLine.clear();
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_CENTER].x, src[NUI_SKELETON_POSITION_HIP_CENTER].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_HIP_RIGHT].x, src[NUI_SKELETON_POSITION_HIP_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_KNEE_RIGHT].x, src[NUI_SKELETON_POSITION_KNEE_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_ANKLE_RIGHT].x, src[NUI_SKELETON_POSITION_ANKLE_RIGHT].y);
	pLine.addVertex(src[NUI_SKELETON_POSITION_FOOT_RIGHT].x, src[NUI_SKELETON_POSITION_FOOT_RIGHT].y);
	pLine.draw();

	ofSetColor(pointColor_);
	ofSetLineWidth(0);
	ofFill();
	for(int i = 0; i < NUI_SKELETON_POSITION_COUNT; ++i){
		ofCircle(src[i].x, src[i].y, 5);
	}
	ofPopStyle();
}
