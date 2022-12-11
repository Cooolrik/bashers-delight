#include "bdr_Common.inl"

#include "bdr_ComputePipeline.h"
#include "bdr_ShaderModule.h"
#include "bdr_VertexBuffer.h"
#include "bdr_DescriptorSetLayout.h"

using std::vector;


bdr::ComputePipelineTemplate::ComputePipelineTemplate()
	{
	// pipeline layout is initially empty
	this->PipelineLayoutCreateInfo.setLayoutCount = 0;
	this->PipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	}

void bdr::ComputePipelineTemplate::SetShaderModule( const ShaderModule* shader )
	{
	this->Shader = shader;
	}

unsigned int bdr::ComputePipelineTemplate::AddDescriptorSetLayout( const DescriptorSetLayout* descriptorLayout )
	{
	uint index = (uint)this->DescriptorSetLayouts.size();
	this->DescriptorSetLayouts.emplace_back( descriptorLayout->GetDescriptorSetLayout() );
	this->PipelineLayoutCreateInfo.setLayoutCount = (uint32_t)this->DescriptorSetLayouts.size();
	this->PipelineLayoutCreateInfo.pSetLayouts = this->DescriptorSetLayouts.data();
	return index;
	}

unsigned int bdr::ComputePipelineTemplate::AddPushConstantRange( VkPushConstantRange range )
	{
	uint index = (uint)this->PushConstantRanges.size();
	this->PushConstantRanges.emplace_back( range );
	this->PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)this->PushConstantRanges.size();
	this->PipelineLayoutCreateInfo.pPushConstantRanges = this->PushConstantRanges.data();
	return index;
	}

unsigned int bdr::ComputePipelineTemplate::AddPushConstantRange( VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size )
	{
	VkPushConstantRange range = {};
	range.stageFlags = stageFlags;
	range.offset = offset;
	range.size = size;
	return AddPushConstantRange( range );
	}

