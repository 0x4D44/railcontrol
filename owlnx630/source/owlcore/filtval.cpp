//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2007-05-11 15:29:18 $                                                    //
//
// Implementation of TFilterValidator, a validator that filters out keys that
// are not in the valid character set.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#if !defined(OWL_VALIDATE_H)
# include <owl/validate.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

/** Let the compiler know that the following template instances will be defined elsewhere. **/
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

//
//
//
TFilterValidator::TFilterValidator(const TCharSet& validChars)
:
  TValidator()
{
  ValidChars = validChars;
}

//
//
//
bool
TFilterValidator::IsValidInput(LPTSTR str, bool /*suppressFill*/)
{
  for (LPTSTR p = str; *p; ) {
    uint n = CharSize(p) / sizeof(TCHAR);
    if (n > 1 || !ValidChars.Has((TCHAR)*p))
      return false;
    p += n;
  }
  return true;
}

//
//
//
bool
TFilterValidator::IsValid(LPCTSTR str)
{
  for (LPCTSTR p = str; *p;) {
    uint n = CharSize(p) / sizeof(TCHAR);
    if (n > 1 || !ValidChars.Has((TCHAR)*p))
      return false;
    p += n;
  }
  return true;
}

//
//
//
void
TFilterValidator::Error(TWindow* owner)
{
  PRECONDITION(owner);
  TApplication* app = owner->GetApplication();
  owl_string msg = app->LoadString(IDS_VALINVALIDCHAR);
  owner->MessageBox(msg.c_str(), 0, MB_ICONEXCLAMATION|MB_OK);
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TFilterValidator, TValidator);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TFilterValidator::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TValidator*)GetObject(), is);
  is >> GetObject()->ValidChars;
  return GetObject();
}

//
//
//
void
TFilterValidator::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TValidator*)GetObject(), os);
  os << GetObject()->ValidChars;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE

