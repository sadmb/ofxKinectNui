/******************************************************************/
/**
 * @file	KinectContext.h
 * @brief	Global information about Kinect
 * @note	Do not use or extend this class
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#ifndef KINECT_NUI_KINECT_CONTEXT_H
#define KINECT_NUI_KINECT_CONTEXT_H

#include <vector>

#include <Windows.h>
#include <MSR_NuiApi.h>

#include "kinect/nui/Kinect.h"
#include "win32/SpinLock.h"

namespace kinect {
	namespace nui {
		//////////////////////////////////////////////////////
		//				class declarations					//
		//////////////////////////////////////////////////////
		/****************************************/
		/**
		 * @class	KinectContext
		 * @brief	Global information about Kinect
		 * @note	
		 * @date	Oct. 26, 2011
		 */
		/****************************************/
		class KinectContext
		{
		public:

			/**
			 * @brief	Destructor
			 */
			~KinectContext();


			/**
			 * @brief	Callback function when kinect status changed.
			 */
			 static void CALLBACK StatusProcThunk(const NuiStatusData* pStatusData);

			/**
			 * @brief	Callback function for this class
			 */
			 void CALLBACK StatusProc(const NuiStatusData* pStatusData);

			/**
			 * @brief	Get context (singleton)
			 */
			static KinectContext& GetContext();

			/**
			 * @brief	Create kinect instance
			 */
			INuiInstance* Create(int index);

			/**
			 * @brief	Create kinect instance
			 */
			INuiInstance* Create(const OLECHAR* strInstanceName);

			/**
			 * @brief	Shutdown a kinect device
			 * @param	kinect	kinect object
			 */
			void Shutdown(Kinect& kinect);

			/**
			 * @brief	Shutdown all currently connected kinect devices
			 */
			void ShutdownAll();

			/**
			 * @brief	Add a kinect object
			 * @param	kinect	kinect object	
			 */
			void Add(Kinect& kinect);
			
			/**
			 * @brief	Remove a kinect object
			 * @param	kinect	kinect object
			 */
			void Remove(Kinect& kinect);

			/**
			 * @brief	Remove all currently added kinect objects
			 */
			void RemoveAll();
			
			/**
			 * @brief	Get the number of active Kinect sensor
			 * @return	number
			 */
			int GetActiveCount();

			/**
			 * @brief	Get the number of available (active but not connected) Kinect sensor
			 * @return	number
			 */
			int GetAvailableCount();

			/**
			 * @brief	Get the number of connected Kinect sensor
			 * @return	number
			 */
			int GetConnectedCount();
			
			/**
			 * @brief	Get the index of a Kinect instance
			 * @param	kinect	kinect object	
			 * @return	Instance index starts from 0
			 */
			int GetInstanceIndex(Kinect& kinect);

			typedef std::shared_ptr< INuiInstance > NuiInstance; ///< pointer of kinect instance

			/**
			 * @brief	Get the Kinect object from a kinect instance pointer
			 * @return	NULL if not found
			 */
			Kinect* getKinect( NuiInstance instance );
			
			/**
			 * @brief	Is the index already connected?
			 * @return	true when connected
			 */
			bool IsConnected(int index);

			/**
			 * @brief	Get the index of next available Kinect device
			 * @return	-1 when no available
			 */
			int GetNextAvailableIndex();

		private:
			/**
			 * @brief	Constructor
			 */
			KinectContext();

			std::vector<Kinect*> kinects_; ///< the connected kinect devices
			win32::SpinLock lock_;
		};
	} // namespace nui
} // namespace kinect
#endif // KINECT_NUI_KINECT_CONTEXT_H
