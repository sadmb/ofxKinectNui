/******************************************************************/
/**
 * @file	Kinect.cpp
 * @brief	kinect wrapper class for cpp
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
#include <vector>

#include "kinect/nui/Kinect.h"

namespace kinect {
	namespace nui {

		//----------------------------------------------------------
		Kinect::Kinect(int index /* = -1*/)
		{
			if(index = -1){
				index = KinectContext::GetContext().GetNextAvailableIndex();
			}
			index_ = index;
			isConnected_ = false;
			isInited_ = false;
			kinectListener_ = NULL;
			instanceName_ = NULL;
			// add to the kinect context
			KinectContext::GetContext().Add(*this);
			// create instance if the kinect already plugged in
			Connect(index_);
		}

		Kinect::Kinect(BSTR deviceName)
		{
			index_ = -1;
			isConnected_ = false;
			isInited_ = false;
			kinectListener_ = NULL;
			instanceName_ = deviceName;
			// add to the kinect context
			KinectContext::GetContext().Add(*this);
			// create instance if the kinect already plugged in
			Connect(deviceName);
		}

		//----------------------------------------------------------
		Kinect::~Kinect()
		{
			Shutdown();
			// remove from the kinect context
			KinectContext::GetContext().Remove(*this);
			// release kinect listener
			RemoveKinectListener();
		}

		//----------------------------------------------------------
		/*static*/ int Kinect::GetActiveCount()
		{
			return KinectContext::GetContext().GetActiveCount();
		}

		//----------------------------------------------------------
		/*static*/ int Kinect::GetAvailableCount()
		{
			return KinectContext::GetContext().GetAvailableCount();
		}

		//----------------------------------------------------------
		/*static*/ int Kinect::GetConnectedCount()
		{
			return KinectContext::GetContext().GetConnectedCount();
		}

		//----------------------------------------------------------
		/*static*/ int Kinect::GetNextAvailableIndex()
		{
			return KinectContext::GetContext().GetNextAvailableIndex();
		}

		bool Kinect::IsConnected()
		{
			return isConnected_;
		}

		//----------------------------------------------------------
		bool Kinect::IsInited()
		{
			return isInited_;
		}

		//----------------------------------------------------------
		void Kinect::Initialize( DWORD dwFlags )
		{
			HRESULT ret = instance_->NuiInitialize( dwFlags );
			if (FAILED(ret)) {
				isInited_ = false;
				return;
			}
			isInited_ = true;
		}

		//----------------------------------------------------------
		void Kinect::Shutdown()
		{
			if(isConnected_){
				KinectContext::GetContext().Shutdown(*this);
			}
			isInited_ = false;
		}

		//----------------------------------------------------------
		void Kinect::StatusProc( const NuiStatusData* pStatusData )
		{
			if(SUCCEEDED(pStatusData->hrStatus)){
				if(kinectListener_ != NULL){
					kinectListener_->Plugged();
				}
			}else if(FAILED(pStatusData->hrStatus)){
				if(kinectListener_ != NULL){
					kinectListener_->Unplugged();
				}
			}
		}

		//----------------------------------------------------------
		void Kinect::WaitAndUpdateAll()
		{
			std::vector< HANDLE >   handle;
			if ( VideoStream().hStream_ != NULL ) {
				handle.push_back( VideoStream().event_.get() );
			}

			if ( DepthStream().hStream_ != NULL ) {
				handle.push_back( DepthStream().event_.get() );
			}

			if ( Skeleton().IsEnabled() ) {
				handle.push_back( Skeleton().event_.get() );
			}

			if ( handle.size() == 0 ) {
				return;
			}

			DWORD ret = ::WaitForMultipleObjects( handle.size(), &handle[0], TRUE, INFINITE );
			if ( ret == WAIT_FAILED ) {
				return;
			}
		}

		//----------------------------------------------------------
		void Kinect::SetAngle( LONG lAngleDegrees )
		{
			// comment out here because i got an error.
			HRESULT ret = instance_->NuiCameraElevationSetAngle( lAngleDegrees );
			//if ( ret != S_OK ) {
			//	throw win32::Win32Exception( ret );
			//}
		}

		//----------------------------------------------------------
		LONG Kinect::GetAngle() const
		{
			// comment out here because i got an error.
			LONG angle = 0;
			HRESULT ret = instance_->NuiCameraElevationGetAngle( &angle );
			//if ( ret != S_OK ) {
			//	throw win32::Win32Exception( ret );
			//}

			return angle;
		}

		//----------------------------------------------------------
		LONG Kinect::GetColorPixelCoordinatesFromDepthPixel(LONG lDepthPixel, USHORT usDepthValue)
		{
			LONG lDepthX, lDepthY;
			lDepthX = (LONG)(lDepthPixel % DepthStream().Width());
			lDepthY = (LONG)(lDepthPixel / DepthStream().Width());
			LONG plColorX = 0, plColorY = 0;

			HRESULT ret = instance_->NuiImageGetColorPixelCoordinatesFromDepthPixel(VideoStream().Resolution(), NULL, lDepthX, lDepthY, usDepthValue, &plColorX, &plColorY);
			if(FAILED(ret)) {
				return -1;
			}

			LONG plColorPixel = plColorY * VideoStream().Width() + plColorX;
			return plColorPixel;
		}

		//----------------------------------------------------------
		bool Kinect::Connect(int index)
		{
			INuiInstance* instance = KinectContext::GetContext().Create(index);
			if(instance == NULL){
				return false;
			}
			instance_ = NuiInstance(instance, ::MSR_NuiDestroyInstance);
			instanceName_ = instance_->NuiInstanceName();
			depth_.CopyInstance(instance_);
			video_.CopyInstance(instance_);
			skeleton_.CopyInstance(instance_);
			isConnected_ = true;
			return true;
		}

		//----------------------------------------------------------
		bool Kinect::Connect(BSTR deviceName)
		{
			INuiInstance* instance = KinectContext::GetContext().Create(deviceName);
			if(instance == NULL){
				return false;
			}
			instance_ = NuiInstance(instance, ::MSR_NuiDestroyInstance);
			index_ = instance_->InstanceIndex();
			depth_.CopyInstance(instance_);
			video_.CopyInstance(instance_);
			skeleton_.CopyInstance(instance_);
			isConnected_ = true;
			return true;
		}
	} ///< namespace nui
} ///< namespace kinect
