/******************************************************************/
/**
 * @file	ImageFrame.h
 * @brief	Image frame for kinect video/ depth camera
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	kaorun55
 * @author	sadmb
 * @date	Oct. 26, 2011 modified
 */
/******************************************************************/
#ifndef KINECT_NUI_IMAGE_FRAME_H
#define KINECT_NUI_IMAGE_FRAME_H

#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

namespace kinect {
	namespace nui {
		class ImageStream;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	ImageFrame
		 * @brief	Image frame for kinect video/ depth camera
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class ImageFrame
		{
		public:

            /**
			 * @brief	Constructor
			 */
			ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );

            /**
			 * @brief	Destructor
			 */
			virtual ~ImageFrame();

			/**
			 * @brief	Get the number of byte of each line
			 */
			INT Pitch(){ return lockedRect_.Pitch; };

			/**
			 * @brief	Get the first address of data
			 */
			void* Bits() { return lockedRect_.pBits; };

			/**
			 * @brief	Get the width
			 */
			UINT Width() const;
            
			/**
			 * @brief	Get the hight
			 */
			UINT Height() const;

		protected:

			ImageStream& imageStream_; ///< image stream corresponds

			const NUI_IMAGE_FRAME *imageFrame_;	///< Frame data
			NUI_LOCKED_RECT lockedRect_;	    ///< Rect data. Kinect SDK beta2 transition, Nov. 2, 2011
		};
		
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	ImageFrame
		 * @brief	Image frame for kinect video/ depth camera
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class VideoFrame : public ImageFrame
		{
		public:

            /**
			 * @brief	Constructor
			 */
			VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );

            /**
			 * @brief	Destructor
			 */
			virtual ~VideoFrame();

			/**
			 * @brief	Data cordinated
			 */
			UINT operator () ( UINT x, UINT y );
		};

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	DepthFrame
		 * @brief	Depth frame for Kinect depth camera
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class DepthFrame : public ImageFrame
		{
		public:
            /**
			 * @brief	Constructor
			 */
			DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait = 0 );

            /**
			 * @brief	Destructor
			 */
			virtual ~DepthFrame();

			/**
			 * @brief	Data cordinated
			 */
			USHORT operator () ( UINT x, UINT y );
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_IMAGE_FRAME_H
