//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31.2.1 $
//$Author: jogybl $
//$Date: 2010-02-19 16:49:53 $
//
// Compiler-specific defines for Microsoft Visual C/C++. Included only by
// defs.h
//
// _MSC_VER versions:
//
//		1100 - Microsoft Visual C++ 5.0 - not supported
//		1200 - Microsoft Visual C++ 6.0 - oldest supported version
//		1300 - Microsoft Visual C++ 2003
//		1400 - Microsoft Visual C++ 2005
//		1500 - Microsoft Visual C++ 2008
//		1600 - Microsoft Visual C++ 2010
//		1700 - Microsoft Visual C++ 2012
//
//----------------------------------------------------------------------------
#if !defined(OWL_PRIVATE_MSC_H)
#define OWL_PRIVATE_MSC_H

//----------------------------------------------------------------------------
//  OS-specific flags defined here for DOS, Win16, Win32 & OS/2
//----------------------------------------------------------------------------

//
//  Define a platform flag.
//
//      BI_PLAT_MSW         Microsoft Windows, all versions including console
//
#define BI_NEW_GMODULE

# define BI_OWL_NO_DEBUG_CRT
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
# define _CALLCNVN //WINAPI
#endif

#if !defined(_CCONV)
# define _CCONV __cdecl
#endif

//#include <algorithm>
//using std::min;
//using std::max;

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

/* Fix suggested by Krassimir.Bozhkov@sirma.bg */ 
// _DLL    - Defined when /MD or /MDd (Multithread DLL) is specified.
// _WINDLL - Dynamic Library (.dll), displays linker toolset, specifies /DLL linker option, and adds the _WINDLL define to CL. 
//#if defined(__DLL__) || defined(_DLL)
#if defined(__DLL__) || defined(_WINDLL)
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

//----------------------------------------------------------------------------
//  Compiler-specific flags.
//----------------------------------------------------------------------------
// MSVC uses __declspecs, so just turn import/export into those
#define __export    __declspec(dllexport)
#define __import    __declspec(dllimport)

//
// Specific name of compiler, BI_COMP_xxxx
//
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
#   define BI_DBCS_SUPPORT
# endif
#endif //_MBCS

#if defined(BI_DBCS_SUPPORT)
# if !defined(_MBCS)
#   define _MBCS
# endif
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
#define BI_GLOBALNM_BUG     // Global namespace bug in function member int mfunc(int name = ::someName);
#define BI_NOTEMPLATE_H     // No include <owl/template.h>

// sir Sep 24th 2003 : so Dinkumware SXL, /I "\sxl\include" works
#ifndef _DEBUG
 #define BI_NO_ARRAYNEW      // no global operators new[] & delete[]   -
#define BI_ARRAY_NEW_DEFINED
#else
#define BI_NO_ARRAYNEW      // no global operators new[] & delete[]   -
#endif // _DEBUG

#if _MSC_VER >= 1300 
# define __MINMAX_DEFINED     // get from STL
#endif
//#define BI_NO_BOOL          // Standard bool type           DEFS.H
#if _MSC_VER < 1400 
# define BI_NO_COVAR_RET     // Covariant returns in overrides -
#endif
//#define BI_NO_MUTABLE       // Mutable keyword              DEFS.H
//#define BI_NO_NEW_CASTS     // New style casts              CASTABLE.H
#if _MSC_VER < 1200

# error Versions older than Microsoft Visual C++ 6.0 are not supported anymore


#   define BI_NO_NAMESPACE              // Namespaces
#   define BI_NO_OBJ_STREAMING      // currently ???
#   define BI_NO_THROWSPEC              // Function throw specifiers    EXCEPT.H
#endif
#if !defined(_CPPRTTI)
#error You have to include rtti support
#endif
//#define BI_NO_RTTI          // RTTI and __rtti keyword      DEFS.H
#define _RTTI                 // without __rtti keyword
//#define BI_NOTHUNK            // not used chunk

#if _MSC_VER >= 1200
#   define BI_STD_RTL
#   define BI_STD_STRING
#   define BI_STD_IO
#if _MSC_VER > 1200
#   define BI_STDIO_HASFILECNT // SIR June 19th 2007 MSVC 6 release build fails
#endif
#include <stdio.h>
typedef FILE* __file_id;
#   define BI_STDIO_CTRFILE
#   define BI_STD_NEWSSTREAM
#endif

#if !defined(_CPPUNWIND)
#error You have to include Exception support
#endif
//# define BI_STD_RTL
#define BI_STD_EXCEPTIONS

#define BI_NOSTATIC_FUNCCALL  // static member init by function call has problem, not used 
#define NO_AFTER_RETPARAM     // ??
//#define BI_IMPEXP_TEMPLATES
//#define BI_NEED_EXPL_TPL          //?? not needed and not used??
#define BI_NO_LVAL_CASTS            // Can cast lvalues
#define BI_XBASE_DEFINED            // define if xbase exist -> must be
#if defined(BI_STD_EXCEPTIONS)
# define BI_XALLOC_DEFINED           // define if xalloc exist
# define XALLOC std::bad_alloc       // get it be defined by library
#endif
#define XBASE       std::exception
#define XWHY_STR    what()
#define BI_VIRTUAL_BUG            //problem when derived class add virtual table
#define BI_64BIT_NATIVE           // MSC support __int64
#define BI_TYPECASTOPERATOR_BUG   // problem with type cast operator
#define BI_HAS_PRAGMA_ONCE
#if _MSC_VER >= 1300 
#define BI_NO_CHECK_ARRAY
#endif

