// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Device.h"
#include "bdr_Buffer.h"
#include "bdr_CommandPool.h"

namespace bdr
{
const std::vector<VkBufferCopy> BufferTemplate::fullBufferCopy = { {0,0,VK_WHOLE_SIZE} };

Buffer::Buffer( Device* _module ) : DeviceSubmodule( _module ) 
	{
	}

Buffer::~Buffer()
	{
	this->Cleanup();
	}

status Buffer::Setup( const BufferTemplate& parameters )
	{
	// create the buffer using vma and the parameters
	CheckCall( vmaCreateBuffer(
		this->GetModule()->GetMemoryAllocatorHandle(),
		&parameters.bufferCreateInfo,
		&parameters.allocationCreateInfo,
		&this->BufferHandle,
		&this->BufferAllocation,
		nullptr
	) );
	this->BufferSize = parameters.bufferCreateInfo.size;

	// optionally upload data to the buffer
	if( parameters.upload.has_value() )
		{
		auto &uploadData = parameters.upload.value();
		CheckCall( CopyMemoryToBuffer( uploadData.sourcePtr, uploadData.sourceSize, this, parameters.commandPool, uploadData.copyRegions ) );
		}

	// done, return buffer
	return status::ok;
	}

status Buffer::CopyMemoryRegions( const uint8_t *srcPtr , size_t srcSize , uint8_t *destPtr , size_t destSize , const std::vector<VkBufferCopy> &copyRegions )
	{
	// if no regions are defined, copy the size of dest from the source buffer
	if( copyRegions.empty() )
		{
		Validate( destSize == srcSize , status::invalid_param ) << "When no copyRegion is defined, the source and dest memory sizes must match." << ValidateEnd;

		// copy everything
		memcpy( destPtr, srcPtr, destSize );
		}
	else
		{
		// copy each of the defined regions (no check for overlaps done)
		for(VkBufferCopy bc : copyRegions)
			{
#ifndef NDEBUG
			VkDeviceSize src_end_index = bc.srcOffset + bc.size;
			VkDeviceSize dst_end_index = bc.dstOffset + bc.size;
			Validate( src_end_index <= srcSize && dst_end_index <= destSize , status::invalid_param ) << "At least one of the copy regions are outside of the allowed memory range" << ValidateEnd;
#endif
			const uint8_t* cpySrcPtr = srcPtr + bc.srcOffset;
			uint8_t* cpyDstPtr = destPtr + bc.dstOffset;
			memcpy( cpyDstPtr, cpySrcPtr, (size_t)bc.size );
			}
		}
	return status::ok;	
	}

status Buffer::Cleanup()
	{
	if( this->BufferAllocation )
		{
		vmaDestroyBuffer( this->GetModule()->GetMemoryAllocatorHandle(), this->BufferHandle, this->BufferAllocation );

		this->BufferHandle = nullptr;
		this->BufferAllocation = nullptr;
		this->BufferSize = 0;
		}

	return status::ok;
	}

status Buffer::CopyBufferToMemory( const Buffer *srcBuffer , void *destMemory , size_t destMemorySize , CommandPool *commandPool , const std::vector<VkBufferCopy> &copyRegions )
	{
	Validate( destMemory && srcBuffer && commandPool , status::invalid_param ) << "A source buffer, dest memory and commmand pool must be specified for this command." << ValidateEnd;

	auto memProps = srcBuffer->GetAllocationMemoryProperties();

	// map either a staging buffer or the buffer directly
	if( memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
		{
		// we can map the buffer directly, copy the memory manually
		CheckCall( CopyHostVisibleBufferToMemory( srcBuffer, destMemory, destMemorySize, copyRegions ) );
		}
	else
		{
		// this is a GPU buffer, so needs a copy, use a temporary staging buffer in CPU memory to copy through
		auto stagingBuffer = std::unique_ptr<Buffer>( new Buffer(srcBuffer->Module) );
		CheckCall( stagingBuffer->Setup( BufferTemplate::ManualBuffer(
			VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_AUTO,
			destMemorySize
		) ) );

		// copy from this GPU buffer to the CPU buffer using the copy regions
		CheckCall( CopyBufferToBuffer( srcBuffer , stagingBuffer.get() , commandPool , copyRegions ) );

		// copy the full staging buffer to the dest memory pointer
		CheckCall( CopyHostVisibleBufferToMemory( stagingBuffer.get() , destMemory , destMemorySize ) );
		}
	
	return status::ok;
	}

status Buffer::CopyHostVisibleBufferToMemory( const Buffer *srcBuffer , void *destMemory , size_t destMemorySize , const std::vector<VkBufferCopy> &copyRegions )
	{
	Validate( destMemory && srcBuffer , status::invalid_param ) << "A source buffer and dest memory must be specified for this command." << ValidateEnd;

	CheckRetValCall( srcMemory , srcBuffer->MapMemory() );
	CheckCall( CopyMemoryRegions( (const uint8_t*)srcMemory , srcBuffer->BufferSize , (uint8_t*)destMemory , destMemorySize , copyRegions ) );
	srcBuffer->UnmapMemory();

	return status::ok;
	}

status Buffer::CopyMemoryToBuffer( const void *srcMemory , size_t srcMemorySize , const Buffer *destBuffer , CommandPool *commandPool , const std::vector<VkBufferCopy> &copyRegions )
	{
	Validate( destBuffer && srcMemory , status::invalid_param ) << "A dest buffer and source memory must be specified for this command." << ValidateEnd;

	auto memProps = destBuffer->GetAllocationMemoryProperties();

	// check if host visible
	if( memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
		{
		// we can map the buffer directly, copy the memory manually
		CheckCall( CopyMemoryToHostVisibleBuffer( srcMemory, srcMemorySize, destBuffer, copyRegions ) );
		}
	else
		{
		Validate( commandPool , status::invalid_param ) << "Since the buffer is in GPU memory, a commandPool must be specified for this command." << ValidateEnd;

		// this is a GPU buffer, so needs a copy, use a temporary staging buffer in CPU memory to copy through
		auto stagingBuffer = std::unique_ptr<Buffer>( new Buffer(destBuffer->Module) );

		// setup staging buffer, and copy the full source memory to the staging buffer
		CheckCall( stagingBuffer->Setup( BufferTemplate::ManualBuffer(
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VMA_MEMORY_USAGE_AUTO,
			srcMemorySize,
			srcMemory
		) ) );

		// copy from the CPU buffer into the GPU buffer, using the copyRegions
		CheckCall( CopyBufferToBuffer( stagingBuffer.get() , destBuffer , commandPool , copyRegions ) );
		}
	
	return status::ok;
	}

status Buffer::CopyMemoryToHostVisibleBuffer( const void *srcMemory , size_t srcMemorySize , const Buffer *destBuffer , const std::vector<VkBufferCopy> &copyRegions )
	{
	Validate( destBuffer && srcMemory, status::invalid_param ) << "A dest buffer and source memory must be specified for this command." << ValidateEnd;

	CheckRetValCall( destMemory , destBuffer->MapMemory() );
	CheckCall( CopyMemoryRegions( (uint8_t*)destMemory , destBuffer->BufferSize , (uint8_t*)srcMemory , srcMemorySize , copyRegions ) );
	destBuffer->UnmapMemory();

	return status::ok;
	}
			
status Buffer::CopyBufferToBuffer( const Buffer *srcBuffer , const Buffer *destBuffer , CommandPool *commandPool , const std::vector<VkBufferCopy> &copyRegions )
	{
	Validate( destBuffer && srcBuffer && commandPool , status::invalid_param ) << "A dest buffer, source buffer and commmand pool must be specified for this command." << ValidateEnd;
	
	// create the command buffer
	CheckRetValCall( commandBuffer , commandPool->BeginCommandBuffer() );
	if( copyRegions.empty() )
		{
		// copy the full source buffer
		VkBufferCopy region = { 0, 0, srcBuffer->GetBufferSize() };
		commandBuffer->CopyBuffer( srcBuffer , destBuffer , { region } );
		}
	else
		{
		commandBuffer->CopyBuffer( srcBuffer , destBuffer , copyRegions );
		}
	CheckCall( commandPool->EndCommandBuffer( commandBuffer ) );

	// run the buffer, wait for it to finish
	CheckCall( commandPool->SubmitCommandBuffer( commandBuffer , true ) );

	return status::ok;
	}

status_return<VkDeviceAddress> Buffer::GetDeviceAddress() const
	{
#ifndef NDEBUG
	Validate( this->Module->GetModule()->GetBufferDeviceAddressExtension() , status::not_initialized ) << "BufferDeviceAddressExtension is not enabled." << ValidateEnd;
#endif
	VkBufferDeviceAddressInfo addressInfo = {};
	addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	addressInfo.buffer = this->BufferHandle;
	return vkGetBufferDeviceAddress( this->GetModule()->GetDeviceHandle(), &addressInfo );
	}

status_return<const void*> Buffer::MapMemory() const
	{
	Validate( (this->GetAllocationMemoryProperties() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) , status::invalid ) << "MapMemory can only be called on CPU-visible buffers." << ValidateEnd;

	void *memoryPtr = nullptr;
	CheckCall( vmaMapMemory( this->GetModule()->GetMemoryAllocatorHandle(), this->BufferAllocation, &memoryPtr ) );
	return memoryPtr;
	}

status_return<void*> Buffer::MapMemory()
	{
	Validate( (this->GetAllocationMemoryProperties() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) , status::invalid ) << "MapMemory can only be called on CPU-visible buffers." << ValidateEnd;

	void *memoryPtr = nullptr;
	CheckCall( vmaMapMemory( this->GetModule()->GetMemoryAllocatorHandle(), this->BufferAllocation, &memoryPtr ) );
	return memoryPtr;
	}

void Buffer::UnmapMemory() const
	{
	vmaUnmapMemory( this->GetModule()->GetMemoryAllocatorHandle(), this->BufferAllocation );
	}

VkMemoryPropertyFlags Buffer::GetAllocationMemoryProperties() const
	{
	// get the type of buffer
	VkMemoryPropertyFlags memFlags = {};
	vmaGetAllocationMemoryProperties( this->GetModule()->GetMemoryAllocatorHandle() , this->BufferAllocation , &memFlags );
	return memFlags;
	}

BufferTemplate BufferTemplate::ManualBuffer( VkBufferUsageFlags bufferUsageFlags, VmaAllocationCreateFlags allocationCreateFlags, VmaMemoryUsage memoryUsage, VkDeviceSize bufferSize, const void* src_data , const std::vector<VkBufferCopy> &copyRegions, CommandPool *commandPool )
	{
	BufferTemplate ret;

	// basic create info
	ret.bufferCreateInfo.size = bufferSize;
	ret.bufferCreateInfo.usage = bufferUsageFlags;
	ret.bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// allocation info
	ret.allocationCreateInfo.flags = allocationCreateFlags;
	ret.allocationCreateInfo.usage = memoryUsage;

	ret.commandPool = commandPool;

	// upload info, upload the whole buffer
	if( src_data )
		{
		ret.upload.set();
		auto &uploadData = ret.upload.value();
						
		uploadData.sourcePtr = src_data;
		uploadData.sourceSize = bufferSize;
		uploadData.copyRegions = copyRegions;

		// cap the size if using the predefined param
		if( &copyRegions == &fullBufferCopy )
			{
			uploadData.copyRegions[0].size = bufferSize;
			}
		}

	return ret;
	}

BufferTemplate BufferTemplate::UniformBuffer( VkDeviceSize bufferSize, const void* src_data, CommandPool *commandPool  )
	{
	return ManualBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		bufferSize,
		src_data,
		fullBufferCopy,
		commandPool
		);
	}

}
// namespace bdr

