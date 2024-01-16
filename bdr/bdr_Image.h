// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr
{
class Buffer;

class Image : public DeviceSubmodule
	{
	public:
		~Image();

	private:
		friend status_return<unique_ptr<Image>> Device::CreateObject<Image, ImageTemplate>( const ImageTemplate &parameters );
		Image( Device *_module );
		status Setup( const ImageTemplate &parameters );

	private:
		VkImage ImageHandle = nullptr;
		VkImageView ImageView = nullptr;
		VmaAllocation ImageAllocation = nullptr;

	public:
		// explicitly clear the image data, and release the vulkan handles. 
		// (note that images cannot be re-created after cleanup. Instead, delete and create a new object)
		status Cleanup();

		// Image to memory copy 
		// The copy is done synchronously in the calling thread, and the call retuns after the copy is complete. 
		// subresourceRange defines what part of the image should be locked
		// If subresourceRange is omitted, it is assumed to be all aspects, all mips and all levels (performance warning!)
		// srcImageLayout and srcImageAccessMask are the current layout and access masks of the image, and it will be restored to these after copy.
		// srcPipelineStage is the pipeline stage to wait for before copying the source image. This can be set to VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT to not wait for any stage.
		static status CopyImageToMemory(
			const Image *srcImage,
			VkImageLayout srcImageLayout,
			VkAccessFlags srcImageAccessMask,
			VkPipelineStageFlags srcPipelineStage,
			void *destMemory,
			size_t destMemorySize,
			CommandPool *commandPool,
			const std::vector<VkBufferImageCopy> &copyRegions,
			VkImageSubresourceRange subresourceRange = {}
		);

		// Memory to image copy 
		// The copy is done synchronously in the calling thread, and the call retuns after the copy is complete. 
		// subresourceRange defined what part of the image should be locked
		// If subresourceRange is omitted, it is assumed to be all aspects, all mips and all levels (performance warning!)
		// destImageLayout and destImageAccessMask are the layout and access masks to set on the image after copy
		// destPipelineStage is the pipeline which will be dependent on the image upload. This can be set to VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT to not mark any dependent stage.
		static status CopyMemoryToImage(
			const void *srcMemory,
			size_t srcMemorySize,
			const Image *destImage,
			const VkImageMemoryBarrier &imageMemoryBarrier,
			VkPipelineStageFlags destPipelineStage,
			CommandPool *commandPool,
			const std::vector<VkBufferImageCopy> &copyRegions
		);

		// transition the image layout using a memory barrier. Note that this method is synchronized and the method
		// waits for the command buffer to complete before continuing. 
		status TransitionImageLayout(
			CommandPool *commandPool,
			const VkImageMemoryBarrier &imageMemoryBarrier,
			VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VkPipelineStageFlags destStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
		);

		//void TransitionLayout(
		//	VkImageLayout oldLayout,
		//	VkImageLayout newLayout,
		//	VkAccessFlags srcAccessMask,
		//	VkAccessFlags dstAccessMask,
		//	VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_NONE_KHR,
		//	VkPipelineStageFlags destStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		//	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		//	);

		//// copy the image to a buffer. The image will be put into transfer mode unless it is already in it,
		//// which is specified in oldLayout/srcAccessMask
		//void CopyToBuffer( 
		//	Buffer *destBuffer,
		//	uint32_t width, 
		//	uint32_t height, 
		//	uint32_t depth = 1,
		//	VkImageLayout oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		//	VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		//	VkAccessFlags srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
		//	VkAccessFlags dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
		//	VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		//	VkPipelineStageFlags destStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		//	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		//	);
		//void CopyToBuffer(
		//	Buffer* destBuffer,
		//	const VkBufferImageCopy* region = nullptr,
		//	VkImageLayout oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		//	VkImageLayout newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		//	VkAccessFlags srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
		//	VkAccessFlags dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
		//	VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		//	VkPipelineStageFlags destStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
		//	VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		//	);

		// retrieve Vulkan handles
		VkImage GetImageHandle() const
			{
			return this->ImageHandle;
			}
		VmaAllocation GetImageAllocation() const
			{
			return this->ImageAllocation;
			}
		VkImageView GetImageView() const
			{
			return this->ImageView;
			}
	};

// template used to create an image
class ImageTemplate
	{
	public:
		// image and imageview create infos
		VkImageCreateInfo imageCreateInfo = {};
		VkImageViewCreateInfo imageViewCreateInfo = {};

		// vma allocation object
		VmaAllocationCreateInfo allocationCreateInfo = {};

		// if an upload is to be made, here is the layout transition before the upload and the source pointer to copy from
		class Upload
			{
			public:
				const void *srcMemory = nullptr;
				size_t srcMemorySize = 0;
				std::vector<VkBufferImageCopy> copyRegions;
			};
		optional_value<Upload> upload;

		// optional image transition, applied to finalize the image
		// Note: that if upload is set, layoutTransition *must* also be set, as it will be applied after the upload
		class LayoutTransition
			{
			public:
				VkImageMemoryBarrier imageMemoryBarrier = {};
				VkPipelineStageFlags destPipelineStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			};
		optional_value<LayoutTransition> layoutTransition;

		// the command pool is needed to upload data, and/or set the final layout of the image
		CommandPool *commandPool = nullptr;

		/////////////////////////////////

		// create a general texture image
		static ImageTemplate Texture( VkImageType imageType, VkImageViewType imageViewType, VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmapLevels, uint32_t arrayLayers, CommandPool *commandPool = nullptr, const void *sourcePtr = nullptr, size_t sourceSize = 0, const std::vector<VkBufferImageCopy> copyRegions = {} );

		// create a single layer 2d color texture 
		static ImageTemplate Texture2D( VkFormat format, uint32_t width, uint32_t height, uint32_t mipmapLevels = 1, CommandPool *commandPool = nullptr, const void *sourcePtr = nullptr, size_t sourceSize = 0, const std::vector<VkBufferImageCopy> copyRegions = {} );

		// create a 2d general layout (color) image that can be used for storage and sampling in shaders
		//static ImageTemplate General2D( VkFormat format, uint32_t width, uint32_t height, uint32_t mipmapLevels = 1 );

		// create a color or depth attachment image
		static ImageTemplate ColorAttachment( VkFormat format, uint32_t width, uint32_t height, CommandPool *commandPool, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );
		static ImageTemplate DepthAttachment( VkFormat format, uint32_t width, uint32_t height, CommandPool *commandPool, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT );
	};

}
//namespace bdr