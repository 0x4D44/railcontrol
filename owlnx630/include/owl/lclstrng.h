//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:33 $
//
// TLocaleString - localized name support
//----------------------------------------------------------------------------

#if !defined(OWL_LCLSTRNG_H)
#define OWL_LCLSTRNG_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif
#if !defined(BI_PLAT_MSW)
#error Locale classes are only supported under MSW
#endif


__OWL_BEGIN_NAMESPACE

#define DEFAULT_SYSTEM_LANGID 0x409   // in case 16-bit with no NLS support
typedef uint16 TLangId;               // language ID - same as NT LANGID
typedef int (*TstricmpLang)(const _TCHAR *, const char *, TLangId);
const TLangId LangSysDefault  = 0x0800;
const TLangId LangUserDefault = 0x0400;
const TLangId LangNeutral     = 0x0000;

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//FMM
#if defined(__HPUX_SOURCE)
#include <wtypes.h>
#endif



//----------------------------------------------------------------------------
// TLocaleString - localizable substitute for char*
//

struct _OWLCLASS TLocaleString {
  //TLocaleString():Private(0){}
  //TLocaleString(const TLocaleString& lstr):Private(lstr.Private){}
  //TLocaleString(const _TCHAR* str):Private(str){}
  const _TCHAR * Translate(TLangId lang);                      // translate string
  operator const _TCHAR *();      // return current string
  void operator =(const _TCHAR * str) { Private = str; }
  int   Compare(const _TCHAR* str, TLangId lang);   // insensitive compare

  const _TCHAR* Private;         // string pointer used for initialization

  static TLangId GetSystemLangId();  // platform-dependent implementation
  static TLangId GetUserLangId();    // platform-dependent implementation
  static int     IsNativeLangId(TLangId lang);  // returns bool true if native
  static TLangId SystemDefaultLangId;// must define and set to system language
  static TLangId UserDefaultLangId;  // same as system language if single user
  static TLangId NativeLangId;   // must define and set to lang. of literals
  static HINSTANCE Module;       // must define and set to resource module
  static TLocaleString Null;     // reference a null string
  static int CompareLang(const _TCHAR * s1, const _TCHAR * s2, TLangId);
          // CompareLang() may be implemented in another module for NLS support
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

__OWL_END_NAMESPACE

//
// Prefix characters for locale strings
//
#define AUTOLANG_RCID  '#' // indicates name specified by numeric ID
#define AUTOLANG_XLAT  '!' // indicates name to be localized (binary)
#define AUTOLANG_LOAD  '@' // indicates resource name to load (binary)

//
// Custom resource for translations
//
#define RT_LOCALIZATION MAKEINTRESOURCE(201)




#endif  // OWL_LCLSTRNG_H
