#include "kinect/nui/ImageFrame.h"
#include "kinect/nui/ImageStream.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        ImageFrame::ImageFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : imageStream_( imageStream )
            , imageFrame_( imageStream.GetNextFrame( dwMillisecondsToWait ) )
        {
			if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->LockRect( 0, &lockedRect_, NULL, 0 );
			}
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        ImageFrame::~ImageFrame()
        {
			if(imageFrame_.pFrameTexture != NULL){
				imageFrame_.pFrameTexture->UnlockRect(0);
			}
            imageStream_.ReleaseFrame( imageFrame_ );
        }
 
		//----------------------------------------------------------
		/**
			@brief	Get the width
		*/
        UINT ImageFrame::Width() const
        {
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Width;
        }

		//----------------------------------------------------------
		/**
			@brief	Get the hight
		*/
        UINT ImageFrame::Height() const
        {
            NUI_SURFACE_DESC desc = { 0 };
			if(imageFrame_.pFrameTexture != NULL){
	            imageFrame_.pFrameTexture->GetLevelDesc( 0, &desc );
			}
            return desc.Height;
        }
 

		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        VideoFrame::VideoFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        VideoFrame::~VideoFrame()
        {
        }

		//----------------------------------------------------------
		/**
			@brief	Data cordinated
		*/
        UINT VideoFrame::operator () ( UINT x, UINT y )
        {
#ifdef NUI_IMAGE_TYPE_COLOR_INFRARED
			if (imageFrame_.eImageType == NUI_IMAGE_TYPE_COLOR_INFRARED)

			{
	            UINT16* video = (UINT16*)lockedRect_.pBits;
				unsigned char c = video[(Width() * y) + x] >> 8;
		        return (UINT)(c << 16 | c << 8 | c);
			}
			else
#endif
			{
	            UINT* video = (UINT*)lockedRect_.pBits;
				return video[(Width() * y) + x];
			}
        }

    
		//----------------------------------------------------------
        /**
			@brief	Constructor
		*/
        DepthFrame::DepthFrame( ImageStream& imageStream, DWORD dwMillisecondsToWait /*= 0*/ )
            : ImageFrame( imageStream, dwMillisecondsToWait )
        {
        }

		//----------------------------------------------------------
        /**
			@brief	Destructor
		*/
        DepthFrame::~DepthFrame()
        {
        }

		//----------------------------------------------------------
		/**
			@brief	Data cordinated
		*/
        USHORT DepthFrame::operator () ( UINT x, UINT y )
        {
            USHORT* depth = (USHORT*)lockedRect_.pBits;
            UCHAR* d = (UCHAR*)&depth[(Width() * y) + x];
            return  (USHORT)(d[0] | d[1] << 8 );
        }
    } // namespace nui
} // namespace kinect