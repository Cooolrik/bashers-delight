#pragma once

#include "bdr_Instance.h"

namespace bdr
    {
    class ShaderModule
        {
        private:
            ShaderModule() = default;
            ShaderModule( const ShaderModule& other );
            friend class ComputePipeline;
            friend class Renderer;

            VkShaderStageFlagBits Stage{};
            char* Name = nullptr;
            char* Entrypoint = nullptr;
            std::vector<char> Shader{};

        public:
            static ShaderModule* CreateFromFile(
                VkShaderStageFlagBits shaderStage,
                const char* shaderFilepath,
                const char* entrypoint = "main",
                const char* shaderName = nullptr
                );

            ~ShaderModule();

            //BDGetMacro( VkShaderStageFlagBits, Stage );
            //BDGetConstRefMacro( std::vector<char>, Shader );
            //BDGetMacro( char*, Name );
            //BDGetMacro( char*, Entrypoint );
        };
    };

