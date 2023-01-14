// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_BufferDeviceAddressExtension.h"

#include <bdr/bdr_Buffer.h>

namespace bdr
{

Status bdr::BufferDeviceAddressExtension::AddRequiredDeviceExtensions( 
	VkPhysicalDeviceFeatures2* physicalDeviceFeatures,
	VkPhysicalDeviceProperties2* /*physicalDeviceProperties*/,
	std::vector<const char*>* extensionList
	)
	{
	// enable extensions needed for ray tracing
	Extension::AddExtensionToList( extensionList, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME );

	// set up query structs

	// features
	InitializeLinkedVulkanStructure( physicalDeviceFeatures, this->BufferDeviceAddressFeaturesQuery, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR );

	return status_code::ok;
	}

bool bdr::BufferDeviceAddressExtension::SelectDevice(
	const VkSurfaceCapabilitiesKHR& /*surfaceCapabilities*/,
	const std::vector<VkSurfaceFormatKHR>& /*availableSurfaceFormats*/,
	const std::vector<VkPresentModeKHR>& /*availablePresentModes*/,
	const VkPhysicalDeviceFeatures2& /*physicalDeviceFeatures*/,
	const VkPhysicalDeviceProperties2& /*physicalDeviceProperties*/
	)
	{
	// check for needed features
	if( !this->BufferDeviceAddressFeaturesQuery.bufferDeviceAddress )
		return false;

	return true;
	}

Status bdr::BufferDeviceAddressExtension::CreateDevice( VkDeviceCreateInfo* deviceCreateInfo )
	{
	InitializeLinkedVulkanStructure( deviceCreateInfo , this->BufferDeviceAddressFeaturesCreate, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR );

	// enable required features
	this->BufferDeviceAddressFeaturesCreate.bufferDeviceAddress = VK_TRUE;

	return status_code::ok;
	}

}
