/* ARRIVALS.CPP
*  ============
*
*  PROGRAM DECSRIPTION
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

#include "classdef.h"

/*************************************************************************/
/* Methods of TArrivals follow...*/

TArrivals::TArrivals(PTWindowsObject AParent, int XPos, int YPos)
  : TWindow(AParent, "Arrivals")
{
  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 600;
  Attr.H = 120;
  Attr.Style = Attr.Style | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX;

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
  HFont = CreateFontIndirect(&Font);
 }


TArrivals::~TArrivals()
{
  // Free resources
  DeleteObject(HFont);
}


void TArrivals::GetWindowClass(WNDCLASS& WndClass)
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, "ARRIVAL");
}


LPSTR TArrivals::GetClassName()
{
  return "Arrival Window";
}


void TArrivals::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}


BOOL TArrivals::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = GetMenu(PMainWindow(Parent)->HWindow);
  CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_UNCHECKED);
  PMainWindow(Parent)->ArrivalHan = NULL;

  // Return value
  return TRUE;
}


void TArrivals::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);

  // Check the iconized status of the window
  if (IsIconic(HWindow))
  {
    PMainWindow(Parent)->ArrivalIcon = TRUE;
  }
  else
  {
    PMainWindow(Parent)->ArrivalIcon = FALSE;
  }
}


void TArrivals::WMMove(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMMove(Msg);

  // Update parent fields
  PMainWindow(Parent)->ArrivalX = Attr.X;
  PMainWindow(Parent)->ArrivalY = Attr.Y;
}


void TArrivals::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call default paint routine
  TWindow::Paint(PaintDC, PaintInfo);

  // Call main painting routine
  UpdateDisplay(PaintDC, TRUE);
}


