#include "ofxKinectNuiSoundRecorder.h"
#include <tchar.h>

//---------------------------------------------------------------------------
/**
	@brief	Constructor
*/
ofxKinectNuiSoundRecorder::ofxKinectNuiSoundRecorder() {
	bIsActive = false;
}

//---------------------------------------------------------------------------
/**
	@brief	Destructor
*/
ofxKinectNuiSoundRecorder::~ofxKinectNuiSoundRecorder() {
	close();
}


//---------------------------------------------------------------------------
/**
	@brief	Setup recorder
	@param	kinect		which kinect to record
	@param	filename	filename to output data
*/
void ofxKinectNuiSoundRecorder::setup(ofxKinectNui& kinect, const string& filename) {
	mKinect = &kinect;
	string path = ofToDataPath(filename);
	int	len = MultiByteToWideChar( CP_ACP, 0, path.c_str(), -1, NULL, 0 );
	WCHAR *	buf = new WCHAR[ len + 1 ];
    MultiByteToWideChar( CP_ACP, 0, path.c_str(), -1, buf, len + 1 );
	*( buf + len ) = '\0';
	waveWriter.Initialize(buf, 16000, 16, 1);
	bIsActive = true;
}

//---------------------------------------------------------------------------
/**
	@brief	Update recording
*/
void ofxKinectNuiSoundRecorder::update() {
	std::vector<BYTE> buffer = mKinect->getSoundBuffer();
	waveWriter.Write(buffer, buffer.size());
}

//---------------------------------------------------------------------------
/**
	@brief	Close the recorder
*/
void ofxKinectNuiSoundRecorder::close() {
	waveWriter.Flush();
	bIsActive = false;
}

//---------------------------------------------------------------------------
/**
	@brief		Is recording?
	@return		true if is recording 
*/
bool ofxKinectNuiSoundRecorder::isActive(){
	return bIsActive;
}
