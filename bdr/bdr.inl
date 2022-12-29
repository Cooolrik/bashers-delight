#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <string>
#include "bdr_Renderer.h"

// makes sure the return value is VK_SUCCESS or throws an exception
#define VLK_CALL( s ) { VkResult VLK_CALL_res = s; if( VLK_CALL_res != VK_SUCCESS ) { throw_vulkan_error( VLK_CALL_res , "Vulkan call " #s " failed (did not return VK_SUCCESS)"); } }

// makes a new unique_ptr object with this class as owner
#define BD_NEW_UNIQUE_PTR( _class ) std::unique_ptr<_class>( new _class(this) )

using bdr::uint;
using std::vector;
using std::runtime_error;
using std::min;
using std::max;
using std::string;
using std::unique_ptr;

#ifdef VR_LESS_STRICT_WARNINGS
#pragma warning( disable: 4100 )
#endif

// dont warn about throw_vulkan_error local functions removed, if not used in a file
#pragma warning( push )
#pragma warning( disable : 4505 )
static void throw_vulkan_error( VkResult errorvalue, const char* errorstr )
	{
	char str[20];
	sprintf_s( str, "%d", (int)errorvalue );
	throw runtime_error( errorstr );
	}
#pragma warning( pop )

#define VR_GET_EXTENSION_FUNCTION_ADDRESS( func ) func = (PFN_##func)vkGetInstanceProcAddr( this->GetModule()->GetInstance(), #func ); if( !func ) { throw std::runtime_error( "vkGetInstanceProcAddr failed, could not retreive address for " #func ); }

#define VR_ADD_STRUCT_TO_VULKAN_LINKED_LIST( plist , strct , stype )\
	strct = {};\
	strct.sType = stype;\
	strct.pNext = (void*)plist->pNext;\
	plist->pNext = &strct;
	



//#define BDSubmoduleMacro( _classname , _parentclass , _mainmodule )\
//	public:\
//		~_classname();\
//	protected:\
//		_classname( const _mainmodule* _module ) : _parentclass( _module ) {};\
//		_classname( const _classname& other );\
//		friend class _mainmodule;\
//	private:
//
//#define BDSubmoduleBaseMacro( _classname , _mainmodule )\
//	class _mainmodule;\
//	class _classname\
//		{\
//		protected:\
//			const _mainmodule* Module = nullptr;\
//		public:\
//			~_classname() {};\
//			const _mainmodule* GetModule() const { return this->Module; };\
//		protected:\
//			_classname( const _mainmodule* _module ) { this->Module = _module; };\
//			_classname( const _classname& other );\
//			friend class _mainmodule;\
//		};
//
//// used if a submodule is part of an extension but derives from one of the main classes
//#define BDExtensionSubmoduleMacro( _classname , _parentclass , _exmodule )\
//	protected:\
//		const _exmodule* ExModule = nullptr;\
//	public:\
//		~_classname();\
//		const _exmodule* GetExModule() const { return this->ExModule; };\
//	protected:\
//		_classname( const _exmodule* _exmodule ) : _parentclass( _exmodule->GetModule() ) {};\
//		_classname( const _classname& other );\
//		friend class _exmodule;\
//	private:
//
//
//


	/*typedef unsigned int uint;

	class Buffer;
	class BufferTemplate;
	class VertexBuffer;
	class VertexBufferTemplate;
	class IndexBuffer;
	class IndexBufferTemplate;
	
	class DescriptorSetLayout;
	class DescriptorSetLayoutTemplate;

	class DescriptorPool;
	class DescriptorPoolTemplate;

	class Pipeline;
	class GraphicsPipelineTemplate;
	class ComputePipelineTemplate;

	class CommandPool;
	
	class VertexBufferDescription;
	class Image;
	class ImageTemplate;
	class Texture;
	class Extension;
	class RayTracingExtension;
	class BufferDeviceAddressExtension;
	class DescriptorIndexingExtension;
	class Sampler;
	class SamplerTemplate;

	class ShaderModule;

	*/



	BDSubmoduleBaseMacro( RendererSubmodule , Renderer );
