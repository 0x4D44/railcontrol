/* SELECTOR.CPP
*  ============
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"  // Most important


/*************************************************************************/
/* Methods of TSelector follow...*/


// Initialize static data memebers
int TSelector::Holdpoint = -1;
int TSelector::Locoyard = -1;
int TSelector::HoldSection = -1;

TSelector::TSelector(PTWindowsObject AParent, int XPos, int YPos, int Width, int Height,
		     int GSelectNum, int GType, int PlatRef, int WPR, int LPR, BOOL En)
 : TWindow(AParent, "")
{
  // Window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = Width;
  Attr.H = Height;
  Attr.Style = WS_CHILD | WS_VISIBLE;

  // Store default window attributes
  DXPos = XPos;
  DYPos = YPos;
  DWidth = Width;
  DHeight = Height;
  SelectNum = GSelectNum;

  // Store the type value: the following codes are used
  // 1: Input line
  // 2: Output line
  // 3: Non-electrified platform (suitable for diesel trains only)
  // 4: Electrified platform (suitable for all trains)
  // 5: Loco-yard
  // 6: Hold point (suitable only for light loco movements)
  Type = GType;
  if (Type == 5)
  {
    if (Holdpoint == -1)
    {
      Holdpoint = SelectNum;
      HoldSection = PlatRef;
    }
    else
    {
      Type = 4;
    }
  }
  if (Type == 6)
  {
    if (Locoyard == -1)   {Locoyard = SelectNum; }  else  {Type = 4;}
  }
  if ((Type == 3) || (Type == 4))  {PlatformRef = PlatRef;}  else  {PlatformRef = 0;}

  // Initialise member variables
  EnableWindow(HWindow, En);
  WPAR = WPR;
  LPAR = LPR;

  // The following cannot be set through the constructor
  SelectCur = TRUE;
  SelectOld = TRUE;
  LockedOut = FALSE;
  LMseButton =  FALSE;

  // Log font record
  Font.lfHeight = 16;
  Font.lfWidth = 0;
  Font.lfEscapement = 0;
  Font.lfOrientation = 0;
  Font.lfWeight = FW_NORMAL;
  Font.lfItalic = 0;
  Font.lfUnderline = 0;
  Font.lfStrikeOut = 0;
  Font.lfCharSet = ANSI_CHARSET;
  Font.lfOutPrecision = OUT_STROKE_PRECIS;
  Font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  Font.lfQuality = PROOF_QUALITY;
  Font.lfPitchAndFamily = FF_DONTCARE;
  strcpy(Font.lfFaceName, "Arial");
}


TSelector::~TSelector()
{
  // Delete objects
  ;
}


void TSelector::GetWindowClass(WNDCLASS& WndClass)
{
  // Call ancestor method
  TWindow::GetWindowClass(WndClass);
  WndClass.style = CS_HREDRAW | CS_VREDRAW;
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, IDI_APPLICATION);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
}


LPSTR TSelector::GetClassName()
{
  return "Selector button";
}


void TSelector::SetupWindow()
{
  // Call ancestor method
  TWindow::SetupWindow();

  // Initialize class
  EnableWindow(HWindow, FALSE);
  SetText("");
}


void TSelector::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call ancestor method
  TWindow::Paint(PaintDC, PaintInfo);

  // Actually paint the button
  PaintButton(PaintDC);
}


