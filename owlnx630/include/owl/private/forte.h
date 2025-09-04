//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//
// Compiler-specific defines for Solaris with MainWin Windows
// porting library (using Forte compiler v6, AKA SunWorkshop compiler). 
// Included only by defs.h
//
// Compiler-specific defines for forte. Included only by
// defs.h. This was used only in combination with MainWin porting library.
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_FORTE_H)
#define OWL_PRIVATE_FORTE_H

#define BI_OWL_NO_DEBUG_CRT

#define NOMINMAX
#ifndef _OWLPCH
#define _OWLPCH
#endif

//STL file <algorithm> also defines these templates, so
// make sure it hasn't already defined them by checking for its
// sentinal macro . Can't just include <algorithm> because of
// strange default template parameter problem

#if !defined(__MINMAX_DEFINED) && !defined(__STD_ALGORITHM)

#undef min
#undef max
#define __MINMAX_DEFINED
#define BI_NO_MINMAX_TMPL

namespace std
{
  template <class T> inline const T& min( const T& t1, const T& t2 )
  {
	return t1 < t2 ? t1 : t2;
  }
  template <class T> inline const T& max( const T& t1, const T& t2 )
  {
	return t1 > t2 ? t1 : t2;
  }
} //end additions to std namespace

using std::min;
using std::max;


#endif // if not __MINMAX_DEFINED

//----------------------------------------------------------------------------
//  OS-specific flags defined here for DOS, Win16, Win32 & OS/2
//----------------------------------------------------------------------------

//
//  Define a platform flag.
//
//      BI_PLAT_MSW         Microsoft Windows, all versions including console
//
# define BI_PLAT_MSW

#if !defined(__FLAT__)
# define __FLAT__
#endif

//
//  Specify the Windows API if we're compiling for Windows. Must
//  define one of:
//
//      BI_PLAT_WIN16       Microsoft Windows 16
//      BI_PLAT_WIN32       Microsoft Win32
//
#define BI_PLAT_WIN32

// Define library-wide function calling conventions. Must rebuild everything
// if changed, and must use appropriate modifier on derived classes.
//
#if !defined(_CALLCNVN)
//WINAPI
# define _CALLCNVN
#endif

#if !defined(_CCONV)
# define _CCONV __cdecl
#endif


//
//  Define the pointer size. Must define one of:
//
//      BI_PTR_16_16        Intel 16:16 pointer
//      BI_PTR_0_32         32-bit flat pointer (including Intel 0:32)
//
#define BI_PTR_0_32

//
//  Define the type of application being built. Must be one of:
//
//      BI_APP_EXECUTABLE
//      BI_APP_DLL
//
#if defined(__DLL__) || defined(_DLL)
# define BI_APP_DLL
#else
# define BI_APP_EXECUTABLE
#endif


//
//  Only enable multi-thread classes when the OS supports them, and/or when the
//  runtime library support them
//
#define BI_MULTI_THREAD

// if RTLDLL we must been multitreaded
#if defined(_MT) || defined(BI_APP_DLL)
# define BI_MULTI_THREAD_RTL
#endif

#define NO_STREAMABLE

#define _WCHAR_T_DEFINED

//----------------------------------------------------------------------------
//  Compiler-specific flags.
//----------------------------------------------------------------------------
// MSVC uses __declspecs, so just turn import/export into those
#ifndef __export
#define __export    __declspec(dllexport)
#endif

#ifndef __import
#define __import    __declspec(dllimport)
#endif

//
// Specific name of compiler, BI_COMP_xxxx
//
#define BI_COMP_FORTE
#define BI_COMP_MSC
#define _RTLENTRY
#define _USERENTRY
#define _FAR
#define _FARFUNC
#define _EXPCLASS _CRTIMP
//#define _EXPDATA(type)  _CRTIMP type
#define _EXPDATA _CRTIMP 
//#define _EXPFUNC(type)  _CRTIMP type
#define _EXPFUNC _CRTIMP 


#ifdef _MBCS
# if !defined(BI_DBCS_SUPPORT)
#		define BI_DBCS_SUPPORT
#endif
#endif //_MBCS

