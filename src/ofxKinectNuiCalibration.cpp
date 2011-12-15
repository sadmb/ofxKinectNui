/******************************************************************/
/**
 * @file	ofxKinectNuiCalibration.cpp
 * @brief	kinect depth calibration for oF
 * @note	ofxKinectCalibration.h created by arturo: 03/01/2011
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#include "ofxKinectNuiCalibration.h"

#ifdef _MSC_VER
	// round() is C99. MSVC doesn't support C99.
	float round(float n) {
	   return n >= 0 ? int(n + 0.5) : int(n - 0.5);
	}
#endif

/*
 these values constrain the maximum distance in the depthPixels image to:
 - as near as possible (raw value of 0)
 - 4 meters away, maximum
 both near and far clipping planes should be user-settable
 */
float ofxKinectNuiCalibration::nearClipping = 0, ofxKinectNuiCalibration::farClipping = 4000;

bool ofxKinectNuiCalibration::lookupsCalculated = false;
float ofxKinectNuiCalibration::distancePixelsLookup[65535];
unsigned char ofxKinectNuiCalibration::depthPixelsLookupNearWhite[65535];
unsigned char ofxKinectNuiCalibration::depthPixelsLookupFarWhite[65535];

double ofxKinectNuiCalibration::fx_d = 1.0 / 5.9421434211923247e+02;
double ofxKinectNuiCalibration::fy_d = 1.0 / 5.9104053696870778e+02;
float ofxKinectNuiCalibration::cx_d = 3.3930780975300314e+02;
float ofxKinectNuiCalibration::cy_d = 2.4273913761751615e+02;

//---------------------------------------------------------------------------
ofxKinectNuiCalibration::ofxKinectNuiCalibration()
{
	depthPixels				= NULL;
	distancePixels 			= NULL;
	bDepthNearValueWhite	= true;
	calculateLookups();
}


//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::setClippingInMilimeters(float nearClipping, float farClipping) {
	ofxKinectNuiCalibration::nearClipping = nearClipping;
	ofxKinectNuiCalibration::farClipping = farClipping;
	lookupsCalculated = false;
	calculateLookups();
}

//---------------------------------------------------------------------------
float ofxKinectNuiCalibration::getNearClipping() {
	return nearClipping;
}

//---------------------------------------------------------------------------
float ofxKinectNuiCalibration::getFarClipping() {
	return farClipping;
}

//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::calculateLookups() {
	if(!lookupsCalculated) {
		ofLog(OF_LOG_VERBOSE, "Setting up LUT for distance and depth values.");
		
		for(int i = 0; i < 65535; i++){
			if(i == 65534) {
				distancePixelsLookup[i] = 0;
				depthPixelsLookupNearWhite[i] = 0;
				depthPixelsLookupFarWhite[i] = 0;
			} else {
				distancePixelsLookup[i] = i;
				depthPixelsLookupFarWhite[i] = ofMap(distancePixelsLookup[i], nearClipping, farClipping, 0, 255, true);
				depthPixelsLookupNearWhite[i] = 255 - depthPixelsLookupFarWhite[i];
				// set outside of clipped area white
/*				if(depthPixelsLookupFarWhite[i] == 0 || depthPixelsLookupNearWhite[i] == 0){
					depthPixelsLookupFarWhite[i] = 255;
					depthPixelsLookupNearWhite[i] = 255;
				}*/
			}
		}
	}
	lookupsCalculated = true;
}

//---------------------------------------------------------------------------
ofxKinectNuiCalibration::~ofxKinectNuiCalibration() {
	// TODO Auto-generated destructor stub
}

