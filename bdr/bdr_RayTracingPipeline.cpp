#include "bdr_Common.inl"

#include "bdr_RayTracingPipeline.h"
#include "bdr_ShaderModule.h"
#include "bdr_VertexBuffer.h"
#include "bdr_DescriptorSetLayout.h"
#include "bdr_RayTracingShaderBindingTable.h"

using std::vector;

void bdr::RayTracingPipelineTemplate::SetRaygenShaderModule( const ShaderModule* shader )
	{
	this->RaygenShader = shader;
	}

uint bdr::RayTracingPipelineTemplate::AddMissShaderModule( const ShaderModule* shader )
	{
	uint index = (uint)this->MissShaders.size();
	this->MissShaders.push_back( shader );
	return index;
	}

uint bdr::RayTracingPipelineTemplate::AddClosestHitShaderModule( const ShaderModule* shader )
	{
	uint index = (uint)this->ClosestHitShaders.size();
	this->ClosestHitShaders.push_back( shader );
	return index;
	}

bdr::RayTracingPipelineTemplate::RayTracingPipelineTemplate()
	{
	// pipeline layout is initially empty
	this->PipelineLayoutCreateInfo.setLayoutCount = 0;
	this->PipelineLayoutCreateInfo.pushConstantRangeCount = 0;

	// set the base values
	this->RayTracingPipelineCreateInfo.maxPipelineRayRecursionDepth = 2;

	}


uint bdr::RayTracingPipelineTemplate::AddDescriptorSetLayout( const DescriptorSetLayout* descriptorLayout )
	{
	uint index = (uint)this->DescriptorSetLayouts.size();
	this->DescriptorSetLayouts.emplace_back( descriptorLayout->GetDescriptorSetLayout() );
	this->PipelineLayoutCreateInfo.setLayoutCount = (uint32_t)this->DescriptorSetLayouts.size();
	this->PipelineLayoutCreateInfo.pSetLayouts = this->DescriptorSetLayouts.data();
	return index;
	}

uint bdr::RayTracingPipelineTemplate::AddPushConstantRange( VkPushConstantRange range )
	{
	uint index = (uint)this->PushConstantRanges.size();
	this->PushConstantRanges.emplace_back( range );
	this->PipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)this->PushConstantRanges.size();
	this->PipelineLayoutCreateInfo.pPushConstantRanges = this->PushConstantRanges.data();
	return index;
	}

uint bdr::RayTracingPipelineTemplate::AddPushConstantRange( VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size )
	{
	VkPushConstantRange range = {};
	range.stageFlags = stageFlags;
	range.offset = offset;
	range.size = size;
	return AddPushConstantRange( range );
	}

bdr::RayTracingPipeline::~RayTracingPipeline() 
	{
	};