#include "bdr_Common.inl"

#include "bdr_DescriptorPool.h"
#include "bdr_DescriptorSetLayout.h"
#include "bdr_Renderer.h"
#include "bdr_Image.h"
#include "bdr_RayTracingAccelerationStructure.h"
#include "bdr_Buffer.h"

using std::vector;

bdr::DescriptorPool::~DescriptorPool()
	{
	if( this->Pool )
		{
		vkDestroyDescriptorPool( this->Module->GetDevice(), this->Pool, nullptr );
		}
	}

VkDescriptorSet bdr::DescriptorPool::BeginDescriptorSet( DescriptorSetLayout *descriptorLayout )
	{
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout layouts[1] = { descriptorLayout->GetDescriptorSetLayout() };

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = this->Pool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = layouts;

	VLK_CALL( vkAllocateDescriptorSets( this->Module->GetDevice(), &descriptorSetAllocateInfo, &descriptorSet ) );

	// allocate the bindings descriptors
	std::vector<VkDescriptorSetLayoutBinding> Bindings = descriptorLayout->GetBindings();
	
	this->WriteDescriptorSets.resize( Bindings.size() );
	this->WriteDescriptorInfos.resize( Bindings.size() );

	// set up all the bindings
	for( size_t bindingIndex = 0; bindingIndex < Bindings.size(); ++bindingIndex )
		{
		this->WriteDescriptorSets[bindingIndex] = {};
		this->WriteDescriptorSets[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		this->WriteDescriptorSets[bindingIndex].dstSet = descriptorSet;
		this->WriteDescriptorSets[bindingIndex].dstBinding = (uint32_t)bindingIndex;
		this->WriteDescriptorSets[bindingIndex].dstArrayElement = 0;
		this->WriteDescriptorSets[bindingIndex].descriptorType = Bindings[bindingIndex].descriptorType;
		this->WriteDescriptorSets[bindingIndex].descriptorCount = Bindings[bindingIndex].descriptorCount;

		VkWriteDescriptorSet* writeDescriptorSet = &this->WriteDescriptorSets[bindingIndex];
		DescriptorInfo* descriptorInfo = &this->WriteDescriptorInfos[bindingIndex];
		uint descriptorCount = Bindings[bindingIndex].descriptorCount;

		// allocate write descriptors
		switch( Bindings[bindingIndex].descriptorType )
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
				descriptorInfo->DescriptorBufferInfo.resize( descriptorCount );
				writeDescriptorSet->pBufferInfo = descriptorInfo->DescriptorBufferInfo.data();
				break;
			
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
				descriptorInfo->DescriptorImageInfo.resize( descriptorCount );
				writeDescriptorSet->pImageInfo = descriptorInfo->DescriptorImageInfo.data();
				break;

			// acceleration structure for the ray tracing extension
			case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:

				// set up extension write structure, add to linked list of extensions
				descriptorInfo->WriteDescriptorSetAccelerationStructureKHR = {};
				descriptorInfo->WriteDescriptorSetAccelerationStructureKHR.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
				descriptorInfo->WriteDescriptorSetAccelerationStructureKHR.accelerationStructureCount = descriptorCount;
				descriptorInfo->WriteDescriptorSetAccelerationStructureKHR.pNext = writeDescriptorSet->pNext;
				writeDescriptorSet->pNext = &descriptorInfo->WriteDescriptorSetAccelerationStructureKHR;

				descriptorInfo->AccelerationStructureKHR.resize( descriptorCount );
				descriptorInfo->WriteDescriptorSetAccelerationStructureKHR.pAccelerationStructures = descriptorInfo->AccelerationStructureKHR.data();
				break;
			}
		}

	return descriptorSet;
	}

void bdr::DescriptorPool::SetBuffer( uint bindingIndex, Buffer* buffer, uint byteOffset )
	{
	this->SetBufferInArray( bindingIndex, 0, buffer, byteOffset );
	}

