#pragma once

#include "bdr_Image.h"

namespace bdr
{

// Wrapping and conversion object for VkImageMemoryBarrier
class ImageMemoryBarrier
	{
	public:
		// Clears the structure and sets sType
		ImageMemoryBarrier();

		// Simple image barrier which ignores oldLayout and srcAccessMask, assumes all levels, all mips, ignores queue family, and defaults to color aspect
		// Caveat: This setup *may* destroy the current data in the image, and should only be used when inserting new content
		ImageMemoryBarrier(
			const Image *image,
			VkImageLayout newLayout,
			VkAccessFlags dstAccessMask,
			VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		);

		// Image barrier which ignores oldLayout and srcAccessMask, ignores queue family, and takes in subresourceRange
		// Caveat: This setup *may* destroy the current data in the image, and should only be used when inserting new content
		ImageMemoryBarrier(
			const Image *image,
			VkImageLayout newLayout,
			VkAccessFlags dstAccessMask,
			VkImageSubresourceRange subresourceRange
		);

		// Simple image barrier which assumes all levels, all mips, ignores queue family, and defaults to color aspect
		ImageMemoryBarrier(
			const Image *image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		);

		// Image barrier which ignores queue family, and takes in subresourceRange
		ImageMemoryBarrier(
			const Image *image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkImageSubresourceRange subresourceRange
		);

		// Full image barrier, with queue and subresourceRange
		ImageMemoryBarrier(
			const Image *image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			uint32_t baseMipLevel = 0,
			uint32_t levelCount = VK_REMAINING_MIP_LEVELS,
			uint32_t baseArrayLayer = 0,
			uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS
		);

		VkImageMemoryBarrier imageMemoryBarrier = {};

		operator const VkImageMemoryBarrier &( )
			{
			return imageMemoryBarrier;
			}
	};

inline ImageMemoryBarrier::ImageMemoryBarrier()
	{
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	}

inline ImageMemoryBarrier::ImageMemoryBarrier(
	const Image *image,
	VkImageLayout newLayout,
	VkAccessFlags dstAccessMask,
	VkImageAspectFlags aspectMask 
)
	{
	this->imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	this->imageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;
	this->imageMemoryBarrier.dstAccessMask = dstAccessMask;
	this->imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	this->imageMemoryBarrier.newLayout = newLayout;
	this->imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.image = image->GetImageHandle();
	this->imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	this->imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	this->imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	this->imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	this->imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	}

inline ImageMemoryBarrier::ImageMemoryBarrier(
	const Image *image,
	VkImageLayout newLayout,
	VkAccessFlags dstAccessMask,
	VkImageSubresourceRange subresourceRange
)
	{
	this->imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	this->imageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;
	this->imageMemoryBarrier.dstAccessMask = dstAccessMask;
	this->imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	this->imageMemoryBarrier.newLayout = newLayout;
	this->imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.image = image->GetImageHandle();
	this->imageMemoryBarrier.subresourceRange = subresourceRange;
	}

inline ImageMemoryBarrier::ImageMemoryBarrier(
	const Image *image,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkImageAspectFlags aspectMask
)
	{
	this->imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	this->imageMemoryBarrier.srcAccessMask = srcAccessMask;
	this->imageMemoryBarrier.dstAccessMask = dstAccessMask;
	this->imageMemoryBarrier.oldLayout = oldLayout;
	this->imageMemoryBarrier.newLayout = newLayout;
	this->imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.image = image->GetImageHandle();
	this->imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	this->imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	this->imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	this->imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	this->imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	}

inline ImageMemoryBarrier::ImageMemoryBarrier(
	const Image *image,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkImageSubresourceRange subresourceRange
)
	{
	this->imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	this->imageMemoryBarrier.srcAccessMask = srcAccessMask;
	this->imageMemoryBarrier.dstAccessMask = dstAccessMask;
	this->imageMemoryBarrier.oldLayout = oldLayout;
	this->imageMemoryBarrier.newLayout = newLayout;
	this->imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->imageMemoryBarrier.image = image->GetImageHandle();
	this->imageMemoryBarrier.subresourceRange = subresourceRange;
	}

inline ImageMemoryBarrier::ImageMemoryBarrier(
	const Image *image,
	VkImageLayout oldLayout,
	VkImageLayout newLayout,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	uint32_t srcQueueFamilyIndex,
	uint32_t dstQueueFamilyIndex,
	VkImageAspectFlags aspectMask,
	uint32_t baseMipLevel,
	uint32_t levelCount,
	uint32_t baseArrayLayer,
	uint32_t layerCount
)
	{
	this->imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	this->imageMemoryBarrier.srcAccessMask = srcAccessMask;
	this->imageMemoryBarrier.dstAccessMask = dstAccessMask;
	this->imageMemoryBarrier.oldLayout = oldLayout;
	this->imageMemoryBarrier.newLayout = newLayout;
	this->imageMemoryBarrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
	this->imageMemoryBarrier.dstQueueFamilyIndex = dstQueueFamilyIndex;
	this->imageMemoryBarrier.image = image->GetImageHandle();
	this->imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	this->imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
	this->imageMemoryBarrier.subresourceRange.levelCount = levelCount;
	this->imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	this->imageMemoryBarrier.subresourceRange.layerCount = layerCount;
	}

}
//namespace bdr
