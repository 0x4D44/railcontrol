//----------------------------------------------------------------------------
// ObjectWindows, OWL NExt
// Copyright (c) 1998-1999 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:36 $
//
// Compiler-specific defines for GNU C/C++. Included only by
// defs.h
//
//----------------------------------------------------------------------------

#if !defined(OWL_PRIVATE_GNUC_H)
#define OWL_PRIVATE_GNUC_H

#ifdef __CYGWIN__
#error only mingw supported now
#endif

//////////////////////////
// let STL do some work
#undef min
#undef max
#include <algorithm>
using std::min;
using std::max;
#define __MINMAX_DEFINED   //avoid use definition of min max from Borland/others headers
#define BI_NO_MINMAX_TMPL  //don't need the OWL min max templates definition

#include <stddef.h>
#include <exception>

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
#if !defined BI_PLAT_WIN64 && !defined BI_PLAT_LINUX32 && !defined BI_PLAT_SOLARIS32 && !defined BI_PLAT_OSX 
#define BI_PLAT_WIN32
#endif

//check defs.h:132 for description, basicaly use gModule instead of Module
#define BI_NEW_GMODULE

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
//
#define BI_MULTI_THREAD

// GNU suport multithreading ???
#if defined(__STL_WIN32THREADS)
#define BI_MULTI_THREAD_RTL
#endif

//----------------------------------------------------------------------------
//  Compiler-specific flags.
//----------------------------------------------------------------------------
// MinGW uses __declspecs, so just turn import/export into those
// MSVC uses __declspecs, so just turn import/export into those

//JJH this stuff is defined in winelib's winnt.h also
#if !defined(__export) && !defined(__import)
#define __export    __declspec(dllexport)
#define __import    __declspec(dllimport)
#endif

//#define __export    __attribute__(( dllexport )) 
//#define __import    __attribute__(( dllimport ))

//
// Specific name of compiler, BI_COMP_xxxx
//
#define BI_COMP_GNUC
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
#if __GNUC__ <4 ||  __GNUC__ == 3 && __GNUCMINOR__ <2
#define BI_NO_COVAR_RET       // Covariant returns in overrides -
#endif
//#define BI_NO_MUTABLE       // Mutable keyword              DEFS.H
//#define BI_NO_NEW_CASTS     // New style casts              CASTABLE.H
#define BI_SUPPORT_TMPLFNC_FRIEND

#if __GNUC__ >=3
#define BI_STD_RTL
#define BI_STD_NEWSSTREAM
#define BI_STD_IO
# if !defined(__STL_USE_EXCEPTIONS)
#    define __STL_USE_EXCEPTIONS
# endif
#else
# define __export    __declspec(dllexport)
#endif

#if __GNUC__ < 3
# define BI_USE_LONG_HNAMES     // Using long names for header
#endif

// change __stl_assert -> OWL alwas GUI application
#ifdef __STL_ASSERTIONS
#undef __stl_assert
# define __stl_assert(expr) CHECK(expr)
#endif

//#if defined(__STL_USE_NAMESPACES)
//#  define BI_NO_NAMESPACE     // Namespaces
//#endif

#define BI_NO_OBJ_STREAMING    // currently
#if !defined(__STL_USE_EXCEPTIONS)
#  define BI_NO_THROWSPEC        // Function throw specifiers    EXCEPT.H
#endif

//#define BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
#define _RTTI                 // without __rtti keyword  //???????????????
//#define BI_NOTHUNK            // not used chunk

// GCC not support new STL iostreams 
#if defined(__STL_USE_NEW_IOSTREAMS)
#  define BI_STD_IO
#endif
#define BI_STD_STRING
#define BI_STD_EXCEPTIONS
#define BI_STD_RTTI


//#define BI_NOSTATIC_FUNCCALL  // static member init by function call not supported
#define NO_AFTER_RETPARAM        // __import int FuncName(...)
//#define BI_IMPEXP_TEMPLATES
//#define BI_NEED_EXPL_TPL      //
//#define BI_NO_LVAL_CASTS      // Can cast lvalues
#define BI_XBASE_DEFINED        // define if xbase exist -> must be
#define BI_XALLOC_DEFINED        // define if xalloc exist
#define XALLOC bad_alloc        // get it be defined by library
//#endif

#define XBASE     std::exception
#define XWHY_STR   what()
//#define BI_VIRTUAL_BUG            //??????????????????????????
#define BI_64BIT_NATIVE           // MinGW support __int64

//JJH just to get rid of warning in wine's basetsd.h header file
//#if defined(WINELIB)
//typedef long long __int64;
//#else
#if !defined(__int64)
#define __int64 long long 
#endif
//#endif

//#define BI_TYPECASTOPERATOR_BUG   // problem with type cast operator
#define BI_NEWARR_DEFINED   // new[] defined in header new

#define BI_NO_TMPL_ALLOCATOR     //????????????????????
//#define BI_NO_DEFAULT_TEMPLATES  //to check version compatibility. see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=57
#define BI_OWL_NO_DEBUG_CRT
#define BI_COMMCTRL_OLDNAMES     // old names in commctrl.h  
//#define BI_NOINCLUDE_NS_OWL      //??????????????????????
#if !defined(OWL_NATIVECTRL_ALWAYS)
#define OWL_NATIVECTRL_ALWAYS     // no WIN16
#endif
//
// Aligning for OWL
//
#if !defined BI_OWL_PACKING
#define BI_OWL_PACKING 4 // for ??? default packing 
#endif


// reference
#define THROW_BAD_CAST  bad_cast()
#define THROW_TYPE_ID    bad_typeid()

//#define seek_dir  seekdir   //conflicts with ios_base.h
#ifdef offsetof
#define _OFFSETOF offsetof
#endif 

#if defined(__MINGW32__)
#  include <owl/private/mingw32.h>
#elif defined(__CYGWIN__)
#  include <owl/private/cygwin.h>
#error structured exception handling not yet supported
#elif defined(WINELIB) //JJH
#  include <owl/private/winelib.h>
#else
#error Unknown GNU Compiler
#endif

#ifndef BI_SUPPRESS_OLE
#  define INC_OLE2
#else
# define BI_HAS_WCHAR // platform has wide char functions available
#endif

#define OwlOutputDebugString(lpsz) ::OutputDebugString(lpsz) 

//need again to undef min/max (check later private/winelib.h->winsock2.h use)
#undef min
#undef max


#if !defined(_MAX_PATH)
#define _MAX_PATH MAX_PATH
#endif

//C runtime equivalents 
#define _stricmp strcasecmp 
#define _strnicmp strncasecmp

//both _fullpath and _wfullpath are in the msvcrt implementation of Winelib (stdlib.h), 
//but we prefer a to use only functions of gnu lib
#define _fullpath myfullpath  //used in file.cpp

char *myfullpath(char *,const char *,int );


#endif  // OWL_PRIVATE_GNUC_H