//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::init(NUI_IMAGE_RESOLUTION videoResolution /* =NUI_IMAGE_RESOLUTION_640x480*/, NUI_IMAGE_RESOLUTION depthResolution /* =NUI_IMAGE_RESOLUTION_320x240*/){
	switch(videoResolution){
	case NUI_IMAGE_RESOLUTION_1280x1024:
		width = 1280;
		height = 1024;
		break;
	case NUI_IMAGE_RESOLUTION_640x480:
		width = 640;
		height = 480;
		break;
	case NUI_IMAGE_RESOLUTION_320x240:
	case NUI_IMAGE_RESOLUTION_80x60:
	default:
		string error = "Invalid video resolution: select 1280x1024 or 640x480.";
		ofLog(OF_LOG_ERROR, "ofxKinectNuiCalibration: " + error);
		return;
	}
	switch(depthResolution){
	case NUI_IMAGE_RESOLUTION_640x480:
		depthWidth = 640;
		depthHeight = 480;
		break;
	case NUI_IMAGE_RESOLUTION_320x240:
		depthWidth = 320;
		depthHeight = 240;
		break;
	case NUI_IMAGE_RESOLUTION_80x60:
		depthWidth = 80;
		depthHeight = 60;
		break;
	case NUI_IMAGE_RESOLUTION_1280x1024:
	default:
		string error = "Invalid depth resolution: select 320x240, 80x60 or you must disable grabLabel when you select 640x480.";
		ofLog(OF_LOG_ERROR, "ofxKinectNuiCalibration: " + error);
		return;
	}
	int length = depthWidth * depthHeight;
	depthPixels = new unsigned char[length];
	distancePixels = new float[length];

	memset(depthPixels, 0, length*sizeof(unsigned char));
	memset(distancePixels, 0, length*sizeof(float));

}

//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::clear(){

	if(depthPixels != NULL){
		delete[] depthPixels; depthPixels = NULL;
		delete[] distancePixels; distancePixels = NULL;
	}
}

//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::update(unsigned short * depth){
	int n = depthWidth * depthHeight;
	if(bDepthNearValueWhite) {
		for(int i = 0; i < n; i++){
			distancePixels[i] = distancePixelsLookup[depth[i]];
			depthPixels[i] = depthPixelsLookupNearWhite[depth[i]];
		}
	} else {
		for(int i = 0; i < n; i++){
			distancePixels[i] = distancePixelsLookup[depth[i]];
			depthPixels[i] = depthPixelsLookupFarWhite[depth[i]];
		}
	}
}

//---------------------------------------------------------------------------
void ofxKinectNuiCalibration::enableDepthNearValueWhite(bool bEnabled){
	bDepthNearValueWhite = bEnabled;
}

//---------------------------------------------------------------------------
bool ofxKinectNuiCalibration::isDepthNearValueWhite(){
	return bDepthNearValueWhite;
}

//---------------------------------------------------------------------------
unsigned char * ofxKinectNuiCalibration::getDepthPixels(){
	return depthPixels;
}

//---------------------------------------------------------------------------
float * ofxKinectNuiCalibration::getDistancePixels(){
	return distancePixels;
}

//---------------------------------------------------------------------------
float ofxKinectNuiCalibration::getDistanceAt(int x, int y){
	return distancePixels[y * depthWidth + x];
}

//---------------------------------------------------------------------------
float ofxKinectNuiCalibration::getDistanceAt(const ofPoint & p){
	return getDistanceAt(p.x, p.y);
}

//---------------------------------------------------------------------------
ofVec3f ofxKinectNuiCalibration::getWorldCoordinateFor(int x, int y){
	const double depth = getDistanceAt(x,y)/1000.0;
	return getWorldCoordinateFor(x,y,depth);
}

//---------------------------------------------------------------------------
ofVec3f ofxKinectNuiCalibration::getWorldCoordinateFor(int x, int y, double z){
	//Based on http://graphics.stanford.edu/~mdfisher/Kinect.html

	ofVec3f result;
	result.x = float((x - cx_d) * z * fx_d);
	result.y = float((y - cy_d) * z * fy_d);
	result.z = z;

	return result;
}
