// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Image.h"
#include "bdr_ImageMemoryBarrier.h"
#include "bdr_BufferMemoryBarrier.h"
#include "bdr_Device.h"
#include "bdr_CommandPool.h"
#include "bdr_Helpers.h"
#include "bdr_Buffer.h"

namespace bdr
{

Image::Image( Device* _module ) : DeviceSubmodule( _module ) 
	{
	}

Image::~Image()
	{
	this->Cleanup();
	}

status Image::Setup( const ImageTemplate &parameters )
	{
	// create the image
	CheckCall( vmaCreateImage(
		this->GetModule()->GetMemoryAllocatorHandle(),
		&parameters.imageCreateInfo,
		&parameters.allocationCreateInfo,
		&this->ImageHandle,
		&this->ImageAllocation,
		nullptr
	) );

	// create the image view
	VkImageViewCreateInfo imageViewCreateInfo = parameters.imageViewCreateInfo;
	imageViewCreateInfo.image = this->ImageHandle;
	CheckCall( vkCreateImageView( this->GetModule()->GetDeviceHandle(), &imageViewCreateInfo, nullptr, &this->ImageView ) );

	// optionally, upload pixel data to the image
	if( parameters.upload.has_value() )
		{
		Validate( parameters.commandPool, status::invalid_param ) << "To upload data when creating an Image, you need to specify a commandPool object to use." << ValidateEnd;
		Validate( parameters.layoutTransition.has_value(), status::invalid_param ) << "When using upload, you must also the the layoutTransition to apply after the upload of data." << ValidateEnd;

		const auto &upload = parameters.upload.value();
		const auto &layoutTransition = parameters.layoutTransition.value();

		CheckCall( CopyMemoryToImage(
			upload.srcMemory,
			upload.srcMemorySize,
			this,
			layoutTransition.imageMemoryBarrier,
			layoutTransition.destPipelineStage,
			parameters.commandPool,
			upload.copyRegions
		) );

		// upload done
		}
	else if( parameters.layoutTransition.has_value() )
		{
		Validate( parameters.commandPool , status::invalid_param ) << "Applying layoutTransition requires the commandPool object to be valid." << ValidateEnd;
		
		const auto &layoutTransition = parameters.layoutTransition.value();

		// not dependent on anything upstream (VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT), defined what is dependent downstream (destPipelineStage)
		CheckCall( this->TransitionImageLayout(
			parameters.commandPool,
			layoutTransition.imageMemoryBarrier,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
			layoutTransition.destPipelineStage
		) );
		
		}

	return status::ok;
	}

status Image::Cleanup()
	{
	if( this->ImageView != nullptr )
		{
		vkDestroyImageView( this->GetModule()->GetDeviceHandle(), this->ImageView, nullptr );
		this->ImageView = nullptr;
		}
	if( this->ImageAllocation != nullptr )
		{
		vmaDestroyImage( this->GetModule()->GetMemoryAllocatorHandle(), this->ImageHandle, this->ImageAllocation );
		this->ImageHandle = nullptr;
		this->ImageAllocation = nullptr;
		}
	return status::ok;
	}


status Image::CopyImageToMemory(
	const Image *srcImage,
	VkImageLayout srcImageLayout,
	VkAccessFlags srcImageAccessMask,
	VkPipelineStageFlags srcPipelineStage,
	void *destMemory,
	size_t destMemorySize,
	CommandPool *commandPool,
	const std::vector<VkBufferImageCopy> &copyRegions,
	VkImageSubresourceRange subresourceRange
)
	{
	Validate( destMemory && srcImage && commandPool, status::invalid_param ) << "A dest memory pointer, source image and commmand pool must be specified for this method." << ValidateEnd;
	Validate( !copyRegions.empty(), status::invalid_param ) << "At least one copy region needs to be specified" << ValidateEnd;

	// if not specified, use default subresource ranges 
	if( !subresourceRange.aspectMask )
		{
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
		}

	// a temporary staging buffer in host-available memory to copy through 
	CheckRetValCall( stagingBuffer, srcImage->Module->CreateObject<Buffer>( BufferTemplate::ManualBuffer(
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		VMA_MEMORY_USAGE_AUTO,
		destMemorySize
	) ) );

	// create the command buffer
	CheckRetValCall( commandBuffer, commandPool->BeginCommandBuffer() );

	// transfer image from current layout and access mask to transfer layout, transfer read access 
	commandBuffer->AddImageMemoryBarrier( ImageMemoryBarrier(
		srcImage,
		srcImageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		srcImageAccessMask, VK_ACCESS_TRANSFER_READ_BIT,
		subresourceRange
	) );
	commandBuffer->AddBufferMemoryBarrier( BufferMemoryBarrier(
		stagingBuffer.get(),
		0, VK_ACCESS_TRANSFER_WRITE_BIT
	) );
	commandBuffer->PipelineBarrier( srcPipelineStage, VK_PIPELINE_STAGE_TRANSFER_BIT );

	// copy the data to the buffer
	commandBuffer->CopyImageToBuffer( srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer.get(), copyRegions );

	// return the image to the original layout and access mask
	commandBuffer->AddImageMemoryBarrier( ImageMemoryBarrier(
		srcImage,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcImageLayout,
		VK_ACCESS_TRANSFER_READ_BIT, srcImageAccessMask,
		subresourceRange
	) );
	commandBuffer->AddBufferMemoryBarrier( BufferMemoryBarrier(
		stagingBuffer.get(),
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT
	) );
	commandBuffer->PipelineBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT );

