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
  // SECURITY FIX: Load bitmaps into RAII guards for automatic cleanup
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
  // SECURITY FIX: Bitmap cleanup now handled automatically by TBitmapGuard
  // (DeleteObject calls removed - RAII guards handle cleanup in destructor)

  // Call ancestor method
  TDialog::CmOk();
}


void TAbout::EvPaint()
{

  HBITMAP     HOldBitmap;
  PAINTSTRUCT PS;
  HDC         MemDC;
  BITMAP      locoInfo = {0};
  BITMAP      dmuInfo = {0};
  RECT        client = {0};

  // Begin paint
  HWND hwnd = GetHandle();
  BeginPaint(hwnd, &PS);
  MemDC = CreateCompatibleDC(PS.hdc);
  HOldBitmap = (HBITMAP) SelectObject(MemDC, HLoco1);

  ::GetClientRect(hwnd, &client);
  ::GetObject(HLoco1, sizeof(BITMAP), &locoInfo);
  ::GetObject(HLoco2, sizeof(BITMAP), &dmuInfo);

  const int margin = 12;
  const int srcLocoW = (locoInfo.bmWidth  > 0) ? locoInfo.bmWidth  : 150;
  const int srcLocoH = (locoInfo.bmHeight > 0) ? locoInfo.bmHeight : 130;
  const int srcDmuW  = (dmuInfo.bmWidth   > 0) ? dmuInfo.bmWidth   : 155;
  const int srcDmuH  = (dmuInfo.bmHeight  > 0) ? dmuInfo.bmHeight  : 150;

  // Target: left image at 90% of original; right image same target size
  const int tgtW = (int)((srcLocoW * 9 + 5) / 10);  // round(0.9 * w)
  const int tgtH = (int)((srcLocoH * 9 + 5) / 10);  // round(0.9 * h)

  const int locoX = margin;
  const int locoY = margin;
  int dmuX = client.right - margin - tgtW;
  if (dmuX < margin)  dmuX = margin;
  const int dmuY = margin;

  // Left bitmap (scaled to 90%)
  SetStretchBltMode(PS.hdc, HALFTONE);
  StretchBlt(PS.hdc, locoX, locoY, tgtW, tgtH, MemDC, 0, 0, srcLocoW, srcLocoH, SRCCOPY);

  // Right bitmap (force same size as left)
  SelectObject(MemDC, HLoco2);
  SetStretchBltMode(PS.hdc, HALFTONE);
  StretchBlt(PS.hdc, dmuX, dmuY, tgtW, tgtH, MemDC, 0, 0, srcDmuW, srcDmuH, SRCCOPY);
  SelectObject(MemDC, HOldBitmap);

  // Delete compatible DC
  DeleteDC(MemDC);

  // End paint
  EndPaint(hwnd, &PS);
}


DEFINE_RESPONSE_TABLE1(TAbout, TDialog)
	 EV_WM_PAINT,
END_RESPONSE_TABLE;
