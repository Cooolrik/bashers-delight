#pragma once

// disable warning for "enum class" since we can't modify Vulkan SDK
#pragma warning( disable : 26812 )

#include "bdr_Instance.h"

namespace bdr
    {
    class Extension : public MainSubmodule
        {
        protected:
            Extension( const Instance* _module ) : MainSubmodule(_module) {};

        protected:
            static void AddExtensionToList( std::vector<const char*>* extensionList, const char* extensionName );
 
        public:
            // called before instance is created
            virtual VkResult CreateInstance( VkInstanceCreateInfo* instanceCreateInfo, std::vector<const char*>* extensionList );

            // called after instance is created, good place to set up dynamic methods and call stuff post create instance 
            virtual VkResult PostCreateInstance();

            // called to add required device extensions
            virtual VkResult AddRequiredDeviceExtensions( 
                VkPhysicalDeviceFeatures2* physicalDeviceFeatures, 
                VkPhysicalDeviceProperties2* physicalDeviceProperties,
                std::vector<const char*>* extensionList
                );

            // called to select pysical device. return true if the device is acceptable
            virtual bool SelectDevice(
                const VkSurfaceCapabilitiesKHR& surfaceCapabilities,
                const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats,
                const std::vector<VkPresentModeKHR>& availablePresentModes,
                const VkPhysicalDeviceFeatures2& physicalDeviceFeatures,
                const VkPhysicalDeviceProperties2& physicalDeviceProperties
            );

            // called before device is created
            virtual VkResult CreateDevice( VkDeviceCreateInfo* deviceCreateInfo );

            // called after device is created, good place to set up dynamic methods and call stuff post create device 
            virtual VkResult PostCreateDevice();

            // called before any extension is deleted. makes it possible to remove data that is dependent on some other extension
            virtual VkResult Cleanup();
        };
    };
