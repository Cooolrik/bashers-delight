#include "bdr_CommandPool.h"
#include "bdr_GraphicsPipeline.h"
#include "bdr_ComputePipeline.h"
#include "bdr_VertexBuffer.h"
#include "bdr_IndexBuffer.h"
#include "bdr_Image.h"

#include "Extensions/bdr_RayTracingExtension.h"
#include "Extensions/bdr_RayTracingPipeline.h"
#include "Extensions/bdr_RayTracingShaderBindingTable.h"

#include <stdexcept>
#include <algorithm>

using std::runtime_error;

// makes sure the return value is VK_SUCCESS or throws an exception
#define VLK_CALL( s ) if( s != VK_SUCCESS ) { throw runtime_error( "Vulkan call " #s " failed (did not return VK_SUCCESS)"); }
#define ASSERT_RECORDING() if( !this->IsRecordingBuffer ) { throw runtime_error( "Error: " __FUNCTION__ "(): currently not recording buffer."); }

bdr::CommandPool::~CommandPool()
	{
	// the allocated buffers will be automatically deallocated
	vkDestroyCommandPool( this->Module->GetDevice(), Pool, nullptr );
	}

void bdr::CommandPool::ResetCommandPool()
	{
	// make sure we are not recording
	if( this->IsRecordingBuffer )
		{
		throw runtime_error( "Error: ResetCommandPool(), currently recording buffer. End buffer before begin new or resetting." );
		}

	VLK_CALL( vkResetCommandPool( this->Module->GetDevice(), this->Pool, 0 ) );

	// reset index
	this->CurrentBufferIndex = -1;
	}

VkCommandBuffer bdr::CommandPool::BeginCommandBuffer()
	{
	// make sure we are not already recording
	if( this->IsRecordingBuffer )
		{
		throw runtime_error( "Error: BeginCommandBuffer(), currently recording buffer. End buffer before begin new or resetting." );
		}
	this->IsRecordingBuffer = true;

	// step up buffer index, make sure we are not out of buffers
	++this->CurrentBufferIndex;
	if( (size_t)this->CurrentBufferIndex >= this->Buffers.size() )
		{
		throw runtime_error( "Error: BeginCommandBuffer(), out of buffers. Allocate enough buffers when allocating pool." );
		}

	// begin the buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = 0; 
	commandBufferBeginInfo.pInheritanceInfo = nullptr; 
	VLK_CALL( vkBeginCommandBuffer( this->Buffers[this->CurrentBufferIndex], &commandBufferBeginInfo ) );

	// return buffer handle 
	return this->Buffers[this->CurrentBufferIndex];
	}

void bdr::CommandPool::BeginRenderPass( VkFramebuffer destFramebuffer )
	{
	ASSERT_RECORDING();

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = this->Module->RenderPass;
	renderPassBeginInfo.framebuffer = destFramebuffer;

	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = this->Module->RenderExtent;

	VkClearValue clearValues[2]{};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass( this->Buffers[this->CurrentBufferIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );
	}

void bdr::CommandPool::EndRenderPass()
	{
	ASSERT_RECORDING();

	vkCmdEndRenderPass( this->Buffers[this->CurrentBufferIndex] );
	}

void bdr::CommandPool::BindPipeline( Pipeline* pipeline )
	{
	ASSERT_RECORDING();

	vkCmdBindPipeline( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineBindPoint(), pipeline->GetPipeline() );
	}

void bdr::CommandPool::BindVertexBuffer( VertexBuffer* buffer )
	{
	ASSERT_RECORDING();

	VkBuffer vertexBuffers[] = { buffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers( this->Buffers[this->CurrentBufferIndex], 0, 1, vertexBuffers, offsets );
	}

void bdr::CommandPool::BindIndexBuffer( IndexBuffer* buffer )
	{
	ASSERT_RECORDING();

	vkCmdBindIndexBuffer( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), 0, buffer->GetIndexType() );
	}

void bdr::CommandPool::BindDescriptorSet( Pipeline* pipeline, VkDescriptorSet set )
	{
	ASSERT_RECORDING();

	vkCmdBindDescriptorSets( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineBindPoint(), pipeline->GetPipelineLayout(), 0, 1, &set, 0, nullptr );
	}

void bdr::CommandPool::PushConstants( Pipeline* pipeline , VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues )
	{
	ASSERT_RECORDING();

	vkCmdPushConstants( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineLayout(), stageFlags, offset, size, pValues );
	}

void bdr::CommandPool::SetViewport( VkViewport viewport )
	{
	ASSERT_RECORDING();

	vkCmdSetViewport( this->Buffers[this->CurrentBufferIndex], 0, 1, &viewport );
	}

void bdr::CommandPool::SetViewport( float x, float y, float width, float height, float minDepth, float maxDepth )
	{
	VkViewport viewport;
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = minDepth;
	viewport.maxDepth = maxDepth;
	this->SetViewport( viewport );
	}

void bdr::CommandPool::SetScissorRectangle( VkRect2D scissorRectangle )
	{
	ASSERT_RECORDING();

	vkCmdSetScissor( this->Buffers[this->CurrentBufferIndex], 0, 1, &scissorRectangle );
	}

void bdr::CommandPool::SetScissorRectangle( int32_t x, int32_t y, uint32_t width, uint32_t height )
	{
	VkRect2D scissorRectangle;
	scissorRectangle.offset.x = x;
	scissorRectangle.offset.y = y;
	scissorRectangle.extent.width = width;
	scissorRectangle.extent.height = height;
	this->SetScissorRectangle( scissorRectangle );
	}

void bdr::CommandPool::UpdateBuffer( Buffer* buffer, VkDeviceSize dstOffset, uint32_t dataSize, const void* pData )
	{
	ASSERT_RECORDING();

	vkCmdUpdateBuffer( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), dstOffset, dataSize, pData );
	}

