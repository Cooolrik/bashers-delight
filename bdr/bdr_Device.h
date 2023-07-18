// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Instance.h"

namespace bdr 
	{
	class Device : public InstanceSubmodule
		{
		public:
			~Device();

		private:
			// The device can only be created by the Instance::CreateDevice method
            friend status_return<Device*> Instance::CreateDevice( const DeviceTemplate& parameters );
			Device( Instance* _module );

			VkDevice DeviceHandle = VK_NULL_HANDLE;
			VkQueue GraphicsQueueHandle = VK_NULL_HANDLE;
			VkQueue PresentQueueHandle = VK_NULL_HANDLE;

			vector<const char*> DeviceExtensionList;

			VkPhysicalDevice PhysicalDeviceHandle = VK_NULL_HANDLE;
			uint PhysicalDeviceQueueGraphicsFamily = (uint)-1;
			uint PhysicalDeviceQueuePresentFamily = (uint)-1;
			VkPhysicalDeviceFeatures2 PhysicalDeviceFeatures = {};
			VkPhysicalDeviceProperties2 PhysicalDeviceProperties = {};

			VkSurfaceKHR SurfaceHandle = VK_NULL_HANDLE;
			VkSurfaceCapabilitiesKHR SurfaceCapabilities = {};
			vector<VkSurfaceFormatKHR> AvailableSurfaceFormats;
			vector<VkPresentModeKHR> AvailablePresentModes;

			VmaAllocator MemoryAllocatorHandle = VK_NULL_HANDLE;

			DeviceAllocationMap<AllocationsBlock> AllocationsBlocks;

			std::mutex GraphicsQueueAccessMutex;
			
			//
			//struct TargetImage
			//	{
			//	std::unique_ptr<Image> Color; // color channel RGBA
			//	std::unique_ptr<Image> Depth; // depth channel 
			//	};

			//
			

			//

			//VkDevice Device = nullptr;
			//VkQueue GraphicsQueue = nullptr;
			//VkQueue PresentQueue = nullptr;
			//VkSurfaceFormatKHR SurfaceFormat{};
			//VkPresentModeKHR PresentMode{};
			//VkExtent2D RenderExtent{};
			//VkFormat SurfaceDepthFormat{};
			//bool DepthFormatHasStencil = false;
			//VkRenderPass RenderPass = nullptr;

			//

			//VkSwapchainKHR SwapChain = nullptr;
			//std::vector<VkImage> SwapChainImages;


			//VkCommandPool InternalCommandPool = nullptr; 

			//std::vector<VkSemaphore> ImageAvailableSemaphores;
			//std::vector<VkSemaphore> RenderFinishedSemaphores;
			//std::vector<VkFence> InFlightFences;
			//std::vector<VkFence> ImagesInFlight;
			//uint CurrentFrame = 0;
			//uint CurrentImage = 0;


			//// generic functions to create vulkan device buffer 
			//VkBuffer CreateVulkanBuffer( VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryPropertyFlags, VkDeviceSize deviceSize, VmaAllocation& deviceMemory ) const;

			///// create base vulkan buffer
			//template<class B,class BT> B* NewBuffer( const BT& bt ) const;

			//VkCommandBuffer BeginInternalCommandBuffer() const;
			//void EndAndSubmitInternalCommandBuffer( VkCommandBuffer buffer ) const;

			// requests updated surface caps, formats and present modes from the selected physical device
			status UpdateSurfaceCapabilitiesFormatsAndPresentModes();

		public:

			// creates an allocations block. allocations blocks are used to allocate all other 
			// object types, and there can be any number of allocations blocks in the application.
			// creation and destruction of allocations blocks is not a thread safe operation, and it is the 
			// caller's responsibility to syncronize, either by only accessing through a "main thread"
			// or through a mutex.
			status_return<AllocationsBlock*> CreateAllocationsBlock();

			// deletes an allocation block.
			// creation and destruction of allocations blocks should only be done by a single thread
			status DestroyAllocationsBlock( AllocationsBlock *block );

			// manually create and handover an object. the user will have responsibility of the object after creation
			template<typename _Ty, typename _TemplateTy>
			status_return<unique_ptr<_Ty>> CreateObject( const _TemplateTy &parameters )
			{
				if( !this || !this->DeviceHandle )
					return status::not_initialized;
				auto obj = std::unique_ptr<_Ty>(new _Ty(this));
				auto res = obj->Setup( parameters );
				if( !res )
					return res;
				return obj;
			}

			// explicitly cleans up the object, and also destroys all objects owned by it
			status Cleanup();

			// public get methods
			VkDevice GetDeviceHandle() const { return this->DeviceHandle; }
			VkPhysicalDevice GetPhysicalDeviceHandle() const { return this->PhysicalDeviceHandle; }
			VkQueue GetGraphicsQueueHandle() const { return this->GraphicsQueueHandle; }
			VkQueue GetPresentQueueHandle() const { return this->PresentQueueHandle; }
			uint GetPhysicalDeviceQueueGraphicsFamily() const { return this->PhysicalDeviceQueueGraphicsFamily; }
			uint GetPhysicalDeviceQueuePresentFamily() const { return this->PhysicalDeviceQueuePresentFamily; }
			VkPhysicalDeviceFeatures2 GetPhysicalDeviceFeatures() const { return this->PhysicalDeviceFeatures; }
			VkPhysicalDeviceProperties2 GetPhysicalDeviceProperties() const { return this->PhysicalDeviceProperties; }

			// returns a copy of the device extension list
			vector<const char*> GetDeviceExtensionList() const { return this->DeviceExtensionList; }

			// returns copies of the surface information
			VkSurfaceKHR GetSurfaceHandle() const { return this->SurfaceHandle; }
			vector<VkSurfaceFormatKHR> GetAvailableSurfaceFormats() const { return this->AvailableSurfaceFormats; }
			vector<VkPresentModeKHR> GetAvailablePresentModes() const { return this->AvailablePresentModes; }
			VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return this->SurfaceCapabilities; }

			// get the memory allocator handle
			VmaAllocator GetMemoryAllocatorHandle() const { return this->MemoryAllocatorHandle; }

			// thread safe submit commands to the graphics queue
			status GraphicsQueueSubmit( size_t submitCount, const VkSubmitInfo* submitInfos, VkFence completedFence = nullptr );


		};

	// Device template creation parameters
	class DeviceTemplate
		{
		public:
			// the surface handle
			VkSurfaceKHR SurfaceHandle = {};
		};

	};
