// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE

#pragma once

#include "bdr_Device.h"
#include "bdr_MultithreadPool.h"

namespace bdr
{

class Framebuffer
	{
	public:
		Framebuffer();
		~Framebuffer();

		// the image objects of the framebuffer
		vector<unique_ptr<Image>> images;

		// the vulkan framebuffer handle
		VkFramebuffer framebufferHandle;

		// a semaphore object which can be used to signal render finish
		VkSemaphore semaphoreHandle;
	};

// FramebufferPool keeps a number of frame buffers. Each framebuffer has a number of images attached, in a common setup.
class FramebufferPool : public MultithreadPoolTemplate<Framebuffer, Device>
	{
	public:
		~FramebufferPool();

	private:
		friend status_return<unique_ptr<FramebufferPool>> Device::CreateObject<FramebufferPool, FramebufferPoolTemplate>( const FramebufferPoolTemplate &parameters );
		FramebufferPool( Device *_module );
		status Setup( const FramebufferPoolTemplate &parameters );

		// surface formats of the images, and count of vector defines the number of images in each framebuffer
		vector<VkFormat> ImageFormats;

		// the size of the framebuffers in the pool
		VkExtent2D ImagesExtent;

	public:
		// explicitly cleanups the object, and also clears all objects owned by it
		status Cleanup();

		// get the number of images per framebuffer
		size_t GetImagesPerFramebuffer() const
			{
			return this->ImageFormats.size();
			}

		// get the format of a specific image in a framebuffer
		VkFormat GetImageFormat( size_t index ) const
			{
			return this->ImageFormats[index];
			}

		// get the allocated extents of a frambuffer in the pool
		VkExtent2D GetImagesExtent() const
			{
			return this->ImagesExtent;
			}

		// returns true if a framebuffer is available to be acquired from the pool
		bool FramebufferIsAvailable()
			{
			return this->ItemIsAvailable();
			}

		// acquire a framebuffer from the pool
		status_return<const Framebuffer *> AcquireFramebuffer();

		// return a framebuffer to the pool
		status ReturnFramebuffer( const Framebuffer *framebuffer );

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

		// the command pool to use for the setup process
		CommandPool *commandPool = nullptr;

		/////////////////////////////////

		// make a framebuffer pool that has the same setup as a renderpass' attachments
		static status_return<FramebufferPoolTemplate> FromRenderPass( size_t framebuffersCount, uint32_t width, uint32_t height, const RenderPass *renderPass, CommandPool *commandPool );
	};

}
// namespace bdr
