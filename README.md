# Microsoft Kinect for Windows SDK addon for openFrameworks
version 0.04 for Visual Studio 2012 with openFrameworks 0.8.0.

## License
built by sadmb  
facebook: sadmb  
twitter: @sadmb  
mail: sadam@sadmb.com

MIT license  
http://ja.wikipedia.org/wiki/MIT_License

## Disclaimer
Users must take full responsibility while using this software.  
We will not be liable for any loss or damage caused by this program.

## Requirement
1: Visual Studio 2012, openFrameworks 0.8.0 and latest Kinect for Windows SDK.
2: Place this to openframeworks addons folder.

## How to Use
1: Use projectGenerator, check ofxKinectNui addon and generate project files with platforms: windows visual studio.
2: Add "$(KINECTSDK10_DIR)\inc" value to Project Property -> Configuration Properties -> C/C++ -> Additional Include Directories.
3: Add "$(KINECTSDK10_DIR)\lib\x86" value to Project Property -> Configuration Properties -> Linker -> Additional Library Directories.
4: Add "Kinect10.lib" value to Project Property -> Configuration Properties -> Linker -> Input -> Additional Dependencies.

5: If you want to use audio stream, then you have to add "USES_KINECT_AUDIOSTREAM" value to Project Property -> Configuration Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions.