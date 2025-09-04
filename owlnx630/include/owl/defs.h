//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.4 $
//$Author: jogybl $
//$Date: 2010-02-23 10:45:14 $
//
// General definitions used by all ObjectWindows programs. Included directly
// by ObjectWindows source modules.
//------------------------------------------------------------------------------

#if !defined(OWL_DEFS_H)
#define OWL_DEFS_H
#define __OWL_OWLDEFS_H   // Old-style define for VbxGen compatibility

//------------------------------------------------------------------------------


#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>      // Supporting defines for app classes
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

//------------------------------------------------------------------------------
#if defined(BI_STD_RTL)
# define __OWL_USING_STD_NAMESPACE  using namespace std;
#  if !defined(BI_STD_STRING)
#    define BI_STD_STRING
#  endif
#  if !defined(BI_STD_IO)
#    define BI_STD_IO
#  endif
#  if !defined(BI_STD_EXCEPTIONS)
#    define BI_STD_EXCEPTIONS
#  endif
#  if !defined(BI_STD_RTTI)
#    define BI_STD_RTTI
#  endif
#else
# define __OWL_USING_STD_NAMESPACE
#endif

#if !defined (__OWL_TYPEINFO)
# define __OWL_TYPEINFO type_info
#endif


//------------------------------------------------------------------------------
// Get Winsys & Services headers common to all of Owl
//

#if !defined(OWL_PRIVATE_MEMORY_H)
# include <owl/private/memory.h>    // Common memory manipulation functions
#endif

#if !defined(OWL_PRIVATE_EXCEPT_H)
# include <owl/private/except.h>
#endif


#if !defined(BI_STD_STRING)
#  if defined(BI_COMP_BORLANDC) && __BORLANDC__ < 0x530
#    include <cstring.h>
    typedef string owl_string;
#  else
#    include <owl/private/cstring.h>
    typedef __OWL string owl_string;
#  endif
#else
# include <string>

#  if defined(_UNICODE)
#   if !defined(BI_NO_NAMESPACE)
      typedef std::wstring owl_string;
#   else
      typedef wstring owl_string;
#   endif
#else
#   if !defined(BI_NO_NAMESPACE)
      typedef std::string owl_string;
#   else
      typedef string owl_string;
#   endif
#endif    
#if _MSC_VER < 1600  
//Jogy - this does not link correctly with VC2010. In 6.32 and later the class TResString is removed, 
//       but here we keep it for backward binary compatibility for other compilers
  class _OWLCLASS TResString : public owl_string {
    public:
      // Constructor to make a string from a resource
      TResString(HINSTANCE instance, uint id, int len = 255);
  };
#endif  
  const size_t NPOS = size_t(-1);
#endif

#if !defined(_ttof)
# if defined(UNICODE)
#  define _ttof       _wtof
# else
#  define _ttof       atof
# endif
#endif


#if defined(UNICODE) && !defined(BI_STD_EXCEPTIONS)
#error Unicode is not supported when no standard exceptions are available
#endif



#include <owl/private/checks.h>    // Runtime diagnostic macros

#if !defined(OWL_WSYSINC_H)
# include <owl/wsysinc.h>     // Windowing system headers
#endif
#if !defined(OWL_SYSTEM_H)
# include <owl/system.h>
#endif
#if !defined(OWL_PRIVATE_NUMBER_H)
# include <owl/private/number.h>     // 64/128 bit arithmetic
#endif

#include <owl/version.h>    // OWL version # constants


//------------------------------------------------------------------------------
// Additional windows.h related defines & undefs for Owl compatibility
//
extern HINSTANCE _hInstance;

# undef GetNextWindow
# undef GetWindowTask
# define WM_SYSTEMERROR      0x0017
# define WM_CTLCOLOR         0x0019
# define HTASK HANDLE    // allow users to use the same type in Win16 & Win32
#undef  SetWindowFont
#undef  GetWindowFont

#if defined(_M_CEE)   // this is defined when /clr flag is used

#if !defined(GetClassName)
#ifdef UNICODE
#define GetClassName  GetClassNameW
#else
#define GetClassName  GetClassNameA
#endif // !UNICODE
#endif

#if !defined(SendMessage)
#ifdef UNICODE
#define SendMessage  SendMessageW
#else
#define SendMessage  SendMessageA
#endif // !UNICODE
#endif

#if !defined(GetTextMetrics)
#ifdef UNICODE
#define GetTextMetrics  GetTextMetricsW
#else
#define GetTextMetrics  GetTextMetricsA
#endif // !UNICODE
#endif

#if !defined(DrawText)
#ifdef UNICODE
#define DrawText  DrawTextW
#else
#define DrawText  DrawTextA
#endif // !UNICODE
#endif

#endif

//------------------------------------------------------------------------------
// Owl settings for runtime diagnostics in owl.defs.h & classlib/checks.h
//
#define   OWL_INI     "OWL.INI"
#define   OWL_CDLEVEL  2
#if !defined(OWL_DIAGINFO)
# define OWL_DIAGINFO
#endif

//------------------------------------------------------------------------------
// Strict data makes all data members private. Accessors must then be used
// to access the data members.
//
#if defined(OWL_STRICT_DATA)
# define public_data    private
# define protected_data private
#else
# define public_data    public
# define protected_data protected
#endif


//------------------------------------------------------------------------------
// Common external Owl functions & data
//

__OWL_BEGIN_NAMESPACE

// Get version of OWL at runtime
//
_OWLFUNC(uint32)  OWLGetVersion();   

// WinSys extensions to the runtime diagnostics in classlib/checks.h
//
_OWLFUNC(uint8) GetDiagEnabled(LPCSTR file, LPCSTR name, uint8 e);
_OWLFUNC(uint8) GetDiagLevel(LPCSTR file, LPCSTR name, uint8 l);

__OWL_END_NAMESPACE

#  define DIAG_DEFINE_GROUP_INIT(f,g,e,l)\
  DIAG_DEFINE_GROUP(g, NS_OWL::GetDiagEnabled(f,#g,e), NS_OWL::GetDiagLevel(f,#g,l));
#  define DIAG_DEFINE_EXPORTGROUP_INIT(f,g,e,l)\
  DIAG_DEFINE_EXPORTGROUP(g, NS_OWL::GetDiagEnabled(f,#g,e), NS_OWL::GetDiagLevel(f,#g,l));

//------------------------------------------------------------------------------
//#define OWL_NATIVECTRL_ALWAYS   // for common controls
//------------------------------------------------------------------------------

#if !defined(BI_NOINCLUDE_NS_OWL)
#pragma message("Warning: The OWL namespace is dumped into the global namespace. Define BI_NOINCLUDE_NS_OWL to avoid this.")
__OWL_USING_NAMESPACE 
#endif

#endif  // OWL_DEFS_H
