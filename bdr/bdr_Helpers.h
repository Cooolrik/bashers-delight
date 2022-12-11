#pragma once

#include <vector>
#include <set>
#include <map>

namespace bdr
	{
	extern uint32_t GetVulkanFormatByteSize( VkFormat format );
	extern uint32_t GetVulkanFormatChannelCount( VkFormat format );
	}