// OWLCVT 05/11/95 22:37:20
/* STARTUP.CPP
*  ===========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"   		// Most important
#define CM_READ1 101


TStartup::TStartup(TFrameWindow* parent)
	 : TWindow(parent, "")
{
  RECT     Therect;

  // Get the desktop size
  ::GetWindowRect(::GetDesktopWindow(), &Therect);

  // Set up window attributes
  Attr.X = ((Therect.right-Therect.left)/2)-231;
  Attr.Y = ((Therect.bottom-Therect.top)/2)-153;
  Attr.W = 463;
  Attr.H = 307;
  Attr.Style = WS_POPUP;

  // Load screen bitmap
  HLoader = ::LoadBitmap(GetModule()->GetHandle(), TEXT("SPLASHWN"));
}


TStartup::~TStartup()
{
  HLoader.Reset();
}


void TStartup::SetupWindow()
{
  // Call ancestor method
  TWindow::SetupWindow();
}


void TStartup::Paint(TDC& dc, bool erase, TRect& rect)
{
  TDcGuard memDcGuard(CreateCompatibleDC(dc));
  HDC memDC = memDcGuard.Get();
  if (!memDC)
  {
    return;
  }

  SelectObject(memDC, HLoader.Get());

  // BitBlt it
  BitBlt(dc, 0, 0, Attr.W, Attr.H, memDC, 0, 0, SRCCOPY);
}



