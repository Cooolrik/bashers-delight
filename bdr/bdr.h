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
#include <mutex>

// include Vulkan and VMA 
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h> 

// ctle includes
#include <ctle/log.h>
#include <ctle/status_return.h>
#include <ctle/status.h>
#include <ctle/optional_value.h>
#include <ctle/prop.h>

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

	// property templates
	using Prop = ctle::prop;
	template<class _Ty, Prop::type _type=Prop::value> using GetProp = ctle::prop_get<_Ty, _type>;
	template<class _Ty, Prop::type _type=Prop::value> using GetSetProp = ctle::prop_get_set<_Ty, _type>;
	template<class _Ty, Prop::type _type=Prop::value> using SetProp = ctle::prop_set<_Ty, _type>;

	// fwd declares 
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
	class AllocationsBlock;
	class AllocationsBlockTemplate;
	class FramebufferPool;
	class FramebufferPoolTemplate;
	class Buffer;
	class BufferTemplate;
	class ImageMemoryBarrier;
	class BufferMemoryBarrier;
	class RenderPass;
	class RenderPassTemplate;

	// define submodule class template, which all submodules derive from
	template <class _ModuleTy> class SubmoduleTemplate
		{
		protected:
			_ModuleTy* Module;

		public:
			~SubmoduleTemplate() {};
			_ModuleTy* GetModule() { return this->Module; };
			const _ModuleTy* GetModule() const { return this->Module; };

		protected:
			SubmoduleTemplate( _ModuleTy* _module ) : Module(_module) {};
		};

	// define submodules of the main renderer and extensions
	using InstanceSubmodule = SubmoduleTemplate<Instance>;
	using DeviceSubmodule = SubmoduleTemplate<Device>;
	using DescriptorIndexingSubmodule = SubmoduleTemplate<DescriptorIndexingExtension>;
	using BufferDeviceAddressSubmodule = SubmoduleTemplate<BufferDeviceAddressExtension>;
	using RayTracingSubmodule = SubmoduleTemplate<RayTracingExtension>;

	// a map of Submodules, used to allocate, and keep allocations grouped
	template <class _ModuleTy , class _SubmoduleTy> class AllocationMapTemplate : public SubmoduleTemplate<_ModuleTy>
		{
		private:
			unordered_map<_SubmoduleTy*,unique_ptr<_SubmoduleTy>> moduleMap;

		public:
			AllocationMapTemplate( _ModuleTy *_module ) : SubmoduleTemplate<_ModuleTy>(_module) {}
			~AllocationMapTemplate() {}

			// Create an object of the Submodule type. Calls the setup method of the object and checks for errors before inserting into map.
			// On success, return a copy of the pointer to the user. Note that the submodule object is owned by the map, and
			// should not be deleted manually.
			template<class _SubmoduleTemplateTy> status_return<_SubmoduleTy*> Create( const _SubmoduleTemplateTy& parameters )
				{
				auto result = this->GetModule()->CreateObject<_SubmoduleTy,_SubmoduleTemplateTy>( parameters );
				if( !result.status() )
					return result.status();
				auto pSubmodule = result.value().get();
				this->moduleMap.insert( { pSubmodule , std::move(result.value()) } );
				return pSubmodule;
				}

			// Destroys the specific Submodule object, and removes it from the map.
			// Note that this is not a necessary step for clean up purposes, the map deletes
			// the objects automatically.
			status Destroy( _SubmoduleTy *pSubmodule )
				{
				auto it = this->moduleMap.find( pSubmodule );
				if( it == this->moduleMap.end() )
					return status_code::invalid_param; 

				// explicitly clean up object
				status result = it->second->Cleanup();
				if( !result )
					return result;

				// remove from map
				this->moduleMap.erase( it );
				return status_code::ok;
				}

			// Clears all objects owned by the Submodule map explicitly by calling the
			// Cleanup method. Note that the cleanup will stop if one of the objects
			// returns an error.
			status Cleanup()
				{
				// explicitly Clean all objects and check status return 
				auto it = this->moduleMap.begin();
				while( it != this->moduleMap.end() )
					{
					// explicitly clean up object
					status result = it->second->Cleanup();
					if( !result )
						return result;

					// remove from map
					it = this->moduleMap.erase( it );
					}

				return status_code::ok;
				}
		};

	// alias SubmoduleMaps for the main renderer and extensions
	template<class _SubmoduleTy> using DeviceAllocationMap = AllocationMapTemplate<Device,_SubmoduleTy>;
	template<class _SubmoduleTy> using DescriptorIndexingAllocationMap = AllocationMapTemplate<DescriptorIndexingExtension,_SubmoduleTy>;
	template<class _SubmoduleTy> using BufferDeviceAddressAllocationMap = AllocationMapTemplate<BufferDeviceAddressExtension,_SubmoduleTy>;
	template<class _SubmoduleTy> using RayTracingAllocationMap = AllocationMapTemplate<RayTracingExtension,_SubmoduleTy>;

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


