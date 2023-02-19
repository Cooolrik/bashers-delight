// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr.h"
#include "bdr_Device.h"

namespace bdr 
	{
	class Swapchain : public MainSubmodule
		{
		public:
			~Swapchain();

		private:
            friend status Device::CreateSwapchain( const SwapchainTemplate& parameters );
			Swapchain( const Instance* _module );

			VkSwapchainKHR SwapchainHandle = VK_NULL_HANDLE;

			VkPresentModeKHR PresentMode{};
			VkExtent2D RenderExtent{};

			VkSurfaceFormatKHR SurfaceFormat{};

			VkFormat SurfaceDepthFormat{};
			bool DepthFormatHasStencil = false;

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
			//vector<VkImage> SwapchainImageHandles;
			
			//std::vector<VkSemaphore> ImageAvailableSemaphores;
			//std::vector<VkSemaphore> RenderFinishedSemaphores;
			//std::vector<VkFence> InFlightFences;
			//std::vector<VkFence> ImagesInFlight;
			//uint CurrentFrame = 0;
			//uint CurrentImage = 0;

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
	// Optional values will be handeled automatically if not set.
	class SwapchainTemplate
		{
		public:
			// The wanted surface format.
			optional_value<VkSurfaceFormatKHR> SurfaceFormat;

			// The wanted surface depth format.
			optional_value<VkFormat> SurfaceDepthFormat;

			// The wanted presentation format. 
			optional_value<VkPresentModeKHR> PresentMode;

			// The render extents to use on the surface. 
			optional_value<VkExtent2D> RenderExtent;

			// The number of images in the swap chain. 
			optional_value<uint> SwapchainLength;
		};

	};
