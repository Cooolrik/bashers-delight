#pragma once

// disable warning for "enum class" since we can't modify Vulkan SDK
#pragma warning( disable : 26812 )

#include "bdr_Renderer.h"
#include "bdr_Pipeline.h"

namespace bdr
    {
    class ShaderModule;

    class ComputePipelineTemplate
        {
        private:
            // dont allow copy-by-value, because of inter-struct links
            ComputePipelineTemplate( const ComputePipelineTemplate& other );
            const ComputePipelineTemplate& operator = ( const ComputePipelineTemplate& other );

        public:
            // the shader module to use
            const ShaderModule* Shader = nullptr;

            // pipeline layout structures
            std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
            std::vector<VkPushConstantRange> PushConstantRanges;
            VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

            // pipeline create info
            VkComputePipelineCreateInfo ComputePipelineCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };

            //////////////////////////////////////

             // creates an initial pipeline. 
            ComputePipelineTemplate();

            // set the shader stage to the pipeline
            void SetShaderModule( const ShaderModule* shader );

            // adds a descriptor set layout. returns the index of the set in the list of layouts
            unsigned int AddDescriptorSetLayout( const DescriptorSetLayout* descriptorLayout );

            // adds a push constant range. returns the index of the range in the list of layouts
            unsigned int AddPushConstantRange( VkPushConstantRange range );
            unsigned int AddPushConstantRange( VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size );

        };
    };

