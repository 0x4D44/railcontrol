//----------------------------------------------------------------------------
// Borland WinSys Library
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// TLocaleString default NLS compare function - used only if non-OLE2
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
#include <owl/lclstrng.h>
#endif

__OWL_BEGIN_NAMESPACE


TLangId
TLocaleString::GetSystemLangId()
{
  return ::GetSystemDefaultLangID();
}

TLangId
TLocaleString::GetUserLangId()
{
  return ::GetUserDefaultLangID();
}

int
TLocaleString::CompareLang(const _TCHAR * s1, const _TCHAR * s2, TLangId lang)
{
  typedef int (WINAPI *TCompareString)(LCID, DWORD, LPCTSTR, int, LPCTSTR, int);
#if defined(UNICODE)  
  static int (WINAPI *compareString)(LCID, DWORD, LPCTSTR, int, LPCTSTR, int) =
    (TCompareString)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "CompareStringW");
#else
  static int (WINAPI *compareString)(LCID, DWORD, LPCTSTR, int, LPCTSTR, int) =
    (TCompareString)::GetProcAddress(::GetModuleHandle(_T("kernel32")), "CompareStringA");
#endif
  // Use CompareStringA if it is available
  //
  if (compareString)
    return compareString(lang, NORM_IGNORECASE | NORM_IGNORENONSPACE,
                          s1,-1, s2,-1) - 2;

  // Otherwise just use RTL function
  //
  return _tcsicmp(s1, s2);  // only permissible if not an OLE application
}


__OWL_END_NAMESPACE

