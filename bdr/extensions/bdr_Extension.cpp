#define VR_LESS_STRICT_WARNINGS
#include "bdr_Common.inl"
#include "bdr_Extension.h"

void bdr::Extension::AddExtensionToList( std::vector<const char*>* extensionList, const char* extensionName )
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


VkResult bdr::Extension::CreateInstance( VkInstanceCreateInfo* instanceCreateInfo, std::vector<const char*>* extensionList )
	{
	return VkResult::VK_SUCCESS;
	}

VkResult bdr::Extension::PostCreateInstance()
	{
	return VkResult::VK_SUCCESS;
	}

VkResult bdr::Extension::AddRequiredDeviceExtensions( 
	VkPhysicalDeviceFeatures2* PhysicalDeviceFeatures,
	VkPhysicalDeviceProperties2* PhysicalDeviceProperties,
	std::vector<const char*>* extensionList )
	{
	return VkResult::VK_SUCCESS;
	}

bool bdr::Extension::SelectDevice(
	const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
	const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats,
	const std::vector<VkPresentModeKHR>& availablePresentModes,
	const VkPhysicalDeviceFeatures2& physicalDeviceFeatures,
	const VkPhysicalDeviceProperties2& physicalDeviceProperties 
	)
	{ 
	return true; 
	}

VkResult bdr::Extension::CreateDevice( VkDeviceCreateInfo* deviceCreateInfo )
	{
	return VkResult::VK_SUCCESS;
	}

VkResult bdr::Extension::PostCreateDevice()
	{
	return VkResult::VK_SUCCESS;
	}

VkResult bdr::Extension::Cleanup()
	{
	return VkResult::VK_SUCCESS;
	}

bdr::Extension::~Extension()
	{
	}
