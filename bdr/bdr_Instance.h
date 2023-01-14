#pragma once

#include "bdr.h"

namespace bdr 
	{
	class Instance
		{
		public:
			class Template;

		private:
			Instance() = default;
			Instance( const Instance& other );

			VkInstance InstanceHandle = {};
			VkDebugUtilsMessengerEXT DebugUtilsMessenger = {};

			bool EnableValidation = false;
			vector<const char*> ExtensionList;

			// extensions
			vector<Extension*> EnabledExtensions;
			unique_ptr<DescriptorIndexingExtension> DescriptorIndexingExtension;
			unique_ptr<BufferDeviceAddressExtension> BufferDeviceAddressExtension;
			unique_ptr<RayTracingExtension> RayTracingExtension;

			//
			//struct TargetImage
			//	{
			//	std::unique_ptr<Image> Color; // color channel RGBA
			//	std::unique_ptr<Image> Depth; // depth channel 
			//	};

			//
			//std::vector<const char*> DeviceExtensionList;

			//VkSurfaceKHR Surface = nullptr;
			

			//VkPhysicalDevice PhysicalDevice = nullptr;
			//uint PhysicalDeviceQueueGraphicsFamily = (uint)-1;
			//uint PhysicalDeviceQueuePresentFamily = (uint)-1;

			//VkDevice Device = nullptr;
			//VkQueue GraphicsQueue = nullptr;
			//VkQueue PresentQueue = nullptr;
			//VkSurfaceFormatKHR SurfaceFormat{};
			//VkPresentModeKHR PresentMode{};
			//VkExtent2D RenderExtent{};
			//VkFormat SurfaceDepthFormat{};
			//bool DepthFormatHasStencil = false;
			//VkRenderPass RenderPass = nullptr;

			//VmaAllocator MemoryAllocator = nullptr;

			//VkSwapchainKHR SwapChain = nullptr;
			//std::vector<VkImage> SwapChainImages;

			//std::vector<TargetImage> TargetImages;
			//std::vector<VkFramebuffer> Framebuffers;

			//VkSurfaceCapabilitiesKHR SurfaceCapabilities{};
			//std::vector<VkSurfaceFormatKHR> AvailableSurfaceFormats;
			//std::vector<VkPresentModeKHR> AvailablePresentModes;
			//
			//VkPhysicalDeviceFeatures2 PhysicalDeviceFeatures{};
			//VkPhysicalDeviceProperties2 PhysicalDeviceProperties{};

			//VkCommandPool InternalCommandPool = nullptr; 

			//std::vector<VkSemaphore> ImageAvailableSemaphores;
			//std::vector<VkSemaphore> RenderFinishedSemaphores;
			//std::vector<VkFence> InFlightFences;
			//std::vector<VkFence> ImagesInFlight;
			//uint CurrentFrame = 0;
			//uint CurrentImage = 0;



			//bool LookupPhysicalDeviceQueueFamilies();
			//bool ValidatePhysicalDeviceRequiredExtensionsSupported();

			//void DeleteSwapChain();

			//// generic functions to create vulkan device buffer 
			//VkBuffer CreateVulkanBuffer( VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryPropertyFlags, VkDeviceSize deviceSize, VmaAllocation& deviceMemory ) const;

			///// create base vulkan buffer
			//template<class B,class BT> B* NewBuffer( const BT& bt ) const;

			//VkCommandBuffer BeginInternalCommandBuffer() const;
			//void EndAndSubmitInternalCommandBuffer( VkCommandBuffer buffer ) const;

		public:
			static StatusPair<unique_ptr<Instance>> Create( const Template& parameters );
		
			// template method which makes a unique_ptr to a submodule 
			template<typename T, typename... Args>
			std::unique_ptr<T> Make(Args&&... args)
			{
				return std::unique_ptr<T>(new T(this, std::forward<Args>(args)...));
			}

			//// setup the device with the created surface
			//void CreateDevice( VkSurfaceKHR surface );

			///// create the rendering swap chain
			//struct CreateSwapChainParameters
			//	{
			//	// the wanted surface format. get the default selected with GetSurfaceFormat()
			//	VkSurfaceFormatKHR SurfaceFormat;

			//	// the wanted presentation format. get the default selected with GetPresentMode()
			//	VkPresentModeKHR PresentMode;

			//	// the render extents on the surface, as reported by the framework.
			//	VkExtent2D RenderExtent;
			//	};
			//void CreateSwapChainAndFramebuffers( const CreateSwapChainParameters& parameters );
			//void UpdateSurfaceCapabilitiesAndFormats();
			//void RecreateSwapChain( const CreateSwapChainParameters& parameters );
			//
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

			~Instance();

			//// public get methods
			VkInstance GetInstance() const { return this->InstanceHandle; }

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

			//// get extensions
			//RayTracingExtension* GetRayTracingExtension() const { return this->RayTracingEXT; }

		};

	// Instance template creation parameters
	class Instance::Template
		{
		public:
			// the application name
			string ApplicationName = "BDR Application";

			// the version of the application (as registered with Vulkan)
			uint32_t ApplicationVersion = VK_MAKE_VERSION( 1, 0, 0 );

			// enable validation (for debugging purposes)
			bool EnableValidation = false;
				
			// true to enable ray tracing extension. 
			bool EnableRayTracingExtension = false;

			// list of needed extensions for eg windowing system
			uint NeededExtensionsCount = 0;
			const char** NeededExtensions = {};

			// debug message callback and type/severity masks
			PFN_vkDebugUtilsMessengerCallbackEXT DebugMessageCallback = {};
			VkDebugUtilsMessageSeverityFlagsEXT DebugMessageSeverityMask = {};
			VkDebugUtilsMessageTypeFlagsEXT DebugMessageTypeMask = {};
		};

	};
