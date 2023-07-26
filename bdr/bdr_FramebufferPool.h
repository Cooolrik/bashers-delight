// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"

namespace bdr 
	{
	// FramebufferPool keeps a number of frame buffers. Each framebuffer has a number of images attached, in a common setup.
	class FramebufferPool : public DeviceSubmodule
		{
		public:
			~FramebufferPool();

		private:
			friend status_return<unique_ptr<FramebufferPool>> Device::CreateObject<FramebufferPool,FramebufferPoolTemplate>( const FramebufferPoolTemplate &parameters );
			FramebufferPool( Device* _module );
			status Setup( const FramebufferPoolTemplate& parameters );

			class Framebuffer
				{
				public:
					vector<unique_ptr<Image>> Images;
					VkFramebuffer FramebufferHandle;
				};
			vector<Framebuffer> Framebuffers;

			// surface formats of the images, and count of vector defines the number of images in each framebuffer
			vector<VkFormat> ImageFormats;

			// the size of the framebuffers in the pool
			VkExtent2D ImagesExtent;

		public:
			// explicitly cleanups the object, and also clears all objects owned by it
			status Cleanup();

			// get the number of framebuffers in the pool
			size_t GetFramebuffersCount() const { return this->Framebuffers.size(); }

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
			// Note: all framebuffers in the pool share the same layout
			size_t framebuffersCount = 1;

			// the extent in pixels of each of the images and framebuffers in the pool
			VkExtent2D extent = {};

			// image setup of each framebuffer
			class ImageSetup
				{
				public:
					VkFormat format;
					VkSampleCountFlagBits samples;
				};
			vector<ImageSetup> imageSetups;

			// the render pass to use for the framebuffer
			const RenderPass *renderPass = nullptr;

			// the command pool to use for the setup
			CommandPool *commandPool = nullptr;

			/////////////////////////////////

			// make a framebuffer pool that has the same setup as a renderpass' attachments
			static status_return<FramebufferPoolTemplate> FromRenderPass( size_t framebuffersCount, uint32_t width, uint32_t height, const RenderPass *renderPass, CommandPool *commandPool );
		};

	};
