// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr 
	{
	class FramebufferPool : public DeviceSubmodule
		{
		public:
			~FramebufferPool();

		private:
			friend status_return<unique_ptr<FramebufferPool>> Device::CreateObject<FramebufferPool,FramebufferPoolTemplate>( const FramebufferPoolTemplate &parameters );
			FramebufferPool( const Device* _module );
			status Setup( const FramebufferPoolTemplate& parameters );

			// the handles of each framebuffer in the pool
			vector<VkFramebuffer> FramebuffersHandles;

			// surface formats of the images, and count of vector defines the number of images in each framebuffer
			vector<VkFormat> ImageFormats;

			// the size of the framebuffers in the pool
			VkExtent2D ImagesExtent;

		public:
			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

			// get the number of framebuffers in the pool
			size_t GetFramebuffersCount() const { return this->FramebuffersHandles.size(); }

			// get the number of images per framebuffer
			size_t GetImagesPerFramebuffer() const { return this->ImageFormats.size(); }

			// get the format of a specific image in a framebuffer
			VkFormat GetImageFormat( size_t index ) const { return this->ImageFormats[index]; }

			// get the allocated extents of a frambuffer in the pool
			VkExtent2D GetImagesExtent() const { return this->ImagesExtent; }
		};

	// FramebufferPool creation template
	class FramebufferPoolTemplate
		{
		public:
			// the number of Framebuffers to be created in the pool
			// all framebuffer will have the same layout, as defined by the parameters in the template
			size_t FramebuffersCount = 1;

			// surface formats of the images, one per each image in one of the framebuffers in the pool
			// the index of the format in the vector will be the index of the image in each frambuffer
			vector<VkFormat> ImageFormats;

			// the extents in pixels of the framebuffers in the pool
			VkExtent2D ImagesExtent = {};
		};

	};
