// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_RenderPass.h"
#include "bdr_Device.h"
#include "bdr_CommandPool.h"
#include "bdr_Helpers.h"
#include "bdr_Buffer.h"

namespace bdr
{

RenderPass::RenderPass( Device *_module ) 
	: DeviceSubmodule( _module )
	, RenderPassHandle(_RenderPassHandle)
	, Template(_Template)
	{}

RenderPass::~RenderPass()
	{
	this->Cleanup();
	}

status RenderPass::Setup( const RenderPassTemplate &parameters )
	{
	Validate( parameters.attachmentDescriptions.size() > 0, status::invalid_param ) << "Needs at least one image attachment." << ValidateEnd;
	Validate( parameters.subpassDescriptions.size() > 0, status::invalid_param ) << "Needs at least one subpass." << ValidateEnd;
	Validate( parameters.subpassDependencies.size() > 0, status::invalid_param ) << "Needs at least one subpass dependency." << ValidateEnd;

	const size_t subpassCount = parameters.subpassDescriptions.size();

	// setup the descriptor structures
	vector<VkSubpassDescription> subpassDescriptions( subpassCount );
	for( size_t subpassInx = 0; subpassInx < subpassCount; ++subpassInx )
		{
		auto &colorAttachments = parameters.subpassDescriptions[subpassInx].colorAttachments;
		auto &depthAttachmentReference = parameters.subpassDescriptions[subpassInx].depthStencilAttachment;
		auto &subpassDescription = subpassDescriptions[subpassInx];

		subpassDescription.colorAttachmentCount = (uint32_t)colorAttachments.size();
		subpassDescription.pColorAttachments = colorAttachments.data();
		if( depthAttachmentReference.has_value() )
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference.value();

		subpassDescription.flags = parameters.subpassDescriptions[subpassInx].flags;
		subpassDescription.pipelineBindPoint = parameters.subpassDescriptions[subpassInx].pipelineBindPoint;
		}

	// create the render pass
	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = (uint32_t)parameters.attachmentDescriptions.size();
	renderPassCreateInfo.pAttachments = parameters.attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = (uint32_t)subpassDescriptions.size();
	renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
	renderPassCreateInfo.dependencyCount = (uint32_t)parameters.subpassDependencies.size();
	renderPassCreateInfo.pDependencies = parameters.subpassDependencies.data();
	CheckCall( vkCreateRenderPass( this->Module->GetDeviceHandle(), &renderPassCreateInfo, nullptr, &this->_RenderPassHandle ) );

	// copy the template
	this->_Template = std::make_unique<RenderPassTemplate>(parameters);

	return status::ok;
	}

status RenderPass::Cleanup()
	{
	if( this->RenderPassHandle != nullptr )
		{
		vkDestroyRenderPass( this->GetModule()->GetDeviceHandle(), this->_RenderPassHandle, nullptr );
		this->_RenderPassHandle = nullptr;
		}
	this->_Template.reset();

	return status::ok;
	}

RenderPassTemplate RenderPassTemplate::SingleSubPass( VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat, VkSampleCountFlagBits samples )
	{
	const vector<VkFormat> colorAttachmentFormats = { colorAttachmentFormat };
	return SingleSubPass( colorAttachmentFormats, depthAttachmentFormat, samples );
	}

RenderPassTemplate RenderPassTemplate::SingleSubPass( const vector<VkFormat> &colorAttachmentFormats, VkFormat depthAttachmentFormat, VkSampleCountFlagBits samples )
	{
	RenderPassTemplate ret = {};

	// attachments, depth added at the end
	ret.attachmentDescriptions.resize( colorAttachmentFormats.size() + 1 );
	for( size_t inx = 0; inx < colorAttachmentFormats.size() + 1; ++inx )
		{
		auto &desc = ret.attachmentDescriptions[inx];

		// color / depth
		if( inx < colorAttachmentFormats.size() )
			{
			desc.format = colorAttachmentFormats[inx];
			desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}
		else
			{
			desc.format = depthAttachmentFormat;
			desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

		desc.samples = samples;
		desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		}

	// add the one subpass
	ret.subpassDescriptions.resize( 1 );
	auto &subpassDescription = ret.subpassDescriptions[0];

	// color attachments
	subpassDescription.colorAttachments.resize( colorAttachmentFormats.size() );
	for( size_t inx = 0; inx < colorAttachmentFormats.size(); ++inx )
		{
		subpassDescription.colorAttachments[inx].attachment = (uint32_t)inx;
		subpassDescription.colorAttachments[inx].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

	// depth attachment
	subpassDescription.depthStencilAttachment.set();
	subpassDescription.depthStencilAttachment.value().attachment = (uint32_t)colorAttachmentFormats.size();
	subpassDescription.depthStencilAttachment.value().layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// add the one subpass dependency
	ret.subpassDependencies.resize( 1 );
	auto &subpassDependency = ret.subpassDependencies[0];

	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	return ret;
	}

}
//namespace bdr