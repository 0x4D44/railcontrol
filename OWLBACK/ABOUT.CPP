/* ABOUT.CPP
*  =========
*
*  PROGRAM DECSRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     :
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"

/***************************************************************/
/* Methods of TAbout follow...  */


TAbout::TAbout(PTWindowsObject AParent, LPSTR AName)
 : TDialog(AParent, AName)
{
  // Load screen bitmap
  HLoco1 = 0;
  HLoco1 = LoadBitmap(GetApplication()->hInstance, "LOCO");
  HLoco2 = 0;
  HLoco2 = LoadBitmap(GetApplication()->hInstance, "DMU");
}


void TAbout::SetupWindow()
{
  // Call ancestor method
  TDialog::SetupWindow();
}


void TAbout::Ok(RTMessage Msg)
{
  // Delete bitmaps
  DeleteObject(HLoco1);
  DeleteObject(HLoco2);

  // Call ancestor method
  TDialog::Ok(Msg);
}


void TAbout::WMPaint(RTMessage)
{
  HBITMAP     HOldBitmap;
  PAINTSTRUCT PS;
  HDC         MemDC;

  // Begin paint
  BeginPaint(HWindow, &PS);
  MemDC = CreateCompatibleDC(PS.hdc);
  HOldBitmap = (HBITMAP) SelectObject(MemDC, HLoco1);

  // BitBlt it
  BitBlt(PS.hdc, 5, 10, 150, 130, MemDC, 0, 0, SRCCOPY);

  // And again...
  SelectObject(MemDC, HLoco2);
  BitBlt(PS.hdc, 450, 1, 155, 150, MemDC, 0, 0, SRCCOPY);
  SelectObject(MemDC, HOldBitmap);

  // Delete compatible DC
  DeleteDC(MemDC);

  // End paint
  EndPaint(HWindow, &PS);
}

