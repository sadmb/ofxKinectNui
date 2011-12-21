/******************************************************************/
/**
 * @file	Kinect.h
 * @brief	Kinect wrapper class for cpp
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
#ifndef KINECT_NUI_KINECT_H
#define KINECT_NUI_KINECT_H

#include <memory>
#include <map>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "kinect/nui/ImageStream.h"
#include "kinect/nui/SkeletonEngine.h"
#include "kinect/nui/ImageFrame.h"
#include "kinect/nui/KinectContext.h"
#include "kinect/nui/KinectListener.h"

namespace kinect {
	namespace nui {

		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	Kinect
		 * @brief	Kinect wrapper class for cpp
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class Kinect
		{
		public:

			/**
			 * @brief	Constructor
			 * @param	index	Index number of Kinect instance
			 * @arg		-1 when search next available device
			 */
			Kinect(int index = -1);

			/**
			 * @brief	Constructor
			 * @param	deviceName	Device Name of the kinect sensor
			 * @details	To know the device name, open control panel, see property of Microsoft Kinect -> Microsoft Kinect Device, select Details tab, choose "Parent" property and it shows on Value.
			 */
			Kinect(BSTR deviceName);

			/**
			 * @brief	Destructor
			 */
			~Kinect();

			/**
			 * @brief	Initialize Kinect
			 * @param	dwFlags Initializing Flags
			 * @arg		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX	Get depth data and player index
			 * @arg		NUI_INITIALIZE_FLAG_USES_COLOR					Get video data
			 * @arg		NUI_INITIALIZE_FLAG_USES_SKELETON				Get skeleton data
			 * @arg		NUI_INITIALIZE_FLAG_USES_DEPTH					Get depth data
			 */
			void Initialize( DWORD dwFlags );

			/**
			 * @brief	Close the kinect stream
			 */
			void Close();

			/**
			 * @brief	To add listener called when Kinect is plugged/ unplugged.
			 * @param	object				object which has functions
			 * @param	pluggedFunction		listener function pointer called when the kinect plugged
			 * @param	unpluggedFunction	listener function pointer called when the kinect unplugged
			 */
			template <class T> void AddKinectListener(T* object, void(T::* pluggedFunction)(), void(T::* unpluggedFunction)())
			{
				long key = reinterpret_cast<long>(object);
				kinectListeners_.insert(std::pair<long, KinectListenerBase*>(key, new KinectListener<T>(object, pluggedFunction, unpluggedFunction)));
			}

			/**
			 * @brief	To remove listener called when Kinect is plugged/ unplugged.
			 */
			template <class T> void RemoveKinectListener(T* object)
			{
				long key = reinterpret_cast<long>(object);
				std::map<long, KinectListenerBase*>::iterator it = kinectListeners_.find(key);
				kinectListeners_.erase(it);
			}

			/**
			 * @brief	Wait until all the kinect handler updated
			 */
			void WaitAndUpdateAll();

			/**
			 * @brief	Set the angle of kinect tilt
			 * @param	lAngleDegrees	Target angle in degrees
			 */
			void SetAngle( LONG lAngleDegrees );

			/**
			 * @brief	Get the current angle of kinect tilt
			 * @return	Current angle in degrees
			 */
			LONG GetAngle() const;

			/**
			 * @brief	Get the number of plugged kinect devices
			 * @return	The number of active kinects
			 */
			static int GetActiveCount();

			/**
			 * @brief	Get the number of available kinect devices
			 * @return	The number of available kinects
			 */
			static int GetAvailableCount();

			/**
			 * @brief	Get the number of available kinect devices
			 * @return	The number of available kinects
			 */
			static int GetConnectedCount();

			/**
			 * @brief	Get the number of available kinect devices
			 * @return	The number of available kinects
			 */
			static int GetNextAvailableIndex();
			
			/**
			 * @brief	Is inited?
			 * @return	true when inited
			 */
			bool IsInited();

			/**
			 * @brief	Is connected?
			 * @return	true when connected
			 */
			bool IsConnected();

			/**
			 * @brief	Get this index of Kinect instance
			 * @return	Instane index starts from 0
			 */
			int GetInstanceIndex() const { return index_; }

			/**
			 * @brief	Get the Video Image from Kinect camera
			 * @return	Reference of ImageStream
			 */
			ImageStream& VideoStream() { return video_; }

			/**
			 * @brief	Get the depth image from Kinect depth sensor
			 * @return	Reference of ImageStream
			 */
			ImageStream& DepthStream() { return depth_; }

			/**
			 * @brief	Get the SkeletonEngine
			 * @return	Reference of SkeletonEngine
			 */
			SkeletonEngine& Skeleton() { return skeleton_; }

			/**
			 * @brief	Get the number of available kinect devices
			 * @return	The number of available kinects
			 */
			LONG GetColorPixelCoordinatesFromDepthPixel(LONG lDepthIndex, USHORT usDepthValue);

			typedef std::shared_ptr< INuiInstance > NuiInstance; ///< pointer of instance

			static const LONG CAMERA_ELEVATION_MAXIMUM = NUI_CAMERA_ELEVATION_MAXIMUM;	///< Maximum value of camera elevation 
			static const LONG CAMERA_ELEVATION_MINIMUM = NUI_CAMERA_ELEVATION_MINIMUM;	///< Minimum value of camera elevation
			
		private:
			friend class KinectContext;

			/**
			 * @brief	Connect the kinect
			 * @param	index	index for Kinect
			 */
			bool Connect(int index);

			/**
			 * @brief	Connect the kinect
			 * @param	deviceName	deviceName for Kinect
			 */
			bool Connect(BSTR deviceName);

			/**
			 * @brief	Disconnect the kinect
			 */
			void Disconnect();

			/**
			 * @brief	Callback when kinect state changed
			 */
			void StatusProc( const NuiStatusData* pStatusData );

			NuiInstance instance_;			///< instance

			ImageStream video_;				///< Video image from Kinect camera
			ImageStream depth_;				///< Depth image from Kinect depth sensor
			SkeletonEngine  skeleton_;		///< Skeleton Tracking
			
			BSTR instanceName_;				///< instanceName of this Kinect sensor
			int index_;						///< index of this Kinect sensor
			bool isConnected_;				///< is connected?
			bool isInited_;					///< is initialized?

			std::map<long, KinectListenerBase*> kinectListeners_;	///< listener when kinect plugged/ unplugged
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_KINECT_H
