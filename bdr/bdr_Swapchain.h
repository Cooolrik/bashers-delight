// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr 
	{
	class Swapchain : public DeviceSubmodule
		{
		public:
			~Swapchain();

		private:
			friend status_return<unique_ptr<Swapchain>> Device::CreateObject<Swapchain,SwapchainTemplate>( const SwapchainTemplate &parameters );
			Swapchain( Device* _module );
			status Setup( const SwapchainTemplate& parameters );

			VkSwapchainKHR SwapchainHandle = nullptr;
			vector<VkImage> SwapchainImageHandles;

			//unique_ptr<FramebufferPool> Framebuffers;

			//VkRenderPass RenderPassHandle = nullptr;
			//vector<VkFramebuffer> FramebuffersHandles;
			//
			//struct TargetImage
			//	{
			//	std::unique_ptr<Image> Color; // color channel RGBA
			//	std::unique_ptr<Image> Depth; // depth channel 
			//	};
			//
			//vector<TargetImage> TargetImages;
			//
			
			//std::vector<VkSemaphore> ImageAvailableSemaphores;
			//std::vector<VkSemaphore> RenderFinishedSemaphores;
			//std::vector<VkFence> InFlightFences;
			//std::vector<VkFence> ImagesInFlight;
			//uint CurrentFrame = 0;
			//uint CurrentImage = 0;

			unique_ptr<SwapchainTemplate> Template;

			void DeleteSwapchain();

		public:
			
			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

			//void UpdateSurfaceCapabilitiesAndFormats();
			//void RecreateSwapChain( const CreateSwapchainParameters& parameters );
			
			///// get a new frame to draw to. index will receive the index of the image in the swap chain.
			///// if the return value is VK_ERROR_OUT_OF_DATE_KHR, then the swap chain needs to be recreated before rendering
			//VkResult AcquireNextFrame( uint& index );

			///// submit command buffers to the current frame
			//VkResult SubmitRenderCommandBuffersAndPresent( const std::vector<VkCommandBuffer>& buffers );

			///// wait for device to idle, for synching, eg shutting down
			//void WaitForDeviceIdle();

			///// create a Graphics Pipeline object based on template
			//Pipeline* CreateGraphicsPipeline( const GraphicsPipelineTemplate& gpt ) const;

			///// create a Compute Pipeline object based on template
			//Pipeline* CreateComputePipeline( const ComputePipelineTemplate& cpt ) const;

			///// create a CommandPool object. set number of buffers to allocate
			//CommandPool* CreateCommandPool( uint bufferCount ) const;

			///// create buffers based on templates
			//Buffer* CreateBuffer( const BufferTemplate &bt ) const;
			//VertexBuffer* CreateVertexBuffer( const VertexBufferTemplate& bt ) const;
			//IndexBuffer* CreateIndexBuffer( const IndexBufferTemplate& bt ) const;
			//
			///// (temporary) create descriptor layout
			//DescriptorSetLayout* CreateDescriptorSetLayout( const DescriptorSetLayoutTemplate& dst ) const;

			///// Create a descriptor pool 
			//DescriptorPool* CreateDescriptorPool( const DescriptorPoolTemplate& dpt ) const;

			//// create image
			//Image* CreateImage( const ImageTemplate& it );

			//// create sampler
			//Sampler* CreateSampler( const SamplerTemplate& st );

			//// Create and submit synchronously a command buffer with the internal command pool.
			//// The call is blocking until the command has been run, so only use during inits.
			//void RunBlockingCommandBuffer( std::function<void (VkCommandBuffer cmd)> fp ) const;

			//BDRGetMacro( VkPhysicalDevice, PhysicalDevice );
			//BDRGetMacro( VkDevice, Device );
			//BDRGetMacro( VkQueue, GraphicsQueue );
			//BDRGetMacro( VkQueue, PresentQueue );
			//BDRGetMacro( std::vector<VkSurfaceFormatKHR>, AvailableSurfaceFormats );
			//BDRGetMacro( std::vector<VkPresentModeKHR>, AvailablePresentModes );
			//BDRGetMacro( VkPhysicalDeviceFeatures2, PhysicalDeviceFeatures );
			//BDRGetMacro( VkSurfaceCapabilitiesKHR, SurfaceCapabilities );
			//BDRGetMacro( VkSurfaceFormatKHR, SurfaceFormat );
			//BDRGetMacro( VkPresentModeKHR, PresentMode );
			//BDRGetMacro( VkExtent2D, RenderExtent );
			//BDRGetMacro( std::vector<VkFramebuffer>, Framebuffers );
			//BDRGetMacro( std::vector<VkImage>, SwapChainImages );
			//BDRGetMacro( VkRenderPass, RenderPass );
			//BDRGetMacro( VmaAllocator, MemoryAllocator );

			//const Image* GetColorTargetImage( uint index ) const { return this->TargetImages[index].Color.get(); }
			//const Image* GetDepthTargetImage( uint index ) const { return this->TargetImages[index].Depth.get(); }


		};

	// Swapchain template creation parameters
	// Optional values will be handled automatically if not set.
	class SwapchainTemplate
		{
		public:
			// Number of images in the swap chain. 
			uint swapchainLength;

			// Selected surface format and color space
			VkSurfaceFormatKHR surfaceFormat;

			// Surface presentation mode. 
			VkPresentModeKHR presentMode;

			// Render extents to use 
			VkExtent2D renderExtent;

			////////////////////////

			// Creates a swapchain for the current surface of the provided device, and tries to select the best fit from available modes and formats
			static status_return<SwapchainTemplate> DeviceCompatible( Device *device, VkExtent2D renderExtent = {0,0}, uint swapchainLength = 0 );

		};

	};
