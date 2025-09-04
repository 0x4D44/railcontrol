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
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_UIHELPER_H)
# include <owl/uihelper.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Empty constructor.
//
TUIPart::TUIPart()
{
}

//
// Draw the part onto a DC.
// The type and state control how the part should be painted.
//
bool
TUIPart::Paint(TDC& dc, TRect& rect, TType type, TState state)
{
  return DrawFrameControl(dc, rect, type, state);
}

//
// Wrapper for the DrawFrameControl API.
//
#if !defined(BI_COMP_GNUC)
#pragma warn -par
#endif
bool
TUIPart::DrawFrameControl(TDC& dc, TRect& rect, TType type, TState state)
{
  static bool hasDrawFrameControl = true;

  // Try once to see if the API call is available. If not, do ourselves.
  //
  if (hasDrawFrameControl) {
    if (::DrawFrameControl(dc, &rect, type, state))
      return true;
    if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
      hasDrawFrameControl = false;
    else
      return false;
  }

  //!BB Emulate call
  //
  //
  return false;
}
#if !defined(BI_COMP_GNUC)
#pragma warn .par
#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

