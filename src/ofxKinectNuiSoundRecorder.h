/******************************************************************/
/**
 * @file	ofxKinectNuiSoundRecorder.h
 * @brief	Sound recorder for a kinect sdk
 * @note
 * @todo
 * @bug	
 *
 * @author	sadmb
 * @date	Feb. 07, 2011
 */
/******************************************************************/
#ifndef OFX_KINECT_NUI_SOUND_RECORDER
#define OFX_KINECT_NUI_SOUND_RECORDER

#include <Windows.h>
#include "kinect/nui/WaveFileWriter.h"
#include "kinect/nui/WaveOutput.h"
#include "ofxKinectNui.h"
#include "ofMain.h"

//////////////////////////////////////////////////////
//				class declarations					//
//////////////////////////////////////////////////////
/****************************************/
/**
 * @class	ofxKinectNuiSoundRecorder
 * @brief	Sound recorder for a kinect sdk
 * @note	
 * @author	sadmb
 * @date	Feb. 07, 2011
 */
/****************************************/
class ofxKinectNuiSoundRecorder {
public:
	ofxKinectNuiSoundRecorder();
	virtual ~ofxKinectNuiSoundRecorder();

	void setup(ofxKinectNui& kinect, const string& filename);

	void update();
	void close();

	bool isActive();

private:
	kinect::nui::WaveFileWriter waveWriter;
	bool bIsActive;

	ofxKinectNui* mKinect;
};
#endif // OFX_KINECT_NUI_SOUND_RECORDER
