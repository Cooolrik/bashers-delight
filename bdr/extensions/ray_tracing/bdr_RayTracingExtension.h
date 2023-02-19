// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

#include <bdr/bdr_Extension.h>

namespace bdr
    {
    class RayTracingPipeline;
    class RayTracingPipelineTemplate;
    class RayTracingAccelerationStructure;
    class RayTracingBLASEntry;
    class RayTracingTLASEntry;
    class RayTracingShaderBindingTable;

    class RayTracingExtension : public Extension
        {
        private:
            // The extension can only be created by the Instance::Create method
            friend status_return<unique_ptr<Instance>> Instance::Create( const InstanceTemplate& parameters );
            RayTracingExtension( const Instance* _instance ) : Extension(_instance) {};

            VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeaturesQuery{};
            VkPhysicalDeviceRayTracingPipelineFeaturesKHR RayTracingPipelineFeaturesQuery{};
            VkPhysicalDeviceHostQueryResetFeatures HostQueryResetFeaturesQuery{};

            VkPhysicalDeviceAccelerationStructureFeaturesKHR AccelerationStructureFeaturesCreate{};
            VkPhysicalDeviceRayTracingPipelineFeaturesKHR RayTracingPipelineFeaturesCreate{};
            VkPhysicalDeviceHostQueryResetFeatures HostQueryResetFeaturesCreate{};

            VkPhysicalDeviceAccelerationStructurePropertiesKHR AccelerationStructureProperties{};
            VkPhysicalDeviceRayTracingPipelinePropertiesKHR RayTracingPipelineProperties{};

            vector<RayTracingAccelerationStructure*> BLASes;
            RayTracingAccelerationStructure *TLAS{};

            unordered_set<RayTracingPipeline*> RayTracingPipelines;
            void RemoveRayTracingPipeline( RayTracingPipeline* pipeline );

            RayTracingAccelerationStructure* CreateAccBuffer( VkAccelerationStructureCreateInfoKHR createInfo );

            VkCommandPool CreateInternalCommandPool();
            void CreateInternalCommandBuffers( VkCommandPool cmdPool, uint32_t num_entries, VkCommandBuffer* cmdBuffers );
            void SubmitAndFreeInternalCommandBuffers( VkCommandPool cmdPool, uint32_t num_entries, VkCommandBuffer* cmdBuffers );

            VkResult BeginInternalCommandBuffer( VkCommandBuffer cmdBuffer );

        public:
            // build the BLAS from the list of entries
            // TODO: make this more dynamic so we can add and remove entries in the blas
            void BuildBLAS( const std::vector<RayTracingBLASEntry*> &BLASEntries );

            // build the BLAS from the list of entries
            // TODO: make this more dynamic so we can add and remove entries in the blas
            void BuildTLAS( const std::vector<RayTracingTLASEntry*>& TLASEntries );

            // get the TLAS
            RayTracingAccelerationStructure* GetTLAS();

            // create a ray tracing pipeline object
            RayTracingPipeline* CreateRayTracingPipeline( const RayTracingPipelineTemplate& rtt );

            // create a shader binding table for a created pipeline
            RayTracingShaderBindingTable* CreateShaderBindingTable( const RayTracingPipeline* pipeline );

            // ####################################
            //
            // Extension code
            //

            // called after instance is created, good place to set up dynamic methods and call stuff post create instance 
            virtual status PostCreateInstance();

            // called to add required device extensions
            virtual status AddRequiredDeviceExtensions( 
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
            virtual status CreateDevice( VkDeviceCreateInfo* deviceCreateInfo );

            // called after device is created, good place to set up dynamic methods and call stuff post create device 
            virtual status PostCreateDevice();

            // called before any extension is deleted. makes it possible to remove data that is dependent on some other extension
            virtual status Cleanup();

            //BDGetConstRefMacro( VkPhysicalDeviceAccelerationStructurePropertiesKHR, AccelerationStructureProperties );
            //BDGetConstRefMacro( VkPhysicalDeviceRayTracingPipelinePropertiesKHR, RayTracingPipelineProperties );

            // Extension dynamic methods

            // VK_KHR_acceleration_structure
            static PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR;
            static PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR;
            static PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR;
            static PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR;
            static PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR;
            static PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR;
            static PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR;
            static PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR;
            static PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR;
            static PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR;
            static PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR;
            static PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR;
            static PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR;
            static PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR;
            static PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR;
            static PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR;

            // VK_KHR_ray_tracing_pipeline
            static PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR;
            static PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR;
            static PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR;
            static PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR;
            static PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR;
            static PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR;
            static PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR;
        };
    };













