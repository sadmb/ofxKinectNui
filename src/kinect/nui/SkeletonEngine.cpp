/******************************************************************/
/**
 * @file	SkeletonEngine.cpp
 * @brief	Skeleton engine for kinect
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
#include "kinect/nui/SkeletonEngine.h"

namespace kinect {
	namespace nui {
		//----------------------------------------------------------
		SkeletonEngine::SkeletonEngine()
			: instance_(NULL)
			, event_( 0 )
			, isEnabled_( false )
		{
		}

		//----------------------------------------------------------
		SkeletonEngine::~SkeletonEngine()
		{
		}
		
		//----------------------------------------------------------
		void SkeletonEngine::CopyInstance( std::shared_ptr< INuiInstance >& instance )
		{
			instance_ = instance;
		}

		//----------------------------------------------------------
		void SkeletonEngine::Enable( DWORD dwFlags /*= 0*/ )
		{
			HRESULT ret = instance_->NuiSkeletonTrackingEnable( event_.get(), dwFlags );
			if (FAILED(ret)) {
				return;
			}

			isEnabled_ = true;
		}

		//----------------------------------------------------------
		void SkeletonEngine::Disable()
		{
			HRESULT ret = instance_->NuiSkeletonTrackingDisable();
			if (FAILED(ret)) {
				return;
			}

			isEnabled_ = false;
		}
 
 		//----------------------------------------------------------
		SkeletonFrame SkeletonEngine::GetNextFrame( DWORD dwMillisecondsToWait /*= 0*/ )
		{
			return SkeletonFrame( instance_, dwMillisecondsToWait );
		}
	} // namespace nui
} // namespace kinect