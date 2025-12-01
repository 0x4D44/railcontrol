// OWLCVT 05/11/95 22:39:46
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

TToolbutton::TToolbutton(TWindow* parent, int XPos, int YPos, int ButtNum, WORD WPAR,
					long LPAR, HWND SentWin, HWND StatWin, const char* UpTxt,
			 const char* DownTxt)
  :TFrameWindow(parent, "")
{
  // Set up toolbutton position
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 24;
  Attr.H = 22;

  // Set window styles
  Attr.Style = WS_CHILD | WS_VISIBLE;

  // The button is initially in the up position and the left mouse
  // button is not selected.
  CopyBuffer(UpText, UpTxt ? UpTxt : "");
  CopyBuffer(DownText, DownTxt ? DownTxt : "");
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
  TFrameWindow::SetupWindow();
}


void TToolbutton::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Draw the button in
  DrawScreen(dc);
}


void TToolbutton::DrawScreen(HDC TheDC)
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

  TDcGuard memDcGuard(CreateCompatibleDC(TheDC));
  HDC MemDC = memDcGuard.Get();
  if (!MemDC)
  {
    return;
  }

  // Get a handle to the parent
  TToolbar* toolbar = TYPESAFE_DOWNCAST(Parent, TToolbar);

  HBITMAP OldBitmap = nullptr;
  int XPos = 0;
  int YPos = 0;

  if (UpPosition)
  {
    OldBitmap = (HBITMAP) SelectObject(MemDC, toolbar->HUpButt.Get());
    XPos = 4;
    YPos = 3;
  }
  else
  {
    OldBitmap = (HBITMAP) SelectObject(MemDC, toolbar->HDownButt.Get());
    XPos = 5;
    YPos = 4;
  }

  BitBlt(TheDC, 0, 0, 24, 22, MemDC, 0, 0, SRCCOPY);

  SelectObject(MemDC, toolbar->ButtBitmap);

  const int MemYPos = Enabled ? 0 : 15;

  BitBlt(TheDC, XPos, YPos, 16, 15, MemDC, (ButtonNum - 1) * 16, MemYPos, SRCCOPY);

  if (OldBitmap)
  {
    SelectObject(MemDC, OldBitmap);
  }
}


void TToolbutton::GetWindowClass(WNDCLASS &WndClass)
{
  // Call ancestor method 
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.style = CS_HREDRAW | CS_VREDRAW;
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
}


auto TToolbutton::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Toolbutton Class"));
}


void TToolbutton::Enable(BOOL GivEnabled)
{
  // Enable or disable the tool button
  Enabled = GivEnabled;
  DrawScreen(0);
}


void TToolbutton::EvLButtonDown(owl::uint modKeys, const TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvLButtonDown(modKeys, pnt);

  if (Enabled)
  {
	 // Reset button
	 LMseButton = TRUE;
	 UpPosition = FALSE;
	 DrawScreen(0);
	 ::SetCapture(HWindow);

	 // Update status display
	 if (StatWindow != 0)
	 {
		::SendMessage(StatWindow,
				 SB_SETTEXT,
				 0,
				 reinterpret_cast<LPARAM>(DownText.Data()));
	 }
  }
}


void TToolbutton::EvLButtonUp(owl::uint, const TPoint&)
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
      ::PostMessage(SentWindow, WM_COMMAND, WParam, LParam);
    }

    // Update status display
    if (StatWindow != 0)
    {
      ::SendMessage(StatWindow,
	          SB_SETTEXT,
	          0,
		  reinterpret_cast<LPARAM>(UpText.Data()));
    }
  }
}


void TToolbutton::EvMouseMove(owl::uint, const TPoint& pnt)
{
  // Reset button
  if (((pnt.x < 0) || (pnt.x > Attr.W)) || ((pnt.y < 0) || (pnt.y > Attr.H)))
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


void TToolbutton::SetDownText(const char* TextString)
{
  // Set the down button text string
      CopyBuffer(DownText, TextString ? TextString : "");
}


void TToolbutton::SetUpText(const char* TextString)
{
  // Set the down button text string
      CopyBuffer(UpText, TextString ? TextString : "");
}




DEFINE_RESPONSE_TABLE1(TToolbutton, TFrameWindow)
    EV_WM_LBUTTONDOWN,
    EV_WM_LBUTTONUP,
    EV_WM_MOUSEMOVE,
END_RESPONSE_TABLE;