	// run the buffer, wait for it to finish
	CheckCall( commandPool->EndCommandBuffer( commandBuffer ) );
	CheckCall( commandPool->SubmitCommandBuffer( commandBuffer, true ) );

	// copy the full staging buffer to the dest memory pointer
	CheckCall( Buffer::CopyBufferToMemory( stagingBuffer.get(), destMemory, destMemorySize, commandPool ) );

	return status::ok;
	}

status Image::CopyMemoryToImage(
	const void *srcMemory,
	size_t srcMemorySize,
	const Image *destImage,
	const VkImageMemoryBarrier &destImageMemoryBarrier,
	VkPipelineStageFlags destPipelineStage,
	CommandPool *commandPool,
	const std::vector<VkBufferImageCopy> &copyRegions
)
	{
	Validate( destImage && srcMemory && commandPool, status::invalid_param ) << "A destination image, source memory pointer, and commmand pool must be specified for this method." << ValidateEnd;
	Validate( !copyRegions.empty(), status::invalid_param ) << "At least one copy region needs to be specified" << ValidateEnd;

	// staging buffer in host-available memory to copy through 
	CheckRetValCall( stagingBuffer, destImage->Module->CreateObject<Buffer>( BufferTemplate::ManualBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		VMA_MEMORY_USAGE_AUTO,
		srcMemorySize,
		srcMemory
	) ) );

	// create the command buffer
	CheckRetValCall( commandBuffer, commandPool->BeginCommandBuffer() );

	// transfer image into transfer destination, use the subresourceRange of the final imageMemoryBarrier layout
	commandBuffer->AddImageMemoryBarrier( ImageMemoryBarrier(
		destImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		destImageMemoryBarrier.subresourceRange
	) );
	commandBuffer->PipelineBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT );

	// copy the data from the buffer to the image
	commandBuffer->CopyBufferToImage( stagingBuffer.get(), destImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegions );

	// apply the dest image memory barrier, to place image in correct layout after the memory transfer 
	commandBuffer->AddImageMemoryBarrier( destImageMemoryBarrier, destImage );
	commandBuffer->PipelineBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT, destPipelineStage );

	// run the buffer, wait for it to finish
	CheckCall( commandPool->EndCommandBuffer( commandBuffer ) );
	CheckCall( commandPool->SubmitCommandBuffer( commandBuffer, true ) );

	return status::ok;
	}

status Image::TransitionImageLayout(
	CommandPool *commandPool,
	const VkImageMemoryBarrier &imageMemoryBarrier,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags destStageMask
)
	{
	Validate( commandPool, status::invalid_param ) << "A commmand pool must be specified for this method." << ValidateEnd;

	// create the command buffer
	CheckRetValCall( commandBuffer, commandPool->BeginCommandBuffer() );

	// apply the dest image memory barrier
	commandBuffer->AddImageMemoryBarrier( imageMemoryBarrier, this );
	commandBuffer->PipelineBarrier( srcStageMask, destStageMask );
	
	// run the buffer, wait for it to finish
	CheckCall( commandPool->EndCommandBuffer( commandBuffer ) );
	CheckCall( commandPool->SubmitCommandBuffer( commandBuffer, true ) );

	return status::ok;
	}

