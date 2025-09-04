//-------------------------------------------------------------------
// OWL File Extensions (OWLFX) Class Library
// OWL NExt Project
// Copyright(c) 1999 by Yura Bidus.
// All rights reserved.
//
// OWLFX's macro magic and conditional defines
//
//$Revision: 1.1.4.2 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//-------------------------------------------------------------------
#if !defined (OWLFX_DEFS_H)
#define OWLFX_DEFS_H

//-------------------------------------------------------------------
// OWLEXT can't be compiled as anything other than C++
#if !defined(__cplusplus)
# error Must compile OWLFX Class Library as C++
#endif

//-------------------------------------------------------------------
// Pick up subsystem headers
#if !defined (OWL_DEFS_H)
# include <owl/defs.h>
#endif

//-------------------------------------------------------------------
// Handle the OWLFXCLASS macro magic
#if defined (_BUILD_OWLFXDLL)
// OWLEXT being built as a DLL
# define _OWLFXCLASS	_OWLEXPORTCLASS
#	define _OWLFXFUNC		_OWLEXPORTFUNC
#	define _OWLFXCFUNC	_OWLEXPORTCFUNC
#	define _OWLFXDATA		_OWLEXPORTDATA
// We must use all libraries in DLLs if we are using OWLEXT in a DLL
# if !defined (_OWLDLL)
#  define _OWLDLL
# endif

#elif defined (_OWLFXDLL)
// OWLFX being used as a DLL
# define _OWLFXCLASS		_OWLIMPORTCLASS
# define _OWLFXFUNC 		_OWLIMPORTFUNC
# define _OWLFXCFUNC 		_OWLIMPORTCFUNC
# define _OWLFXDATA 		_OWLIMPORTDATA
#else
// OWLFX being built or used as a static library
# define _OWLFXCLASS
# define _OWLFXFUNC(p)	p
# define _OWLFXCFUNC(p) p
# define _OWLFXDATA(p)	p

#endif


#if !defined(BI_NO_NAMESPACE) || defined(BI_NAMESPACE)
#	define	_OWLFX_BEGIN_NAMESPACE namespace OwlFx {
#	define	_OWLFX_END_NAMESPACE };
#	define 	_OWLFX_USING_NAMESPACE using namespace OwlFx;
#	define 	__OWLFX	OwlFx::
# define	__OWLFX_DECLCLASS(cls) namespace OwlFx { class _OWLFXCLASS cls; }
#else
#	define	_OWLFX_BEGIN_NAMESPACE
#	define	_OWLFX_END_NAMESPACE
#	define 	_OWLFX_USING_NAMESPACE
#	define 	__OWLFX
# define __OWLFX_DECLCLASS(cls) class _OWLFXCLASS cls; 
#endif


#if defined(BI_COMP_BORLANDC)
extern __OWL TModule* Module;
#define gModule ::Module
#else
extern __OWL TModule* gModule;
#endif

#if defined(BI_COMP_BORLANDC)
extern "C" _OWLFXFUNC(__OWL TModule* WINAPI) GetOwlFxModule();
#else
STDAPI_(__OWL TModule*) GetOwlFxModule();
#endif

// include libraries
#if !defined(_BUILD_OWLFXDLL)
#include <owlfx/link.h>
#endif

#endif  // OWLFX_DEFS_H

