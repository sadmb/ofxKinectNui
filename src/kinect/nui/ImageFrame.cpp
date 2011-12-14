/******************************************************************/
/**
 * @file	ImageFrame.cpp
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
#include "kinect/nui/ImageFrame.h"
#include "kinect/nui/ImageStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
        ImageFrame::ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : imageStream_( imageStream )
            , imageFrame_( imageStream.GetNextFrame( dwMillisecondsToWait ) )
        {
			if(imageFrame_ != NULL){
	            imageFrame_->pFrameTexture->LockRect( 0, &lockedRect_, 0, 0 );
			}
        }

		//----------------------------------------------------------
        ImageFrame::~ImageFrame()
        {
            imageStream_.ReleaseFrame( imageFrame_ );
        }
 
		//----------------------------------------------------------
        UINT ImageFrame::Width() const
        {
			// Kinect SDK beta2 compatible, Nov. 2, 2011
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_ != NULL){
	            imageFrame_->pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Width;
        }

		//----------------------------------------------------------
        UINT ImageFrame::Height() const
        {
			// Kinect SDK beta2 compatible, Nov. 2, 2011
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_ != NULL){
	            imageFrame_->pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Height;
        }
 

		//----------------------------------------------------------
        VideoFrame::VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

		//----------------------------------------------------------
        VideoFrame::~VideoFrame()
        {
        }

		//----------------------------------------------------------
        UINT VideoFrame::operator () ( UINT x, UINT y )
        {
            UINT* video = (UINT*)lockedRect_.pBits;
            return video[(Width() * y) + x];
        }

    
		//----------------------------------------------------------
        DepthFrame::DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

		//----------------------------------------------------------
        DepthFrame::~DepthFrame()
        {
        }

		//----------------------------------------------------------
        USHORT DepthFrame::operator () ( UINT x, UINT y )
        {
            USHORT* depth = (USHORT*)lockedRect_.pBits;
            UCHAR* d = (UCHAR*)&depth[(Width() * y) + x];
            return  (USHORT)(d[0] | d[1] << 8 );
        }
    }
}