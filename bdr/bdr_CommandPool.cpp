// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_CommandPool.h"
#include "bdr_Buffer.h"
#include "bdr_Image.h"

//#include "bdr_GraphicsPipeline.h"
//#include "bdr_ComputePipeline.h"
//#include "bdr_VertexBuffer.h"
//#include "bdr_IndexBuffer.h"


//#include "Extensions/bdr_RayTracingExtension.h"
//#include "Extensions/bdr_RayTracingPipeline.h"
//#include "Extensions/bdr_RayTracingShaderBindingTable.h"

#include <stdexcept>
#include <algorithm>

namespace bdr
{
	CommandPool::CommandPool( Device* _module ) : DeviceSubmodule(_module) 
		{
		LogThis;
		}
	
	status CommandPool::Setup( const CommandPoolTemplate& parameters )
		{
		Validate( parameters.BufferCount > 0 , status_code::invalid_param ) << "The parameters.BufferCount cannot be 0" << ValidateEnd;
		
		auto device = this->GetModule();

		// create the command pool vulkan object
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = device->GetPhysicalDeviceQueueGraphicsFamily();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CheckCall( vkCreateCommandPool( device->GetDeviceHandle(), &poolInfo, nullptr, &this->CommandPoolHandle ) );
		
		// allocate buffer objects
		std::vector<VkCommandBuffer> bufferObjects(parameters.BufferCount);
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = this->CommandPoolHandle;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)parameters.BufferCount;
		CheckCall( vkAllocateCommandBuffers( device->GetDeviceHandle(), &commandBufferAllocateInfo, bufferObjects.data() ) );

		// fill in the buffer objects
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		this->BuffersCount = parameters.BufferCount;
		this->Buffers = new CommandBuffer[this->BuffersCount];
		for( size_t inx=0; inx<this->BuffersCount; ++inx )
			{
			this->Buffers[inx].CommandPool_ = this;
			this->Buffers[inx].BufferIndex = inx;
			this->Buffers[inx].CommandBufferHandle = bufferObjects[inx];

			CheckCall( vkCreateFence( this->Module->GetDeviceHandle(), &fenceCreateInfo, nullptr, &this->Buffers[inx].SubmitFenceHandle ));
			}

