// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include <bdr/bdr_Buffer.h>

#include "bdr_DescriptorIndexingExtension.h"

namespace bdr
{

Status bdr::DescriptorIndexingExtension::AddRequiredDeviceExtensions( 
	VkPhysicalDeviceFeatures2* physicalDeviceFeatures,
	VkPhysicalDeviceProperties2* /*physicalDeviceProperties*/,
	std::vector<const char*>* extensionList
	)
	{
	// enable extensions needed for ray tracing
	Extension::AddExtensionToList( extensionList, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME );

	// set up query structs

	// features
	InitializeLinkedVulkanStructure( physicalDeviceFeatures, this->DescriptorIndexingFeaturesQuery, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES );

	return status_code::ok;
	}

bool bdr::DescriptorIndexingExtension::SelectDevice(
	const VkSurfaceCapabilitiesKHR& /*surfaceCapabilities*/,
	const std::vector<VkSurfaceFormatKHR>& /*availableSurfaceFormats*/,
	const std::vector<VkPresentModeKHR>& /*availablePresentModes*/,
	const VkPhysicalDeviceFeatures2& /*physicalDeviceFeatures*/,
	const VkPhysicalDeviceProperties2& /*physicalDeviceProperties*/
	)
	{
	// check for needed features
	if( !this->DescriptorIndexingFeaturesQuery.shaderStorageBufferArrayNonUniformIndexing )
		return false;
	if( !this->DescriptorIndexingFeaturesQuery.shaderSampledImageArrayNonUniformIndexing )
		return false;
	if( !this->DescriptorIndexingFeaturesQuery.runtimeDescriptorArray )
		return false;

	return true;
	}

Status bdr::DescriptorIndexingExtension::CreateDevice( VkDeviceCreateInfo* deviceCreateInfo )
	{
	InitializeLinkedVulkanStructure( deviceCreateInfo, this->DescriptorIndexingFeaturesCreate, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES );

	// enable required features
	this->DescriptorIndexingFeaturesCreate.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
	this->DescriptorIndexingFeaturesCreate.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
	this->DescriptorIndexingFeaturesCreate.runtimeDescriptorArray = VK_TRUE;

	return status_code::ok;
	}

}
