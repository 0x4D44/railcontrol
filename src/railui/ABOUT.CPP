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
  const int locoWidth = (locoInfo.bmWidth > 0) ? locoInfo.bmWidth : 150;
  const int locoHeight = (locoInfo.bmHeight > 0) ? locoInfo.bmHeight : 130;
  const int dmuWidth = (dmuInfo.bmWidth > 0) ? dmuInfo.bmWidth : 155;
  const int dmuHeight = (dmuInfo.bmHeight > 0) ? dmuInfo.bmHeight : 150;

  const int locoX = margin;
  const int locoY = margin;
  int dmuX = client.right - margin - dmuWidth;
  if (dmuX < margin)  dmuX = margin;
  const int dmuY = margin;

  // BitBlt it
  BitBlt(PS.hdc, locoX, locoY, locoWidth, locoHeight, MemDC, 0, 0, SRCCOPY);

  // And again...
  SelectObject(MemDC, HLoco2);
  BitBlt(PS.hdc, dmuX, dmuY, dmuWidth, dmuHeight, MemDC, 0, 0, SRCCOPY);
  SelectObject(MemDC, HOldBitmap);

  // Delete compatible DC
  DeleteDC(MemDC);

  // End paint
  EndPaint(hwnd, &PS);
}


DEFINE_RESPONSE_TABLE1(TAbout, TDialog)
	 EV_WM_PAINT,
END_RESPONSE_TABLE;
