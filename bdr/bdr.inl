// Bashers Delight Renderer, Copyright (c) 2023 Ulrik Lindahl
// Licensed under the MIT license https://github.com/Cooolrik/bashers-delight/blob/main/LICENSE
#ifndef BDR_SOURCE_FILE
#define BDR_SOURCE_FILE

#include "bdr.h"

#include <stdio.h>
#include <string.h>

#include <ctle/string_funcs.h>

// Source-file additional definitions. 
// These are macros and functions which are only available in BDR source files

// General function signature macro
#if defined(_MSC_VER)
#define BDR_FUNCTION_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__)
#define BDR_FUNCTION_SIGNATURE __PRETTY_FUNCTION__ 
#endif

// STRINGIZE converts a number macro (like __LINE__) into a string. 
// The STRINGIZE_DETAIL is needed because of how macros work in the preprocessor
// Use e.g. STRINGIZE(__LINE__) to use __LINE__ as a string macro
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

// define logging macros
#define LogLevel( msg_level )\
	if( ctle::log_level::msg_level <= ctle::get_global_log_level() ) {\
		ctle::log_msg _ctle_log_entry(ctle::log_level::msg_level,__FILE__,__LINE__,BDR_FUNCTION_SIGNATURE); _ctle_log_entry.message()
#define LogError LogLevel( error )
#define LogWarning LogLevel( warning )
#define LogInfo LogLevel( info )
#define LogDebug LogLevel( debug )
#define LogVerbose LogLevel( verbose )
#define LogEnd ""; }

// logs the current this pointer to the log, for tracing
#define LogThis LogInfo << this << LogEnd;

// checks an expression, and logs an error and returns if the statement is not true
#define Validate( statement , error_code_on_error )\
	if( !(statement) ) {\
		const status _error_code = error_code_on_error;\
		LogError
#define ValidateEnd\
		LogEnd;\
	return _error_code; }

// in debug mode, checks expressions which are assumed to be true. if not, throws a runtime error
#ifndef NDEBUG
#define SanityCheck( statement )\
	if( !(statement) ) {\
		LogError << "SanityCheck failed: " << std::string(#statement) << LogEnd;\
		throw std::runtime_error( "SanityCheck " #statement " failed in " __FILE__ " line " STRINGIZE(__LINE__) " function " STRINGIZE(BDR_FUNCTION_SIGNATURE) );\
	}
#else
#define SanityCheck( statement )
#endif

// calls a BDR function which returns a status value, checks that value and reports/returns the value if it is an error
#define CheckCall( s ) {\
	status BDRCall_status = s; \
	if( !BDRCall_status ) {\
		LogError << "Call: " << #s << " failed, returned status_code: " << BDRCall_status << LogEnd;\
		return BDRCall_status;\
		}\
	}
#define CheckRetValCall( retval , scall )\
	decltype( scall )::value_type retval = {};\
	{\
	auto BDRCall_statuspair = scall; \
	if( !BDRCall_statuspair.status() ) {\
		LogError << "Call: " << #scall << " failed, returned status_code: " << BDRCall_statuspair.status() << LogEnd;\
		return BDRCall_statuspair.status();\
		}\
	retval = std::move(BDRCall_statuspair.value());\
	}

// convenience macro which calls vkGetInstanceProcAddr to retreive a specific function pointer address, and 
// places the resulting address in a function pointer which is named as the function.
// the macro assumes it is used within a method which returns a status value, and returns an error if the call fails.
#define GetVulkanInstanceProcAddr( func ) \
	func = (PFN_##func)vkGetInstanceProcAddr( this->GetModule()->GetInstanceHandle(), #func );\
	if( !func ) {\
		LogError << "vkGetInstanceProcAddr failed, could not retreive address for proc: " << #func << LogEnd;\
		return status_code::vulkan_initialization_failed;\
		}

// convenience function which sets up a struct and adds it to a list of Vulkan structures
template <class _ListTy , class _Ty> inline void InitializeLinkedVulkanStructure( _ListTy *_plist , _Ty &_strct , VkStructureType _stype ) 
	{
	_strct = {};
	_strct.sType = _stype;
	_strct.pNext = (void*)(_plist->pNext);
	_plist->pNext = &_strct;
	}

// convenience macro to destroy a specific object only if it is allocated, and also reset the handle object
#define SafeVkDestroy( handle , destroy_call )\
	if( handle != VK_NULL_HANDLE ) {\
		destroy_call;\
		handle = VK_NULL_HANDLE;\
		}

namespace bdr
	{
	inline int stricmp( const char *s1, const char *s2 )
		{
#if defined(_MSC_VER)
		return _stricmp( s1 , s2 );
#elif defined(__GNUC__)
		return strcasecmp( s1 , s2 );
#endif
		}
	}


#endif//BDR_SOURCE_FILE