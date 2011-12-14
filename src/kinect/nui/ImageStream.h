/******************************************************************/
/**
 * @file	ImageStream.h
 * @brief	Image stream for kinect video/ depth camera
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
#ifndef KINECT_NUI_IMAGE_STREAM_H
#define KINECT_NUI_IMAGE_STREAM_H

#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "win32/Event.h"

namespace kinect {
	namespace nui {
		class Kinect;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	ImageStream
		 * @brief	Image stream for kinect video/ depth camera
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class ImageStream
		{
			friend class Kinect;

		public:
			/**
			 * @brief Destructor
			 */
			~ImageStream();

			/**
			 * @brief Open stream
			 */
			void Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution, DWORD dwImageFrameFlags_NotUsed = 0,
								DWORD dwFrameLimit = 2 );

			/**
			 * @brief Get the next frame
			 */
    		const NUI_IMAGE_FRAME* GetNextFrame( DWORD dwMillisecondsToWait = 0 );

			/**
			 * @brief Release the frame
			 */
			void ReleaseFrame( CONST NUI_IMAGE_FRAME *pImageFrame );

			/**
			 * @brief Wait
			 */
			bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

			/**
			 * @brief Get width
			 */
			UINT Width() const { return width_; }

			/**
			 * @brief Get height
			 */
			UINT Height() const { return height_; }

			/**
			 * @brief Get resolution
			 */
			NUI_IMAGE_RESOLUTION Resolution() const { return resolution_; }


		private:

			/**
			 * @brief Constructor only to be called by kinect class
			 */
			ImageStream();

			/**
			 * @brief Constructor to block copy constructor
			 * @param	rhs
			 */
			ImageStream( const ImageStream& rhs );

			/**
			 * @brief Copy instance
			 * @param	instance	pointer of NuiInstance
			 */
			void CopyInstance( std::shared_ptr< INuiInstance >& instance );
		private:

			std::shared_ptr< INuiInstance > instance_; ///< kinect instance
			HANDLE hStream_; ///< handle
			win32::Event    event_; ///< event handle
			NUI_IMAGE_RESOLUTION resolution_; ///< resolution of this stream

			UINT    width_; ///< width
			UINT    height_; ///< height
		};

	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_IMAGE_STREAM_H
