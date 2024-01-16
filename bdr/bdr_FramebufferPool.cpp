// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_FramebufferPool.h"
#include "bdr_Image.h"
#include "bdr_RenderPass.h"
#include "bdr_Helpers.h"

namespace bdr
{
Framebuffer::Framebuffer() 
	{}

Framebuffer::~Framebuffer() 
	{}

FramebufferPool::FramebufferPool( Device *_module ) : MultithreadPoolTemplate( _module )
	{
	LogThis;
	}

FramebufferPool::~FramebufferPool()
	{
	LogThis;

	this->Cleanup();
	}

status FramebufferPool::Setup( const FramebufferPoolTemplate &parameters )
	{
	Validate( parameters.framebuffersCount > 0, status::invalid_param ) << "Must allocate at least one framebuffer." << ValidateEnd;
	Validate( parameters.renderPass, status::invalid_param ) << "A RenderPass object is required to set up framebuffer." << ValidateEnd;
	Validate( parameters.commandPool, status::invalid_param ) << "A CommandPool object is required to set up framebuffer." << ValidateEnd;

	const size_t imageCount = parameters.imageSetups.size();
	const auto commandPool = parameters.commandPool;
	const auto width = parameters.extent.width;
	const auto height = parameters.extent.height;

	// set up the image templates
	vector<ImageTemplate> imageTemplates( imageCount );
	for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
		{
		const auto format = parameters.imageSetups[imageInx].format;
		const auto samples = parameters.imageSetups[imageInx].samples;

		if( HasVulkanFormatDepth( format ) )
			imageTemplates[imageInx] = ImageTemplate::DepthAttachment( format, width, height, commandPool, samples );
		else
			imageTemplates[imageInx] = ImageTemplate::ColorAttachment( format, width, height, commandPool, samples );
		}

	// allocate an initial framebuffers vector, and for each framebuffer, set up the images 
	vector<unique_ptr<Framebuffer>> framebuffers( parameters.framebuffersCount );
	for( size_t framebufferInx = 0; framebufferInx < parameters.framebuffersCount; ++framebufferInx )
		{
		framebuffers[framebufferInx] = std::make_unique<Framebuffer>();
		auto &framebuffer = *( framebuffers[framebufferInx].get() );

		// allocate images for the buffer
		framebuffer.images.resize( imageCount );
		for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
			{
			CheckRetValCall( image, this->Module->CreateObject<Image>( imageTemplates[imageInx] ) );
			framebuffer.images[imageInx] = std::move( image );
			}

		// setup the framebuffer object
		vector<VkImageView> attachments( imageCount );
		for( size_t imageInx = 0; imageInx < imageCount; ++imageInx )
			{
			attachments[imageInx] = framebuffer.images[imageInx]->GetImageView();
			}

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = parameters.renderPass->RenderPassHandle;
		framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;
		CheckCall( vkCreateFramebuffer( this->Module->GetDeviceHandle(), &framebufferCreateInfo, nullptr, &framebuffer.framebufferHandle ) );

		// setup the semaphore
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		CheckCall( vkCreateSemaphore( this->Module->GetDeviceHandle(), &semaphoreCreateInfo, nullptr, &framebuffer.semaphoreHandle ) );
		}

	// hand over framebuffers to object pool
	this->SetupPool( framebuffers );

	return status::ok;
	}

status FramebufferPool::Cleanup()
	{
	// clear the pool, and destroy the framebuffers
	vector<unique_ptr<Framebuffer>> framebuffers;
	this->ClearPool( framebuffers );
	for( size_t framebufferInx = 0; framebufferInx < framebuffers.size(); ++framebufferInx )
		{
		vkDestroyFramebuffer( this->Module->GetDeviceHandle(), framebuffers[framebufferInx]->framebufferHandle, nullptr );
		vkDestroySemaphore( this->Module->GetDeviceHandle(), framebuffers[framebufferInx]->semaphoreHandle, nullptr );
		}

	return status::ok;
	}


status_return<const Framebuffer *> FramebufferPool::AcquireFramebuffer()
	{
	CheckRetValCall( item, this->AcquireItem() );
	return item;
	}

status FramebufferPool::ReturnFramebuffer( const Framebuffer *framebuffer )
	{
	return this->ReturnItem( framebuffer );
	}

status_return<FramebufferPoolTemplate> FramebufferPoolTemplate::FromRenderPass( size_t framebuffersCount, uint32_t width, uint32_t height, const RenderPass *renderPass, CommandPool *commandPool )
	{
	Validate( renderPass, status::invalid_param ) << "A valid RenderPass object is required." << ValidateEnd;

	FramebufferPoolTemplate ret = {};

	ret.framebuffersCount = framebuffersCount;
	ret.extent = { width, height };
	ret.renderPass = renderPass;
	ret.commandPool = commandPool;

	const auto &tmpl = renderPass->Template.get();

	const size_t imageCount = tmpl.attachmentDescriptions.size();
	ret.imageSetups.resize( imageCount );
	for( size_t inx = 0; inx < imageCount; ++inx )
		{
		const auto &desc = tmpl.attachmentDescriptions[inx];
		ret.imageSetups[inx].format = desc.format;
		ret.imageSetups[inx].samples = desc.samples;
		}

	return ret;
	}

}
// namespace bdr