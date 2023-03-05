// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Swapchain.h"

namespace bdr
{
	Swapchain::Swapchain( const Instance* _module ) : MainSubmodule(_module) 
		{
		LogThis;
		}

	Swapchain::~Swapchain()
		{
		LogThis;

		this->Cleanup();
		}
		
	status Swapchain::Setup( const bdr::SwapchainTemplate& /*parameters*/ )
		{
		//Validate( !this->Swapchain_ , status::already_initialized ) << "Swap chain already setup. Use RecreateSwapChain after first creation." << ValidateEnd;

		//this->SwapchainTemplate_ = std::make_unique<SwapchainTemplate>(parameters);

		//// create a swapchain object 
		//auto pSwapchain = unique_ptr<bdr::Swapchain>( new bdr::Swapchain(this->GetModule()) );

		//CheckCall( this->UpdateSurfaceCapabilitiesFormatsAndPresentModes() );

		//// set or select suface format
		//if( parameters.SurfaceFormat.has_value() )
		//	{
		//	pSwapchain->SurfaceFormat = parameters.SurfaceFormat;
		//	}
		//else
		//	{
		//	// select default surface format. if we find sRGB 32-bit, prefer that.
		//	pSwapchain->SurfaceFormat = this->AvailableSurfaceFormats[0];
		//	for( const auto& availableFormat : this->AvailableSurfaceFormats )
		//		{
		//		if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
		//		 && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
		//			{
		//			pSwapchain->SurfaceFormat = availableFormat;
		//			break;
		//			}
		//		}
		//	}

		//// set or select presentation mode
		//if( parameters.PresentMode.has_value() )
		//	{
		//	pSwapchain->PresentMode = parameters.PresentMode;
		//	}
		//else
		//	{
		//	// default to fifo mode, but if we can use mailbox buffering, do that.
		//	pSwapchain->PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		//	for( const auto& availablePresentMode : this->AvailablePresentModes )
		//		{
		//		if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
		//			{
		//			pSwapchain->PresentMode = availablePresentMode;
		//			break;
		//			}
		//		}
		//	}

		//// handle render extent
		//if( parameters.RenderExtent.has_value() )
		//	{
		//	pSwapchain->RenderExtent = parameters.RenderExtent;

		//	pSwapchain->RenderExtent.width = clamp( 
		//		pSwapchain->RenderExtent.width , 
		//		this->SurfaceCapabilities.minImageExtent.width, 
		//		this->SurfaceCapabilities.maxImageExtent.width );
		//	pSwapchain->RenderExtent.height = clamp( 
		//		pSwapchain->RenderExtent.height , 
		//		this->SurfaceCapabilities.minImageExtent.height, 
		//		this->SurfaceCapabilities.maxImageExtent.height );
		//	}
		//else
		//	{
		//	pSwapchain->RenderExtent = this->SurfaceCapabilities.currentExtent;
		//	}

		//// set up the wanted number of swap chain images
		//uint32_t imageCount = 0;
		//if( parameters.SwapchainLength.has_value() )
		//	{
		//	imageCount = parameters.SwapchainLength;
		//	}
		//else
		//	{
		//	imageCount = this->SurfaceCapabilities.minImageCount + 1;
		//	}

		//// clamp the image count if there are min and max values set (non-zero values)
		//imageCount = max( imageCount , this->SurfaceCapabilities.minImageCount );
		//if( this->SurfaceCapabilities.maxImageCount > 0 )
		//	{
		//	imageCount = min( imageCount , this->SurfaceCapabilities.maxImageCount );
		//	}

		//// set up swap chain
		//VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		//swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		//swapchainCreateInfo.surface = this->SurfaceHandle;
		//swapchainCreateInfo.minImageCount = imageCount;
		//swapchainCreateInfo.imageFormat = pSwapchain->SurfaceFormat.format;
		//swapchainCreateInfo.imageColorSpace = pSwapchain->SurfaceFormat.colorSpace;
		//swapchainCreateInfo.imageExtent = pSwapchain->RenderExtent;
		//swapchainCreateInfo.imageArrayLayers = 1;
		//swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		//swapchainCreateInfo.preTransform = this->SurfaceCapabilities.currentTransform;
		//swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		//swapchainCreateInfo.presentMode = pSwapchain->PresentMode;
		//swapchainCreateInfo.clipped = VK_TRUE;
		//swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		//// select concurrent or exclusive sharing mode depending if we have one or two queues 
		//uint32_t queueFamilyIndices[] = { PhysicalDeviceQueueGraphicsFamily, PhysicalDeviceQueuePresentFamily };
		//if( PhysicalDeviceQueueGraphicsFamily == PhysicalDeviceQueuePresentFamily )
		//	{
		//	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		//	}
		//else
		//	{
		//	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		//	swapchainCreateInfo.queueFamilyIndexCount = 2;
		//	swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		//	}

		//// create the swap chain
		//CheckCall( vkCreateSwapchainKHR( this->DeviceHandle, &swapchainCreateInfo, nullptr, &pSwapchain->SwapchainHandle ) );

		//// retrieve the swap chain images, check how many we actually got
		//CheckCall( vkGetSwapchainImagesKHR( this->DeviceHandle, pSwapchain->SwapchainHandle, &imageCount, nullptr ) );
		//pSwapchain->SwapchainImageHandles.resize( imageCount );
		//CheckCall( vkGetSwapchainImagesKHR( this->DeviceHandle, pSwapchain->SwapchainHandle, &imageCount, pSwapchain->SwapchainImageHandles.data() ) );

		//// allocate render target vector
		//pSwapchain->TargetImages.resize( imageCount );

		//// create render color targets, same format as swap chain for now
		//for(uint i = 0; i < imageCount; i++)
		//	{
		//	VkImage image = {};
		//	VkImageView imageView = {};
		//	VmaAllocation imageAllocation = {};
		//
		//	VkImageCreateInfo colorImageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		//	colorImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		//	colorImageCreateInfo.extent.width = pSwapchain->RenderExtent.width;
		//	colorImageCreateInfo.extent.height = pSwapchain->RenderExtent.height;
		//	colorImageCreateInfo.extent.depth = 1;
		//	colorImageCreateInfo.mipLevels = 1;
		//	colorImageCreateInfo.arrayLayers = 1;
		//	colorImageCreateInfo.format = pSwapchain->SurfaceFormat.format;
		//	colorImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		//	colorImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//	colorImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		//	colorImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		//	colorImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		//	VmaAllocationCreateInfo allocationCreateInfo{};
		//	allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		//	allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		//	CheckCall( vmaCreateImage( this->MemoryAllocatorHandle, &colorImageCreateInfo, &allocationCreateInfo, &image, &imageAllocation, nullptr ) );

		//	// transition the color image from undefined, to device specific layout
		//	VkCommandBuffer commandBuffer = this->BeginInternalCommandBuffer();
		//	VkImageMemoryBarrier imageMemoryBarrier{};
		//	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//	imageMemoryBarrier.image = image;
		//	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		//	imageMemoryBarrier.subresourceRange.levelCount = 1;
		//	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		//	imageMemoryBarrier.subresourceRange.layerCount = 1;
		//	imageMemoryBarrier.srcAccessMask = 0;
		//	imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//	vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier );
		//	this->EndAndSubmitInternalCommandBuffer( commandBuffer );

		//	VkImageViewCreateInfo imageViewCreateInfo{};
		//	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//	imageViewCreateInfo.image = image;
		//	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		//	imageViewCreateInfo.format = pSwapchain->SurfaceFormat.format;
		//	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		//	imageViewCreateInfo.subresourceRange.levelCount = 1;
		//	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		//	imageViewCreateInfo.subresourceRange.layerCount = 1;
		//	CheckCall( vkCreateImageView( this->DeviceHandle, &imageViewCreateInfo, nullptr, &imageView ) );

		//	// create Image object
		//	pSwapchain->TargetImages[i].Color = this->Module->Make<Image>();
		//	//pSwapchain->TargetImages[i].Color->ImageHandle = image;
		//	//pSwapchain->TargetImages[i].Color->ImageView = imageView;
		//	//pSwapchain->TargetImages[i].Color->Allocation = imageAllocation;
		//	}

		//// set a depth format
		//if( parameters.SurfaceDepthFormat.has_value() )
		//	{
		//	pSwapchain->SurfaceDepthFormat = parameters.SurfaceDepthFormat;
		//	}
		//else
		//	{
		//	// select a depth format
		//	pSwapchain->SurfaceDepthFormat = VK_FORMAT_UNDEFINED;

		//	// possible depth formats
		//	static const vector<VkFormat> depthFormats =
		//		{
		//		VK_FORMAT_D32_SFLOAT,
		//		VK_FORMAT_D32_SFLOAT_S8_UINT,
		//		VK_FORMAT_D24_UNORM_S8_UINT
		//		};

		//	for( VkFormat format : depthFormats )
		//		{
		//		VkFormatProperties props;
		//		vkGetPhysicalDeviceFormatProperties( this->PhysicalDeviceHandle, format, &props );
		//		if( ( props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT ) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
		//			{
		//			pSwapchain->SurfaceDepthFormat = format;
		//			break;
		//			}
		//		}

		//	Validate( pSwapchain->SurfaceDepthFormat , status_code::invalid ) << "Could not find a compatible depth surface format." << ValidateEnd;
		//	}
		//pSwapchain->DepthFormatHasStencil = ( pSwapchain->SurfaceDepthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || pSwapchain->SurfaceDepthFormat == VK_FORMAT_D24_UNORM_S8_UINT );
		//
		//// create the depth images
		////this->DepthImages.resize( imageCount );
		////this->DepthImageViews.resize( imageCount );
		////this->DepthImageAllocations.resize( imageCount );
		//for (uint i = 0; i < imageCount; i++)
		//	{
		//	VkImage image;
		//	VkImageView imageView;
		//	VmaAllocation imageAllocation;

		//	VkImageCreateInfo depthImageCreateInfo{};
		//	depthImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		//	depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		//	depthImageCreateInfo.extent.width = this->RenderExtent.width;
		//	depthImageCreateInfo.extent.height = this->RenderExtent.height;
		//	depthImageCreateInfo.extent.depth = 1;
		//	depthImageCreateInfo.mipLevels = 1;
		//	depthImageCreateInfo.arrayLayers = 1;
		//	depthImageCreateInfo.format = this->SurfaceDepthFormat;
		//	depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		//	depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//	depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		//	depthImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		//	depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		//	VmaAllocationCreateInfo allocationCreateInfo{};
		//	allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		//	allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		//	VLK_CALL( vmaCreateImage( this->MemoryAllocator, &depthImageCreateInfo, &allocationCreateInfo, &image, &imageAllocation, nullptr ) );

		//	// create the depth image view
		//	VkImageViewCreateInfo depthImageViewCreateInfo {};
		//	depthImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		//	depthImageViewCreateInfo.image = image;
		//	depthImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		//	depthImageViewCreateInfo.format = this->SurfaceDepthFormat;
		//	depthImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		//	depthImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		//	depthImageViewCreateInfo.subresourceRange.levelCount = 1;
		//	depthImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		//	depthImageViewCreateInfo.subresourceRange.layerCount = 1;
		//	VLK_CALL( vkCreateImageView( this->Device, &depthImageViewCreateInfo, nullptr, &imageView ) );

		//	// transition the depth image from undefined, to device specific layout
		//	VkCommandBuffer commandBuffer = this->BeginInternalCommandBuffer();
		//	VkImageMemoryBarrier imageMemoryBarrier{};
		//	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		//	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		//	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		//	imageMemoryBarrier.image = image;
		//	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		//	if (this->DepthFormatHasStencil)
		//		{
		//		imageMemoryBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		//		}
		//	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		//	imageMemoryBarrier.subresourceRange.levelCount = 1;
		//	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		//	imageMemoryBarrier.subresourceRange.layerCount = 1;
		//	imageMemoryBarrier.srcAccessMask = 0;
		//	imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		//	vkCmdPipelineBarrier( commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier );
		//	this->EndAndSubmitInternalCommandBuffer( commandBuffer );

		//	// create Image object
		//	this->TargetImages[i].Depth = BD_NEW_UNIQUE_PTR( Image );
		//	this->TargetImages[i].Depth->ImageHandle = image;
		//	this->TargetImages[i].Depth->ImageView = imageView;
		//	this->TargetImages[i].Depth->Allocation = imageAllocation;
		//	}

		//// set up the render pass, color and depth buffers 
		//VkAttachmentDescription attachmentDescriptions[2] = {};
		//// color
		//attachmentDescriptions[0].format = this->SurfaceFormat.format;
		//attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		//attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//// depth
		//attachmentDescriptions[1].format = this->SurfaceDepthFormat;
		//attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
		//attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE; 
		//attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//// subpass with color and depth
		//VkAttachmentReference colorAttachmentReference{};
		//colorAttachmentReference.attachment = 0;
		//colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//VkAttachmentReference depthAttachmentReference{};
		//depthAttachmentReference.attachment = 1;
		//depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		//VkSubpassDescription subpassDescription{};
		//subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		//subpassDescription.colorAttachmentCount = 1;
		//subpassDescription.pColorAttachments = &colorAttachmentReference;
		//subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

		//// subpass dependency description
		//VkSubpassDependency subpassDependency{};
		//subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		//subpassDependency.dstSubpass = 0;
		//subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//subpassDependency.srcAccessMask = 0;
		//subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//// create the render pass
		//VkRenderPassCreateInfo renderPassCreateInfo{};
		//renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassCreateInfo.attachmentCount = 2;
		//renderPassCreateInfo.pAttachments = attachmentDescriptions;
		//renderPassCreateInfo.subpassCount = 1;
		//renderPassCreateInfo.pSubpasses = &subpassDescription;
		//renderPassCreateInfo.dependencyCount = 1;
		//renderPassCreateInfo.pDependencies = &subpassDependency;
		//VLK_CALL( vkCreateRenderPass( this->Device, &renderPassCreateInfo, nullptr, &this->RenderPass ) );

		//// set up swap chain framebuffers
		//this->Framebuffers.resize( imageCount );
		//for( uint i = 0; i < imageCount; ++i )
		//	{
		//	VkImageView attachments[2] = { this->TargetImages[i].Color->GetImageView() , this->TargetImages[i].Depth->GetImageView() };

		//	VkFramebufferCreateInfo framebufferCreateInfo{};
		//	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		//	framebufferCreateInfo.renderPass = this->RenderPass;
		//	framebufferCreateInfo.attachmentCount = 2;
		//	framebufferCreateInfo.pAttachments = attachments;
		//	framebufferCreateInfo.width = this->RenderExtent.width;
		//	framebufferCreateInfo.height = this->RenderExtent.height;
		//	framebufferCreateInfo.layers = 1;
		//	VLK_CALL( vkCreateFramebuffer( this->Device, &framebufferCreateInfo, nullptr, &Framebuffers[i] ) );
		//	}

		//// resize list of image fences
		//ImagesInFlight.resize( this->SwapChainImages.size(), VK_NULL_HANDLE );
		
		return status_code::ok;
		}

	status Swapchain::Cleanup()
		{
		return status_code::ok;
		}




}