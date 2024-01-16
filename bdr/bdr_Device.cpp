// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Device.h"
#include "bdr_AllocationsBlock.h"

namespace bdr
{

Device::Device( Instance *_module ) : InstanceSubmodule( _module ), AllocationsBlocks( this )
	{
	LogThis;
	}

Device::~Device()
	{
	LogThis;

	this->Cleanup();
	}

status Device::UpdateSurfaceCapabilitiesFormatsAndPresentModes()
	{
	Validate( this->PhysicalDeviceHandle, status_code::not_initialized ) << "Device is not set up." << ValidateEnd;

	// update surface capabilities formats and present modes
	uint count = 0;
	CheckCall( vkGetPhysicalDeviceSurfaceCapabilitiesKHR( this->PhysicalDeviceHandle, this->SurfaceHandle, &SurfaceCapabilities ) );
	CheckCall( vkGetPhysicalDeviceSurfaceFormatsKHR( this->PhysicalDeviceHandle, this->SurfaceHandle, &count, nullptr ) );
	if( count > 0 )
		{
		AvailableSurfaceFormats.resize( count );
		CheckCall( vkGetPhysicalDeviceSurfaceFormatsKHR( this->PhysicalDeviceHandle, this->SurfaceHandle, &count, AvailableSurfaceFormats.data() ) );
		}
	vkGetPhysicalDeviceSurfacePresentModesKHR( this->PhysicalDeviceHandle, this->SurfaceHandle, &count, nullptr );
	if( count != 0 )
		{
		AvailablePresentModes.resize( count );
		CheckCall( vkGetPhysicalDeviceSurfacePresentModesKHR( this->PhysicalDeviceHandle, this->SurfaceHandle, &count, AvailablePresentModes.data() ) );
		}

	return status_code::ok;
	}

status Device::Cleanup()
	{
	std::lock_guard<std::mutex> lock( this->GraphicsQueueAccessMutex );

	if( this->DeviceHandle )
		{
		if( this->GraphicsQueueHandle )
			{
			vkQueueWaitIdle( this->GraphicsQueueHandle );
			}

		vkDeviceWaitIdle( this->DeviceHandle );
		}

	this->AllocationsBlocks.Cleanup();

	SafeVkDestroy( this->MemoryAllocatorHandle, vmaDestroyAllocator( this->MemoryAllocatorHandle ) );
	SafeVkDestroy( this->DeviceHandle, vkDestroyDevice( this->DeviceHandle, nullptr ) );
	SafeVkDestroy( this->SurfaceHandle, vkDestroySurfaceKHR( this->GetModule()->GetInstanceHandle(), this->SurfaceHandle, nullptr ) );

	return status_code::ok;
	}

status_return<AllocationsBlock *> Device::CreateAllocationsBlock()
	{
	return AllocationsBlocks.Create( AllocationsBlockTemplate() );
	}

status Device::DestroyAllocationsBlock( AllocationsBlock *block )
	{
	CheckCall( AllocationsBlocks.Destroy( block ) );
	return status_code::ok;
	}

status Device::GraphicsQueueSubmit( size_t submitCount, const VkSubmitInfo *submitInfos, VkFence completedFence )
	{
	std::lock_guard<std::mutex> lock( this->GraphicsQueueAccessMutex );
	return vkQueueSubmit( this->GraphicsQueueHandle, (uint)submitCount, submitInfos, completedFence );
	}

}
// namespace bdr