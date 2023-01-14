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
#endif

// standard library headers
#include <vector>
#include <string>
#include <set>
#include <memory>
#include <algorithm>

// include Vulkan and VMA 
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h> 

// ctle includes
#include <ctle/log.h>
#include <ctle/status_return.h>
#include <ctle/status.h>

// re-enable warnings 
#if defined(_MSC_VER)
#pragma warning( pop )
#elif defined(__GNUC__)
#endif

namespace bdr
{
	using uint = std::uint32_t;
	using std::vector;
	using std::set;
	using std::string;
	using std::unique_ptr;
	using std::min;
	using std::max;
	using std::tuple;
	using ctle::status_code;

	// Status and Status paired with another return value
	using Status = ctle::status;
	template<class _Ty> using StatusPair = ctle::status_return<ctle::status,_Ty>;

	// list all classes 
	class Instance;
	class Device;
	class Extension;
	class DescriptorIndexingExtension;
	class BufferDeviceAddressExtension;
	class RayTracingExtension;

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
}