void TSelector::PaintButton(HDC TheDC)
{
  HDC   MemDC;
  BOOL  MadeDC;
  HFONT HFont, OldFont;

  // Check to see that we are given a DC
  if (TheDC == 0)
  {
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
    MadeDC = FALSE;

  // Is the button up or down?
  if (SelectCur)
  {
    // Draw the 3D box - for our purposes we have to assume that the parent
    // has a dark gray pen on offer (DkGrayPen)
    SelectObject(TheDC, GetStockObject(WHITE_PEN));
    MoveTo(TheDC, 0, Attr.H-1);
    LineTo(TheDC, 0, 0);
    LineTo(TheDC, Attr.W-1, 0);

    SelectObject(TheDC, PMainWindow(PLayout(Parent)->Parent)->DkGrayPen);
    LineTo(TheDC, Attr.W-1, Attr.H-1);
    LineTo(TheDC, 0, Attr.H-1);

    // Now draw the text on the window
    Font.lfHeight = (8*Attr.H)/10;
    Font.lfWidth = (8*Attr.W)/30;
    HFont = CreateFontIndirect(&Font);
    OldFont = (HFONT) SelectObject(TheDC, HFont);
    SetTextAlign(TheDC, TA_CENTER);
    SetBkColor(TheDC, RGB(192, 192, 192));

    // Choose the text colour
    if (IsWindowEnabled(HWindow))
    {
      SetTextColor(TheDC, RGB(0, 0, 0));
    }
    else
    {
      SetTextColor(TheDC, RGB(128, 128, 128));
    }

    // Draw the text
    TextOut(TheDC, (Attr.W/2), (Attr.H-(8*Attr.H)/10)/2,
	    MainText, strlen(MainText));
    SelectObject(TheDC, OldFont);
    DeleteObject(HFont);
  }
  else
  {
    // Box is in the down position
    SelectObject(TheDC, PMainWindow(PLayout(Parent)->Parent)->DkGrayPen);
    MoveTo(TheDC, 0, Attr.H-1);
    LineTo(TheDC, 0, 0);
    LineTo(TheDC, Attr.W-1, 0);

    SelectObject(TheDC, GetStockObject(WHITE_PEN));
    LineTo(TheDC, Attr.W-1, Attr.H-1);
    LineTo(TheDC, 0, Attr.H-1);

    // Now draw the text on the window
    Font.lfHeight = (8*Attr.H)/10;
    Font.lfWidth = (8*Attr.W)/30;
    HFont = CreateFontIndirect(&Font);
    OldFont = (HFONT) SelectObject(TheDC, HFont);
    SetTextAlign(TheDC, TA_CENTER);
    SetBkColor(TheDC, RGB(192, 192, 192));

    // Choose the text colour
    if (IsWindowEnabled(HWindow))
    {
      SetTextColor(TheDC, RGB(0, 0, 0));
    }
    else
    {
      SetTextColor(TheDC, RGB(128, 128, 128));
    }

    // Draw the text
    TextOut(TheDC, (Attr.W/2)+1, (Attr.H-(8*Attr.H)/10)/2+1,
	    MainText, strlen(MainText));
    SelectObject(TheDC, OldFont);
    DeleteObject(HFont);
  }

  // Release the DC if we obtained one
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TSelector::WMLButtonDown(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMLButtonDown(Msg);

  if (IsWindowEnabled(HWindow))
  {
    // Reset button
    LMseButton = TRUE;
    SelectOld = SelectCur;
    SelectCur = !SelectCur;
    InvalidateRect(HWindow, NULL, TRUE);
    SetCapture(HWindow);

/*    // Update status display
    if (PMainWindow(PToolbar(Parent)->Parent)->StatbarExist)
    {
      if (strcmp("DSJDGFF", DownText) != 0)
      {
	PMainWindow(PToolbar(Parent)->Parent)->StatbarHan->SetText(0, DownText);
      }
    }*/
  }
}


void TSelector::WMLButtonUp(RTMessage Msg)
{
  if (IsWindowEnabled(HWindow))
  {
    // Reset button
    LMseButton = FALSE;
    ReleaseCapture();

    // Send message to main window
    if (SelectCur != SelectOld)
    {
      PaintButton(0);
      PostMessage(PLayout(Parent)->HWindow, WM_COMMAND, WPAR, LPAR);
    }

    InvalidateRect(HWindow, NULL, TRUE);

/*    // Update status display
    if (PMainWindow(PToolbar(Parent)->Parent)->StatbarExist)
    {
      if (strcmp("DSJDGFF", UpText) != 0)
      {
	PMainWindow(PToolbar(Parent)->Parent)->StatbarHan->SetText(0, UpText);
      }
    }*/
  }

  DefWndProc(Msg);
}


void TSelector::WMMouseMove(RTMessage Msg)
{
  int XPos, YPos;

  // Only bother if the left mouse button is down
  if (LMseButton)
  {
    XPos = LOWORD(Msg.LParam);
    YPos = HIWORD(Msg.LParam);

    // Reset button
    if (((XPos < 0) | (XPos > Attr.W)) | ((YPos < 0) | (YPos > Attr.H)))
    {
      if (SelectCur != SelectOld)
      {
        // Mouse has moved out of bounds of the window
	SelectCur = SelectOld;
	InvalidateRect(HWindow, NULL, TRUE);
      }   
    }
    else
    {
      // Mouse has moved back within selector button limits
      if (LMseButton && (SelectCur == SelectOld))
      {
        SelectCur = !SelectOld;
	InvalidateRect(HWindow, NULL, TRUE);
      }
    }
  }
}


void TSelector::Release()
{
  // Release the selector - ie "deselect" it
  SelectOld = FALSE;
  SelectCur = TRUE;
  PaintButton(0);
}


void TSelector::Reset()
{
  // Reset the hold point etc to its default value
  Holdpoint = -1;
  Locoyard = -1;
  HoldSection = -1;
}


void TSelector::SetText(char *TextString)
{
  // Sets the window text to that specified in TextString
  strncpy(MainText, TextString, 4);
}


BOOL TSelector::IsSelected()
{
  // Is the selector selected?
  return SelectCur;
}


void TSelector::GetText(char *TextString, int NumChars)
{
  // Return the button text
  strncpy(TextString, MainText, (NumChars-1));
  TextString[NumChars-1] = 0;
}


int TSelector::GetDXPos()
{
  // Return value
  return DXPos;
}


int TSelector::GetDYPos()
{
  // Return value
  return DYPos;
}


int TSelector::GetDWidth()
{
  // Return value
  return DWidth;
}


int TSelector::GetDHeight()
{
  // Return value
  return DHeight;
}


int TSelector::GetHoldPoint()
{
  // Return the number of the hold point
  return Holdpoint;
}


int TSelector::GetLocoyard()
{
  // Return the number of the loco yard
  return Locoyard;
}


int TSelector::GetHoldSection()
{
  // Return the number of the loco yard
  return HoldSection;
}


int TSelector::GetType()
{
  // Return the section type
  return Type;
}


int TSelector::GetPlatRef()
{
  // Return platform reference
  return PlatformRef;
}


