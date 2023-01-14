// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#ifndef BDR_SOURCE_FILE
#define BDR_SOURCE_FILE

#include "bdr.h"

// Source-file additional definitions. 
// These are macros and functions which are only available in BDR source files

// define logging macros
#define LogLevel( msg_level )\
	if( ctle::log_level::##msg_level <= ctle::get_global_log_level() ) {\
		ctle::log_msg _ctle_log_entry(ctle::log_level::##msg_level,__FILE__,__LINE__,__FUNCTION__); _ctle_log_entry.message()
#define LogError LogLevel( error )
#define LogWarning LogLevel( warning )
#define LogInfo LogLevel( info )
#define LogDebug LogLevel( debug )
#define LogVerbose LogLevel( verbose )
#define LogEnd ""; }

// calls a BDR function which returns a Status value, checks that value and reports/returns the value if it is an error
#define CheckCall( s ) {\
	Status BDRCall_status = s; \
	if( !BDRCall_status ) {\
		LogError << "Call: " << #s << " failed, returned status_code: " << BDRCall_status << LogEnd;\
		return BDRCall_status;\
		}\
	}

// convenience macro which calls vkGetInstanceProcAddr to retreive a specific function pointer address, and 
// places the resulting address in a function pointer which is named as the function.
// the macro assumes it is used within a method which returns a Status value, and returns an error if the call fails.
#define GetVulkanInstanceProcAddr( func ) \
	func = (PFN_##func)vkGetInstanceProcAddr( this->GetModule()->GetInstance(), #func );\
	if( !func ) {\
		LogError << "vkGetInstanceProcAddr failed, could not retreive address for proc: " << #func << LogEnd;\
		return status_code::vulkan_initialization_failed;\
		}

// convenience function which sets up a struct and adds it to a list of Vulkan structures
template <class _ListTy , class _Ty> void InitializeLinkedVulkanStructure( _ListTy *_plist , _Ty &_strct , VkStructureType _stype ) 
	{
	_strct = {};
	_strct.sType = _stype;
	_strct.pNext = (void*)(_plist->pNext);
	_plist->pNext = &_strct;
	}

#endif//BDR_SOURCE_FILE