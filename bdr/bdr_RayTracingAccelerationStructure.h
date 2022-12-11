#pragma once

#include "bdr_RayTracingExtension.h"

namespace bdr
    {
    class RayTracingAccelerationStructure : public RayTracingExtensionSubmodule
        {
        BDSubmoduleMacro( RayTracingAccelerationStructure , RayTracingExtensionSubmodule, RayTracingExtension );

        private:
            std::unique_ptr<Buffer> ASBuffer; // buffer to hold the structure
            VkAccelerationStructureKHR AccelerationStructure = nullptr;

        public:
            BDGetMacro( VkAccelerationStructureKHR, AccelerationStructure );
        };
    };
