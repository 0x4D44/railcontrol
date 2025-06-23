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


TStartup::TStartup(PTWindowsObject AParent)
	 : TWindow(AParent, "")
{
  RECT     Therect;

  // Get the desktop size
  GetWindowRect(GetDesktopWindow(), &Therect);

  // Set up window attributes
  Attr.X = ((Therect.right-Therect.left)/2)-231;
  Attr.Y = ((Therect.bottom-Therect.top)/2)-153;
  Attr.W = 463;
  Attr.H = 307;

  SetFlags(WB_MDICHILD, FALSE);
  Attr.Style = WS_POPUP;

  // Load screen bitmap
  HLoader = 0;
  HLoader = LoadBitmap(GetApplication()->hInstance, "SPLASHWN");
}


TStartup::~TStartup()
{
  DeleteObject(HLoader);
}


void TStartup::SetupWindow()
{
  // Call ancestor method
  TWindow::SetupWindow();
}


void TStartup::Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo)
{
  HDC   MemDC;

  // Call ancestor method
  TWindow::Paint(PaintDC, PaintInfo);

  // Create a compatible DC
  MemDC = CreateCompatibleDC(PaintDC);
  SelectObject(MemDC, HLoader);

  // BitBlt it
  BitBlt(PaintDC, 0, 0, Attr.W, Attr.H, MemDC, 0, 0, SRCCOPY);

  // Delete compatible DC
  DeleteDC(MemDC);
}



