#pragma once

#include "bdr_Device.h"

namespace bdr
{

// A buffer is the main class of data buffers in bdr. 
class Buffer : public DeviceSubmodule
	{
	public:
		~Buffer();

	private:
		friend status_return<unique_ptr<Buffer>> Device::CreateObject<Buffer, BufferTemplate>( const BufferTemplate &parameters );

		Buffer( Device *_module );
		status Setup( const BufferTemplate &parameters );

	protected:
		VkBuffer BufferHandle = nullptr;
		VmaAllocation BufferAllocation = nullptr;
		VkDeviceSize BufferSize = 0;

		// copies memory regions from source to dest. if copyRegions is empty, copy all of dest's size from source (srcSize must be >= destSize)
		static status CopyMemoryRegions( const uint8_t *src, size_t srcSize, uint8_t *dest, size_t destSize, const std::vector<VkBufferCopy> &copyRegions );

		// Copy buffer data from another buffer into this buffer, using the specific command pool. Use one or multiple regions of data to copy. 
		// The copy is done synchronously in the calling thread, and the call retuns after the copy is complete. 
		static status CopyBufferToBuffer( const Buffer *srcBuffer, const Buffer *destBuffer, CommandPool *commandPool, const std::vector<VkBufferCopy> &copyRegions = {} );

	public:
		// explicitly clear the buffer data, and release the vulkan handles. 
		// (note that buffers cannot be re-created after cleanup. Instead, delete and create a new object)
		status Cleanup();

		// returns the device address of the buffer. 
		// Caveat: needs BufferDeviceAddressExtension enabled
		status_return<VkDeviceAddress> GetDeviceAddress() const;

		// Map/unmap buffer memory. 
		// Caveat: This is only for CPU/host-visible buffers.
		status_return<const void *> MapMemory() const;
		status_return<void *> MapMemory();
		void UnmapMemory() const;

		// get the Vulkan handles
		VkBuffer GetBufferHandle() const { return this->BufferHandle; }
		VmaAllocation GetBufferAllocation() const { return this->BufferAllocation; }
		VkDeviceSize GetBufferSize() const { return this->BufferSize; }
		VkMemoryPropertyFlags GetAllocationMemoryProperties() const;

		// Buffer to memory copy. 
		// The copy is done synchronously in the calling thread, and the call retuns after the copy is complete. 
		// If the buffer is in host/CPU memory, the copy is done using MapMemory, and the CommandPool is not needed.
		// If the buffer is in GPU memory, the copy is done through a staging buffer, which is sized to the size of the destination memory, and copied fully to destMemory
		// If copyRegions is empty, the full size of the buffer is copied to the dest memory, which must exactly the same size as the buffer
		static status CopyBufferToMemory( const Buffer *srcBuffer, void *destMemory, size_t destMemorySize, CommandPool *commandPool, const std::vector<VkBufferCopy> &copyRegions = {} );
		static status CopyHostVisibleBufferToMemory( const Buffer *srcBuffer, void *destMemory, size_t destMemorySize, const std::vector<VkBufferCopy> &copyRegions = {} );

		// Memory to buffer copy. 
		// The copy is done synchronously in the calling thread, and the call retuns after the copy is complete. 
		// If the buffer is in host/CPU memory, the copy is done using MapMemory, and the CommandPool is not needed.
		// If the buffer is in GPU memory, the copy is done through a staging buffer, which is a full copy of the source memory
		// If copyRegions is empty, the full size of the buffer is copied to the source memory, which must exactly the same size as the buffer
		static status CopyMemoryToBuffer( const void *srcMemory, size_t srcMemorySize, const Buffer *destBuffer, CommandPool *commandPool, const std::vector<VkBufferCopy> &copyRegions = {} );
		static status CopyMemoryToHostVisibleBuffer( const void *srcMemory, size_t srcMemorySize, const Buffer *destBuffer, const std::vector<VkBufferCopy> &copyRegions = {} );
	};

class BufferTemplate
	{
	public:
		BufferTemplate()
			{
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			}

		// const parameter which copies the full buffer
		static const std::vector<VkBufferCopy> fullBufferCopy;

	public:

		// initial create information
		VkBufferCreateInfo bufferCreateInfo = {};

		// vma allocation object
		VmaAllocationCreateInfo allocationCreateInfo = {};

		// the command pool to use (only needed if uploading data to a GPU buffer)
		CommandPool *commandPool = nullptr;

		// if an upload is to be made, allocate this structure. the copy can be one or multiple ranges.
		class UploadData
			{
			public:
				const void *sourcePtr = nullptr;
				VkDeviceSize sourceSize = 0;
				std::vector<VkBufferCopy> copyRegions; // if empty, the full data size is copied from the source (make sure sourceSize is at least the size of the created buffer)
			};
		optional_value<UploadData> upload;

		/////////////////////////////////

		// create a buffer manually, and (optionally) copy the whole data size from a memory address
		static BufferTemplate ManualBuffer(
			VkBufferUsageFlags bufferUsageFlags,
			VmaAllocationCreateFlags allocationCreateFlags,
			VmaMemoryUsage memoryUsageFlags,
			VkDeviceSize bufferSize,
			const void *src_data = nullptr,
			const std::vector<VkBufferCopy> &copyRegions = fullBufferCopy,
			CommandPool *commandPool = nullptr
		);

		// create a uniform buffer, prefer GPU size, but with unbuffered CPU access
		static BufferTemplate UniformBuffer(
			VkDeviceSize bufferSize,
			const void *src_data = nullptr,
			CommandPool *commandPool = nullptr
		);

	};

}
// namespace bdr