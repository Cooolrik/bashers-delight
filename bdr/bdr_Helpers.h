#pragma once

#include "bdr.h"

namespace bdr
{

extern uint32_t GetVulkanFormatByteSize( VkFormat format );
extern uint32_t GetVulkanFormatChannelCount( VkFormat format );

extern bool HasVulkanFormatDepth( VkFormat format );
extern bool HasVulkanFormatStencil( VkFormat format );

}
//namespace bdr