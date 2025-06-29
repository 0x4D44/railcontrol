/* DEPARTUR.CPP
*  ============
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

/*************************************************************************/
/* Methods of TDepartur follow...*/

TDepartur::TDepartur(PTWindowsObject AParent, int XPos, int YPos)
  : TWindow(AParent, "Departures")
{
  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 250;
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


TDepartur::~TDepartur()
{
  // Free resources
  DeleteObject(HFont);
}


void TDepartur::GetWindowClass(WNDCLASS& WndClass)
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, "DEPART");
}


LPSTR TDepartur::GetClassName()
{
  return "Departure Window";
}


void TDepartur::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}


BOOL TDepartur::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = GetMenu(PMainWindow(Parent)->HWindow);
  CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_UNCHECKED);
  PMainWindow(Parent)->DeparturHan = NULL;

  // Return value
  return TRUE;
}


void TDepartur::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);

  // Check the iconized status of the window
  if (IsIconic(HWindow))
  {
    PMainWindow(Parent)->DeparturIcon = TRUE;
  }
  else
  {
    PMainWindow(Parent)->DeparturIcon = FALSE;
  }
}


void TDepartur::WMMove(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMMove(Msg);

  // Update parent fields
  PMainWindow(Parent)->DeparturX = Attr.X;
  PMainWindow(Parent)->DeparturY = Attr.Y;
}


void TDepartur::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call default paint routine
  TWindow::Paint(PaintDC, PaintInfo);

  // Call main painting routine
  UpdateDisplay(PaintDC, TRUE);
}


void TDepartur::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL    MadeDC;
  RECT    TheRect;
  int     i, XPos, YPos, A;
  int     Dep;
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
    LineTo(TheDC, TheRect.right-4
    , 23);
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
    strcpy(TextString, "Dep:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
    strcpy(TextString, "Type:");
    TextOut(TheDC, 50, 5, TextString, strlen(TextString));
    strcpy(TextString, "Plat:");
    TextOut(TheDC, 90, 5, TextString, strlen(TextString));
    strcpy(TextString, "Description:");
    TextOut(TheDC, 130, 5, TextString, strlen(TextString));
  }

  if (PMainWindow(Parent)->GameInProgress)
  {
    // Now we draw in the timetable information
    for (i=0; i<4; i++)
    {
      Dep = PMainWindow(Parent)->DisplayHan->Depart[i];
      if (Dep > 0)
      {
        if (Redraw || PMainWindow(Parent)->DisplayHan->DepChng[i] ||
	    (PMainWindow(Parent)->DisplayHan->PTimetableInfo[Dep]->GetStatus() != OldStatus[i]))
        {
          TimeTabInfo(TheDC, Dep, 25+17*i);
	}
      }
      else
      {
	TimeTabInfo(TheDC, Dep, 25+17*i);
      }

    }

    // Update the status array
    for (i=0; i<4; i++)
    {
      Dep = PMainWindow(Parent)->DisplayHan->Depart[i];
      if (Dep > 0)
      {
	OldStatus[i] = PMainWindow(Parent)->DisplayHan->PTimetableInfo[Dep]->GetStatus();
      }
      else
      {
	OldStatus[i] = 0;
      }
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TDepartur::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  // This routine draws in the timetable information as specified in
  // <Timetable[TimetabVal]>. It is called exclusively by
  // <UpdateDisplay>

  int     a, b, i;
  char    TextString[21], PlatName[10];
  DWORD   dwColor;
  BOOL    NotFound;

  if (TheDC == 0) return;

  Rectangle(TheDC, 9, YPos, 241, YPos+16);
  SetTextColor(TheDC, RGB(0, 0, 0));

  if (TabVal > 0)
  {
    // Departure time
    ConvertTime(PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepTime(),
		TextString);
    TextOut(TheDC, 10, YPos, TextString, strlen(TextString));

    // Departure code
    a = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepCode();
    if (a == SC_HST) TextOut(TheDC, 55, YPos, "HST", 3);
    if (a == SC_EMU) TextOut(TheDC, 55, YPos, "EMU", 3);
    if (a == SC_DELTIC) TextOut(TheDC, 55, YPos, "DEL", 3);
    if (a == SC_NORMAL) TextOut(TheDC, 55, YPos, "NRM", 3);
    if (a == SC_RELIEF) TextOut(TheDC, 55, YPos, "RLF", 3);
    if (a == SC_DMU1) TextOut(TheDC, 55, YPos, "DMU", 3);
    if (a == SC_PUSHPULL) TextOut(TheDC, 55, YPos, "P/P", 3);
    if (a == SC_CLASS37) TextOut(TheDC, 55, YPos, "37/4", 4);

    // Decide which colour text is to be
    switch (PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus())
    {
      case ST_INPLAT:   SetTextColor(TheDC, RGB(0, 0, 0)); break;
      case ST_RELEASE:  SetTextColor(TheDC, RGB(225, 225, 0)); break;
      case ST_STOCKOK:  SetTextColor(TheDC, RGB(0, 127, 0)); break;
      case ST_READYDEP: SetTextColor(TheDC, RGB(0, 0, 255)); break;
      default: SetTextColor(TheDC, RGB(128, 128, 128)); break;
    }


    a = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepTime();
    if (PMainWindow(Parent)->DisplayHan->WorkTime > 2*(a - 40*int(a/100)))
    {
      SetTextColor(TheDC, RGB(255, 0, 0));
    }

    // Departure platform
    NotFound = TRUE;
    i = 1;
    while (i<50 && NotFound)
    {
      if (PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i] != 0)
      {
	if (PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetTimetable() == TabVal)
        {
          // Get platform name text
	  a = PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
	  PMainWindow(Parent)->DisplayHan->PSelectorInfo[a]->GetText(PlatName, 4);
	  NotFound = FALSE;
        }
      }
      i++;
    }

    if (!NotFound)
    {
      b = PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetStatus();
      if ((b == ST_READYDEP) || (b == ST_STOCKOK))
      {
	// Train is ready to depart
	SetTextAlign(TheDC, TA_CENTER);
	TextOut(TheDC, 100, YPos, PlatName, strlen(PlatName));
	SetTextAlign(TheDC, TA_LEFT);
      }
      else
      {
	// Train is not ready to depart
	strcpy(TextString, "(");
	strcat(TextString, PlatName);
	strcat(TextString, ")");
	SetTextAlign(TheDC, TA_CENTER);
	TextOut(TheDC, 100, YPos, TextString, strlen(TextString));
	SetTextAlign(TheDC, TA_LEFT);
      }
    }

    // Departure description
    dwColor = GetTextColor(TheDC);
    if (dwColor != RGB(128, 128, 128))  SetTextColor(TheDC, RGB(0, 0, 0));
    PMainWindow(Parent)->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString);
    TextOut(TheDC, 130, YPos, TextString, strlen(TextString));
  }
}


void TDepartur::ConvertTime(int TimeVal, char *TimeStr)
{
  // This routine converts an integer time field (eg 657) into a string
  // - eg "06:57" for this example

  int  Hr, Mn;
  char TextString[7], TextStrtwo[7];

  if ((TimeVal < 0) | (TimeVal > 2359))
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