		return status::ok;
		}

	CommandPool::~CommandPool()
		{
		LogThis;

		this->Cleanup();
		}

	status CommandPool::Cleanup()
		{
		SafeVkDestroy( this->CommandPoolHandle , vkDestroyCommandPool( this->GetModule()->GetDeviceHandle(), this->CommandPoolHandle, nullptr ) )
		
		// clean up buffers
		for( size_t inx=0; inx<this->BuffersCount; ++inx )
			{
			vkDestroyFence( this->Module->GetDeviceHandle(), this->Buffers[inx].SubmitFenceHandle, nullptr );
			}
		SafeDestroy( this->Buffers );
		this->BuffersCount = 0;

		return status::ok;
		}
	
	status CommandPool::UpdateActiveBuffers( bool remove_submitted, bool remove_recorded )
		{
		// lazily check for submitted buffers which are completed, and remove them from the set
		auto it = this->ActiveBuffers.begin();
		while( it != this->ActiveBuffers.end() )
			{
			SanityCheck( (*it)->BufferState != CommandBuffer::State::Available );

			bool remove = false;

			// remove submitted, finished buffers
			if( remove_submitted 
			 && (*it)->BufferState == CommandBuffer::State::Submitted 
			 && vkGetFenceStatus( this->GetModule()->GetDeviceHandle() , (*it)->SubmitFenceHandle )	== VK_SUCCESS )
				{
				remove = true;
				}

			// remove recorded, non-submitted buffers
			if( remove_recorded 
			 && (*it)->BufferState == CommandBuffer::State::Recorded )
				{
				remove = true;
				}
			
			// remove and/or move on
			if( remove )
				{
				// reset state to available, and remove from set of active
				CheckCall( (*it)->Reset() );
				it = this->ActiveBuffers.erase( it );
				}
			else
				{
				// move on
				++it;
				}
			}

		return status_code::ok;
		}

	status CommandPool::ResetCommandPool()
		{
		// discard all buffers. the call will fail if there are currently buffers currently recording, or submitted but not completed
		CheckCall( this->UpdateActiveBuffers( true , true ) );
		Validate( this->ActiveBuffers.empty() , status_code::invalid ) << "Cannot reset command pool, there is at least one buffer which is still active (recording or submitted but not marked as completed)" << ValidateEnd;

#ifndef NDEBUG
		// make sure all buffers are marked as available
		for( size_t inx=0; inx<this->BuffersCount; ++inx )
			{
			SanityCheck( this->Buffers[inx].BufferState == CommandBuffer::State::Available );
			}
#endif//NDEBUG

		CheckCall( vkResetCommandPool( this->GetModule()->GetDeviceHandle(), this->CommandPoolHandle, 0 ) );
		return status_code::ok;
		}

	status_return<CommandBuffer*> CommandPool::BeginCommandBuffer()
		{
		// if we are out of buffers, update the ones which have been submitted, and check again
		if( this->ActiveBuffers.size() >= this->BuffersCount )
			{
			CheckCall( this->UpdateActiveBuffers( true , false ) );
			Validate( this->ActiveBuffers.size() < this->BuffersCount , status_code::invalid ) << "Cannot allocate a buffer for recording, they are all used up." << ValidateEnd;
			}

		// look for an available buffer
		// round robin look using CurrentBufferIndex
		for(;;)
			{
			this->CurrentBufferIndex = (this->CurrentBufferIndex + 1) % this->BuffersCount;
			if( this->Buffers[this->CurrentBufferIndex].BufferState == CommandBuffer::State::Available )
				break;
			}
		SanityCheck( this->Buffers[this->CurrentBufferIndex].BufferIndex == this->CurrentBufferIndex );

		// mark buffer as recording and place in active set
		this->Buffers[this->CurrentBufferIndex].BufferState = CommandBuffer::State::Recording;
		this->ActiveBuffers.insert( &this->Buffers[this->CurrentBufferIndex] );
		SanityCheck( this->ActiveBuffers.size() <= this->BuffersCount );

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
		Validate( commandBuffer->BufferState == CommandBuffer::State::Recording , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " is not recording" << ValidateEnd;
		Validate( this->ActiveBuffers.find(commandBuffer) != this->ActiveBuffers.end() , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " does not belong to this pools active set" << ValidateEnd;

		SanityCheck( &this->Buffers[commandBuffer->BufferIndex] == commandBuffer );

		// done recording to the buffer, mark as recorded
		CheckCall( vkEndCommandBuffer( commandBuffer->CommandBufferHandle ) );
		commandBuffer->BufferState = CommandBuffer::State::Recorded;

		return status_code::ok;
		}
	
	status CommandPool::SubmitCommandBuffer( CommandBuffer *commandBuffer , bool wait )
		{
		Validate( commandBuffer->BufferState == CommandBuffer::State::Recorded , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " is not recorded and cannot be submitted" << ValidateEnd;
		Validate( this->ActiveBuffers.find(commandBuffer) != this->ActiveBuffers.end() , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " does not belong to this pools active set" << ValidateEnd;

		SanityCheck( &this->Buffers[commandBuffer->BufferIndex] == commandBuffer );

		// reset the submit fence
		vkResetFences( this->GetModule()->GetDeviceHandle() , 1 , &commandBuffer->SubmitFenceHandle );

		// submit to graphics queue
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer->CommandBufferHandle;
		CheckCall( this->GetModule()->GraphicsQueueSubmit( 1, &submitInfo, commandBuffer->SubmitFenceHandle ) );

		// mark as submitted
		commandBuffer->BufferState = CommandBuffer::State::Submitted;

		// if set, wait for it to complete (note: this waits indefinitely)
		if( wait )
			{
			CheckCall( WaitForCommandBuffer( commandBuffer , UINT64_MAX ) ); 
			}

		return status_code::ok;
		}

	status CommandPool::RecycleCommandBuffer( CommandBuffer *commandBuffer )
		{
		Validate( commandBuffer->BufferState == CommandBuffer::State::Recorded , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " is not recorded, and cannot be recycled" << ValidateEnd;
		Validate( this->ActiveBuffers.find(commandBuffer) != this->ActiveBuffers.end() , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " does not belong to this pools active set" << ValidateEnd;
	
		SanityCheck( &this->Buffers[commandBuffer->BufferIndex] == commandBuffer );

		// mark as available and remove from active set
		CheckCall( commandBuffer->Reset() );
		this->ActiveBuffers.erase( commandBuffer );

		return status_code::ok;
		}

	status CommandPool::WaitForCommandBuffer( CommandBuffer *commandBuffer , uint64_t timeout )
		{
		auto deviceHandle = this->GetModule()->GetDeviceHandle();

		Validate( commandBuffer->BufferState == CommandBuffer::State::Submitted , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " is not submitted, and cannot be wait for" << ValidateEnd;
		Validate( this->ActiveBuffers.find(commandBuffer) != this->ActiveBuffers.end() , status_code::invalid_param ) << "Invalid parameter: The command buffer " << commandBuffer << " does not belong to this pools active set" << ValidateEnd;
		
		SanityCheck( &this->Buffers[commandBuffer->BufferIndex] == commandBuffer );

		// wait for it to complete
		VkResult result = vkWaitForFences( deviceHandle , 1 , &commandBuffer->SubmitFenceHandle , true , timeout );
		if( result != VK_SUCCESS )
			{
			// if we have a timeout, just return the not_ready status, this is no error
			if( result == VK_TIMEOUT )
				{
				return status::not_ready;
				}

			LogError << "Call: to vkWaitForFences failed, returned VkResult code: " << result << LogEnd;
			return result;
			}

		// mark as available, and remove from active set
		CheckCall( commandBuffer->Reset() );
		this->ActiveBuffers.erase( commandBuffer );

		return status_code::ok;
		}

	CommandBuffer::CommandBuffer()
		{
		}

	CommandBuffer::~CommandBuffer()
		{
		}

	status CommandBuffer::Reset()
		{
		Validate( this->CommandBufferHandle , status::not_initialized ) << "Trying to reset a buffer which is not allocated" << ValidateEnd;

		if( this->BufferState != State::Available )
			{
			CheckCall( vkResetCommandBuffer( this->CommandBufferHandle , 0 ) );
			this->BufferState = State::Available;
			}

		return status::ok;
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

	void CommandBuffer::CopyBuffer( const Buffer *srcBuffer , const Buffer *dstBuffer , const std::vector<VkBufferCopy> &copyRegions )
		{
		vkCmdCopyBuffer( this->CommandBufferHandle, srcBuffer->GetBufferHandle(), dstBuffer->GetBufferHandle(), (uint)copyRegions.size(), copyRegions.data() );
		}

	void CommandBuffer::CopyBufferToImage( const Buffer *srcBuffer, const Image *dstImage, VkImageLayout dstImageLayout, const std::vector<VkBufferImageCopy> &copyRegions )
		{
		vkCmdCopyBufferToImage( this->CommandBufferHandle, srcBuffer->GetBufferHandle() , dstImage->GetImageHandle(), dstImageLayout , (uint)copyRegions.size(), copyRegions.data() );
		}

	void CommandBuffer::CopyImageToBuffer( const Image *srcImage, VkImageLayout srcImageLayout, const Buffer *dstBuffer, const std::vector<VkBufferImageCopy> &copyRegions )
		{
		vkCmdCopyImageToBuffer( this->CommandBufferHandle, srcImage->GetImageHandle(), srcImageLayout , dstBuffer->GetBufferHandle() , (uint)copyRegions.size(), copyRegions.data() );
		}
		
	void CommandBuffer::AddBufferMemoryBarrier( const VkBufferMemoryBarrier &bufferMemoryBarrier )
		{
		this->BufferMemoryBarriers.push_back( bufferMemoryBarrier );
		}

	void CommandBuffer::AddBufferMemoryBarrier( const VkBufferMemoryBarrier &bufferMemoryBarrier, const Buffer *buffer )
		{
		this->AddBufferMemoryBarrier( bufferMemoryBarrier );
		this->BufferMemoryBarriers.back().buffer = buffer->GetBufferHandle();
		}

	void CommandBuffer::AddImageMemoryBarrier( const VkImageMemoryBarrier &imageMemoryBarrier )
		{
		this->ImageMemoryBarriers.push_back( imageMemoryBarrier );
		}

	void CommandBuffer::AddImageMemoryBarrier( const VkImageMemoryBarrier &imageMemoryBarrier, const Image *image )
		{
		this->AddImageMemoryBarrier( imageMemoryBarrier );
		this->ImageMemoryBarriers.back().image = image->GetImageHandle();
		}

	void CommandBuffer::PipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
		{
		const VkBufferMemoryBarrier *pbuf = this->BufferMemoryBarriers.data();
		const VkImageMemoryBarrier *pimg = this->ImageMemoryBarriers.data();

		vkCmdPipelineBarrier( 
			this->CommandBufferHandle, 
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