void bdr::CommandPool::Draw( uint vertexCount )
	{
	ASSERT_RECORDING();

	vkCmdDraw( this->Buffers[this->CurrentBufferIndex], vertexCount, 1, 0, 0 );
	}

void bdr::CommandPool::DrawIndexed( uint indexCount, uint instanceCount, uint firstIndex, int vertexOffset, uint firstInstance )
	{
	ASSERT_RECORDING();

	vkCmdDrawIndexed( this->Buffers[this->CurrentBufferIndex], indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}

void bdr::CommandPool::DrawIndexedIndirect( const Buffer *buffer, VkDeviceSize offset, uint drawCount, uint stride )
	{
	ASSERT_RECORDING();

	vkCmdDrawIndexedIndirect( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), offset, drawCount, stride );
	}

void bdr::CommandPool::QueueUpBufferMemoryBarrier( VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDeviceSize offset, VkDeviceSize size )
	{
	VkBufferMemoryBarrier bufferMemoryBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	bufferMemoryBarrier.srcAccessMask = srcAccessMask;
	bufferMemoryBarrier.dstAccessMask = dstAccessMask;
	bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferMemoryBarrier.buffer = buffer;
	bufferMemoryBarrier.offset = offset;
	bufferMemoryBarrier.size = size;
	this->BufferMemoryBarriers.push_back( bufferMemoryBarrier );
	}

void bdr::CommandPool::QueueUpBufferMemoryBarrier( const Buffer* buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDeviceSize offset, VkDeviceSize size )
	{
	this->QueueUpBufferMemoryBarrier(
		buffer->GetBuffer(),
		srcAccessMask,
		dstAccessMask,
		offset,
		(size == VkDeviceSize( ~0 )) ? buffer->GetBufferSize() : size
	);
	}

void bdr::CommandPool::QueueUpImageMemoryBarrier( VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageAspectFlags aspectMask )
	{
	VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = oldLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0; 
	imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	this->ImageMemoryBarriers.push_back( imageMemoryBarrier );
	}

void bdr::CommandPool::QueueUpImageMemoryBarrier( const Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageAspectFlags aspectMask )
	{
	this->QueueUpImageMemoryBarrier(
		image->GetImage(),
		oldLayout,
		newLayout,
		srcAccessMask,
		dstAccessMask,
		aspectMask
		);
	}

void bdr::CommandPool::PipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
	{
	ASSERT_RECORDING();

	VkBufferMemoryBarrier* pbuf = this->BufferMemoryBarriers.data();
	VkImageMemoryBarrier* pimg = this->ImageMemoryBarriers.data();

	vkCmdPipelineBarrier( 
		this->Buffers[this->CurrentBufferIndex], 
		srcStageMask, 
		dstStageMask, 
		VK_DEPENDENCY_BY_REGION_BIT,
		0, nullptr, 
		(uint)this->BufferMemoryBarriers.size(), pbuf,
		(uint)this->ImageMemoryBarriers.size(), pimg
		);

	BufferMemoryBarriers.clear();
	ImageMemoryBarriers.clear();
	}

void bdr::CommandPool::DispatchCompute( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ )
	{
	ASSERT_RECORDING();

	vkCmdDispatch( this->Buffers[this->CurrentBufferIndex], groupCountX, groupCountY, groupCountZ );
	}

void bdr::CommandPool::TraceRays( RayTracingShaderBindingTable* sbt , uint width , uint height )
	{
	ASSERT_RECORDING();

	RayTracingExtension::vkCmdTraceRaysKHR(
		this->Buffers[this->CurrentBufferIndex],
		&sbt->GetRaygenDeviceAddress(),
		&sbt->GetMissDeviceAddress(),
		&sbt->GetClosestHitDeviceAddress(),
		&sbt->GetCallableDeviceAddress(),
		width,
		height,
		1 );
	}

void bdr::CommandPool::EndCommandBuffer()
	{
	ASSERT_RECORDING();

	// done with the buffer
	VLK_CALL( vkEndCommandBuffer( this->Buffers[this->CurrentBufferIndex] ) );
	this->IsRecordingBuffer = false;
	}