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


TStatbar::TStatbar(PTWindowsObject AParent)
	 :TWindow(AParent, "")
{
  // Turn off the MDI flag that parent set
  SetFlags(WB_MDICHILD, FALSE);
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
  TWindow::SetupWindow();
}


void TStatbar::Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo)
{
  // Call ancestor method
  TWindow::Paint(PaintDC, PaintInfo);

  DrawScreen(PaintDC, FALSE);
}


void TStatbar::DrawScreen(HDC TheDC, BOOL TextChanged)
{
  BOOL  MadeDC;                // Were we forced to get our own DC?
  HPEN  TempPen, OldPen;       // Handle to a temporary pen

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

  // Temporarily create a dark gray pen
  TempPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));

  // Draw the 3d status bar
  OldPen = (HPEN) SelectObject(TheDC, TempPen);
  MoveTo(TheDC, 5, 18);
  LineTo(TheDC, 5, 2);
  LineTo(TheDC, Attr.W-8, 2);
  SelectObject(TheDC, GetStockObject(WHITE_PEN));
  LineTo(TheDC, Attr.W-8, 18);
  LineTo(TheDC, 5, 18);

  // Delete the temporary pen
  SelectObject(TheDC, OldPen);
  DeleteObject(TempPen);

  // Temporarily create a light gray pen
  TempPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));

  // Set up the objects
  SetBkColor(TheDC, RGB(200, 200, 200));
  SelectObject(TheDC, GetStockObject(LTGRAY_BRUSH));
  OldPen = (HPEN) SelectObject(TheDC, TempPen);
  if (TextChanged)  Rectangle(TheDC, 6, 3, Attr.W-9, 17);

  // Delete the temporary pen (again)
  SelectObject(TheDC, OldPen);
  DeleteObject(TempPen);

  // Select the appropriate font
  SelectObject(TheDC, TheFont);
  TextOut(TheDC, 9, 3, DisplayedText, strlen(DisplayedText));

  // Release DC if we were forced to get our own
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TStatbar::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
}


LPSTR TStatbar::GetClassName()
{
  // Return the class name
  return "Statbar Class";
}


void TStatbar::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);
}


void TStatbar::SetText(RTMessage Msg)
{
  char*  TextString;

  TextString = (LPSTR) Msg.LParam;

  // Set the status bar text as specified by <TextString>
  strcpy(DisplayedText, TextString);
  DrawScreen(0, TRUE);
}


char* TStatbar::GetText()
{
  // Return the textstring
  return DisplayedText;
}