//
// Disable some MSC warnings, & move some to level 4 since they really are OK
//
#pragma warning(disable:4068)  // unknown pragma, compiler should ignore
#pragma warning(disable:4250)  // inherits <base> via dominance
#pragma warning(disable:4251)  // class 'type' needs to have dll-interface to be used by clients of class 'type2'
#pragma warning(disable:4293)  // disable warning new in default argument will not free memory
#pragma warning(disable:4800)  // disable warning forsing value to bool
//#pragma warning(disable:4003)  // not enough actual parameters for macro
#pragma warning(disable:4275)  // non dll-interface class used as base for dll-interface class
//#pragma warning(disable:4243)  // type cast conversion from '' to '' exists, but is inaccessible
#pragma warning(disable:4355)  // 'this' used in base member initializer list
#pragma warning(disable:4273)  // 'identifier' : inconsistent DLL linkage. dllexport assumed
#pragma warning(disable:4786)  // 'identifier' : identifier was truncated to 'number' characters in the debug information

// SIR June 25th 2007
#if _MSC_VER == 1400
#pragma warning(disable:4996)  // The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name.
#endif

/*
#pragma warning(disable:4514)  // unreferenced inline, compiler internal noise
#pragma warning(disable:4201)  // complains about valid anonymouse unions!
#pragma warning(disable:4238)  // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable:4290)  // no warning about ignoring throw specification
#pragma warning(disable:4511)  // 'class' : copy constructor could not be generated
#pragma warning(disable:4512)  // 'class' : assignment operator could not be generated
#pragma warning(disable:4516)  // 'class::symbol' : access-declarations are deprecated; member using-declarations provide a better alternative
*/

#if _MSC_VER < 1300 
# include <typeinfo.h>
#else
# include <typeinfo>
#endif


// reference
#define THROW_BAD_CAST  bad_cast("OWL_badcast")
#define THROW_TYPE_ID   bad_typeid("OWL_type_id")

#define itoa     _itoa
#define ltoa     _ltoa
#define strcmpi  lstrcmpi
#define stricmp  lstrcmpi
#define tzset    _tzset
#define strnicmp _strnicmp
#define chsize   _chsize

#define BI_OWL_PACKING  8       // for VC default packing == 4

#if !defined(_BUILDOWLDLL)
#   include <owl/private/owllink.h>
#endif

/* Fix suggested by Boris.Vidolov@sirma.bg */ 
#if defined(_DEBUG)
# ifndef BI_OWL_NO_DEBUG_CRT
#   ifndef _INC_CRTDBG
//#     define _CRTDBG_MAP_ALLOC
//#     include <stdlib.h>
//#     include <crtdbg.h>
//#     define _INC_MALLOC /*to avoid including malloc.h which brings no-debugging heaps*/ 
#   endif
# endif // BI_OWL_NO_DEBUG_CRT
#endif // _DEBUG

#define OwlOutputDebugString(lpsz) ::OutputDebugString(lpsz) 


/////////////////////////////////////////////////////////////////////////////
// turn off reference tracking for certain often used symbols
#pragma component(browser, off, references, "CHECK")
#pragma component(browser, off, references, "CHECKX")
#pragma component(browser, off, references, "TRACE")
#pragma component(browser, off, references, "TRACEX")
#pragma component(browser, off, references, "PRECONDITION")
#pragma component(browser, off, references, "PRECONDITIONX")
#pragma component(browser, off, references, "uint8")
#pragma component(browser, off, references, "int8")
#pragma component(browser, off, references, "uint16")
#pragma component(browser, off, references, "int16")
#pragma component(browser, off, references, "unit32")
#pragma component(browser, off, references, "int32")
#pragma component(browser, off, references, "uint")
#pragma component(browser, off, references, "FAR")
#pragma component(browser, off, references, "LPSTR")
#pragma component(browser, off, references, "LPTSTR")
#pragma component(browser, off, references, "LPCSTR")
#pragma component(browser, off, references, "LPCTSTR")
#pragma component(browser, off, references, "NULL")
#pragma component(browser, off, references, "WINAPI")
#pragma component(browser, off, references, "_OWLCLASS") 
#pragma component(browser, off, references, "_OWLFUNC")
#pragma component(browser, off, references, "_OWLDATA")

// also exist from STL

#if defined(_STLPORT_VERSION) // which will be valid after the above include of <typeinfo.h>
# define __MINMAX_DEFINED
# define BI_NO_MINMAX_TMPL
# define BI_STD_IO
#endif

#if !defined(__MINMAX_DEFINED)
#define __MINMAX_DEFINED
#define BI_NO_MINMAX_TMPL

# undef min
# undef max


// SIR June 21st 2007
// see "using namespace std" directive in OBJSTRM.H
#if defined(BI_STD_IO)
namespace std {
#endif
template <class T> inline const T &min( const T &t1, const T &t2 )
{
    return t1 < t2 ? t1 : t2;
}
template <class T> inline const T &max( const T &t1, const T &t2 )
{
    return t1 > t2 ? t1 : t2;
}
// SIR June 21st 2007
#if defined(BI_STD_IO)
}
#endif
#endif /* __MINMAX_DEFINED */


#if _MSC_VER < 1300 
#define BI_NEED_HTMLHELP_H
#endif

#if _MSC_VER < 1300 
#define BI_NEED_UXTHEME_H
#endif

#if _MSC_VER >= 1500
#define BI_HAS_STREAMWCHAROPEN
#define BI_SUPPORT_TMPLFNC_FRIEND
#endif

//Jogy  Uncomment if you want to try the new print dialog with VC6
//#if _MSC_VER < 1300 
//#if !defined(WINVER)
//# define WINVER 0x500
//#endif
//#define BI_NEED_COMMDLG_H
//#endif


#endif  // OWL_PRIVATE_MSC_H
