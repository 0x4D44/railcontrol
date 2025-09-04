//----------------------------------------------------------------------------
// ObjectWindows, OWL NExt
// Copyright (c) 1998-1999 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.22 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:36 $
//
// Compiler-specific defines for Watcom C/C++. Included only by
// defs.h
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_WATCOM_H)
#define OWL_PRIVATE_WATCOM_H

#pragma read_only_file
#pragma once

//////////////////////////
// let STL do some work
//#include <stl_config.h>
#include <stddef.h>
#include <exception>

//#undef min
//#undef max
//#include <algorithm>
//using std::min;
//using std::max;
//#define __MINMAX_DEFINED
//#define BI_NO_MINMAX_TMPL

//----------------------------------------------------------------------------
//  OS-specific flags defined here for DOS, Win16, Win32 & OS/2
//----------------------------------------------------------------------------

//
//  Define a platform flag.
//
//      BI_PLAT_MSW         Microsoft Windows, all versions including console
//
#define BI_PLAT_MSW

#if !defined(__FLAT__)
#  define __FLAT__
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
#  define _CALLCNVN //WINAPI   default//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

#if !defined(_CCONV)
#  define _CCONV __cdecl
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
#define BI_MULTI_THREAD
#if defined(__MT__) || defined(_MT)
# define BI_MULTI_THREAD_RTL
#endif

//----------------------------------------------------------------------------
//  Compiler-specific flags.
//----------------------------------------------------------------------------
// WATCOM uses __declspecs, so just turn import/export into those
// MSVC uses __declspecs, so just turn import/export into those
#define __export    __declspec(dllexport)
#define __import    __declspec(dllimport)

//
// Specific name of compiler, BI_COMP_xxxx
//
#define BI_COMP_WATCOM

#define _RTLENTRY
#define _USERENTRY
#define _FAR
#define _FARFUNC
// uses Microsoft RTL under Windows 
#define _EXPCLASS __import
#define _EXPDATA(type)  type __import
#define _EXPFUNC(type)  type __import


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
//#define BI_NO_ARRAYNEW      // no global operators new[] & delete[]   -
//#define BI_NO_BOOL          // Standard bool type           DEFS.H
//#define BI_NO_COVAR_RET       // Covariant returns in overrides -
//#define BI_NO_MUTABLE       // Mutable keyword              DEFS.H
//#define BI_NO_NEW_CASTS     // New style casts              CASTABLE.H
//#define BI_SUPPORT_TMPLFNC_FRIEND

//#define BI_USE_LONG_HNAMES     // Using long names for header

//#define BI_NO_NAMESPACE     // NO Namespaces - bad support ?????????
#define BI_NO_OBJ_STREAMING    // currently
//#if !defined(__STL_USE_EXCEPTIONS)
//#  define BI_NO_THROWSPEC        // Function throw specifiers    EXCEPT.H
//#endif

//#define BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
#define _RTTI                 // without __rtti keyword  //???????????????
//#define BI_NOTHUNK            // not used chunk

// WATCPM support new STL iostreams 
#if !defined(_STLP_NO_OWN_IOSTREAMS)
# define BI_STD_IO
#endif
#define BI_STD_STRING
#define BI_STD_EXCEPTIONS
#define BI_STD_RTTI

#define BI_HAS_PRAGMA_ONCE


//#define BI_NOSTATIC_FUNCCALL  // static member init by function call not supported
#define NO_AFTER_RETPARAM        // __import int FuncName(...)
//#define BI_IMPEXP_TEMPLATES
//#define BI_NEED_EXPL_TPL      //
//#define BI_NO_LVAL_CASTS      // Can cast lvalues
#define BI_XBASE_DEFINED        // define if xbase exist -> must be
#define BI_XALLOC_DEFINED        // define if xalloc exist
#define XALLOC bad_alloc        // get it be defined by library
//#endif

#define XBASE     exception
#define XWHY_STR   what()
//#define BI_VIRTUAL_BUG            //??????????????????????????
#define BI_64BIT_NATIVE           // MinGW support __int64

//#define BI_TYPECASTOPERATOR_BUG   // problem with type cast operator
#define BI_NEWARR_DEFINED   // new[] defined in header new

#define BI_NO_TMPL_ALLOCATOR     //????????????????????
#define BI_NO_DEFAULT_TEMPLATES  //??????????????????
#define BI_OWL_NO_DEBUG_CRT
#define BI_COMMCTRL_OLDNAMES     // old names in commctrl.h  
//#define BI_NOINCLUDE_NS_OWL      //??????????????????????
#define __MINMAX_DEFINED

//
// Aigning for OWL
#define BI_OWL_PACKING 8 // for ??? default packing 
#define BI_HAS_WCHAR 


// reference
#define THROW_BAD_CAST  bad_cast()
#define THROW_TYPE_ID    bad_typeid()


#define OwlOutputDebugString(lpsz) ::OutputDebugString(lpsz) 


//#ifndef ALL_WARNINGS
#pragma warning 583 4  // typedef redefinition ignored
#pragma warning 7   4  // &array may not produce intended results
#pragma warning 13  4  // unreachable code
//#pragma warning 14  4  // no reference to symbol
#pragma warning 367 4  // conditional in if always true
#pragma warning 368 4  // conditional in if always false
#pragma warning 389 4  // integral value may be truncated
#pragma warning 442 4  // repeated modifier
#pragma warning 549 4  // sizeof contains compiler generated info
#pragma warning 555 4  // while always false
#pragma warning 556 4  // for always false
#pragma warning 583 4  // typedef redefine ignored
#pragma warning 594 4  // ambiguity resolution declaration
#pragma warning 595 4  // ambiguity resolution expression
#pragma warning 604 4  // amgibuity
//#endif


#endif  // OWL_PRIVATE_GNUC_H
