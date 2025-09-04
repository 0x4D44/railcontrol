//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:44 $
//
// Implementation of TRangeValidator, integer numeric range input validator
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
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

// Let the compiler know that the following template instances will be defined elsewhere.
//#pragma option -Jgx

#if !defined(SECTION) || SECTION == 1

//
//
//
TRangeValidator::TRangeValidator(long min, long max)
:
  TFilterValidator(_T("0-9+-"))
{
  if (min >= 0)
    ValidChars -= _T('-');
  Min = min;
  Max = max;
}

//
//
//
void
TRangeValidator::Error(TWindow* owner)
{
  PRECONDITION(owner);
  TApplication* app = owner->GetApplication();
  owl_string msgTmpl = app->LoadString(IDS_VALNOTINRANGE);
  TAPointer<_TCHAR> msg = new _TCHAR[msgTmpl.length() + 10 + 10 + 1];
  _stprintf(msg, msgTmpl.c_str(), Min, Max);
  owner->MessageBox(msg, 0, MB_ICONEXCLAMATION|MB_OK);
}

//
//
//
bool
TRangeValidator::IsValid(LPCTSTR s)
{
  if (TFilterValidator::IsValid(s)) {
    long value = _ttol(s);
    if (value >= Min && value <= Max)
      return true;
  }
  return false;
}

//
//
//
uint
TRangeValidator::Transfer(_TCHAR * s, void* buffer, TTransferDirection direction)
{
  if (Options & voTransfer) {
    if (direction == tdGetData) {
      *(long*)buffer = _ttol(s);
    }
    else if (direction == tdSetData) {
      wsprintf(s, _T("%ld"), *(long*)buffer);  // need wsprintf for char *
    }
    return sizeof(long);
  }
  else
    return 0;
}

//
// Adjust the 'value' of the text, given a cursor position & an amount
// Return the actual amount adjusted.
//
int
TRangeValidator::Adjust(owl_string& text, uint& /*begPos*/, uint& /*endPos*/, int amount)
{
  long value = _ttol(text.c_str());
  long newValue = value + amount;
  if (newValue < Min)
    newValue = Min;
  else if (newValue > Max)
    newValue = Max;

  _TCHAR buffer[15];
  _stprintf(buffer, _T("%ld"), newValue);
  text = buffer;

  return int(newValue - value);
}


#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TRangeValidator, TFilterValidator);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TRangeValidator::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TFilterValidator*)GetObject(), is);
  is >> GetObject()->Min >> GetObject()->Max;
  return GetObject();
}

//
//
//
void
TRangeValidator::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TFilterValidator*)GetObject(), os);
  os << GetObject()->Min << GetObject()->Max;
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

