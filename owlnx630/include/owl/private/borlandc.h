//----------------------------------------------------------------------------
// Object Windows Library
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.35.2.5 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:14 $
//
// Compiler-specific defines for Borland C/C++. Included only by compiler.h
//
// __BORLANDC__ versions:
//
//		0x500 - Borland C++ 5.00 - oldest supported version
//		0x520 - Borland C++ 5.02
//		0x530 - Borland C++ Builder 3.0
//		0x540 - Borland C++ Builder 4.0
//		0x550 - Borland C++ Builder 5.0
//		0x560 - Borland C++ Builder 6.0
//		0x570 - Borland C++ BuilderX (untested)
//		0x580 - Borland C++ Developer Studio 2006
//		0x590 - CodeGear RAD Studio 2007
//		0x610 - CodeGear RAD Studio 2009
//		0x620 - CodeGear RAD Studio 2010
//		0x630 - Embarcadero RAD Studio XE
//
//
// Revised by Yura Bidus
//----------------------------------------------------------------------------
#if !defined(OWL_PRIVATE_BORLANDC_H)
#define OWL_PRIVATE_BORLANDC_H

//----------------------------------------------------------------------------
//  OS-specific flags defined here for DOS, Win16, Win32 & OS/2
//----------------------------------------------------------------------------

//
//  Define a platform flag.
//
//      BI_PLAT_MSW         Microsoft Windows, all versions including console
//
#if defined(_Windows) || defined(_WINDOWS) || defined(_WIN32)
# define BI_PLAT_MSW
#elif defined(__OS2__)
# error OS/2 is no longer supported
#else
# error unsupported platform
#endif

//
//  Specify the Windows API if we're compiling for Windows. Must
//  define one of:
//
//      BI_PLAT_WIN32       Microsoft Win32
//
#if defined(BI_PLAT_MSW)
# if defined(_WIN32) || defined(__FLAT__)
#   define BI_PLAT_WIN32
# else
#   error 16-bit is no longer supported
# endif
#endif

//
//  Define the pointer size. Must define one of:
//
//      BI_PTR_16_16        Intel 16:16 pointer
//      BI_PTR_0_32         32-bit flat pointer (including Intel 0:32)
//
#if defined(__FLAT__) || defined(BI_PLAT_WIN32) 
# define BI_PTR_0_32
#endif


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
#if defined(BI_PLAT_WIN32)
# define BI_MULTI_THREAD
#endif
#if defined(__MT__)
# define BI_MULTI_THREAD_RTL
#endif

//----------------------------------------------------------------------------
//  Compiler-specific flags.
//----------------------------------------------------------------------------

//
// Specific name of compiler, BI_COMP_xxxx
//
#define BI_COMP_BORLANDC

//
//  C++ language features
//
//  When compiling with a compiler that doesn't support these
//  features directly, remove the // from the appropriate
//  #define below or define the macro in the build environment.
//

//
#define BI_XBASE_DEFINED
#define BI_XALLOC_DEFINED

// declarations template fuctions as friend of class
#define BI_SUPPORT_TMPLFNC_FRIEND

// No debug RTL available
#define BI_OWL_NO_DEBUG_CRT 

#if (__BORLANDC__ < 0x500)
# error Unsupported Borland C++ compiler version 
#endif


#if (__BORLANDC__ < 0x520)
# define BI_NO_CHECK_ARRAY
#endif

// Force BC5.0x to use WINVER 5.0
#if (__BORLANDC__ < 0x540) && !defined(WINVER)
# define WINVER 0x500
#endif



//#if (__BORLANDC__ >= 0x540)
//# define BI_NO_CHECK_ARRAY
//#endif

// BCB 6.0
#if (__BORLANDC__ >= 0x560)
# define BI_ARRAY_NEW_DEFINED
#ifdef UNICODE
# define _USE_OLD_RW_STL      // use old RW STL  for now and only for unicode!!!!!!!!!!!!!!!!!
#endif
#if !defined(_USE_OLD_RW_STL)  // problems in stlport::_debug::invalidate_all()
# define BI_STD_NEWSSTREAM
#endif
# if !defined(BI_OWL_PACKING)
#   define BI_OWL_PACKING 8
# endif
#endif

// BCB 5.0
#if (__BORLANDC__ >= 0x540)
# define BI_HAS_MEMBER_TEMPLATE
# define NO_BI_STDIO_CTRFSI
#endif

// BCB v3.0 and later use an rtl much closer to the standard, with
// some old borland specific features now gone.
// HACK!!! __BORLANDC__ is currently the same, so try this
//
#if (__BORLANDC__ >= 0x530)
# include <_defs.h>
# define  BI_STD_RTL   // BI_STD_STRING + BI_STD_IO + BI_STD_EXCEPTIONS + BI_STD_RTTI
# define  BI_STD_STRING
# define  BI_STD_IO
#  define  BI_STD_EXCEPTIONS
#  define  BI_STD_RTTI
#  define   XALLOC           std::bad_alloc
#  define   XBASE           std::exception
#  define   XWHY_STR         what()
#  define   THROW_BAD_CAST   bad_cast()
#  define   THROW_TYPE_ID   bad_typeid()
#  define   BI_STD_SPECIALIZATION
# ifndef __FILE_ID_DEFINED__
#   ifndef _RWSTD_STRICT_ANSI
      typedef int __file_id;