//
//void Image::TransitionLayout( VkImageMemoryBarrier &imageMemoryBarrier, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags destStageMask )
//	{
//	this->Module->RunBlockingCommandBuffer(
//		[&]( VkCommandBuffer cmd )
//		{
//		vkCmdPipelineBarrier( cmd, srcStageMask, destStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier );
//		}
//	);
//	}
//
//
//void Image::TransitionLayout( VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags destStageMask, VkImageAspectFlags aspectMask )
//	{
//	VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
//	imageMemoryBarrier.oldLayout = oldLayout;
//	imageMemoryBarrier.newLayout = newLayout;
//	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageMemoryBarrier.image = this->ImageHandle;
//	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
//	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
//	imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
//	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
//	imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
//	imageMemoryBarrier.srcAccessMask = srcAccessMask;
//	imageMemoryBarrier.dstAccessMask = dstAccessMask;
//
//	this->TransitionLayout( imageMemoryBarrier, srcStageMask, destStageMask );
//	}
//
//void Image::CopyToBuffer( Buffer *destBuffer, uint32_t width, uint32_t height, uint32_t depth, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags destStageMask, VkImageAspectFlags aspectMask )
//	{
//	VkBufferImageCopy bufferImageCopy;
//
//	bufferImageCopy.bufferOffset = 0;
//	bufferImageCopy.bufferRowLength = 0;
//	bufferImageCopy.bufferImageHeight = 0;
//	bufferImageCopy.imageSubresource.aspectMask = aspectMask;
//	bufferImageCopy.imageSubresource.mipLevel = 0;
//	bufferImageCopy.imageSubresource.baseArrayLayer = 0;
//	bufferImageCopy.imageSubresource.layerCount = 1;
//	bufferImageCopy.imageOffset = { 0, 0, 0 };
//	bufferImageCopy.imageExtent.width = width;
//	bufferImageCopy.imageExtent.height = height;
//	bufferImageCopy.imageExtent.depth = depth;
//
//	this->CopyToBuffer( destBuffer, &bufferImageCopy, oldLayout, newLayout, srcAccessMask, dstAccessMask, srcStageMask, destStageMask, aspectMask );
//	}
//
//void Image::CopyToBuffer( Buffer *destBuffer, const VkBufferImageCopy *region, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags destStageMask, VkImageAspectFlags aspectMask )
//	{
//	if( oldLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL || srcAccessMask != VK_ACCESS_TRANSFER_READ_BIT )
//		{
//		// need to transition layout
//		this->TransitionLayout(
//			oldLayout,
//			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//			srcAccessMask,
//			VK_ACCESS_TRANSFER_READ_BIT,
//			srcStageMask,
//			VK_PIPELINE_STAGE_TRANSFER_BIT,
//			aspectMask
//		);
//		}
//
//	// now copy to buffer
//	this->Module->RunBlockingCommandBuffer(
//		[&]( VkCommandBuffer cmd )
//		{
//		vkCmdCopyImageToBuffer( cmd, this->ImageHandle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destBuffer->GetBuffer(), 1, region );
//		}
//	);
//
//	if( newLayout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL || dstAccessMask != VK_ACCESS_TRANSFER_READ_BIT )
//		{
//		// need to transition layout again
//		this->TransitionLayout(
//			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//			newLayout,
//			VK_ACCESS_TRANSFER_READ_BIT,
//			dstAccessMask,
//			VK_PIPELINE_STAGE_TRANSFER_BIT,
//			destStageMask,
//			aspectMask
//		);
//		}
//	}
//
/////////////////////////////////////////////
//
//#define IMAGE_CREATE_INFO_2D( _format , _usage , _mipLevels )\
//		ret.ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;\
//		ret.ImageCreateInfo.extent.width = width;\
//		ret.ImageCreateInfo.extent.height = height;\
//		ret.ImageCreateInfo.extent.depth = 1;\
//		ret.ImageCreateInfo.mipLevels = _mipLevels;\
//		ret.ImageCreateInfo.arrayLayers = 1;\
//		ret.ImageCreateInfo.format = _format;\
//		ret.ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;\
//		ret.ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;\
//		ret.ImageCreateInfo.usage = _usage;\
//		ret.ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;\
//		ret.ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//#define UPLOAD_BUFFER_IMAGE_COPY( _index , _bufferOffset , _aspectMask , _mipMapLevel , _extent )\
//		ret.UploadBufferImageCopies[_index].bufferOffset = _bufferOffset;\
//		ret.UploadBufferImageCopies[_index].bufferRowLength = 0;\
//		ret.UploadBufferImageCopies[_index].bufferImageHeight = 0;\
//		ret.UploadBufferImageCopies[_index].imageSubresource.aspectMask = _aspectMask;\
//		ret.UploadBufferImageCopies[_index].imageSubresource.mipLevel = _mipMapLevel;\
//		ret.UploadBufferImageCopies[_index].imageSubresource.baseArrayLayer = 0;\
//		ret.UploadBufferImageCopies[_index].imageSubresource.layerCount = 1;\
//		ret.UploadBufferImageCopies[_index].imageOffset = { 0, 0, 0 };\
//		ret.UploadBufferImageCopies[_index].imageExtent = _extent;
//
//#define IMAGE_MEMORY_BARRIER( _object , _oldLayout , _newLayout , _aspectMask , _srcAccessMask , _dstAccessMask )\
//		_object.oldLayout = _oldLayout;\
//		_object.newLayout = _newLayout;\
//		_object.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;\
//		_object.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;\
//		_object.subresourceRange.aspectMask = _aspectMask;\
//		_object.subresourceRange.baseMipLevel = 0;\
//		_object.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;\
//		_object.subresourceRange.baseArrayLayer = 0;\
//		_object.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;\
//		_object.srcAccessMask = _srcAccessMask;\
//		_object.dstAccessMask = _dstAccessMask;
//
//#define IMAGE_VIEW_CREATE_INFO_2D( _format , _aspectMask , _mipmap_levels )\
//		ret.ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;\
//		ret.ImageViewCreateInfo.format = _format;\
//		ret.ImageViewCreateInfo.subresourceRange.aspectMask = _aspectMask;\
//		ret.ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;\
//		ret.ImageViewCreateInfo.subresourceRange.levelCount = _mipmap_levels;\
//		ret.ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;\
//		ret.ImageViewCreateInfo.subresourceRange.layerCount = 1;
//
//
//
//static void FillInCreateStructures(
//	ImageTemplate &params,
//	VkImageType imageType,
//	VkImageCreateFlags imageFlags,
//	VkFormat format,
//	VkImageUsageFlags usage,
//	VkImageAspectFlags aspectMask,
//	uint32_t width,
//	uint32_t height,
//	uint32_t depth,
//	uint32_t mipmapCount,
//	uint32_t arrayLayerCount,
//	VkSampleCountFlagBits samples,
//	VkImageViewCreateFlags imageViewFlags,
//	VkImageViewType viewType,
//)
//	{
//	
//
//
//
//
//	// vma allocation
//	ret.AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
//
//	// setup image view
//	IMAGE_VIEW_CREATE_INFO_2D( format, aspectMask, mipmap_levels );
//
//	// should we upload?
//	if( source_ptr )
//		{
//		// if source_size is not set, assume per-pixel and multiply (will not work with compressed formats)
//		if( source_size == 0 )
//			{
//			source_size = (VkDeviceSize)
//				ret.ImageCreateInfo.extent.width *
//				ret.ImageCreateInfo.extent.height *
//				ret.ImageCreateInfo.extent.depth *
//				GetVulkanFormatByteSize( format );
//			}
//
//		ret.UploadSourcePtr = source_ptr;
//		ret.UploadSourceSize = source_size;
//
//		if( source_mipmap_offsets )
//			{
//			// upload mipmaps
//			ret.UploadBufferImageCopies.resize( mipmap_levels );
//
//			// set up mipmap transfers
//			VkExtent3D extent = ret.ImageCreateInfo.extent;
//			for( uint32_t m = 0; m < mipmap_levels; ++m )
//				{
//				UPLOAD_BUFFER_IMAGE_COPY( m, source_mipmap_offsets[m], aspectMask, m, extent );
//				extent.width = ( extent.width > 1 ) ? extent.width / 2 : 1;
//				extent.height = ( extent.height > 1 ) ? extent.height / 2 : 1;
//				extent.depth = ( extent.depth > 1 ) ? extent.depth / 2 : 1;
//				}
//			}
//		else
//			{
//			// only one transfer 
//			ret.UploadBufferImageCopies.resize( 1 );
//			UPLOAD_BUFFER_IMAGE_COPY( 0, 0, aspectMask, 0, ret.ImageCreateInfo.extent );
//			}
//
//		IMAGE_MEMORY_BARRIER( ret.UploadLayoutTransition, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspectMask, VK_ACCESS_NONE_KHR, VK_ACCESS_TRANSFER_WRITE_BIT );
//		}
//
//	// tell the image setup to do a final transition of the image
//	ret.TransitionImageLayout = transitionImageLayout;
//	if( transitionImageLayout )
//		{
//		// setup the final layout
//		if( source_ptr )
//			ret.FinalLayoutTransition.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		else
//			ret.FinalLayoutTransition.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		ret.FinalLayoutTransition.newLayout = finalLayout;
//		ret.FinalLayoutTransition.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		ret.FinalLayoutTransition.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		ret.FinalLayoutTransition.subresourceRange.aspectMask = aspectMask;
//		ret.FinalLayoutTransition.subresourceRange.baseMipLevel = 0;
//		ret.FinalLayoutTransition.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
//		ret.FinalLayoutTransition.subresourceRange.baseArrayLayer = 0;
//		ret.FinalLayoutTransition.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
//		if( source_ptr )
//			ret.FinalLayoutTransition.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		else
//			ret.FinalLayoutTransition.srcAccessMask = VK_ACCESS_NONE_KHR;
//		ret.FinalLayoutTransition.dstAccessMask = finalAccessMask;
//		ret.FinalLayoutStageMask = finalLayoutStageMask;
//		}
//
//	return ret;
//	}
//
//
//
//	,
//	const void *source_ptr,
//	VkDeviceSize source_size,
//	const VkDeviceSize *source_mipmap_offsets,
//	bool transitionImageLayout,
//	VkImageLayout finalLayout,
//	VkAccessFlags finalAccessMask,
//	VkPipelineStageFlags finalLayoutStageMask
//


