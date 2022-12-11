
#include "bdr_Common.inl"

#include "bdr_Buffer.h"

VkDeviceAddress bdr::Buffer::GetDeviceAddress() const
	{
	VkBufferDeviceAddressInfo addressInfo{};
	addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	addressInfo.buffer = this->BufferHandle;
	return vkGetBufferDeviceAddress( this->Module->GetDevice(), &addressInfo );
	}

void* bdr::Buffer::MapMemory()
	{
	void* memoryPtr;
	VLK_CALL( vmaMapMemory( this->Module->GetMemoryAllocator(), this->Allocation, &memoryPtr ) );
	return memoryPtr;
	}

void bdr::Buffer::UnmapMemory()
	{
	vmaUnmapMemory( this->Module->GetMemoryAllocator(), this->Allocation );
	}

bdr::Buffer::~Buffer()
	{
	if( this->Allocation != nullptr )
		{
		vmaDestroyBuffer( this->Module->GetMemoryAllocator(), this->BufferHandle, this->Allocation );
		}
	}

bdr::BufferTemplate bdr::BufferTemplate::ManualBuffer( VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryPropertyFlags, VkDeviceSize bufferSize, const void* src_data )
	{
	BufferTemplate ret;

	// basic create info
	ret.BufferCreateInfo.size = bufferSize;
	ret.BufferCreateInfo.usage = bufferUsageFlags;
	ret.BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	// allocation info
	ret.AllocationCreateInfo.usage = memoryPropertyFlags;

	// upload info
	if(src_data)
		{
		ret.UploadSourcePtr = src_data;
		ret.UploadSourceSize = bufferSize;

		// one copy, the whole buffer
		ret.UploadBufferCopies.resize(1);
		ret.UploadBufferCopies[0].srcOffset = 0;
		ret.UploadBufferCopies[0].dstOffset = 0;
		ret.UploadBufferCopies[0].size = bufferSize;
		}

	return ret;
	}

bdr::BufferTemplate bdr::BufferTemplate::UniformBuffer( VkDeviceSize bufferSize, const void* src_data )
	{
	return ManualBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU,
		bufferSize,
		src_data
		);
	}


