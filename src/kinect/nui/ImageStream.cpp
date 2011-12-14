/******************************************************************/
/**
 * @file	ImageStream.cpp
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
#include "kinect/nui/ImageStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
        ImageStream::ImageStream()
            : instance_( NULL )
            , hStream_( 0 )
            , event_( 0 )
        {
        }

		//----------------------------------------------------------
        ImageStream::~ImageStream()
        {
        }
		
		//----------------------------------------------------------
		void ImageStream::CopyInstance( std::shared_ptr< INuiInstance >& instance )
		{
			instance_ = instance;
		}

		//----------------------------------------------------------
		void ImageStream::Open( NUI_IMAGE_TYPE eImageType, NUI_IMAGE_RESOLUTION eResolution, DWORD dwImageFrameFlags_NotUsed /*= 0*/,
								DWORD dwFrameLimit /*= 2*/ )
		{
			HRESULT ret = instance_->NuiImageStreamOpen( eImageType, eResolution, dwImageFrameFlags_NotUsed,
														dwFrameLimit, event_.get(), &hStream_ );
			resolution_ = eResolution;
			if (FAILED(ret)) {
				return;
			}

            ::NuiImageResolutionToSize( eResolution, (DWORD&)width_, (DWORD&)height_ );
		}

		//----------------------------------------------------------
		const NUI_IMAGE_FRAME* ImageStream::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			const NUI_IMAGE_FRAME* pImageFrame = 0;
			HRESULT ret = instance_->NuiImageStreamGetNextFrame( hStream_, dwMillisecondsToWait, &pImageFrame );
			if (FAILED(ret)) {
				return NULL;
			}

			return pImageFrame;
		}

		//----------------------------------------------------------
        void ImageStream::ReleaseFrame( CONST NUI_IMAGE_FRAME *pImageFrame )
        {
            HRESULT ret = instance_->NuiImageStreamReleaseFrame( hStream_, pImageFrame );
			if (FAILED(ret)) {
				return;
			}
        }
    }
}