ImageTemplate ImageTemplate::Texture( VkImageType imageType, VkImageViewType imageViewType, VkFormat format, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmapLevels, uint32_t arrayLayers, CommandPool *commandPool, const void *sourcePtr, size_t sourceSize, const std::vector<VkBufferImageCopy> copyRegions )
	{
	ImageTemplate ret;

	// fill in image create info
	ret.imageCreateInfo = {};
	ret.imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ret.imageCreateInfo.flags = 0; 
	ret.imageCreateInfo.imageType = imageType;
	ret.imageCreateInfo.format = format;
	ret.imageCreateInfo.extent.width = width;
	ret.imageCreateInfo.extent.height = height;
	ret.imageCreateInfo.extent.depth = depth;
	ret.imageCreateInfo.mipLevels = mipmapLevels;
	ret.imageCreateInfo.arrayLayers = arrayLayers;
	ret.imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	ret.imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	ret.imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	ret.imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ret.imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	// fill in image view create info
	ret.imageViewCreateInfo = {};
	ret.imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ret.imageViewCreateInfo.viewType = imageViewType;
	ret.imageViewCreateInfo.format = format;
	ret.imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	ret.imageViewCreateInfo.subresourceRange.layerCount = arrayLayers;
	ret.imageViewCreateInfo.subresourceRange.levelCount = mipmapLevels;

	// set the command pool
	ret.commandPool = commandPool;

	// if sourcePtr is set, add upload data
	if( sourcePtr )
		{
		ret.upload.set();
		Upload &upload = ret.upload;

		upload.srcMemory = sourcePtr;
		upload.srcMemorySize = sourceSize;
		upload.copyRegions = copyRegions;
		}

	// add a layout transition, either from an uploaded state, or from the original undefined state
	ret.layoutTransition.set();
	LayoutTransition &layoutTransition = ret.layoutTransition;
	layoutTransition.imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	if( sourcePtr )
		{
		layoutTransition.imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		layoutTransition.imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
	else
		{
		layoutTransition.imageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;
		layoutTransition.imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		}
	layoutTransition.imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	layoutTransition.imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	layoutTransition.imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransition.imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	layoutTransition.imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	layoutTransition.imageMemoryBarrier.subresourceRange.layerCount = arrayLayers;
	layoutTransition.imageMemoryBarrier.subresourceRange.levelCount = mipmapLevels;
	
	return ret;
	}

ImageTemplate ImageTemplate::Texture2D( VkFormat format, uint32_t width, uint32_t height, uint32_t mipmapLevels, CommandPool *commandPool, const void *sourcePtr, size_t sourceSize, const std::vector<VkBufferImageCopy> copyRegions )
	{
	return Texture(
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_VIEW_TYPE_2D,
		format,
		width,
		height,
		1,
		mipmapLevels,
		1,
		commandPool,
		sourcePtr,
		sourceSize,
		copyRegions
	);
	}


//ImageTemplate ImageTemplate::General2D( VkFormat format, uint32_t width, uint32_t height, uint32_t mipmap_levels )
//	{
//	// setup general 2d image
//	return Standard2DImage(
//		format,
//		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
//		VK_IMAGE_ASPECT_COLOR_BIT,
//		width, height,
//		mipmap_levels,
//		nullptr,
//		0,
//		nullptr,
//		true,
//		VK_IMAGE_LAYOUT_GENERAL,
//		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_READ_BIT,
//		VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT
//	);
//	}

// create a 2d color image which is optimized for texture sampling. If source ptr is set, uploads data to the texture. 




};