#   else
      typedef FILE* __file_id;
#   endif
#   define __FILE_ID_DEFINED__
# endif
# define __OWL_TYPEINFO std::type_info
#endif

#if (__BORLANDC__ < 0x540)
#define BI_STDIO_HASFILECNT
#endif

#ifndef __FILE_ID_DEFINED__
# define __FILE_ID_DEFINED__
  typedef int __file_id;
#endif

#if (__BORLANDC__ < 0x530)

#define BI_NO_NAMESPACE
#define BI_NO_DEFAULT_TEMPLATES
#define BI_NO_TMPL_ALLOCATOR

//#define type_info typeinfo
# define __OWL_TYPEINFO typeinfo
#define _USE_OLD_STLNAMES // include old style STL headers to
                          // prevent include ROGUE_WAVE STL -> buggy
#define XALLOC             xalloc
#define XBASE             xmsg
#define XWHY_STR           why().c_str()
#define THROW_BAD_CAST     Bad_cast()
#define THROW_TYPE_ID     Bad_typeid()
#define bad_cast          Bad_cast
#define bad_typeid        Bad_typeid

typedef int INT_PTR;

#endif

#if (__BORLANDC__ >= 0x520)
#  define BI_64BIT_NATIVE     // Borland C++ 5.02 Has support for __int64
#endif


// BC v5.0 introduces the ability to import/export templates
//
//#if (__BORLANDC__ >= 0x500)
# define BI_IMPEXP_TEMPLATES // Import and export template instances
//#endif

// These were not available previous to BC 5.0
//
//#if (__BORLANDC__ < 0x500)
//# define BI_NO_BOOL          // Standard bool type           DEFS.H
//# define BI_NO_MUTABLE       // Mutable keyword              DEFS.H
//# define BI_NO_NAMESPACE     // Namespaces
//#endif

// BC has had these since 4.0
//
//#if (__BORLANDC__ < 0x400)
//# define BI_NO_ARRAYNEW      // operators new[] & delete[]   -
//# define BI_NO_COVAR_RET     // Covariant returns in overrides -
//# define BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
//# define BI_NO_THROWSPEC     // Function throw specifiers    EXCEPT.H
//# define BI_NO_LVAL_CASTS    // Can cast lvalues
//#endif

//#if (__BORLANDC__ < 0x400)
//# define BI_NO_NEW_CASTS     // New style casts              CASTABLE.H
//#endif

//
// Hand define macros for BC 4.x to be compatible with win SDK headers
//
//#if (__BORLANDC__ < 0x500)
//# if !defined(_WCHAR_T_DEFINED)
//#   define _WCHAR_T_DEFINED  // done already in win32/windows.h ?
//# endif
//# if defined(BI_PLAT_WIN32) && !defined(_WIN32)
//#   define _WIN32
//# endif
//#endif

/* has constructor tt(file_id,const char*,int)*/
#if !defined(NO_BI_STDIO_CTRFSI) && !defined(BI_STDIO_CTRFSI)
# if (__BORLANDC__ != 0x530)
# define BI_STDIO_CTRFSI
# endif  // (__BORLANDC__ != 0x530)
#endif
// also exist from STL
#define BI_DEFINE_MINMAX_TMPL

// define HUGE
//#define BI_DEFINE_HUGE


#define OwlOutputDebugString(lpsz) ::OutputDebugString(lpsz)

#if (__BORLANDC__ < 0x530)
#define wmemset _wmemset
#endif

#if (__BORLANDC__ < 0x570)
#define BI_NEED_UXTHEME_H
#endif

#if (__BORLANDC__ < 0x560)
#define BI_NEED_ZMOUSE_H
#endif

#if (__BORLANDC__ < 0x530)
#define BI_NEED_COMMCTRL_H
#define BI_NEED_COMMDLG_H
#define BI_NEED_HTMLHELP_H
#define BI_NEED_RICHEDIT_H
#define BI_NEED_SHLWAPI_H
#define BI_NEED_WININET_H
#define BI_NEED_OLEAUTO_DLL
#endif

#if (__BORLANDC__ >= 0x580)
#define DINKUMWARE_STL
#ifndef _RWSTD_NO_EXPLICIT
#define _EXPLICIT explicit
#else
#define _EXPLICIT 
#endif

//#if (__BORLANDC__ == 0x590)
//#define BI_TYPECASTOPERATOR_BUG   // problem with type cast operator
//#endif

#endif

#if !defined(_BUILDOWLDLL)
#   include <owl/private/owllink.h>
#endif

#if (__BORLANDC__ >= 0x580)
#define BI_HAS_STREAMWCHAROPEN
#define BI_HAS_STREAM_NOINIT
#endif

#if defined(_UNICODE) && (__BORLANDC__ < 0x610)
    extern wchar_t **    _RTLENTRY _EXPDATA _wargv;
#endif

#if (__BORLANDC__ >= 0x620)
#define BI_HAS_PRAGMA_ONCE
#endif

#endif  // OWL_PRIVATE_BORLANDC_H


