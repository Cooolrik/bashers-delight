#pragma once

#include "bdr_Device.h"

namespace bdr
{

// RenderPass describes how a pipeline generates output, along with the images used (color, depth, other)
class RenderPass : public DeviceSubmodule
	{
	public:
		~RenderPass();

	private:
		friend status_return<unique_ptr<RenderPass>> Device::CreateObject<RenderPass, RenderPassTemplate>( const RenderPassTemplate &parameters );
		RenderPass( Device *_module );
		status Setup( const RenderPassTemplate &parameters );

	private:
		VkRenderPass RenderPassHandle = nullptr;
		unique_ptr<RenderPassTemplate> Template;

	public:
		// explicitly clear the RenderPass data, and release the vulkan handles. 
		// (note that RenderPasses cannot be re-created after cleanup. Instead, delete and create a new object)
		status Cleanup();

		// retrieve Vulkan handles
		VkRenderPass GetRenderPassHandle() const { return this->RenderPassHandle; }

		// retrieve the template used to create the render pass
		const RenderPassTemplate &GetTemplate() const { return *(this->Template.get()); }
	};

// template used to create an RenderPass
class RenderPassTemplate
	{
	public:
		// the framebuffer attachments of the render pass
		vector<VkAttachmentDescription> attachmentDescriptions;

		// subpass descriptions, along with references to attachments used by each subpass
		class SubpassDescription
			{
			public:
				// the references to the color attachments
				vector<VkAttachmentReference> colorAttachments;

				// the reference to the depth attachment (and stencil, if one exists)
				optional_value<VkAttachmentReference> depthStencilAttachment;

				// subpass creation settings (flags and pipeline type)
				VkSubpassDescriptionFlags flags = {};
				VkPipelineBindPoint pipelineBindPoint = {};
			};
		vector<SubpassDescription> subpassDescriptions;

		// subpass dependencies, memory dependencies between the subpasses
		vector<VkSubpassDependency> subpassDependencies;

		/////////////////////////////////

		// create a render pass with a single subpass, with colors and depth output
		static RenderPassTemplate SingleSubPass( VkFormat colorAttachmentFormat, VkFormat depthAttachmentFormat, VkSampleCountFlagBits samples );
		static RenderPassTemplate SingleSubPass( const vector<VkFormat> &colorAttachmentFormats, VkFormat depthAttachmentFormat, VkSampleCountFlagBits samples );
	};
};
