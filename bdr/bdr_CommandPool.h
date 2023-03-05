// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr.h"

namespace bdr
	{
	class CommandPool : public MainSubmodule
		{
		public:
			~CommandPool();

		private:
			friend status_return<CommandPool*> MainSubmoduleMap<CommandPool>::CreateSubmodule<CommandPoolTemplate>( const CommandPoolTemplate& parameters );
			CommandPool( const Instance* _module );
			status Setup( const CommandPoolTemplate& parameters );

			VkCommandPool CommandPoolHandle = VK_NULL_HANDLE; 

			CommandBuffer *Buffers = nullptr;
			const size_t BuffersCount = 0;

			std::unordered_set<size_t> ActiveBuffers;
			size_t CurrentBufferIndex = 0;

		public:
			status ResetCommandPool();

			status_return<CommandBuffer*> BeginCommandBuffer();
			status EndCommandBuffer( CommandBuffer *commandBuffer );

			// explicitly cleans up the object, and also destroys all data and objects owned by it
			status Cleanup();

			// returns true if at least one buffer is currently recording
			bool IsRecording() const { return !ActiveBuffers.empty(); }

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

			CommandBuffer();
			~CommandBuffer();

			//std::vector<VkBufferMemoryBarrier> BufferMemoryBarriers;
			//std::vector<VkImageMemoryBarrier> ImageMemoryBarriers;

		public:
			void BeginRenderPass( VkRenderPass renderPass , VkFramebuffer framebuffer , VkRect2D renderArea , size_t clearValuesCount , const VkClearValue *clearValues );
			void EndRenderPass();
			
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

			//void QueueUpBufferMemoryBarrier( 
			//    VkBuffer      buffer,
			//    VkAccessFlags srcAccessMask,
			//    VkAccessFlags dstAccessMask,
			//    VkDeviceSize  offset,
			//    VkDeviceSize  size
			//    );

			//void QueueUpBufferMemoryBarrier(
			//    const Buffer* buffer,
			//    VkAccessFlags srcAccessMask,
			//    VkAccessFlags dstAccessMask,
			//    VkDeviceSize  offset = 0,
			//    VkDeviceSize  size = VkDeviceSize( ~0 )
			//  );

			//void QueueUpImageMemoryBarrier(
			//    VkImage image,
			//    VkImageLayout oldLayout,
			//    VkImageLayout newLayout,
			//    VkAccessFlags srcAccessMask,
			//    VkAccessFlags dstAccessMask,
			//    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			//    );

			//void QueueUpImageMemoryBarrier(
			//    const Image* image,
			//    VkImageLayout oldLayout,
			//    VkImageLayout newLayout,
			//    VkAccessFlags srcAccessMask,
			//    VkAccessFlags dstAccessMask,
			//    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			//    );

			//// creates a command barries with queued up barriers
			//void PipelineBarrier(
			//    VkPipelineStageFlags srcStageMask,
			//    VkPipelineStageFlags dstStageMask
			//    );

			//void DispatchCompute( 
			//    uint32_t groupCountX, 
			//    uint32_t groupCountY = 1, 
			//    uint32_t groupCountZ = 1 
			//    );

			//void TraceRays( RayTracingShaderBindingTable* sbt , uint width, uint height );
		};

	};