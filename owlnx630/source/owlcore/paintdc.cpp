//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of TPaintDC, an encapsulation of client DCs used when
// responding to paint messages
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
//
//
TPaintDC::TPaintDC(HWND hWnd)
:
  TDC()
{
  Wnd = hWnd;
  Handle = ::BeginPaint(hWnd, &Ps);
  CheckValid();
}

//
//
//
TPaintDC::~TPaintDC()
{
  RestoreObjects();
  if (ShouldDelete)
    ::EndPaint(Wnd, &Ps);
}

__OWL_END_NAMESPACE
/* ========================================================================== */

