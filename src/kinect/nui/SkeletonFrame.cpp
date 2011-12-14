/******************************************************************/
/**
 * @file	SkeletonFrame.cpp
 * @brief	Skeleton frame and skeleton data for kinect video/ depth camera
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
#include "kinect/nui/SkeletonFrame.h"
#include "kinect/nui/SkeletonEngine.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		SkeletonFrame::SkeletonFrame( std::shared_ptr< INuiInstance >& instance, DWORD dwMillisecondsToWait /*= 0*/ )
			: instance_( instance )
		{
			GetNextFrame( dwMillisecondsToWait );
		}

		//----------------------------------------------------------
		SkeletonFrame::~SkeletonFrame()
		{
		}

		//----------------------------------------------------------
		void SkeletonFrame::TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams /*= 0*/ )
		{
			HRESULT ret = instance_->NuiTransformSmooth( &skeletonFrame_, pSmoothingParams );
			if (FAILED(ret)) {
				// TODO fail announce
				return;
			}
		}

		//----------------------------------------------------------
		bool SkeletonFrame::IsFoundSkeleton() const
		{
			for( int i = 0; i < NUI_SKELETON_COUNT; ++i ) {
				if( skeletonFrame_.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ) {
					return true;
				}
			}

			return false;
		}
 
		//----------------------------------------------------------
		void SkeletonFrame::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			HRESULT ret = instance_->NuiSkeletonGetNextFrame( dwMillisecondsToWait, &skeletonFrame_ );
			if (FAILED(ret)) {
				// TODO fail announce
				return;
			}
		}
	}
}
