#pragma once

#include "bdr_Buffer.h"

namespace bdr
{

// Wrapping and conversion object for VkBufferMemoryBarrier
class BufferMemoryBarrier
	{
	public:
		// Clears the structure and sets sType
		BufferMemoryBarrier();

		// Buffer barrier which ignores queue family, and defaults to whole buffer
		BufferMemoryBarrier(
			const Buffer *buffer,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkDeviceSize offset = 0,
			VkDeviceSize size = VK_WHOLE_SIZE
		);

		VkBufferMemoryBarrier bufferMemoryBarrier = {};

		operator const VkBufferMemoryBarrier &( )
			{
			return bufferMemoryBarrier;
			}
	};

inline BufferMemoryBarrier::BufferMemoryBarrier()
	{
	bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	}

inline BufferMemoryBarrier::BufferMemoryBarrier(
	const Buffer *buffer,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkDeviceSize offset,
	VkDeviceSize size
)
	{
	this->bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	this->bufferMemoryBarrier.srcAccessMask = srcAccessMask;
	this->bufferMemoryBarrier.dstAccessMask = dstAccessMask;
	this->bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->bufferMemoryBarrier.buffer = buffer->GetBufferHandle();
	this->bufferMemoryBarrier.offset = offset;
	this->bufferMemoryBarrier.size = size;
	}

}
// namespace bdr
