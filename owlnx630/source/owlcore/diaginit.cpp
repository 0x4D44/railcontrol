//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// Implementation of diagnostic initialization functions. Settings are stored
// in a profile (name is provided) in a section named "Diagnostics". Profile
// format in an ascii .ini file looks like:
//
//  [Diagnostics]
//  Enabled = <enabled>
//  <groupname1> = <enabled> <level>
//  <groupname2> = <enabled> <level>
//    ...
//  <groupnameN> = <enabled> <level>
//
// where:
//  <enabled> = 0|1
//  <level>   = 0..255
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_PROFILE_H)
# include <owl/profile.h>
#endif

#include <string.h>
#include <stdlib.h>

//FMM Needs some definitions made in memory.h such as _A2W and _USES_CONVERSION
#if defined(__HPUX_SOURCE)
#include <owl/private/memory.h>
#endif

__OWL_BEGIN_NAMESPACE

static const _TCHAR Section[] = _T("Diagnostics");
static int DiagsAreEnabled = 0;

//
// Retrieve a diag's enabled flag from a private .ini file. Master enable
// switch 'Enabled' overrides individual group settings. .ini file is searched
// for in standard profile places
//
_OWLFUNC(uint8)
GetDiagEnabled(LPCSTR filename, LPCSTR name, uint8 defEnabled)
{
  _USES_CONVERSION;
  TProfile diagProfile(Section, _A2W(filename));

  _TCHAR buf[32];
  static bool initialCheck = false;
  if (!initialCheck) {
    initialCheck = true;
    diagProfile.GetString(_T("Enabled"), buf, 32, _T("0"));
    DiagsAreEnabled = _ttoi(buf);
  }
  if (!DiagsAreEnabled)
    return 0;

  diagProfile.GetString(_A2W(name), buf, 32, _T(""));
  TCHAR* enabled = _tcstok(buf, _T(" "));
  return enabled ? uint16(_ttoi(enabled)) : defEnabled;
}

//
// Retrieve a diag's level setting from a private .ini file.
//
_OWLFUNC(uint8)
GetDiagLevel(LPCSTR filename, LPCSTR name, uint8 defLevel)
{
  _USES_CONVERSION;
  _TCHAR buf[32];
  TProfile(Section, _A2W(filename)).GetString(_A2W(name), buf, 32, _T(""));

  _tcstok(buf, _T(" "));
  _TCHAR* level = _tcstok(0, _T(" "));
  return level ? uint16(_ttoi(level)) : defLevel;
}

__OWL_END_NAMESPACE

