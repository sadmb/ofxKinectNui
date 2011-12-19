/******************************************************************/
/**
 * @file	KinectContext.cpp
 * @brief	Global information about Kinect.
 * @note	
 * @todo
 * @bug	
 * @see		https://github.com/sadmb/kinect_sdk_sandbox/tree/master/kinect_cpp_wrapper
 *
 * @author	sadmb
 * @date	Oct. 26, 2011
 */
/******************************************************************/
#include "kinect/nui/KinectContext.h"

namespace kinect {
	namespace nui {

		//----------------------------------------------------------
		KinectContext::KinectContext()
		{
			::MSR_NuiSetDeviceStatusCallback(&StatusProcThunk);
		}

		//----------------------------------------------------------
		KinectContext::~KinectContext()
		{
			ShutdownAll();
			RemoveAll();
		}

		//----------------------------------------------------------
		/*static*/ void CALLBACK KinectContext::StatusProcThunk(const NuiStatusData* pStatusData)
		{
			KinectContext::GetContext().StatusProc(pStatusData);
		}
		
		//----------------------------------------------------------
		void CALLBACK KinectContext::StatusProc(const NuiStatusData* pStatusData)
		{
			int size;
			// create instance automatically (not init/ open stream yet)
			if(SUCCEEDED(pStatusData->hrStatus)){
				bool found = false;
				size = (int)kinects_.size();
				// if theres already a Kinect class corresponding to this kinect device.
				for(int i = 0; i < size; i++){
					if(kinects_[i]->instanceName_ != NULL && 0 == wcscmp(kinects_[i]->instanceName_, pStatusData->instanceName)){
						kinects_[i]->Connect(kinects_[i]->instanceName_);
						std::wcout << "KINECT DETECTED, Device Name:: " << pStatusData->instanceName << "\n" << std::endl;
						found = true;
						break;
					}
				}
				if(!found){
					int index = GetNextAvailableIndex();
					if(index >= 0){
						size = (int)kinects_.size();
						for(int i = 0; i < size; i++){
							if(!kinects_[i]->IsConnected()){
								kinects_[i]->index_ = index;
								kinects_[i]->Connect(index);
								std::wcout << "KINECT DETECTED, Device Name:: " << pStatusData->instanceName << "\n" << std::endl;
								found = true;
								break;
							}
						}
					}
				}
			}

			size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(0 == wcscmp(kinects_[i]->instanceName_, pStatusData->instanceName)){
					kinects_[i]->StatusProc(pStatusData);
					break;
				}
			}

			// shutdown automatically
			if(FAILED(pStatusData->hrStatus)){
				size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(0 == wcscmp(kinects_[i]->instanceName_, pStatusData->instanceName)){
						kinects_[i]->Shutdown();
						std::wcout<< "\n" << "KINECT LOST, Device Name::  " << pStatusData->instanceName << "\n" << std::endl;
						break;
					}
				}
			}
		}
		
		//----------------------------------------------------------
		/*static*/ KinectContext& KinectContext::GetContext()
		{
			static KinectContext context;
			return context;
		}

		//----------------------------------------------------------
		INuiInstance* KinectContext::Create(int index)
		{
			INuiInstance* instance = NULL;
			HRESULT ret = ::MSR_NuiCreateInstanceByIndex(index, &instance);
			if (SUCCEEDED(ret)) {
				int size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(kinects_[i]->index_ == instance->InstanceIndex()){
						return instance;
					}
				}
			}
			return instance;
		}

		//----------------------------------------------------------
		INuiInstance* KinectContext::Create(const OLECHAR* strInstanceName)
		{
			INuiInstance* instance = NULL;
			HRESULT ret = ::MSR_NuiCreateInstanceByName(strInstanceName, &instance);
			if (SUCCEEDED(ret)) {
				lock_.lock();
				int size = (int)kinects_.size();
				for(int i = 0; i < size; i++){
					if(0 == wcscmp(kinects_[i]->instanceName_, instance->NuiInstanceName())){
						return instance;
					}
				}
				lock_.unlock();
			}
			return instance;
		}

		//----------------------------------------------------------
		void KinectContext::Shutdown(Kinect& kinect)
		{
			if(IsConnected(kinect.index_)){
				lock_.lock();
				std::wcout << "Kinect[" << kinect.index_ << "] Shutdown, Device Name:: " << kinect.instanceName_ << "\n" << std::endl;
				kinect.instance_->NuiShutdown();
				lock_.unlock();
			}
		}

		//----------------------------------------------------------
		void KinectContext::ShutdownAll()
		{
			lock_.lock();
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
					Shutdown(*kinects_[i]);
			}
			lock_.unlock();
		}

		//----------------------------------------------------------
		void KinectContext::Add(Kinect& kinect)
		{
			lock_.lock();
			kinects_.push_back(&kinect);
			lock_.unlock();
		}

		//----------------------------------------------------------
		void KinectContext::Remove(Kinect& kinect)
		{
			lock_.lock();
			std::vector<Kinect*>::iterator it;
			if(kinects_.size() > 0){
				for(it = kinects_.begin(); it != kinects_.end(); ++it){
					Kinect* kn = *it;
					if(kn == &kinect){
						kinects_.erase(it);
						break;
					}
				}
			}
			lock_.unlock();
		}


		//----------------------------------------------------------
		void KinectContext::RemoveAll()
		{
			lock_.lock();
			kinects_.clear();
			lock_.unlock();
		}
		
		//----------------------------------------------------------
		int KinectContext::GetActiveCount()
		{
			int count = 0;
			HRESULT ret = ::MSR_NUIGetDeviceCount( &count );
			if (FAILED(ret)) {
				return 0;
			}

			return count;
		}

		//----------------------------------------------------------
		int KinectContext::GetAvailableCount()
		{
			return (GetActiveCount() - GetConnectedCount());
		}
		
		//----------------------------------------------------------
		int KinectContext::GetConnectedCount()
		{
			int count = 0;
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(kinects_[i]->IsConnected()){
					count++;
				}
			}
			return count;
		}

		//----------------------------------------------------------
		int KinectContext::GetInstanceIndex(Kinect& kinect)
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(&kinect == kinects_[i]){
					return kinects_[i]->GetInstanceIndex();
				}
			}
			return -1;
		}

		//----------------------------------------------------------
		Kinect* KinectContext::getKinect( NuiInstance instance )
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(instance == kinects_[i]->instance_){
					return kinects_[i];
				}
			}
			return NULL;
		}

		//----------------------------------------------------------
		bool KinectContext::IsConnected(int index)
		{
			int size = (int)kinects_.size();
			for(int i = 0; i < size; i++){
				if(index == kinects_[i]->GetInstanceIndex()){
					return kinects_[i]->IsConnected();
				}
			}
			return false;
		}

		//----------------------------------------------------------
		int KinectContext::GetNextAvailableIndex()
		{
			int size = GetActiveCount();
			for(int i = 0; i < size; i++){
				if(!IsConnected(i)){
					return i;
				}
			}
			return -1;
		}
	} // namespace kinect
} // namespace nui

