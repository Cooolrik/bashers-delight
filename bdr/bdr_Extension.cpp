// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#include <bdr/bdr.inl>

#include "bdr_Extension.h"

namespace bdr
{
	Extension::Extension( Instance* _module ) : InstanceSubmodule(_module) 
		{
		LogThis;
		}

	Extension::~Extension()
		{
		LogThis;

		this->Cleanup();
		}

	void Extension::AddExtensionToList( std::vector<const char *> *extensionList, const char *extensionName )
		{
		for( auto it = extensionList->begin(); it != extensionList->end(); it++ )
			{
			if( strcmp( ( *it ), extensionName ) == 0 )
				{
				return;
				}
			}

		// not found, add it
		extensionList->push_back( extensionName );
		}


	status Extension::CreateInstance( VkInstanceCreateInfo * /*instanceCreateInfo*/, std::vector<const char *> * /*extensionList*/ )
		{
		return status_code::ok;
		}

	status Extension::PostCreateInstance()
		{
		return status_code::ok;
		}

	status Extension::AddRequiredDeviceExtensions(
		VkPhysicalDeviceFeatures2 * /*PhysicalDeviceFeatures*/,
		VkPhysicalDeviceProperties2 * /*PhysicalDeviceProperties*/,
		std::vector<const char *> * /*extensionList*/ )
		{
		return status_code::ok;
		}

	bool Extension::SelectDevice(
		const VkSurfaceCapabilitiesKHR & /*surfaceCapabilities*/,
		const std::vector<VkSurfaceFormatKHR> & /*availableSurfaceFormats*/,
		const std::vector<VkPresentModeKHR> & /*availablePresentModes*/,
		const VkPhysicalDeviceFeatures2 & /*physicalDeviceFeatures*/,
		const VkPhysicalDeviceProperties2 & /*physicalDeviceProperties*/
	)
		{
		return true;
		}

	status Extension::CreateDevice( VkDeviceCreateInfo * /*deviceCreateInfo*/ )
		{
		return status_code::ok;
		}

	status Extension::PostCreateDevice()
		{
		return status_code::ok;
		}

	status Extension::Cleanup()
		{
		return status_code::ok;
		}

}
