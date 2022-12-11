#include "bdr_Common.inl"

#include "bdr_VertexBuffer.h"

bdr::VertexBuffer::~VertexBuffer()
	{
	}

void bdr::VertexBufferDescription::SetVertexInputBindingDescription( uint32_t binding, uint32_t stride, VkVertexInputRate inputRate )
	{
	this->VertexInputBindingDescription.binding = binding;
	this->VertexInputBindingDescription.stride = stride;
	this->VertexInputBindingDescription.inputRate = inputRate;
	}

void bdr::VertexBufferDescription::AddVertexInputAttributeDescription( uint32_t binding, uint32_t location, VkFormat format, uint32_t offset )
	{
	VkVertexInputAttributeDescription desc;

	desc.binding = binding;
	desc.location = location;
	desc.format = format;
	desc.offset = offset;

	this->VertexInputAttributeDescriptions.push_back( desc );
	}

bdr::VertexBufferTemplate bdr::VertexBufferTemplate::VertexBuffer( const VertexBufferDescription& description, uint vertexCount, const void* src_data, VkBufferUsageFlags additionalBufferUsageFlags )
	{
	// calc the size of the buffer
	VkDeviceSize bufferSize = (VkDeviceSize)description.GetVertexInputBindingDescription().stride * vertexCount;

	// init the template with the base buffer
	VertexBufferTemplate ret = {};
	(BufferTemplate &)ret = BufferTemplate::ManualBuffer(
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | additionalBufferUsageFlags,
		VMA_MEMORY_USAGE_GPU_ONLY,
		bufferSize,
		src_data
	);

	// set the description 
	ret.Description = description;
	return ret;
	}