void TArrivals::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL    MadeDC;
  RECT    TheRect;
  int     i, XPos, YPos, A;
  int     Exp;
  char    TextString[40];

  if (TheDC == 0)
  {
    // We need to create a new DC
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
  {
    MadeDC = FALSE;
  }

  // Draw a bounding box (3D effect)
  GetClientRect(HWindow, &TheRect);

  if ((TheRect.right > 3) && (TheRect.bottom > 3))
  {
    SelectObject(TheDC, GetStockObject(WHITE_PEN));
    MoveTo(TheDC, TheRect.right-4, 23);
    LineTo(TheDC, TheRect.right-4, TheRect.bottom-4);
    LineTo(TheDC, 3, TheRect.bottom-4);
    SelectObject(TheDC, PMainWindow(Parent)->DkGrayPen);
    LineTo(TheDC, 3, 23);
    LineTo(TheDC, TheRect.right-4, 23);
  }

  // Set up text attributes
  SelectObject(TheDC, HFont);
  SelectObject(TheDC, PMainWindow(Parent)->LtGrayPen);
  SelectObject(TheDC, GetStockObject(LTGRAY_BRUSH));
  SetTextColor(TheDC, RGB(0, 0, 0));
  SetBkColor(TheDC, RGB(192, 192, 192));

  // Now draw timetable header information
  if (Redraw)
  {
    strcpy(TextString, "Due:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
    strcpy(TextString, "On:");
    TextOut(TheDC, 50, 5, TextString, strlen(TextString));
    strcpy(TextString, "+:");
    TextOut(TheDC, 80, 5, TextString, strlen(TextString));
    strcpy(TextString, "Loco:");
    TextOut(TheDC, 100, 5, TextString, strlen(TextString));
    strcpy(TextString, "Description:");
    TextOut(TheDC, 150, 5, TextString, strlen(TextString));
    strcpy(TextString, "Status:");
    TextOut(TheDC, 285, 5, TextString, strlen(TextString));
    strcpy(TextString, "Dept:");
    TextOut(TheDC, 415, 5, TextString, strlen(TextString));
    strcpy(TextString, "Description:");
    TextOut(TheDC, 455, 5, TextString, strlen(TextString));
  }

  if (PMainWindow(Parent)->GameInProgress)
  {
    // Now we draw in the timetable information
    if (Redraw | PMainWindow(Parent)->DisplayHan->ExpChng[0])
    {
      Exp = PMainWindow(Parent)->DisplayHan->Expect[0];
      TimeTabInfo(TheDC, Exp, 25);
    }
    if (Redraw | PMainWindow(Parent)->DisplayHan->ExpChng[1])
    {
      Exp = PMainWindow(Parent)->DisplayHan->Expect[1];
      TimeTabInfo(TheDC, Exp, 42);
    }
    if (Redraw | PMainWindow(Parent)->DisplayHan->ExpChng[2])
    {
      Exp = PMainWindow(Parent)->DisplayHan->Expect[2];
      TimeTabInfo(TheDC, Exp, 59);
    }
    if (Redraw | PMainWindow(Parent)->DisplayHan->ExpChng[3])
    {
      Exp = PMainWindow(Parent)->DisplayHan->Expect[3];
      TimeTabInfo(TheDC, Exp, 76);
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TArrivals::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  // This routine draws in the timetable information as specified in
  // <Timetable[TimetabVal]>. It is called exclusively by
  // <UpdateDisplay>

  long    a, LocoVal;
  char    TextString[21], Str2[21], ArrPntName[4];

  if (TheDC == 0) return;

  Rectangle(TheDC, 9, YPos, 561, YPos+16);

  if (TabVal > 0)
  {
    // Arrival time
    ConvertTime(PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetArrTime(),
		TextString);
    TextOut(TheDC, 10, YPos, TextString, strlen(TextString));

    // Arrival line
    a = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetArrPnt();
    PMainWindow(Parent)->DisplayHan->PSelectorInfo[a]->GetText(ArrPntName, 4);
    TextOut(TheDC, 50, YPos, ArrPntName, strlen(ArrPntName));

    // Minutes late
    if (PMainWindow(Parent)->DelayEnable)
    {
      itoa(PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetMinLate(), TextString, 10);
    }
    else
    {
      strcpy(TextString, "");
    }
    TextOut(TheDC, 80, YPos, TextString, strlen(TextString));

    // Loco attached
    a = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetLoco();
    if ((a > 0) && (a < 500))
    {
      // Get loco number as a character array
      PMainWindow(Parent)->DisplayHan->PLocosInfo[a]->LocoNumStr(TextString);
    }
    else
    {
      strcpy(TextString, "XXXXX");
    }
    TextOut(TheDC, 100, YPos, TextString, strlen(TextString));

    // Arrival description
    PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetArrDesc(TextString);
    TextOut(TheDC, 150, YPos, TextString, strlen(TextString));

    // Arrival status
    if (PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_DUE)
    {
      SetTextColor(TheDC, RGB(0, 0, 0));
      sprintf(TextString, "Expected on %s", ArrPntName);
      TextOut(TheDC, 285, YPos, TextString, strlen(TextString));
    }
    if (PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_APPROACH)
    {
      SetTextColor(TheDC, RGB(225, 225, 0));
      sprintf(TextString, "Approaching on %s", ArrPntName);
      TextOut(TheDC, 285, YPos, TextString, strlen(TextString));
    }
    if (PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_HELD)
    {
      SetTextColor(TheDC, RGB(255, 0, 0));
      sprintf(TextString, "Held on %s", ArrPntName);
      TextOut(TheDC, 285, YPos, TextString, strlen(TextString));
    }
    if (PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_HOLBY)
    {
      SetTextColor(TheDC, RGB(255, 0, 0));
      sprintf(TextString, "First held on %s", ArrPntName);
      TextOut(TheDC, 285, YPos, TextString, strlen(TextString));
    }
    if ((PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() >= ST_SETPLAT) &&
	(PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus() <= ST_ARRC))
    {
      SetTextColor(TheDC, RGB(0, 127, 0));
      a = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetRoute();
      PMainWindow(Parent)->DisplayHan->PSelectorInfo[PMainWindow(Parent)->DisplayHan->PRoutesInfo[a]->GetTo()]->GetText(Str2, 20);
      strcpy(TextString, "Set to platform ");
      strcat(TextString, Str2);
      TextOut(TheDC, 285, YPos, TextString, strlen(TextString));
    }

    // Departure time
    SetTextColor(TheDC, RGB(0, 0, 0));
    ConvertTime(PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepTime(),
		TextString);
    TextOut(TheDC, 415, YPos, TextString, strlen(TextString));

    // Departure description
    PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString);
    TextOut(TheDC, 455, YPos, TextString, strlen(TextString));
  }
}


void TArrivals::ConvertTime(int TimeVal, char *TimeStr)
{
  // This routine converts an integer time field (eg 657) into a string
  // - eg "06:57" for this example

  int  Hr, Mn;
  char TextString[7], TextStrtwo[7];

  if ((TimeVal <= 0) | (TimeVal > 2400))
  {
    // Invalid time specified
    strcpy(TextString, "XX:XX");
  }
  else
  { 
    Hr = TimeVal /100;
  
    itoa(Hr, TextStrtwo, 10);
    if (Hr < 10)
    {
      strcpy(TextString, "0");
      strcat(TextString, TextStrtwo);
    }
    else
    {
      strcpy(TextString, TextStrtwo);
    }
    strcat(TextString, ":");

    Mn = TimeVal - 100*Hr;

    if (Mn < 10) strcat(TextString, "0");
    itoa(Mn, TextStrtwo, 10);
    strcat(TextString, TextStrtwo); 
  }

  // Copy time field back to main field
  strcpy(TimeStr, TextString);
}





