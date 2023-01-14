// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

#include "../bdr_Instance.h"

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
            virtual Status CreateInstance( VkInstanceCreateInfo* instanceCreateInfo, std::vector<const char*>* extensionList );

            // called after instance is created, good place to set up dynamic methods and call stuff post create instance 
            virtual Status PostCreateInstance();

            // called to add required device extensions
            virtual Status AddRequiredDeviceExtensions( 
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
            virtual Status CreateDevice( VkDeviceCreateInfo* deviceCreateInfo );

            // called after device is created, good place to set up dynamic methods and call stuff post create device 
            virtual Status PostCreateDevice();

            // called before any extension is deleted. makes it possible to remove data that is dependent on some other extension
            virtual Status Cleanup();
        };
    };
