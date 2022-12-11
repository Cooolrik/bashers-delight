#include "bdr_Common.inl"
#include "bdr_DescriptorSetLayout.h"
#include "bdr_Renderer.h"
#include "bdr_Image.h"

bdr::DescriptorSetLayout::~DescriptorSetLayout()
	{
	if( this->DescriptorSetLayoutHandle != nullptr )
		{
		vkDestroyDescriptorSetLayout( this->Module->GetDevice(), this->DescriptorSetLayoutHandle, nullptr );
		}
	}

uint bdr::DescriptorSetLayoutTemplate::AddUniformBufferBinding( VkShaderStageFlags shaderStages, uint arrayCount )
	{
	uint bindingIndex = (uint)this->Bindings.size();
	this->Bindings.emplace_back();
	this->Bindings[bindingIndex].binding = bindingIndex;
	this->Bindings[bindingIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	this->Bindings[bindingIndex].descriptorCount = arrayCount;
	this->Bindings[bindingIndex].stageFlags = shaderStages;

	return bindingIndex;
	}

uint bdr::DescriptorSetLayoutTemplate::AddStorageBufferBinding( VkShaderStageFlags shaderStages, uint arrayCount )
	{
	uint bindingIndex = (uint)this->Bindings.size();
	this->Bindings.emplace_back();
	this->Bindings[bindingIndex].binding = bindingIndex;
	this->Bindings[bindingIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	this->Bindings[bindingIndex].descriptorCount = arrayCount;
	this->Bindings[bindingIndex].stageFlags = shaderStages;

	return bindingIndex;
	}

uint bdr::DescriptorSetLayoutTemplate::AddSamplerBinding( VkShaderStageFlags shaderStages, uint arrayCount )
	{
	uint bindingIndex = (uint)this->Bindings.size();
	this->Bindings.emplace_back();
	this->Bindings[bindingIndex].binding = bindingIndex;
	this->Bindings[bindingIndex].descriptorCount = arrayCount;
	this->Bindings[bindingIndex].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	this->Bindings[bindingIndex].stageFlags = shaderStages;

	return bindingIndex;
	}

uint bdr::DescriptorSetLayoutTemplate::AddAccelerationStructureBinding( VkShaderStageFlags shaderStages, uint arrayCount )
	{
	uint bindingIndex = (uint)this->Bindings.size();
	this->Bindings.emplace_back();
	this->Bindings[bindingIndex].binding = bindingIndex;
	this->Bindings[bindingIndex].descriptorCount = arrayCount;
	this->Bindings[bindingIndex].descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	this->Bindings[bindingIndex].stageFlags = shaderStages;

	return bindingIndex;
	}

uint bdr::DescriptorSetLayoutTemplate::AddStoredImageBinding( VkShaderStageFlags shaderStages, uint arrayCount )
	{
	uint bindingIndex = (uint)this->Bindings.size();
	this->Bindings.emplace_back();
	this->Bindings[bindingIndex].binding = bindingIndex;
	this->Bindings[bindingIndex].descriptorCount = arrayCount;
	this->Bindings[bindingIndex].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	this->Bindings[bindingIndex].stageFlags = shaderStages;

	return bindingIndex;
	}
