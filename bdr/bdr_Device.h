// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr.h"
#include "bdr_Instance.h"

namespace bdr 
	{
	class Device : public MainSubmodule
		{
		public:
			~Device();

		private:
			// The device can only be created by the Instance::CreateDevice method
            friend status_return<Device*> Instance::CreateDevice( const DeviceTemplate& parameters );
			Device( const Instance* _module );

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

			// the swapchain and the template used to create the swapchain
			unique_ptr<Swapchain> Swapchain_;
			unique_ptr<SwapchainTemplate> SwapchainTemplate_;

			// the allocated CommandPool objects
			unordered_map<CommandPool*,unique_ptr<CommandPool>> CommandPools;

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

			VkCommandBuffer BeginInternalCommandBuffer() const;
			void EndAndSubmitInternalCommandBuffer( VkCommandBuffer buffer ) const;

		public:
			// creates a swapchain with framebuffer objects
			status CreateSwapchain( const SwapchainTemplate& parameters );

			// recreates a lost swapchain 
			status RecreateSwapchain( const SwapchainTemplate& parameters );

			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

			// create a command pool object
			status_return<CommandPool*> CreateCommandPool( const CommandPoolTemplate& parameters );

			// destroy a command pool object
			status DestroyCommandPool( CommandPool *commandPool );

			// requests updated surface caps, formats and present modes from the selected physical device
			status UpdateSurfaceCapabilitiesFormatsAndPresentModes();


			// public get methods
			VkDevice GetDeviceHandle() const { return this->DeviceHandle; }

			//BDRGetMacro( VkPhysicalDevice, PhysicalDevice );
			//BDRGetMacro( VkDevice, Device );
			//BDRGetMacro( VkQueue, GraphicsQueue );
			//BDRGetMacro( VkQueue, PresentQueue );
			//BDRGetMacro( std::vector<VkSurfaceFormatKHR>, AvailableSurfaceFormats );
			//BDRGetMacro( std::vector<VkPresentModeKHR>, AvailablePresentModes );
			//BDRGetMacro( VkPhysicalDeviceFeatures2, PhysicalDeviceFeatures );
			//BDRGetMacro( VkSurfaceCapabilitiesKHR, SurfaceCapabilities );
			//BDRGetMacro( VkSurfaceFormatKHR, SurfaceFormat );
			//BDRGetMacro( VkPresentModeKHR, PresentMode );
			//BDRGetMacro( VkExtent2D, RenderExtent );
			//BDRGetMacro( std::vector<VkFramebuffer>, Framebuffers );
			//BDRGetMacro( std::vector<VkImage>, SwapChainImages );
			//BDRGetMacro( VkRenderPass, RenderPass );
			//BDRGetMacro( VmaAllocator, MemoryAllocator );

			//const Image* GetColorTargetImage( uint index ) const { return this->TargetImages[index].Color.get(); }
			//const Image* GetDepthTargetImage( uint index ) const { return this->TargetImages[index].Depth.get(); }

		};

	// Device template creation parameters
	class DeviceTemplate
		{
		public:
			// the surface handle
			VkSurfaceKHR SurfaceHandle = {};
		};

	};
