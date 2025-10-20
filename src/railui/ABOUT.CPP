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

  const int margin = 10;
  const int centerWidth = 180; // Leave room for centered text

  // Compute destination panes for left/right images
  const int clientWidth = client.right - client.left;
  const int clientHeight = client.bottom - client.top;
  const int paneWidth = (clientWidth - (3 * margin) - centerWidth) / 2;
  const int paneHeight = clientHeight - (2 * margin);
  const int leftX = margin;
  const int leftY = margin;
  const int rightX = margin + paneWidth + centerWidth + margin;
  const int rightY = margin;

  // Helper to scale preserving aspect ratio
  auto stretchBitmap = [&](HBITMAP bmp, const BITMAP& info, int dx, int dy, int dw, int dh)
  {
    int sw = (info.bmWidth > 0) ? info.bmWidth : dw;
    int sh = (info.bmHeight > 0) ? info.bmHeight : dh;
    if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0)  return;

    double sx = double(dw) / double(sw);
    double sy = double(dh) / double(sh);
    double s = (sx < sy) ? sx : sy;
    int tw = int(sw * s);
    int th = int(sh * s);
    int tx = dx + (dw - tw) / 2;
    int ty = dy + (dh - th) / 2;

    HBITMAP old = (HBITMAP) SelectObject(MemDC, bmp);
    SetStretchBltMode(PS.hdc, HALFTONE);
    StretchBlt(PS.hdc, tx, ty, tw, th, MemDC, 0, 0, sw, sh, SRCCOPY);
    SelectObject(MemDC, old);
  };

  // Draw left and right images fitted into their panes
  stretchBitmap(HLoco1, locoInfo, leftX, leftY, paneWidth, paneHeight);
  stretchBitmap(HLoco2, dmuInfo, rightX, rightY, paneWidth, paneHeight);
  SelectObject(MemDC, HOldBitmap);

  // Delete compatible DC
  DeleteDC(MemDC);

  // End paint
  EndPaint(hwnd, &PS);
}


DEFINE_RESPONSE_TABLE1(TAbout, TDialog)
	 EV_WM_PAINT,
END_RESPONSE_TABLE;
