#pragma once

// disable warning for "enum class" since we can't modify Vulkan SDK
#pragma warning( disable : 26812 )

#include "bdr_Renderer.h"

namespace bdr
    {
    class ShaderModule;
    class VertexBuffer;
    class DescriptorSetLayout;

    class Pipeline : public RendererSubmodule
        {
        BDSubmoduleMacro( Pipeline, RendererSubmodule, Renderer );

        protected:
            VkPipeline PipelineHandle = nullptr;
            VkPipelineLayout PipelineLayout = nullptr;
            VkPipelineBindPoint PipelineBindPoint = {};

        public:
            BDGetCustomNameMacro( VkPipeline, Pipeline , PipelineHandle );
            BDGetMacro( VkPipelineLayout, PipelineLayout );
            BDGetMacro( VkPipelineBindPoint, PipelineBindPoint );
        };
    };