// OWLCVT 05/11/95 22:39:27
/* TOOLBAR.CPP
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
*  Version  : 2.02
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"   		// Most important


/***************************************************************/
/* General program methods follow...  */


/***************************************************************/
/* Methods of TToolbar follow...  */


TToolbar::TToolbar(TWindow * AParent, int NumButtons, Toolbuttondata ButtonData, HBITMAP ButtonBitmap)
	  :TFrameWindow(AParent, "")
{
  // Store given information
  NumButt = NumButtons;
  ButtData = ButtonData;
  ButtBitmap = ButtonBitmap;

  // Turn off the MDI flag that parent set
  Attr.Style = WS_BORDER | WS_CHILD | WS_VISIBLE;

  // Load the up and down button bitmaps
  HUpButt = LoadBitmap(*GetApplication(), "UPBUTTON");
  HDownButt = LoadBitmap(*GetApplication(), "DOWNBUTTON");
}


TToolbar::~TToolbar()
{
  // Delete objects
  DeleteObject(HUpButt);
  DeleteObject(HDownButt);
}


void TToolbar::SetupWindow()
{
  int i;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Create the toolbar buttons - we are given a bitmap in the form
  //
  //  <enabled-pic><disabled-pic>   - for button 1
  //  <enabled-pic><disabled-pic>   - for button 2
  //  ...........................
  //  ...........................
  //  <enabled-pic><disabled-pic>   - for button n
  //
  //  and we are also told how many buttons to display. We must also
  //  be told the correct button spacing. Thus we are also passed a
  //  pointer to an arrray containing the X spacing information. A
  //  second pointer is also passed which contains information
  //  as to what message value to send when a particular button is
  //  pressed. All toolbar handles are internally stored within this
  //  class.

  for (i=1; i<=NumButt; i++)
  {
    // Create the Toolbar buttons
    PToolbutt[i] = new TToolbutton(this,                      // this application
				   ButtData.XPos[i],          // X position
				   3,                         // Total num buttons
				   i,                         // Button num
				   ButtData.WParam[i],        // WParam
				   ButtData.LParam[i],        // LParam
				   ButtData.SentWindow[i],    // Window handle to send message to
				   ButtData.StatWindow[i],    // Window handle to send status bar info to
				   ButtData.UpText[i],        // Up text
				   ButtData.DownText[i]);     // Down text

    GetApplication()->MakeWindow(PToolbutt[i]);
  }
}


void TToolbar::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Update the screen display
  DrawScreen(dc);
}


void TToolbar::DrawScreen(HDC TheDC)
{
  BOOL  MadeDC;

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

  // Release DC if we were forced to get our own
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TToolbar::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


LPSTR TToolbar::GetClassName()
{
  return "Toolbar Class";
}


void TToolbar::EvSize(UINT sizeType, TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
}


DEFINE_RESPONSE_TABLE1(TToolbar, TFrameWindow)
    EV_WM_SIZE,
END_RESPONSE_TABLE;