void bdr::DescriptorPool::SetBufferInArray( uint bindingIndex, uint arrayIndex, Buffer* buffer, uint byteOffset )
	{
	if( this->WriteDescriptorSets[bindingIndex].pBufferInfo == nullptr )
		{
		throw runtime_error( "Error: SetBufferInArray(): The bound index is not set up for a buffer" );
		}
	if( arrayIndex >= this->WriteDescriptorSets[bindingIndex].descriptorCount )
		{
		throw runtime_error( "Error: SetBufferInArray(): arrayIndex is out of range" );
		}

	// set up the info at {bindingIndex,arrayIndex} 
	this->WriteDescriptorInfos[bindingIndex].DescriptorBufferInfo[arrayIndex].buffer = buffer->GetBuffer();
	this->WriteDescriptorInfos[bindingIndex].DescriptorBufferInfo[arrayIndex].offset = byteOffset;
	this->WriteDescriptorInfos[bindingIndex].DescriptorBufferInfo[arrayIndex].range = buffer->GetBufferSize() - byteOffset;
	}

void bdr::DescriptorPool::SetImage( uint bindingIndex, VkImageView imageView, VkSampler sampler, VkImageLayout imageLayout )
	{
	this->SetImageInArray( bindingIndex, 0, imageView, sampler , imageLayout );
	}

void bdr::DescriptorPool::SetImageInArray( uint bindingIndex, uint arrayIndex, VkImageView imageView, VkSampler sampler, VkImageLayout imageLayout )
	{
	if(this->WriteDescriptorSets[bindingIndex].pImageInfo == nullptr)
		{
		throw runtime_error( "Error: SetImageInArray(): The bound index is not set up for an image" );
		}
	if(arrayIndex >= this->WriteDescriptorSets[bindingIndex].descriptorCount)
		{
		throw runtime_error( "Error: SetImageInArray(): arrayIndex is out of range" );
		}

	// set up the info at {bindingIndex,arrayIndex} 
	VkDescriptorImageInfo imageInfo;
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;
	imageInfo.imageLayout = imageLayout;
	this->WriteDescriptorInfos[bindingIndex].DescriptorImageInfo[arrayIndex] = imageInfo;
	}

void bdr::DescriptorPool::SetAccelerationStructure( uint bindingIndex, RayTracingAccelerationStructure* accelerationStructure )
	{
	this->SetAccelerationStructureInArray( bindingIndex, 0, accelerationStructure );
	}

void bdr::DescriptorPool::SetAccelerationStructureInArray( uint bindingIndex, uint arrayIndex, RayTracingAccelerationStructure* accelerationStructure )
	{
	if( this->WriteDescriptorSets[bindingIndex].descriptorType != VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR )
		{
		throw runtime_error( "Error: SetAccelerationStructure(): Invalid type bound, this should not be an acceleration structure" );
		}

	// set up the info at {bindingIndex,arrayIndex} 
	this->WriteDescriptorInfos[bindingIndex].AccelerationStructureKHR[arrayIndex] = accelerationStructure->GetAccelerationStructure();
	}

void bdr::DescriptorPool::EndDescriptorSet()
	{
	vkUpdateDescriptorSets( this->Module->GetDevice(), (uint32_t)this->WriteDescriptorSets.size(), this->WriteDescriptorSets.data(), 0, nullptr );
	}


void bdr::DescriptorPool::ResetDescriptorPool()
	{
	vkResetDescriptorPool( this->Module->GetDevice(), this->Pool , 0 );
	}

////////////////////////////////////////////////

bdr::DescriptorPoolTemplate bdr::DescriptorPoolTemplate::General( unsigned int maxDescriptorSets, unsigned int maxDescriptorCount )
	{
	DescriptorPoolTemplate ret;
	
	ret.DescriptorPoolCreateInfo.maxSets = maxDescriptorSets;

	ret.DescriptorPoolSizes = 
		{
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxDescriptorCount },
		};

	return ret;
	}

bdr::DescriptorPoolTemplate bdr::DescriptorPoolTemplate::Maximized( unsigned int maxDescriptorSets, unsigned int maxDescriptorCount )
	{
	DescriptorPoolTemplate ret;

	ret.DescriptorPoolCreateInfo.maxSets = maxDescriptorSets;

	ret.DescriptorPoolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, maxDescriptorCount },
			{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, maxDescriptorCount },
		};

	return ret;
	}
