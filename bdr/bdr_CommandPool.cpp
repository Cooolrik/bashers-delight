// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_CommandPool.h"

//#include "bdr_GraphicsPipeline.h"
//#include "bdr_ComputePipeline.h"
//#include "bdr_VertexBuffer.h"
//#include "bdr_IndexBuffer.h"
//#include "bdr_Image.h"

//#include "Extensions/bdr_RayTracingExtension.h"
//#include "Extensions/bdr_RayTracingPipeline.h"
//#include "Extensions/bdr_RayTracingShaderBindingTable.h"

#include <stdexcept>
#include <algorithm>

namespace bdr
{
	CommandPool::CommandPool( const Instance* _module, VkCommandPool commandPoolHandle, const std::vector<VkCommandBuffer> &bufferObjects ) : MainSubmodule(_module) , CommandPoolHandle( commandPoolHandle ) , BuffersCount( bufferObjects.size() )
		{
		LogThis;

		this->SetupCommandBuffers( bufferObjects );
		}

	CommandPool::~CommandPool()
		{
		LogThis;

		// the allocated buffers will be automatically deallocated
		vkDestroyCommandPool( this->Module->GetDevice()->GetDeviceHandle(), this->CommandPoolHandle, nullptr );

		this->DeleteCommandBuffers();
		}

	void CommandPool::SetupCommandBuffers( const std::vector<VkCommandBuffer> &bufferObjects )
		{
		SanityCheck( bufferObjects.size() == this->BuffersCount );

		// fill in the buffer objects
		this->Buffers = new CommandBuffer[this->BuffersCount];
		for( size_t inx=0; inx<this->BuffersCount; ++inx )
			{
			this->Buffers[inx].CommandPool_ = this;
			this->Buffers[inx].BufferIndex = inx;
			this->Buffers[inx].CommandBufferHandle = bufferObjects[inx];
			}
		}

	void CommandPool::DeleteCommandBuffers()
		{
		SanityCheck( this->Buffers );

		delete [] this->Buffers;
		}


	status CommandPool::ResetCommandPool()
		{
		Validate( !this->IsRecording() , status_code::invalid ) << "Cannot reset command pool, there is at least one buffer still recording" << ValidateEnd;

		CheckCall( vkResetCommandPool( this->Module->GetDevice()->GetDeviceHandle(), this->CommandPoolHandle, 0 ) );
		return status_code::ok;
		}

	status_return<CommandBuffer*> CommandPool::BeginCommandBuffer()
		{
		Validate( this->ActiveBuffers.size() < this->BuffersCount , status_code::invalid ) << "Cannot allocate buffer for recording, they are all used up." << ValidateEnd;

		// look for an available buffer
		// round robin look using CurrentBufferIndex
		for(;;)
			{
			this->CurrentBufferIndex = (this->CurrentBufferIndex + 1) % this->BuffersCount;
			if( this->ActiveBuffers.find( this->CurrentBufferIndex ) == this->ActiveBuffers.end() )
				break;
			}

		SanityCheck( this->Buffers[this->CurrentBufferIndex].BufferIndex == this->CurrentBufferIndex );

		// begin the buffer
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0; 
		commandBufferBeginInfo.pInheritanceInfo = nullptr; 
		CheckCall( vkBeginCommandBuffer( this->Buffers[this->CurrentBufferIndex].CommandBufferHandle, &commandBufferBeginInfo ) );

		// return buffer pointer
		return &this->Buffers[this->CurrentBufferIndex];
		}

