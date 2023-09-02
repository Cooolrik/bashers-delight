// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr 
	{
	class Swapchain : public DeviceSubmodule
		{
		public:
			~Swapchain();

			class Image
				{
				public:
					VkSemaphore imageAvailableSemaphore;
					VkFence imageAvailableFence;
				};

		private:
			friend status_return<unique_ptr<Swapchain>> Device::CreateObject<Swapchain,SwapchainTemplate>( const SwapchainTemplate &parameters );
			Swapchain( Device* _module );
			status Setup( const SwapchainTemplate& parameters );

			VkSwapchainKHR SwapchainHandle = nullptr;
			vector<VkImage> SwapchainImageHandles;

			unique_ptr<SwapchainTemplate> Template;
						
			//std::vector<VkSemaphore> ImageAvailableSemaphores;
			//std::vector<VkSemaphore> RenderFinishedSemaphores;
			//std::vector<VkFence> InFlightFences;
			//std::vector<VkFence> ImagesInFlight;
			//uint CurrentFrame = 0;
			//uint CurrentImage = 0;

		public:
			
			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

			//void RecreateSwapChain( const CreateSwapchainParameters& parameters );
			
			// Acquires an image in the swap chain. 
			// if the return value is status::vulkan_out_of_date_khr, then the swap chain needs to be recreated before using
			status_return<uint> AcquireNextImage();

			///// submit command buffers to the current frame
			//VkResult SubmitRenderCommandBuffersAndPresent( const std::vector<VkCommandBuffer>& buffers );

			///// wait for device to idle, for synching, eg shutting down
			//void WaitForDeviceIdle();
		};

	// Swapchain template creation parameters
	// Optional values will be handled automatically if not set.
	class SwapchainTemplate
		{
		public:
			// Number of images in the swap chain. 
			uint swapchainLength;

			// Selected surface format and color space
			VkSurfaceFormatKHR surfaceFormat;

			// Surface presentation mode. 
			VkPresentModeKHR presentMode;

			// Render extents to use 
			VkExtent2D renderExtent;

			////////////////////////

			// Creates a swapchain for the current surface of the provided device, and tries to select the best fit from available modes and formats
			static status_return<SwapchainTemplate> DeviceCompatible( Device *device, VkExtent2D renderExtent = {0,0}, uint swapchainLength = 0 );

		};

	};
