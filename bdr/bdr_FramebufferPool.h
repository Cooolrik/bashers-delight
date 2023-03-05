// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr.h"
#include "bdr_Device.h"

namespace bdr 
	{
	class FramebufferPool : public MainSubmodule
		{
		public:
			~FramebufferPool();

		//private:
            //friend status Device::CreateFramebufferPool( const SwapchainTemplate& parameters );
			FramebufferPool( const Instance* _module );

		public:
			
			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

		};

	// Swapchain template creation parameters
	// Optional values will be handeled automatically if not set.
	class FramebufferPoolTemplate
		{
		public:
			//// The wanted surface format.
			//optional_value<VkSurfaceFormatKHR> SurfaceFormat;
			//
			//// The wanted surface depth format.
			//optional_value<VkFormat> SurfaceDepthFormat;
			//
			//// The wanted presentation format. 
			//optional_value<VkPresentModeKHR> PresentMode;
			//
			//// The render extents to use on the surface. 
			//optional_value<VkExtent2D> RenderExtent;
			//
			//// The number of images in the swap chain. 
			//optional_value<uint> SwapchainLength;
		};

	};
