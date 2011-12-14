/******************************************************************/
/**
 * @file	SkeletonFrame.h
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
#ifndef KINECT_NUI_SKELETON_FRAME_H
#define KINECT_NUI_SKELETON_FRAME_H

#include <memory>
#include <tuple>

#include <Windows.h>
#include <MSR_NuiApi.h>

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				forward declaration					//
		//////////////////////////////////////////////////////
		class SkeletonFrame;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	SkeletonData
		 * @brief	Skeleton data for kinect video/ depth camera
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class SkeletonData
		{
			friend class SkeletonFrame;

		public:
			/**
			 * @brief Point struct for skeleton joint
			 */
			struct Point {
				float   x;
				float   y;
				USHORT  depth;

				Point() : x( 0 ), y( 0 ) {}
			};

			static const int POSITION_COUNT = NUI_SKELETON_POSITION_COUNT; ///< number of skeleton joints

			/**
			 * @brief transform skeleton data to Point struct
			 */
			Point TransformSkeletonToDepthImage( int index )
			{
				Point p;
				NuiTransformSkeletonToDepthImageF( skeletonData_.SkeletonPositions[index], &p.x, &p.y, &p.depth );
				return p;
			}

			/**
			 * @brief Get tracking state
			 */
			NUI_SKELETON_TRACKING_STATE TrackingState() const { return skeletonData_.eTrackingState; }

			Vector4 operator [] ( int index )
			{
				return skeletonData_.SkeletonPositions[index];
			}

			/**
			 * @brief Get user index
			 */
			DWORD GetUserIndex() const { return skeletonData_.dwUserIndex; }

		private:

			/**
			 * @brief Constructor
			 */
			SkeletonData( NUI_SKELETON_DATA& skeletonData )
				: skeletonData_( skeletonData )
			{
			}

		private:

			NUI_SKELETON_DATA&   skeletonData_; ///< skeleton data
		};


		//////////////////////////////////////////////////////
		//				forward declaration					//
		//////////////////////////////////////////////////////
		class SkeletonEngine;

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	SkeletonFrame
		 * @brief	Skeleton frame for kinect video/ depth camera
		 * @note	
		 * @date	Nov. 21, 2011
		 */
		/****************************************/
		class SkeletonFrame
		{
			friend class SkeletonEngine;

		public:

			static const int SKELETON_COUNT = NUI_SKELETON_COUNT; ///< number of skeletons

			/**
			 * @biref	Destructor
			 */
			virtual ~SkeletonFrame();

			/**
			 * @biref	Transform smoothing data
			 */
			void TransformSmooth( const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams = 0 );

			/**
			 * @biref	If skeleton is found
			 * @return	true when a skeleton founded
			 */
			bool IsFoundSkeleton() const;

			SkeletonData operator [] ( int index ) { return SkeletonData( skeletonFrame_.SkeletonData[index] ); }

		private:

			/**
			 * @biref	Constructor
			 */
			SkeletonFrame( std::shared_ptr< INuiInstance >& instance, DWORD dwMillisecondsToWait = 0 );

			/**
			 * @biref	Get the next frame
			 */
			void GetNextFrame( DWORD dwMillisecondsToWait = 0 );

		protected:

			std::shared_ptr< INuiInstance > instance_; ///< kinect instance

			NUI_SKELETON_FRAME skeletonFrame_; ///< skeleton frame
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_SKELETON_FRAME_H
