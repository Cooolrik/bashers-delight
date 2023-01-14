// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

#include "bdr_Extension.h"

namespace bdr
    {
    class DescriptorIndexingExtension : public Extension
        {
        private:

            VkPhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeaturesQuery{};
            VkPhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeaturesCreate{};

            VkPhysicalDeviceDescriptorIndexingProperties DescriptorIndexingProperties{};

        public:
            DescriptorIndexingExtension( const Instance* _instance ) : Extension(_instance) {};

            // ####################################
            //
            // Extension code
            //


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
        };
    };













