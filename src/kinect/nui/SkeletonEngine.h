/******************************************************************/
/**
 * @file	SkeletonEngine.h
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
#ifndef KINECT_NUI_SKELETON_ENGINE_H
#define KINECT_NUI_SKELETON_ENGINE_H

#include <memory>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "kinect/nui/SkeletonFrame.h"

#include "win32/Event.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				forward declaration					//
		//////////////////////////////////////////////////////
		class Kinect;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	SkeletonEngine
		 * @brief	Skeleton engine for kinect
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class SkeletonEngine
		{
			friend class Kinect;

		public:
			/**
			 * @brief	Destructor
			 */
			~SkeletonEngine();

			/**
			 * @brief	Enable the capturing of Skeleton data
			 */
			void Enable( DWORD dwFlags = 0 );

			/**
			 * @brief	Disable the capturing of Skeleton data
			 */
			void Disable();

			/**
			 * @brief	Get the next frame
			 */
			SkeletonFrame GetNextFrame( DWORD dwMillisecondsToWait = 0 );

			/**
			 * @brief	Is the capturing of skeleton data enabled?
			 * @return	true when enabled
			 */
			bool IsEnabled() const { return isEnabled_; }

			/**
			 * @brief	Wait
			 */
			bool Wait( DWORD dwMilliseconds = INFINITE ) { return event_.Wait( dwMilliseconds ); }

		private:

			/**
			 * @brief	Prohibit constructor
			 */
			SkeletonEngine();

			/**
			 * @brief	Prohibit copy constructor
			 */
			SkeletonEngine( const SkeletonEngine& rhs );

			/**
			 * @brief	Copy kinect instance
			 */
			void CopyInstance ( std::shared_ptr< INuiInstance >& instance );

		private:

			std::shared_ptr< INuiInstance > instance_; ///< pointer for kinect instance 
			win32::Event    event_; ///< event handle

			bool isEnabled_;
		};

	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_SKELETON_ENGINE_H
