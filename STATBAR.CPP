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
  strcpy(TheFontLog.lfFaceName,"Arial");

  // Create the font
  TheFont = CreateFontIndirect(&TheFontLog);

  strcpy(DisplayedText, "");
}


TStatbar::~TStatbar()
{
  // Delete objects created
  DeleteObject(TheFont);
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
  BOOL    MadeDC;              // Were we forced to get our own DC?
  RECT    lRect;
  HPEN    hOldPen, hPen;

  // Check to see if we need to get a DC
  if (TheDC == 0)
  {
	 // need to create a new DC
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
  {
    MadeDC = FALSE;
  }

  // Redraw if the text has changed.
  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));
  if (TextChanged)  
  {
      lRect.left   = 7;
      lRect.top    = 4;
      lRect.right  = Attr.W-9;
      lRect.bottom = 17;
      ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
  }

  // Draw the 3d status bar
  hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
  hOldPen = (HPEN) SelectObject(TheDC, hPen);
  MoveToEx(TheDC, 5, 18, 0);
  LineTo(TheDC, 5, 2);
  LineTo(TheDC, Attr.W-8, 2);
  SelectObject(TheDC, hOldPen);
  DeleteObject(hPen);

  hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
  hOldPen = (HPEN) SelectObject(TheDC, hPen);
  LineTo(TheDC, Attr.W-8, 18);
  LineTo(TheDC, 5, 18);
  SelectObject(TheDC, hOldPen);
  DeleteObject(hPen);

  // Select the appropriate font
  SelectObject(TheDC, TheFont);
  TextOut(TheDC, 9, 3, DisplayedText, strlen(DisplayedText));

  // Release DC if we were forced to get our own
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TStatbar::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


LPSTR TStatbar::GetClassName()
{
  // Return the class name
  return "Statbar Class";
}


void TStatbar::EvSize(UINT sizeType, TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
}


LRESULT TStatbar::SetText(WPARAM, LPARAM lp)
{
  char*  TextString;

  TextString = (char*) lp;

  // Set the status bar text as specified by <TextString>
  strcpy(DisplayedText, TextString);
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
