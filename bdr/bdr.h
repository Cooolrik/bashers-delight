// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#pragma once

// make sure min and max are not defined as macros (in e.g. windows.h)
#if defined(min) || defined(max)
#error "min and max macros cannot be defined before including bdr.h, use NOMINMAX or undef before including bdr"
#endif

// disable warnings caused by external headers 
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 26812 ) // Disable warning for "enum class"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="
#endif

#include <stdio.h>
#include <string.h>

// standard library headers
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <stdexcept>

// include Vulkan and VMA 
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h> 

// ctle includes
#include <ctle/log.h>
#include <ctle/status_return.h>
#include <ctle/status.h>
#include <ctle/optional_value.h>

// re-enable warnings 
#if defined(_MSC_VER)
#pragma warning( pop )
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace bdr
{
	using uint = std::uint32_t;
	using std::vector;
	using std::unordered_set;
	using std::unordered_map;
	using std::string;
	using std::unique_ptr;
	using std::min;
	using std::max;
	using std::tuple;
	using ctle::status_code;
	using ctle::optional_value;

	// status and status paired with another return value
	using status = ctle::status;
	template<class _Ty> using status_return = ctle::status_return<ctle::status,_Ty>;

	// list all classes 
	class Instance;
	class InstanceTemplate;
	class Device;
	class DeviceTemplate;
	class Extension;
	class DescriptorIndexingExtension;
	class BufferDeviceAddressExtension;
	class RayTracingExtension;
	class Swapchain;
	class SwapchainTemplate;
	class Image;
	class ImageTemplate;
	class CommandPool;
	class CommandPoolTemplate;
	class CommandBuffer;
    class RayTracingShaderBindingTable;
    class Pipeline;
    class VertexBuffer;
    class IndexBuffer;

	// define submodule class template, which all submodules derive from
	template <class _ModuleTy> class SubmoduleTemplate
		{
		protected:
			const _ModuleTy* Module;

		public:
			~SubmoduleTemplate() {};
			const _ModuleTy* GetModule() const { return this->Module; };

		protected:
			SubmoduleTemplate( const _ModuleTy* _module ) : Module(_module) {};
		};

	// define submodules of the main renderer and extensions
	using MainSubmodule = SubmoduleTemplate<Instance>;
	using DescriptorIndexingSubmodule = SubmoduleTemplate<DescriptorIndexingExtension>;
	using BufferDeviceAddressSubmodule = SubmoduleTemplate<BufferDeviceAddressExtension>;
	using RayTracingSubmodule = SubmoduleTemplate<RayTracingExtension>;

	// template method which explicitly cleans up a unique_ptr of a bdr object which is handed to it
	template<typename _Ty>
	inline status Release( unique_ptr<_Ty> &ptr )
		{
		status stat;
		if( ptr )
			{
			stat = ptr->Cleanup();
			if( !stat )
				return stat;
			ptr.reset();
			}
		return status_code::ok;
		};

	// template which clamps a value 
	template<typename _Ty>
	inline const _Ty &clamp( const _Ty &value , const _Ty &minvalue , const _Ty &maxvalue )
		{
		return std::min( std::max( value , minvalue ) , maxvalue );
		}
}


