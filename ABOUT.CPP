// OWLCVT 05/11/95 22:37:01
/* ABOUT.CPP
*  =========
*
*  PROGRAM DESCRIPTION
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


TAbout::TAbout(TWindow * parent, LPCTSTR name)
 : TDialog(parent, name)
{
  // Load screen bitmaps
  HINSTANCE hInst = GetModule()->GetHandle();
  HLoco1 = ::LoadBitmap(hInst, TEXT("LOCO"));
  HLoco2 = ::LoadBitmap(hInst, TEXT("DMU"));
}


void TAbout::SetupWindow()
{
  // Call ancestor method
  TDialog::SetupWindow();
}


void TAbout::CmOk()
{
  // Delete bitmaps
  DeleteObject(HLoco1);
  DeleteObject(HLoco2);

  // Call ancestor method
  TDialog::CmOk();
}


void TAbout::EvPaint()
{

  HBITMAP     HOldBitmap;
  PAINTSTRUCT PS;
  HDC         MemDC;

  // Begin paint
  HWND hwnd = GetHandle();
  BeginPaint(hwnd, &PS);
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
  EndPaint(hwnd, &PS);
}


DEFINE_RESPONSE_TABLE1(TAbout, TDialog)
	 EV_WM_PAINT,
END_RESPONSE_TABLE;