	status CommandPool::EndCommandBuffer( CommandBuffer *commandBuffer )
		{
		Validate( this->ActiveBuffers.find( commandBuffer->BufferIndex ) != this->ActiveBuffers.end() , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << "is not recording" << ValidateEnd;

		SanityCheck( &this->Buffers[commandBuffer->BufferIndex] == commandBuffer );

		// done recording to the buffer
		CheckCall( vkEndCommandBuffer( commandBuffer->CommandBufferHandle ) );
		
		// remove from active set
		this->ActiveBuffers.erase( commandBuffer->BufferIndex );
		return status_code::ok;
		}

	CommandBuffer::CommandBuffer()
		{
		}

	CommandBuffer::~CommandBuffer()
		{
		}

	void CommandBuffer::BeginRenderPass( VkRenderPass renderPass , VkFramebuffer framebuffer , VkRect2D renderArea , size_t clearValuesCount , const VkClearValue *clearValues )
		{
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea = renderArea;
		renderPassBeginInfo.clearValueCount = (uint)clearValuesCount;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass( this->CommandBufferHandle, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );
		}

	void CommandBuffer::EndRenderPass()
		{
		vkCmdEndRenderPass( this->CommandBufferHandle );
		}

	//void CommandBuffer::BindPipeline( Pipeline* pipeline )
	//	{
	//	vkCmdBindPipeline( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineBindPoint(), pipeline->GetPipeline() );
	//	}

	//void CommandBuffer::BindVertexBuffer( VertexBuffer* buffer )
	//	{
	//	ASSERT_RECORDING();

	//	VkBuffer vertexBuffers[] = { buffer->GetBuffer() };
	//	VkDeviceSize offsets[] = { 0 };
	//	vkCmdBindVertexBuffers( this->Buffers[this->CurrentBufferIndex], 0, 1, vertexBuffers, offsets );
	//	}

	//void CommandBuffer::BindIndexBuffer( IndexBuffer* buffer )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdBindIndexBuffer( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), 0, buffer->GetIndexType() );
	//	}

	//void CommandBuffer::BindDescriptorSet( Pipeline* pipeline, VkDescriptorSet set )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdBindDescriptorSets( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineBindPoint(), pipeline->GetPipelineLayout(), 0, 1, &set, 0, nullptr );
	//	}

	//void CommandBuffer::PushConstants( Pipeline* pipeline , VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdPushConstants( this->Buffers[this->CurrentBufferIndex], pipeline->GetPipelineLayout(), stageFlags, offset, size, pValues );
	//	}

	//void CommandBuffer::SetViewport( VkViewport viewport )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdSetViewport( this->Buffers[this->CurrentBufferIndex], 0, 1, &viewport );
	//	}

	//void CommandBuffer::SetViewport( float x, float y, float width, float height, float minDepth, float maxDepth )
	//	{
	//	VkViewport viewport;
	//	viewport.x = x;
	//	viewport.y = y;
	//	viewport.width = width;
	//	viewport.height = height;
	//	viewport.minDepth = minDepth;
	//	viewport.maxDepth = maxDepth;
	//	this->SetViewport( viewport );
	//	}

	//void CommandBuffer::SetScissorRectangle( VkRect2D scissorRectangle )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdSetScissor( this->Buffers[this->CurrentBufferIndex], 0, 1, &scissorRectangle );
	//	}

	//void CommandBuffer::SetScissorRectangle( int32_t x, int32_t y, uint32_t width, uint32_t height )
	//	{
	//	VkRect2D scissorRectangle;
	//	scissorRectangle.offset.x = x;
	//	scissorRectangle.offset.y = y;
	//	scissorRectangle.extent.width = width;
	//	scissorRectangle.extent.height = height;
	//	this->SetScissorRectangle( scissorRectangle );
	//	}

	//void CommandBuffer::UpdateBuffer( Buffer* buffer, VkDeviceSize dstOffset, uint32_t dataSize, const void* pData )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdUpdateBuffer( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), dstOffset, dataSize, pData );
	//	}

	//void CommandBuffer::Draw( uint vertexCount )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdDraw( this->Buffers[this->CurrentBufferIndex], vertexCount, 1, 0, 0 );
	//	}

	//void CommandBuffer::DrawIndexed( uint indexCount, uint instanceCount, uint firstIndex, int vertexOffset, uint firstInstance )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdDrawIndexed( this->Buffers[this->CurrentBufferIndex], indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	//	}

	//void CommandBuffer::DrawIndexedIndirect( const Buffer *buffer, VkDeviceSize offset, uint drawCount, uint stride )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdDrawIndexedIndirect( this->Buffers[this->CurrentBufferIndex], buffer->GetBuffer(), offset, drawCount, stride );
	//	}

	//void CommandBuffer::QueueUpBufferMemoryBarrier( VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDeviceSize offset, VkDeviceSize size )
	//	{
	//	VkBufferMemoryBarrier bufferMemoryBarrier = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	//	bufferMemoryBarrier.srcAccessMask = srcAccessMask;
	//	bufferMemoryBarrier.dstAccessMask = dstAccessMask;
	//	bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	bufferMemoryBarrier.buffer = buffer;
	//	bufferMemoryBarrier.offset = offset;
	//	bufferMemoryBarrier.size = size;
	//	this->BufferMemoryBarriers.push_back( bufferMemoryBarrier );
	//	}

	//void CommandBuffer::QueueUpBufferMemoryBarrier( const Buffer* buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkDeviceSize offset, VkDeviceSize size )
	//	{
	//	this->QueueUpBufferMemoryBarrier(
	//		buffer->GetBuffer(),
	//		srcAccessMask,
	//		dstAccessMask,
	//		offset,
	//		(size == VkDeviceSize( ~0 )) ? buffer->GetBufferSize() : size
	//	);
	//	}

	//void CommandBuffer::QueueUpImageMemoryBarrier( VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageAspectFlags aspectMask )
	//	{
	//	VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	//	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	//	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	//	imageMemoryBarrier.oldLayout = oldLayout;
	//	imageMemoryBarrier.newLayout = newLayout;
	//	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	imageMemoryBarrier.image = image;
	//	imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
	//	imageMemoryBarrier.subresourceRange.baseMipLevel = 0; 
	//	imageMemoryBarrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	//	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	//	imageMemoryBarrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	//	this->ImageMemoryBarriers.push_back( imageMemoryBarrier );
	//	}

	//void CommandBuffer::QueueUpImageMemoryBarrier( const Image* image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageAspectFlags aspectMask )
	//	{
	//	this->QueueUpImageMemoryBarrier(
	//		image->GetImage(),
	//		oldLayout,
	//		newLayout,
	//		srcAccessMask,
	//		dstAccessMask,
	//		aspectMask
	//		);
	//	}

	//void CommandBuffer::PipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
	//	{
	//	ASSERT_RECORDING();

	//	VkBufferMemoryBarrier* pbuf = this->BufferMemoryBarriers.data();
	//	VkImageMemoryBarrier* pimg = this->ImageMemoryBarriers.data();

	//	vkCmdPipelineBarrier( 
	//		this->Buffers[this->CurrentBufferIndex], 
	//		srcStageMask, 
	//		dstStageMask, 
	//		VK_DEPENDENCY_BY_REGION_BIT,
	//		0, nullptr, 
	//		(uint)this->BufferMemoryBarriers.size(), pbuf,
	//		(uint)this->ImageMemoryBarriers.size(), pimg
	//		);

	//	BufferMemoryBarriers.clear();
	//	ImageMemoryBarriers.clear();
	//	}

	//void CommandBuffer::DispatchCompute( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ )
	//	{
	//	ASSERT_RECORDING();

	//	vkCmdDispatch( this->Buffers[this->CurrentBufferIndex], groupCountX, groupCountY, groupCountZ );
	//	}

	//void CommandBuffer::TraceRays( RayTracingShaderBindingTable* sbt , uint width , uint height )
	//	{
	//	ASSERT_RECORDING();

	//	RayTracingExtension::vkCmdTraceRaysKHR(
	//		this->Buffers[this->CurrentBufferIndex],
	//		&sbt->GetRaygenDeviceAddress(),
	//		&sbt->GetMissDeviceAddress(),
	//		&sbt->GetClosestHitDeviceAddress(),
	//		&sbt->GetCallableDeviceAddress(),
	//		width,
	//		height,
	//		1 );
	//	}

	}