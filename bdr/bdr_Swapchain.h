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

	private:
		friend status_return<unique_ptr<Swapchain>> Device::CreateObject<Swapchain, SwapchainTemplate>( const SwapchainTemplate &parameters );
		Swapchain( Device *_module );
		status Setup( const SwapchainTemplate &parameters );

		VkSwapchainKHR _SwapchainHandle = nullptr;
		vector<VkImage> SwapchainImageHandles;

		unique_ptr<SwapchainTemplate> _Template;

	public:
		// properties

		// the handle of the Swapchain Vulkan object 
		GetProp<VkSwapchainKHR> SwapchainHandle;

		// the template which was used to generate the Swapchain
		GetProp<SwapchainTemplate, Prop::ptr> Template;

	public:

		// explicitly cleanups the object, and also clears all objects owned by it
		status Cleanup();

		// recreates the swapchain using the template specified when the object was created, with 
		// the option to override the render extents before recreating the swapchain
		status RecreateSwapChain();
		status RecreateSwapChain( const VkExtent2D &renderExtent );

		// Acquire an image in the swap chain. Async, specify a semaphore or fence to signal when the image is available.
		// if the return status is vulkan_out_of_date_khr, then the swap chain needs to be recreated before using
		// if the return value is UINT_MAX, the timeout was reached
		status_return<uint> AcquireNextImage( VkSemaphore semaphore, VkFence fence = nullptr, uint64_t timeout = UINT64_MAX );
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
		static status_return<SwapchainTemplate> DeviceCompatible( Device *device, VkExtent2D renderExtent = { 0,0 }, uint swapchainLength = 0 );

	};

}
//namespace bdr
