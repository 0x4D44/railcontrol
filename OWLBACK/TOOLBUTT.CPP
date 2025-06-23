/* TOOLBUTT.CPP
*  ============
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 25/03/1994
*  Version  : 2.02
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"   		// Most important


/***************************************************************/
/* General program methods follow...  */


/***************************************************************/
/* Methods of TToolButton follow...  */

TToolbutton::TToolbutton(PTWindowsObject AParent, int XPos, int YPos, int ButtNum, WORD WPAR,
		         long LPAR, HWND SentWin, HWND StatWin, char* UpTxt,
			 char* DownTxt)
	    :TWindow(AParent, "")
{
  // Set up toolbutton position
  Attr.X = XPos; 
  Attr.Y = YPos;
  Attr.W = 24;
  Attr.H = 22;

  // Set window styles
  SetFlags(WB_MDICHILD, FALSE);
  Attr.Style = WS_CHILD | WS_VISIBLE;

  // The button is initially in the up position and the left mouse
  // button is not selected.
  strcpy(UpText, UpTxt);
  strcpy(DownText, DownTxt);
  WParam = WPAR;
  LParam = LPAR;
  SentWindow = SentWin;
  StatWindow = StatWin;
  UpPosition = TRUE;
  LMseButton = FALSE;
  Enabled = TRUE;
  ButtonNum = ButtNum;
}


TToolbutton::~TToolbutton()
{

}


void TToolbutton::SetupWindow()
{
  // Call ancestor method
  TWindow::SetupWindow();
}


void TToolbutton::Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo)
{
  // Call ancestor method
  TWindow::Paint(PaintDC, PaintInfo);

  // Draw the button in
  DrawScreen(PaintDC);
}


void TToolbutton::DrawScreen(HDC TheDC)
{
  BOOL      MadeDC;
  HDC       MemDC;
  HBITMAP   OldBitmap;
  int       XPos, YPos, MemYPos;

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

  // Create a compatible DC
  MemDC = CreateCompatibleDC(TheDC);

  // Is the button up or down?
  if (UpPosition)
  {
    OldBitmap = (HBITMAP) SelectObject(MemDC, PToolbar(Parent)->HUpButt);
    XPos = 4;
    YPos = 3;
  }
  else
  {
    OldBitmap = (HBITMAP) SelectObject(MemDC, PToolbar(Parent)->HDownButt);
    XPos = 5;
    YPos = 4;
  }

  // Draw it
  BitBlt(TheDC, 0, 0, 24, 22, MemDC, 0, 0, SRCCOPY);

  // Draw the button face
  SelectObject(MemDC, PToolbar(Parent)->ButtBitmap);

  if (Enabled)
  {
    MemYPos = 0;
  }
  else
  {
    MemYPos = 15;
  }

  // Draw the button
  BitBlt(TheDC, XPos, YPos, 16, 15, MemDC, (ButtonNum-1)*16, MemYPos, SRCCOPY);

  // Reselect the original bitmap
  SelectObject(MemDC, OldBitmap);

  // Delete the compatible DC
  DeleteDC(MemDC);

  // Release DC if we were forced to get our own
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TToolbutton::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TWindow::GetWindowClass(WndClass);
  WndClass.style = CS_HREDRAW || CS_VREDRAW;
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
}


LPSTR TToolbutton::GetClassName()
{
  // Return class name
  return "Toolbutton Class";
}


void TToolbutton::Enable(BOOL GivEnabled)
{
  // Enable or disable the tool button
  Enabled = GivEnabled;
  DrawScreen(0);
}


void TToolbutton::WMLButtonDown(RTMessage Msg)
{
  // Call ancestor method 
  TWindow::WMLButtonDown(Msg);

  if (Enabled)
  {
    // Reset button 
    LMseButton = TRUE;
    UpPosition = FALSE;
    DrawScreen(0);
    SetCapture(HWindow);   

    // Update status display
    if (StatWindow != 0)
    {
      SendMessage(StatWindow,
	          SB_SETTEXT,
	          0,
	          (LPARAM) ((LPSTR) DownText));
    }
  }
}


void TToolbutton::WMLButtonUp(RTMessage)
{
  BOOL   OldUpPosition;    // Store the old up position

  if (Enabled)
  {
    // Reset button
    OldUpPosition = UpPosition;
    LMseButton = FALSE;
    ReleaseCapture();
    UpPosition = TRUE;
    DrawScreen(0);

    // Send message to main window 
    if (!OldUpPosition)
    {
      DrawScreen(0);
      PostMessage(SentWindow, WM_COMMAND, WParam, LParam);
    }

    // Update status display
    if (StatWindow != 0)
    {
      SendMessage(StatWindow,
	          SB_SETTEXT,
	          0,
		  (LPARAM) ((LPSTR) UpText));
    }
  }
}


void TToolbutton::WMMouseMove(RTMessage Msg)
{
  int XPos, YPos;      // Current X and Y mouse positions

  // Get mouse position
  XPos = LOWORD(Msg.LParam);
  YPos = HIWORD(Msg.LParam);

  // Reset button 
  if (((XPos < 0) || (XPos > Attr.W)) || ((YPos < 0) || (YPos > Attr.H)))
  {
    if (!UpPosition)
    {
      UpPosition = TRUE;
      DrawScreen(0);
    }
  }
  else
  {
    if (UpPosition && LMseButton)
    {
      UpPosition = FALSE;
      DrawScreen(0);
    }
  }
}


void TToolbutton::SetDownText(char* TextString)
{
  // Set the down button text string 
  strcpy(DownText, TextString);
}


void TToolbutton::SetUpText(char* TextString)
{
  // Set the down button text string
  strcpy(UpText, TextString);
}



