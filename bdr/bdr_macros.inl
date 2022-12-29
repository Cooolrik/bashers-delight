
// ----------------------------------------------------------------------------------
//
// Temporary macro definitions
// 
// Macros which are defined between includes of the file:
// first include - define the macros
// second include - remove all defined macros
// This is so the macros can be used in header files
//
#ifndef BDR_MACROS_DEFINED
#define BDR_MACROS_DEFINED

#define BDRGetMacro( type , name ) \
	const type Get##name() const { return this->name; }

#define BDRGetCustomNameMacro( type , publicname , privatename ) \
	const type Get##publicname() const { return this->privatename; }

#define BDRGetConstRefMacro( type , name ) const type& Get##name() \
	const { return this->name; }

#define BDRSubmoduleMacro( _classname , _parentclass , _mainmodule )\
	public:\
		~_classname();\
	protected:\
		_classname( const _mainmodule* _module ) : _parentclass( _module ) {};\
		_classname( const _classname& other );\
		friend class _mainmodule;\
	private:

#define BDRSubmoduleBaseMacro( _classname , _mainmodule )\
	class _mainmodule;\
	class _classname\
		{\
		protected:\
			const _mainmodule* Module = nullptr;\
		public:\
			~_classname() {};\
			const _mainmodule* GetModule() const { return this->Module; };\
		protected:\
			_classname( const _mainmodule* _module ) { this->Module = _module; };\
			_classname( const _classname& other );\
			friend class _mainmodule;\
		};

#define BDRExtensionSubmoduleMacro( _classname , _parentclass , _exmodule )\
	protected:\
		const _exmodule* ExModule = nullptr;\
	public:\
		~_classname();\
		const _exmodule* GetExModule() const { return this->ExModule; };\
	protected:\
		_classname( const _exmodule* _exmodule ) : _parentclass( _exmodule->GetModule() ) {};\
		_classname( const _classname& other );\
		friend class _exmodule;\
	private:

#define BDRLog( msg_level )\
	if( ctle::log_level::##msg_level <= get_global_log_level() ) {\
		log_msg _ctle_log_entry(ctle::log_level::##msg_level,__FILE__,__LINE__,__FUNCSIG__); _ctle_log_entry.message()

#define BDRLogError BDRLog( error )
#define BDRLogWarning BDRLog( warning )
#define BDRLogInfo BDRLog( info )
#define BDRLogDebug BDRLog( debug )
#define BDRLogVerbose BDRLog( verbose )

#define BDRLogEnd ""; }

#else//BDR_MACROS_DEFINED

#ifndef BDR_MACROS_END_DEFINED
#error "Mismatch of bdr_macros.inl and bdr_macros_end.inl file includes. Each include of bdr_macros.inl must be matched with an include of bdr_macros_end.inl when macros are no longer used."
#endif

#undef BDR_MACROS_END_DEFINED
#undef BDR_MACROS_DEFINED

#undef BDRGetMacro
#undef BDRGetCustomNameMacro
#undef BDRGetConstRefMacro
#undef BDRSubmoduleMacro
#undef BDRSubmoduleBaseMacro
#undef BDRExtensionSubmoduleMacro

#undef BDRLog
#undef BDRLogError
#undef BDRLogWarning
#undef BDRLogInfo
#undef BDRLogDebug 
#undef BDRLogVerbose
#undef BDRLogEnd 

#endif//BDR_MACROS_DEFINED

// ----------------------------------------------------------------------------------
//
// Source-file macro definitions
// 
// These are macros which are only defined when compiling BDR source files
// and are also only defined once, regardless of how many times the 
// file is included. All source files need to have the BDR_SOURCE_FILE definition
// to mark the file as part of the library source files
//
#if defined(BDR_SOURCE_FILE) && !defined(BDR_SOURCE_MACROS_DEFINED)
#define BDR_SOURCE_MACROS_DEFINED

// makes sure the return value is VK_SUCCESS or throws an exception
#define VLK_CALL( s ) { VkResult VLK_CALL_res = s; if( Failed(VLK_CALL_res) ) { return VLK_CALL_res; } }

#define VR_GET_EXTENSION_FUNCTION_ADDRESS( func ) func = (PFN_##func)vkGetInstanceProcAddr( this->GetModule()->GetInstance(), #func ); if( !func ) { throw std::runtime_error( "vkGetInstanceProcAddr failed, could not retreive address for " #func ); }

#define VR_ADD_STRUCT_TO_VULKAN_LINKED_LIST( plist , strct , stype )\
	strct = {};\
	strct.sType = stype;\
	strct.pNext = (void*)plist->pNext;\
	plist->pNext = &strct;

#endif//defined(BDR_SOURCE_FILE) && !defined(BDR_SOURCE_MACROS_DEFINED)