// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Swapchain.h"
#include "bdr_FramebufferPool.h"

namespace bdr
{
	Swapchain::Swapchain( Device* _module ) : DeviceSubmodule(_module) 
		{
		LogThis;
		}

	Swapchain::~Swapchain()
		{
		LogThis;

		this->Cleanup();
		}
		
	status Swapchain::Setup( const bdr::SwapchainTemplate& parameters )
		{
		Validate( !this->SwapchainHandle , status::already_initialized ) << "Swap chain already setup. Use RecreateSwapChain after first creation." << ValidateEnd;
		
		const auto surfaceCapabilities = this->Module->GetSurfaceCapabilities();
		const auto physicalDeviceQueueGraphicsFamily = this->Module->GetPhysicalDeviceQueueGraphicsFamily();
		const auto physicalDeviceQueuePresentFamily = this->Module->GetPhysicalDeviceQueuePresentFamily();

		// set up swap chain
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = this->Module->GetSurfaceHandle();
		swapchainCreateInfo.minImageCount = parameters.swapchainLength;
		swapchainCreateInfo.imageFormat = parameters.surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = parameters.surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = parameters.renderExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = parameters.presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = nullptr;

		// select concurrent or exclusive sharing mode depending if we have one or two queues 
		const uint32_t queueFamilyIndices[] = { physicalDeviceQueueGraphicsFamily, physicalDeviceQueuePresentFamily };
		if( physicalDeviceQueueGraphicsFamily == physicalDeviceQueuePresentFamily )
			{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			}
		else
			{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
			}

		// create the swap chain
		CheckCall( vkCreateSwapchainKHR( this->Module->GetDeviceHandle(), &swapchainCreateInfo, nullptr, &this->SwapchainHandle ) );

		// retrieve the swap chain images, check how many we actually got
		uint32_t imageCount = 0;
		CheckCall( vkGetSwapchainImagesKHR( this->Module->GetDeviceHandle(), this->SwapchainHandle, &imageCount, nullptr ) );
		this->SwapchainImageHandles.resize( imageCount );
		CheckCall( vkGetSwapchainImagesKHR( this->Module->GetDeviceHandle(), this->SwapchainHandle, &imageCount, this->SwapchainImageHandles.data() ) );

		//// resize list of image fences
		//ImagesInFlight.resize( this->SwapChainImages.size(), VK_NULL_HANDLE );
		
		this->Template = std::make_unique<SwapchainTemplate>(parameters);

		return status_code::ok;
		}

	status Swapchain::Cleanup()
		{
		return status_code::ok;
		}

	status_return<SwapchainTemplate> SwapchainTemplate::DeviceCompatible( Device *device, VkExtent2D renderExtent, uint swapchainLength )
		{
		Validate( device , status::invalid_param ) << "A valid Device object is required." << ValidateEnd;

		SwapchainTemplate ret = {};

		const auto surfaceCapabilities = device->GetSurfaceCapabilities();
		const auto availableSurfaceFormats = device->GetAvailableSurfaceFormats();
		const auto availablePresentModes = device->GetAvailablePresentModes();
		
		Validate( availableSurfaceFormats.size() > 0 , status::not_ready ) << "No surface formats are available, cannot continue." << ValidateEnd;
		Validate( availablePresentModes.size() > 0 , status::not_ready ) << "No present modes are available, cannot continue." << ValidateEnd;

		// select default surface format to the first available format, but if we find sRGB 32-bit, prefer that.
		ret.surfaceFormat = availableSurfaceFormats[0];
		for( const auto& availableFormat : availableSurfaceFormats )
			{
			if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
			 && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
				{
				ret.surfaceFormat = availableFormat;
				break;
				}
			}

		// default to fifo mode, but if we can use mailbox buffering, do that.
		ret.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for( const auto& availablePresentMode : availablePresentModes )
			{
			if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
				{
				ret.presentMode = availablePresentMode;
				break;
				}
			}

		// handle render extent
		// if one is provided, clamp it with the min and max extents
		// else just use the current extents of the whole surface
		if( renderExtent.width > 0 && renderExtent.height > 0 )
			{
			ret.renderExtent = renderExtent;

			ret.renderExtent.width = clamp( ret.renderExtent.width , surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width );
			ret.renderExtent.height = clamp( ret.renderExtent.height , surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );
			}
		else
			{
			ret.renderExtent = surfaceCapabilities.currentExtent;
			}

		// set up the wanted number of swap chain images
		// if not provided, default to one more than the minimum
		if( swapchainLength > 0 )
			{
			ret.swapchainLength = swapchainLength;
			}
		else
			{
			ret.swapchainLength = surfaceCapabilities.minImageCount + 1;
			}

		// clamp the image count if there are min and max values set (non-zero values)
		ret.swapchainLength = max( ret.swapchainLength , surfaceCapabilities.minImageCount );
		if( surfaceCapabilities.maxImageCount > 0 )
			{
			ret.swapchainLength = min( ret.swapchainLength , surfaceCapabilities.maxImageCount );
			}

		return ret;
		}


}