// OWLCVT 05/11/95 22:40:08
/* STATBAR.CPP
*  ===========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 25/03/1994
*  Version  : 1.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"   		// Most important


/***************************************************************/
/* General program methods follow...  */


/***************************************************************/
/* Methods of TStatbar follow...  */


TStatbar::TStatbar(TWindow * parent)
	 :TFrameWindow(parent, "")
{
  Attr.Style = WS_BORDER | WS_CHILD | WS_VISIBLE;

  // Create a font record
  TheFontLog.lfHeight = 15;
  TheFontLog.lfWidth = 0;
  TheFontLog.lfEscapement = 0;
  TheFontLog.lfOrientation = 0;
  TheFontLog.lfWeight = FW_NORMAL;
  TheFontLog.lfItalic = 0;
  TheFontLog.lfUnderline = 0;
  TheFontLog.lfStrikeOut = 0;
  TheFontLog.lfCharSet = ANSI_CHARSET;
  TheFontLog.lfOutPrecision = OUT_DEFAULT_PRECIS;
  TheFontLog.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  TheFontLog.lfQuality = PROOF_QUALITY;
  TheFontLog.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
  CopyBuffer(TheFontLog.lfFaceName, "Arial");

  // Create the font
  TheFont = CreateFontIndirect(&TheFontLog);

  CopyBuffer(DisplayedText, "");
}


TStatbar::~TStatbar()
{
  // Delete objects created
  TheFont.Reset();
}


void TStatbar::SetupWindow()
{
  // Call ancestor method
  TFrameWindow::SetupWindow();
}


void TStatbar::Paint(TDC& dc, bool erase, TRect& rect)
{
  DrawScreen(dc, FALSE);
}


void TStatbar::DrawScreen(HDC TheDC, BOOL TextChanged)
{
  TReleaseDcGuard dcGuard;
  if (TheDC == 0)
  {
    dcGuard.Attach(HWindow, GetDC(HWindow));
    TheDC = dcGuard.Get();
  }
  if (!TheDC)
  {
    return;
  }

  RECT    lRect;
  HPEN    hOldPen;

  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));
  if (TextChanged)  
  {
    lRect.left   = 7;
    lRect.top    = 4;
    lRect.right  = Attr.W-9;
    lRect.bottom = 17;
    ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
  }

  {
    TPenGuard shadowPen(CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW)));
    hOldPen = (HPEN) SelectObject(TheDC, shadowPen);
    MoveToEx(TheDC, 5, 18, 0);
    LineTo(TheDC, 5, 2);
    LineTo(TheDC, Attr.W-8, 2);
    SelectObject(TheDC, hOldPen);
  }

  {
    TPenGuard highlightPen(CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT)));
    hOldPen = (HPEN) SelectObject(TheDC, highlightPen);
    LineTo(TheDC, Attr.W-8, 18);
    LineTo(TheDC, 5, 18);
    SelectObject(TheDC, hOldPen);
  }

  SelectObject(TheDC, TheFont);
  TextOut(TheDC, 9, 3, DisplayedText, strlen(DisplayedText));
}


void TStatbar::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


auto TStatbar::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Statbar Class"));
}


void TStatbar::EvSize(owl::uint sizeType, const TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
}


LRESULT TStatbar::SetText(WPARAM, LPARAM lp)
{
  char*  TextString;

  TextString = (char*) lp;

  // Set the status bar text as specified by <TextString>
  CopyBuffer(DisplayedText, TextString);
  DrawScreen(0, TRUE);
  return 0;
}


char* TStatbar::GetText()
{
  // Return the textstring
  return DisplayedText;
}


DEFINE_RESPONSE_TABLE1(TStatbar, TFrameWindow)
	 EV_WM_SIZE,
	 EV_MESSAGE(1025, SetText),
END_RESPONSE_TABLE;