# if defined(BI_DBCS_SUPPORT)
# if !defined(_MBCS)
#		define _MBCS
#endif
#endif


//
//  C++ language features
//
//  When compiling with a compiler that doesn't support these
//  features directly, remove the // from the appropriate
//  #define below or define the macro in the build environment.
//

//
//  Language feature:          Description                  SERVICES header
//  ~~~~~~~~~~~~~~~~           ~~~~~~~~~~~~~~~~~~~~~~~~~~   ~~~~~~~~~~~~~~~
//#define BI_STD_SPECIALIZATION //supports template specialization using template<>
#define BI_GLOBALNM_BUG     // Global namespace bug in function member int mfunc(int name = ::someName);
#define BI_NOTEMPLATE_H     // No include <owl/template.h>
//#define BI_NO_ARRAYNEW      // no global operators new[] & delete[]   -
//#define BI_NO_BOOL          // Standard bool type           DEFS.H
#define BI_NO_COVAR_RET     // Covariant returns in overrides -
//#define BI_NO_MUTABLE       // Mutable keyword              DEFS.H

//#define BI_NO_NEW_CASTS     // New style casts              CASTABLE.H

#define BI_NO_NAMESPACE				// Namespaces
#define BI_NO_OBJ_STREAMING		// currently ???
#define BI_NO_THROWSPEC				// Function throw specifiers    EXCEPT.H

//#define BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
#define _RTTI                 // without __rtti keyword
#define BI_NOTHUNK            // not used chunk

#	define BI_STD_STRING

//#	define BI_STD_RTL
#	define BI_STD_EXCEPTIONS
#define BI_STD_IO
#define NEW_STD_STREAMS // emf, for forte WS6U2 non STLport
#define NO_XIOSBASE // StlPort and the new sun compiler don't have it

#define BI_NOSTATIC_FUNCCALL  // static member init by function call has problem, not used 
#define NO_AFTER_RETPARAM			// 
//#define BI_IMPEXP_TEMPLATES
//#define BI_NEED_EXPL_TPL			//?? not needed and not used??
#define BI_NO_LVAL_CASTS			// Can cast lvalues
#define BI_XBASE_DEFINED			// define if xbase exist -> must be
#if defined(BI_STD_EXCEPTIONS)
#define BI_XALLOC_DEFINED			// define if xalloc exist
#define XALLOC std::bad_alloc			// get it be defined by library
#endif
#define XBASE 		exception
#define XWHY_STR 	what()
#define BI_VIRTUAL_BUG            //problem when derived class add virtual table
#define BI_64BIT_NATIVE           // MSC support __int64
#define BI_TYPECASTOPERATOR_BUG   // problem with type cast operator
//#define BI_NEW_GMODULE           

//
// Disable some MSC warnings, & move some to level 4 since they really are OK
//

#include <typeinfo.h>

// reference
#define THROW_BAD_CAST	bad_cast("OWL_badcast")
#define THROW_TYPE_ID		bad_typeid("OWL_type_id")

//#define itoa     _itoa
//#define ltoa     _ltoa

#ifndef strcmpi
#define strcmpi  lstrcmpi
#endif

#ifndef stricmp
#define stricmp  lstrcmpi
#endif

#define tzset    _tzset
#define strnicmp _strnicmp
#define chsize 	 _chsize

#define BI_OWL_PACKING	4		// for VC default packing == 4

#define BI_NO_CHECK_ARRAY

//DLN don't bring in owl libraries all the time#if !defined(_BUILDOWLDLL)
//DLN don't bring in owl libraries all the time#	include <owl/private/owllink.h>
//DLN don't bring in owl libraries all the time#endif

#if defined(_DEBUG)
#	ifndef BI_OWL_NO_DEBUG_CRT
#		ifndef _INC_CRTDBG
#			include <crtdbg.h>
#		endif
#	endif // BI_OWL_NO_DEBUG_CRT
#endif // _DEBUG

#define OwlOutputDebugString(lpsz) ::OutputDebugString(lpsz) 



#endif  // OWL_PRIVATE_FORTE_H


