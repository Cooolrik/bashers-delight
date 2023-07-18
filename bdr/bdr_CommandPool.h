// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr
	{
	class CommandPool : public DeviceSubmodule
		{
		public:
			~CommandPool();

		private:
			friend status_return<unique_ptr<CommandPool>> Device::CreateObject<CommandPool,CommandPoolTemplate>( const CommandPoolTemplate &parameters );
			CommandPool( Device* _module );
			status Setup( const CommandPoolTemplate& parameters );

			VkCommandPool CommandPoolHandle = VK_NULL_HANDLE; 

			CommandBuffer *Buffers = nullptr;
			size_t BuffersCount = 0;

			size_t CurrentBufferIndex = 0;
			size_t ActiveCount = 0;

			std::unordered_set<CommandBuffer*> ActiveBuffers;

			status UpdateActiveBuffers( bool submitted, bool recorded );

		public:
			// resets the status of all command buffers to available 
			// note that the call will fail if there are submitted buffers which are not completed, or if there are command buffers which are currently recording
			status ResetCommandPool();

			// begin recording to a buffer
			status_return<CommandBuffer*> BeginCommandBuffer();

			// end recording to a buffer
			status EndCommandBuffer( CommandBuffer *commandBuffer );

			// submit a recorded buffer, and optionally wait (indefinitely) for it to complete using WaitForCommandBuffer
			status SubmitCommandBuffer( CommandBuffer *commandBuffer , bool wait = false );

			// for command buffers which are not explicitly submitted, use this to mark as available again
			status RecycleCommandBuffer( CommandBuffer *commandBuffer );

			// wait for a submitted command buffer to complete. 
			// if timeout is reached (or timeout == 0 and the buffer is not done), status::not_ready is returned
			status WaitForCommandBuffer( CommandBuffer *commandBuffer , uint64_t timeout = UINT64_MAX );

			// explicitly cleans up the object, and also destroys all data and objects owned by it
			status Cleanup();
			
			VkCommandPool GetCommandPoolHandle() const { return CommandPoolHandle; }
		};

	class CommandPoolTemplate
		{
		public:
			// the number of buffers to allocate in the command pool
			size_t BufferCount = 1;
		};

	// CommandBuffer is the accessor for the active buffer
	// it is not owned by the caller, and should only be kept while recording the commands
	class CommandBuffer
		{
		private:
			friend class CommandPool;
			const CommandPool *CommandPool_ = {};

			VkCommandBuffer CommandBufferHandle = VK_NULL_HANDLE;
			size_t BufferIndex = 0; // the buffer index within the CommandPool

			enum State
				{
				Available,	// available for use
				Recording,	// recording
				Recorded,	// recorded, waiting to be submitted
				Submitted,	// submitted
				};
			State BufferState = State::Available; // the current state of the command buffer

			VkFence SubmitFenceHandle = VK_NULL_HANDLE; // the fence which is signaled upon completion of the buffer processing

			status Reset();

			CommandBuffer();
			~CommandBuffer();

			std::vector<VkBufferMemoryBarrier> BufferMemoryBarriers;
			std::vector<VkImageMemoryBarrier> ImageMemoryBarriers;

		public:
			void BeginRenderPass( VkRenderPass renderPass , VkFramebuffer framebuffer , VkRect2D renderArea , size_t clearValuesCount , const VkClearValue *clearValues );
			void EndRenderPass();
			
			// copy commands
			void CopyBuffer( const Buffer *srcBuffer , const Buffer *dstBuffer , const std::vector<VkBufferCopy> &copyRegions );
			void CopyBufferToImage( const Buffer *srcBuffer , const Image *dstImage , VkImageLayout dstImageLayout , const std::vector<VkBufferImageCopy> &copyRegions );
			void CopyImageToBuffer( const Image *srcImage , VkImageLayout srcImageLayout , const Buffer *dstBuffer , const std::vector<VkBufferImageCopy> &copyRegions );

			// add a memory barrier for a buffer (use PipelineBarrier() to commit the added barriers). If buffer is specified, use it to override the buffer handle
			void AddBufferMemoryBarrier( const VkBufferMemoryBarrier &bufferMemoryBarrier );
			void AddBufferMemoryBarrier( const VkBufferMemoryBarrier &bufferMemoryBarrier, const Buffer *buffer );

			// add a memory barrier for an image (use PipelineBarrier() to commit the added barriers). If image is specified, use it to override the image handle
			void AddImageMemoryBarrier( const VkImageMemoryBarrier &imageMemoryBarrier );
			void AddImageMemoryBarrier( const VkImageMemoryBarrier &imageMemoryBarrier, const Image *image );

			// creates a command barries with all the added buffer and/or image memory barriers
			void PipelineBarrier( VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask );
			
			//void BindPipeline( Pipeline* pipeline );
			//
			//void BindVertexBuffer( VertexBuffer* buffer );
			//void BindIndexBuffer( IndexBuffer* buffer );
			//
			//void BindDescriptorSet( Pipeline* pipeline, VkDescriptorSet set );
 
			//void PushConstants( Pipeline* pipeline, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues );
 
			//void SetViewport( VkViewport viewport );
			//void SetViewport( float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f );

			//void SetScissorRectangle( VkRect2D scissorRectangle );
			//void SetScissorRectangle( int32_t x, int32_t y, uint32_t width, uint32_t height );

			//void UpdateBuffer( Buffer* buffer, VkDeviceSize dstOffset, uint32_t dataSize, const void* pData );

			//void Draw( uint vertexCount );
			//
			//void DrawIndexed(
			//    uint indexCount,
			//    uint instanceCount,
			//    uint firstIndex,
			//    int vertexOffset,
			//    uint firstInstance
			//    );

			//void DrawIndexedIndirect(
			//    const Buffer *buffer,
			//    VkDeviceSize offset,
			//    uint drawCount,
			//    uint stride
			//    );

			//void DispatchCompute( 
			//    uint32_t groupCountX, 
			//    uint32_t groupCountY = 1, 
			//    uint32_t groupCountZ = 1 
			//    );

			//void TraceRays( RayTracingShaderBindingTable* sbt , uint width, uint height );
		};

	};