// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

#include <bdr/bdr_Extension.h>

namespace bdr
{

class BufferDeviceAddressExtension : public Extension
	{
	private:
		// The extension can only be created by the Instance::Create method
		friend status_return<unique_ptr<Instance>> Instance::Create( const InstanceTemplate &parameters );
		BufferDeviceAddressExtension( Instance *_instance ) : Extension( _instance )
			{};

		VkPhysicalDeviceBufferDeviceAddressFeaturesKHR BufferDeviceAddressFeaturesQuery{};
		VkPhysicalDeviceBufferDeviceAddressFeaturesKHR BufferDeviceAddressFeaturesCreate{};

	public:
		// ####################################
		//
		// Extension code
		//


		// called to add required device extensions
		virtual status AddRequiredDeviceExtensions(
			VkPhysicalDeviceFeatures2 *physicalDeviceFeatures,
			VkPhysicalDeviceProperties2 *physicalDeviceProperties,
			std::vector<const char *> *extensionList
		);

		// called to select pysical device. return true if the device is acceptable
		virtual bool SelectDevice(
			const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
			const std::vector<VkSurfaceFormatKHR> &availableSurfaceFormats,
			const std::vector<VkPresentModeKHR> &availablePresentModes,
			const VkPhysicalDeviceFeatures2 &physicalDeviceFeatures,
			const VkPhysicalDeviceProperties2 &physicalDeviceProperties
		);

		// called before device is created
		virtual status CreateDevice( VkDeviceCreateInfo *deviceCreateInfo );

	};

}
// namespace bdr













