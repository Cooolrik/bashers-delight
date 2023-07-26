// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_FramebufferPool.h"
#include "bdr_Image.h"
#include "bdr_RenderPass.h"
#include "bdr_Helpers.h"

namespace bdr
{
	FramebufferPool::FramebufferPool( Device* _module ) : DeviceSubmodule(_module) 
		{
		LogThis;
		}

	FramebufferPool::~FramebufferPool()
		{
		LogThis;

		this->Cleanup();
		}

	status FramebufferPool::Setup( const FramebufferPoolTemplate& parameters )
		{
		Validate( parameters.framebuffersCount > 0 , status::invalid_param ) << "Must allocate at least one framebuffer." << ValidateEnd;
		Validate( parameters.renderPass, status::invalid_param ) << "A RenderPass object is required to set up framebuffer." << ValidateEnd;
		Validate( parameters.commandPool, status::invalid_param ) << "A CommandPool object is required to set up framebuffer." << ValidateEnd;
		
		const size_t imageCount = parameters.imageSetups.size();
		const auto commandPool = parameters.commandPool;
		const auto width = parameters.extent.width;
		const auto height = parameters.extent.height;

		// set up the image templates
		vector<ImageTemplate> imageTemplates( imageCount );
		for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
			{
			const auto format = parameters.imageSetups[imageInx].format;
			const auto samples = parameters.imageSetups[imageInx].samples;
			
			if( HasVulkanFormatDepth(format) )
				imageTemplates[imageInx] = ImageTemplate::DepthAttachment( format, width, height, commandPool, samples );
			else
				imageTemplates[imageInx] = ImageTemplate::ColorAttachment( format, width, height, commandPool, samples );
			}

		// allocate framebuffers vector, and for each framebuffer, set up the images 
		this->Framebuffers.resize( parameters.framebuffersCount );
		for( size_t framebufferInx = 0; framebufferInx < parameters.framebuffersCount; ++framebufferInx )
			{
			auto &framebuffer = this->Framebuffers[framebufferInx];

			// allocate images for the buffer
			framebuffer.Images.resize(imageCount);
			for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
				{
				CheckRetValCall( image, this->Module->CreateObject<Image>( imageTemplates[imageInx] ) );
				this->Framebuffers[framebufferInx].Images[imageInx] = std::move(image);
				}

			// setup the framebuffer object
			vector<VkImageView> attachments(imageCount);
			for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
				{
				attachments[imageInx] = this->Framebuffers[framebufferInx].Images[imageInx]->GetImageView();
				}

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = parameters.renderPass->GetRenderPassHandle();
			framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = width;
			framebufferCreateInfo.height = height;
			framebufferCreateInfo.layers = 1;
			CheckCall( vkCreateFramebuffer( this->Module->GetDeviceHandle(), &framebufferCreateInfo, nullptr, &this->Framebuffers[framebufferInx].FramebufferHandle ) );
			}

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

		return status::ok;
		}

	status FramebufferPool::Cleanup()
		{
		;
		for( size_t framebufferInx = 0; framebufferInx < this->Framebuffers.size(); ++framebufferInx )
			{
			vkDestroyFramebuffer( this->Module->GetDeviceHandle(), this->Framebuffers[framebufferInx].FramebufferHandle, nullptr );
			}
		this->Framebuffers.clear();

		return status::ok;
		}

	status_return<FramebufferPoolTemplate> FramebufferPoolTemplate::FromRenderPass( size_t framebuffersCount, uint32_t width, uint32_t height, const RenderPass *renderPass, CommandPool *commandPool )
		{
		Validate( renderPass , status::invalid_param ) << "A valid RenderPass object is required." << ValidateEnd;

		FramebufferPoolTemplate ret = {};

		ret.framebuffersCount = framebuffersCount;
		ret.extent = { width, height };
		ret.renderPass = renderPass;
		ret.commandPool = commandPool;

		const auto &renderPassTemplate = renderPass->GetTemplate();

		const size_t imageCount = renderPassTemplate.attachmentDescriptions.size();
		ret.imageSetups.resize( imageCount );
		for( size_t inx=0; inx<imageCount; ++inx )
			{
			ret.imageSetups[inx].format = renderPassTemplate.attachmentDescriptions[inx].format;
			ret.imageSetups[inx].samples = renderPassTemplate.attachmentDescriptions[inx].samples;
			}

		return ret;
		}

}