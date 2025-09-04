//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1995, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_TIMEGADG_H)
# include <owl/timegadg.h>
#endif
#if !defined(OWL_TIME_H)
# include <owl/time.h>
#endif
#if !defined(OWL_POINTER_H)
# include <owl/pointer.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Constructor for TTimeGadget.
//
TTimeGadget::TTimeGadget(TGetTimeFunc timeFunc, int id, TBorderStyle border,
                         TAlign align, uint numChars, LPCTSTR text,
                         TFont* font)
:
  TTextGadget(id, border, align, numChars, text, font),
  TimeFunc(timeFunc)
{
  SetShrinkWrap(false, true);
}

//
// Overriden from TGadget to inform gadget window to setup a timer
//
void
TTimeGadget::Created()
{
  TGadget::Created();
  Window->EnableTimer();
}

//
// Overridden from TGadget to display the current time.
//
bool
TTimeGadget::IdleAction(long count)
{
  TGadget::IdleAction(count);

  owl_string newTime;
  TimeFunc(newTime);
  SetText(newTime.c_str());

  // NOTE: Don't return true to drain system. Let GadgetWindow Timer
  //       message indirectly trigger IdleAction.
  //
  return false;
}

//
// Retrieve the current time.
//
void
TTimeGadget::GetTTime(owl_string& newTime)
{
  TTime time;
  newTime = time.AsString();
}

//
// Win32 specific
//

//
// Retrieve the system time using the Win32 API.
//
void
TTimeGadget::GetSystemTime(owl_string& newTime)
{
  TAPointer<_TCHAR> dateBuffer = new _TCHAR[100];
  TAPointer<_TCHAR> timeBuffer = new _TCHAR[100];
  LCID lcid = ::GetUserDefaultLCID();
  SYSTEMTIME systemTime;
  ::GetSystemTime(&systemTime);

  if (::GetTimeFormat(lcid, LOCALE_NOUSEROVERRIDE, &systemTime, 0, timeBuffer, 100)) {
    if (::GetDateFormat(lcid, LOCALE_NOUSEROVERRIDE, &systemTime, 0, dateBuffer, 100)) {
      newTime += dateBuffer;
      newTime += _T(" ");
      newTime += timeBuffer;
    }
  }
}

//
// Retrieve the local time using the Win32 API
//
void
TTimeGadget::GetLocalTime(owl_string& newTime)
{
  TAPointer<_TCHAR> dateBuffer = new _TCHAR[100];
  TAPointer<_TCHAR> timeBuffer = new _TCHAR[100];
  LCID lcid = ::GetUserDefaultLCID();
  SYSTEMTIME systemTime;
  ::GetLocalTime(&systemTime);

  if (::GetTimeFormat(lcid, LOCALE_NOUSEROVERRIDE, &systemTime, 0, timeBuffer, 100)) {
    if (::GetDateFormat(lcid, LOCALE_NOUSEROVERRIDE, &systemTime, 0, dateBuffer, 100)) {
      newTime += dateBuffer;
      newTime += _T(" ");
      newTime += timeBuffer;
    }
  }
}


__OWL_END_NAMESPACE
/* ========================================================================== */


