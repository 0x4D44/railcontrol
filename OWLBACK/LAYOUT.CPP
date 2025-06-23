/* LAYOUT.CPP
*  ==========
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

#include "classdef.h"

/*************************************************************************/
/* Methods of TLayout follow...*/


TLayout::TLayout(PTWindowsObject AParent, int XPos, int YPos, int Width, int Height)
  : TWindow(AParent, "")
{
  BOOL Success;
  int  i, j;

  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = Width;
  Attr.H = Height;
  Attr.Style = WS_CHILD | WS_VISIBLE;

  // Load the digital time symbols (ie 1-2-3-4-5-6-7-8-9 etc )
  HTimeSym = LoadBitmap(GetApplication()->hInstance, MAKEINTRESOURCE(HR_DIGITAL));

  // Seed the random number generator
  randomize();

  // Clear all section etc data
  for (i=0; i<1000; i++)  PSectionInfo[i] = 0;
  for (i=0; i<50; i++)    PSelectorInfo[i] = 0;
  for (i=0; i<50; i++)    PPlatDataInfo[i] = 0;
  for (i=0; i<1000; i++)  PRoutesInfo[i] = 0;
  for (i=0; i<500; i++)   PLocosInfo[i] = 0;
  for (i=0; i<200; i++)   PTimetableInfo[i] = 0;
  for (i=0; i<10; i++)    PTrackLoco[i] = 0;

  // Clear text
  for (i=0; i<3; i++)  strcpy(StartText[i], "");
}


TLayout::~TLayout()
{
  int i;

  // Clear any loaded data
  ClearData();

  // Delete time symbol bitmap
  DeleteObject(HTimeSym);
}


void TLayout::ClearData()
{
  int i;

  // Clear any data that may be currently in the structures
  // Clear section information
  for (i=0; i<1000; i++)
  {
    if (PSectionInfo[i] != 0)  delete((PSection) PSectionInfo[i]);
    PSectionInfo[i] = 0;
  }

  // Clear selector information
  for (i=0; i<50; i++)
  {
    if (PSelectorInfo[i] != 0)  delete((PSelector) PSelectorInfo[i]);
    PSelectorInfo[i] = 0;
  }

  // Clear platform information
  for (i=0; i<50; i++)
  {
    if (PPlatDataInfo[i] != 0)  delete((PPlatData) PPlatDataInfo[i]);
    PPlatDataInfo[i] = 0;
  }

  // Clear route information
  for (i=0; i<1000; i++)
  {
    if (PRoutesInfo[i] != 0)  delete((PRoutes) PRoutesInfo[i]);
    PRoutesInfo[i] = 0;
  }

  // Clear loco information
  for (i=0; i<500; i++)
  {
    if (PLocosInfo[i] != 0)  delete((PLocos) PLocosInfo[i]);
    PLocosInfo[i] = 0;
  }

  // Clear timetable information
  for (i=0; i<200; i++)
  {
    if (PTimetableInfo[i] != 0)  delete((PTimetable) PTimetableInfo[i]);
    PTimetableInfo[i] = 0;
  }

  // Clear track loco information
  for (i=0; i<10; i++)
  {
    if (PTrackLoco[i] != 0)  delete((PTimetable) PTrackLoco[i]);
    PTrackLoco[i] = 0;
  }
}


void TLayout::GetWindowClass(WNDCLASS& WndClass)
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, IDI_APPLICATION);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
}


LPSTR TLayout::GetClassName()
{
  return "Layout Class";
}


void TLayout::SetupWindow()
{
  int i;

  // Call ancestor method
  TWindow::SetupWindow();

  for (i=1; i<50; i++)
  {
    if (PSelectorInfo[i] != 0)  PSelectorInfo[i]->Release();
  }

  // Update the selectors
  UpdateSelectors();
}


BOOL TLayout::CanClose()
{
  return TRUE;
}


void TLayout::QuitProgram()
{
  // This is the intialization file saving routine
  ;
}


void TLayout::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);

  // Resize the selectors
  SizeSelectors();
}


void TLayout::WMTimer(RTMessage Msg)
{
  int SkipInt;

  // Handle timer messages
  if (Msg.WParam == ID_TIMER)
  {
    // Increment skip count
    SkipCount++;

    switch (PMainWindow(Parent)->TimerSpeed)
    {
      case 1:  SkipInt = 30; break;
      case 2:  SkipInt = 20; break;
      case 3:  SkipInt = 10;  break;
      default: SkipInt = 20;
    }

    if (SkipCount >= SkipInt)
    {
      WorkTime++;
      if (WorkTime >= 2880) WorkTime = 0;

      // Check to see if we have finished
      if (WorkTime >= StopTime)
      {
        // Pause...
	SendMessage(PMainWindow(Parent)->HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);

        // ...create the dialog and display it...
	GetApplication()->ExecDialog(new TFinish(this, "FINISH"));

	// ...and then stop
	SendMessage(PMainWindow(Parent)->HWindow, WM_COMMAND, CM_MNUFISTOP, 0);
      }
      else
      {
        // Call the 'main' routine - this does a lot of work
	HandleTimeChange();

        // Reset the skip counter
        SkipCount = 0;
      }
    }
  }
}


void TLayout::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call ancestor method
  TWindow::Paint(PaintDC, PaintInfo);
  UpdateDisplay(PaintDC);
}


void TLayout::UpdateDisplay(HDC TheDC)
{
  BOOL    MadeDC;
  RECT    TheRect;
  int     i, XPos, YPos;
  HBRUSH  TheBrush, OldBrush;       // Handles to brushes
  POINT   MSetPoint[4];             // Internal point array
  HDC     MemDC;                    // Copied DC
  HBITMAP HOldBitmap, HLoco1;       // Handle to a bitmap

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
    MoveTo(TheDC, TheRect.right-2, 1);
    LineTo(TheDC, TheRect.right-2, TheRect.bottom-1);
    LineTo(TheDC, 3, TheRect.bottom-1);
    SelectObject(TheDC, PMainWindow(Parent)->DkGrayPen);
    LineTo(TheDC, 3, 1);
    LineTo(TheDC, TheRect.right-2, 1);

    if (PMainWindow(Parent)->GameInProgress)
    {
      // We run through the values in the main <PSectionInfo> array and draw them.
      // However we first scale them all so that they will fit on the screen.
      TSection::SetXScaleFactor(float(TheRect.right)/850.0);
      TSection::SetYScaleFactor(float(TheRect.bottom)/550.0);

      // Draw the sections
      for (i=0; i<1000; i++)
      {
        DrawSection(TheDC, i);
      }

      // Draw the platform status indicators
      TPlatData::SetXScaleFactor(float(TheRect.right)/850.0);
      TPlatData::SetYScaleFactor(float(TheRect.bottom)/550.0);
      for (i=1 ;i<50; i++)
      {
        DrawPlatform(TheDC, i);
      }

      DrawClock(TheDC, TRUE);
    }
    else
    {
      // Stretch the <loco> bitmap to fill the screen
      HLoco1 = LoadBitmap(GetApplication()->hInstance, "LOCO");
      GetClientRect(HWindow, &TheRect);

      MemDC = CreateCompatibleDC(TheDC);
      HOldBitmap = (HBITMAP) SelectObject(MemDC, HLoco1);

      // StretchBlt it
      StretchBlt(TheDC,
		 5,
		 5,
		 (TheRect.right-TheRect.left-10),
		 (TheRect.bottom-TheRect.top-10),
		 MemDC,
		 0,
		 0,
		 150,
		 130,
		 SRCCOPY);
      SelectObject(MemDC, HOldBitmap);

      // Delete compatible DC
      DeleteDC(MemDC);
      DeleteObject(HLoco1);
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::IniVariables()
{
  int i;

  // Initialize all variables
  WorkTime = 0;
  StopTime = 0;
  ShiftLength = 0;
  OldWorkTime = 0;
  TimeNum = 1;
  SkipCount = 0;
  Selector1 = 0;
  Selector2 = 0;
  HoldLoco = 0;
  TrackLocoNum = 0;
  ArrDelay = 0;
  ArrNum = 0;
  OldArrDelay = 1;
  OldArrNum = 1;
  DepDelay = 0;
  DepNum = 0;
  OldDepDelay = 1;
  OldDepNum = 1;

  // Clear all locos in the locoyard
  for (i=0; i<16; i++)  LocoyardLoco[i] = 0;

  // Set all expected/depart trains and changed flags
  for (i=0; i<4; i++)
  {
    Expect[i] = 0;
    ExpChng[i] = TRUE;
    Depart[i] = 0;
    DepChng[i] = TRUE;
  }

  // No routes currently set
  for (i=0; i<4; i++)
    RoutesSet[i] = 0;

  // Clear all occupied platforms
  for (i=1; i<50; i++)
  {
    if (PPlatDataInfo[i] != 0)
    {
      PPlatDataInfo[i]->SetTimetable(0);
      PPlatDataInfo[i]->SetLoco(0);
    }
  }

  // Clear all section occupied flags
  for (i=1; i<1000; i++)
  {
    if (PSectionInfo[i] != 0)  PSectionInfo[i]->SetOccupied(FALSE);
  }

  // Clear text
  for (i=0; i<3; i++)  strcpy(StartText[i], "");
}


BOOL TLayout::StartNew()
{
  MSG  msg;
  int  i, TempArray[8];

  // Start a new game thing
  TSelector::Reset();
  ClearData();
  IniVariables();
  for (i=0; i<8; i++)  TempArray[i] = 0;
  for (i=0; i<10; i++)
  {
    PTrackLoco[i] = new TTimetable("", "", TempArray);
    PTrackLoco[i]->SetStatus(ST_STOCKOK);
  }

  if (!ReadDataFile())
  {
    MessageBox(HWindow,
	       "The data file is either incomplete or inconsistent.\nEnsure that all the necessary data sections are\nincluded. Refer to the online help for more\ninformation.",
	       APPNAME,
	       MB_ICONEXCLAMATION | MB_OK);
    return FALSE;
  }

  // Pass control over to the main running routine
  return TRUE;
}


void TLayout::HandleTimeChange()
{
  HRSRC      TheBeep;    // Handle to resource BEEP.WAV
  HGLOBAL    ResGlobal;
  void far*  ResPoint;
  char       TextString[100];

  // This routine is called every time the time changes and is responsible
  // for ensuring that everything that should be done is done.

  // Draw the time
  strcpy(TextString, "AT DrawClock");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if (PMainWindow(Parent)->GameInProgress)  DrawClock(0, FALSE);

  strcpy(TextString, "AT GetExpecteds");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if (PMainWindow(Parent)->GameInProgress)  GetExpecteds();

  strcpy(TextString, "AT HandleTracking");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if (PMainWindow(Parent)->GameInProgress)  HandleTracking();

  strcpy(TextString, "AT GetDepartures");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if (PMainWindow(Parent)->GameInProgress)  GetDepartures();

  strcpy(TextString, "AT UpdateSelectors");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if (PMainWindow(Parent)->GameInProgress)  UpdateSelectors();

  // Update various windows
  strcpy(TextString, "ATR Arrival");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if ((PMainWindow(Parent)->ArrivalHan != NULL) && (PMainWindow(Parent)->GameInProgress))
    PMainWindow(Parent)->ArrivalHan->UpdateDisplay(0, FALSE);

  strcpy(TextString, "ATR Departure");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if ((PMainWindow(Parent)->DeparturHan != NULL) && (PMainWindow(Parent)->GameInProgress))
    PMainWindow(Parent)->DeparturHan->UpdateDisplay(0, FALSE);

  strcpy(TextString, "ATR Platform");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if ((PMainWindow(Parent)->PlatformHan != NULL) && (PMainWindow(Parent)->GameInProgress))
    PMainWindow(Parent)->PlatformHan->UpdateDisplay(0, FALSE);

  strcpy(TextString, "ATR Locoyard");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  if ((PMainWindow(Parent)->LocoyardHan != NULL) && (PMainWindow(Parent)->GameInProgress))
    PMainWindow(Parent)->LocoyardHan->UpdateDisplay(0, FALSE);

  strcpy(TextString, "ATP sound");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
  // Load the resource and then play the sound if sound is enabled
  if ((PMainWindow(Parent)->SoundEnable) && (PMainWindow(Parent)->GameInProgress))
  {
    TheBeep = FindResource(GetApplication()->hInstance, "BEEP", RT_RCDATA);
    if (TheBeep != NULL)
    {
      ResGlobal = LoadResource(GetApplication()->hInstance, TheBeep);

      ResPoint = LockResource(ResGlobal);
      if (ResPoint != NULL)
      {
	sndPlaySound(LPCSTR(ResPoint), SND_ASYNC | SND_MEMORY);
      }
      FreeResource(ResGlobal);
    }
  }
  strcpy(TextString, "Ready");
  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
}


void TLayout::DrawClock(HDC TheDC, BOOL Redraw)
{
  BOOL    MadeDC, LocalRedraw;
  HDC     MemDC;
  RECT    TheRect;
  int     Val1, Val2, Val3, Val4, Val5;
  int     OldVal1, OldVal2, OldVal3, OldVal4, OldVal5;
  int     XOrigin, YOrigin, Width, Height, XOffset;
  HFONT   HTheFont, HOldFont;


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

  // Draw the clock panel and fill it with the appropriate time
  GetClientRect(HWindow, &TheRect);

  // Create DC and get ready for some bitblt'ing...
  MemDC = CreateCompatibleDC(TheDC);
  SelectObject(MemDC, HTimeSym);

  if (PMainWindow(Parent)->GameInProgress)
  {
    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      // Now we draw the clock time display - firstly draw its bounding box
      XOrigin = 0.2*TheRect.right;
      YOrigin = 0.06*TheRect.bottom;
      Width = 0.15*TheRect.right;
      Height = 0.125*TheRect.bottom;

      // Only draw the background/box if redraw is TRUE
      if (Redraw)
      {
        SelectObject(TheDC, PMainWindow(Parent)->DkGrayPen);
        MoveTo(TheDC, XOrigin, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin);
        LineTo(TheDC, XOrigin+Width, YOrigin);
    
        SelectObject(TheDC, GetStockObject(WHITE_PEN));
        LineTo(TheDC, XOrigin+Width, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin+Height);

        // Fill in the background
        SelectObject(TheDC, GetStockObject(BLACK_BRUSH));
        SelectObject(TheDC, GetStockObject(BLACK_PEN));
        Rectangle(TheDC, XOrigin, YOrigin, XOrigin+Width, YOrigin+Height);
      }

      // Now we get the time from the system...
      Val1 = WorkTime/1200;                                                   
      Val2 = WorkTime/120 - 10*Val1;           

      Val3 = (WorkTime - 120*int(WorkTime/120))/20;
      Val4 = ((WorkTime - 120*int(WorkTime/120)) - 20*Val3)/2;
      Val5 = WorkTime - 2*(WorkTime/2);

      // ...and do the same for the old time values
      OldVal1 = OldWorkTime/1200;
      OldVal2 = OldWorkTime/120 - 10*OldVal1;

      OldVal3 = (OldWorkTime - 120*int(OldWorkTime/120))/20;
      OldVal4 = ((OldWorkTime - 120*int(OldWorkTime/120)) - 20*OldVal3)/2;
      OldVal5 = OldWorkTime -2*(OldWorkTime/2);

      // Update old time values
      OldWorkTime = WorkTime;

      // Val1 and Val2
      if (Redraw || (Val1 != OldVal1))
      {
        if (Val1 != 0)
        {
	  StretchBlt(TheDC, XOrigin, YOrigin, Width/4.5, Height-1,
	    	     MemDC, 0, 252-Val1*23, 13, 23, SRCCOPY);
        }
        else
        {
          StretchBlt(TheDC, XOrigin, YOrigin, Width/4.5, Height-1,
	   	     MemDC, 0, 23, 13, 23, SRCCOPY);
	}
      } 
      if (Redraw || (Val2 != OldVal2))
      {
        StretchBlt(TheDC, XOrigin+2*Width/9, YOrigin, Width/4.5, Height-1,
		   MemDC, 0, 252-Val2*23, 13, 23, SRCCOPY);
      }

      // Now stick the double dot seperator on the screen
      if (Redraw || (OldVal5 != Val5))
      {
        StretchBlt(TheDC, XOrigin+4*Width/9, YOrigin, Width/9, Height-1,
		   MemDC, 3, 275+23*Val5, 6, 23, SRCCOPY);
      }

      // Val3 and Val4
      if (Redraw || (OldVal3 != Val3))
      {
        StretchBlt(TheDC, XOrigin+5*Width/9, YOrigin, Width/4.5, Height-1,
		   MemDC, 0, 252-Val3*23, 13, 23, SRCCOPY);
      }
      if (Redraw || (OldVal4 != Val4))
      {
        StretchBlt(TheDC, XOrigin+7*Width/9, YOrigin, Width/4.5, Height-1,
	           MemDC, 0, 252-Val4*23, 13, 23, SRCCOPY);
      }

      HTheFont = CreateFont(0.06*TheRect.bottom, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
  			    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, (LPSTR)"Arial");
                            
      if (Redraw)
      {
        // Redraw the font items
        HOldFont = (HFONT) SelectObject(TheDC, HTheFont);
        SetTextColor(TheDC, RGB(0, 0, 0));
        SetBkColor(TheDC, RGB(192, 192, 192));
        SetTextAlign(TheDC, TA_CENTER);
        TextOut(TheDC, (0.1375*TheRect.right), (0.8*TheRect.bottom), "Arrival delay:", 14);
        TextOut(TheDC, (0.3375*TheRect.right), (0.8*TheRect.bottom), "Departure delay:", 16);
        SelectObject(TheDC, HOldFont);
      }

      DeleteObject(HTheFont);

      // Now draw in the average arrival delay box
      XOrigin = 0.1*TheRect.right;
      YOrigin = 0.86*TheRect.bottom;
      Width = 0.075*TheRect.right;
      Height = 0.094*TheRect.bottom;

      // Only draw the background/box if redraw is TRUE
      if (Redraw)
      {
        SelectObject(TheDC, PMainWindow(Parent)->DkGrayPen);
        MoveTo(TheDC, XOrigin, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin);
        LineTo(TheDC, XOrigin+Width, YOrigin);

        SelectObject(TheDC, GetStockObject(WHITE_PEN));
        LineTo(TheDC, XOrigin+Width, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin+Height);

        // Fill in the background
        SelectObject(TheDC, GetStockObject(BLACK_BRUSH));
        SelectObject(TheDC, GetStockObject(BLACK_PEN));
        Rectangle(TheDC, XOrigin, YOrigin, XOrigin+Width, YOrigin+Height);
      }

      if (ArrNum > 0)
      {
        // Calculate individual values
        Val1 = int(ArrDelay/(200*ArrNum));
        Val2 = int(ArrDelay/(20*ArrNum)) - 10*Val1;
        Val3 = int(ArrDelay/(2*ArrNum)) - 100*Val1 - 10*Val2;

        // Calculate bitmap X offset - ie whether we use green or red 'LED's
        if ((ArrDelay/ArrNum) <= 10)
        {
          // Use green 'LED's
          XOffset = 13;
        }
        else
        {
          XOffset = 0;
        }
      }
      else
      {
        Val1 = 0;
        Val2 = 0;
        Val3 = 0;
        XOffset = 13;
      }

      // Check to see if we need to redraw
      if ((ArrNum > 0) && (OldArrNum > 0))
      { 
	if (OldArrDelay/(2*OldArrNum) != ArrDelay/(2*ArrNum))
	{
	  LocalRedraw = TRUE;
	}
	else
	{
	  LocalRedraw = FALSE;
	}
      }

      OldArrDelay = ArrDelay;
      OldArrNum = ArrNum;

      // Val1 and Val2
      if (Redraw || LocalRedraw)
      {
        if (Val1 == 0)
        {
          StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 23, 13, 23, SRCCOPY);
        }
        else
        {
	  StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 252-Val1*23, 13, 23, SRCCOPY);
        }

	if ((Val1 == 0) && (Val2 == 0))
        {
          StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 23, 13, 23, SRCCOPY);
        }
        else
        {
          StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 252-Val2*23, 13, 23, SRCCOPY);
        }

	StretchBlt(TheDC, XOrigin+2*Width/3, YOrigin, Width/3, Height-1,
		   MemDC, XOffset, 252-Val3*23, 13, 23, SRCCOPY);
      }

      // Now draw in the average departure delay box
      XOrigin = 0.3*TheRect.right;
      YOrigin = 0.86*TheRect.bottom;
      Width = 0.075*TheRect.right;
      Height = 0.094*TheRect.bottom;

      // Only draw the background/box if redraw is TRUE
      if (Redraw)
      {
        SelectObject(TheDC, PMainWindow(Parent)->DkGrayPen);
        MoveTo(TheDC, XOrigin, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin);
        LineTo(TheDC, XOrigin+Width, YOrigin);

        SelectObject(TheDC, GetStockObject(WHITE_PEN));
        LineTo(TheDC, XOrigin+Width, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin+Height);

        // Fill in the background
        SelectObject(TheDC, GetStockObject(BLACK_BRUSH));
        SelectObject(TheDC, GetStockObject(BLACK_PEN));
        Rectangle(TheDC, XOrigin, YOrigin, XOrigin+Width, YOrigin+Height);
      }


      if (DepNum > 0)
      {
        // Calculate individual values
        Val1 = int(DepDelay/(200*DepNum));
        Val2 = int(DepDelay/(20*DepNum)) - 10*Val1;
        Val3 = int(DepDelay/(2*DepNum)) - 100*Val1 - 10*Val2;

        // Calculate bitmap X offset - ie whether we use green or red 'LED's
        if ((DepDelay/DepNum) <= 10)
        {
          // Use green 'LED's
          XOffset = 13;
        }
        else
        {
          XOffset = 0;
        }
      }
      else
      {
        Val1 = 0;
        Val2 = 0;
        Val3 = 0;
        XOffset = 13;
      }

      // Check to see if we need to redraw
      if ((DepNum > 0) && (OldDepNum > 0))
      { 
	if (OldDepDelay/(2*OldDepNum) != DepDelay/(2*DepNum))
	{
	  LocalRedraw = TRUE;
	}
	else
	{
	  LocalRedraw = FALSE;
	}
      }

      OldDepDelay = DepDelay;
      OldDepNum = DepNum;

      // Val1 and Val2
      if (Redraw || LocalRedraw)
      {
        if (Val1 == 0)
        {
	  StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 23, 13, 23, SRCCOPY);
        }
        else
        {
	  StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 252-Val1*23, 13, 23, SRCCOPY);
        }

	if ((Val1 == 0) && (Val2 == 0))
        {
          StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 23, 13, 23, SRCCOPY);
        }
        else
        {
          StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
		     MemDC, XOffset, 252-Val2*23, 13, 23, SRCCOPY);
        }

	StretchBlt(TheDC, XOrigin+2*Width/3, YOrigin, Width/3, Height-1,
		   MemDC, XOffset, 252-Val3*23, 13, 23, SRCCOPY);
      }
    }
    else
    {
      // Now we draw the clock time display - firstly draw its bounding box
      XOrigin = 0.2*TheRect.right;
      YOrigin = 0.06*TheRect.bottom;
      Width = 0.15*TheRect.right;
      Height = 0.125*TheRect.bottom;

      // Stretch a bitmap
      StretchBlt(TheDC, XOrigin, YOrigin, Width/4.5, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+2*Width/9, YOrigin, Width/4.5, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+5*Width/9, YOrigin, Width/4.5, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+7*Width/9, YOrigin, Width/4.5, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);

      // Now draw in the average arrival delay box
      XOrigin = 0.1*TheRect.right;
      YOrigin = 0.86*TheRect.bottom;
      Width = 0.075*TheRect.right;
      Height = 0.094*TheRect.bottom;

      // Stretch a bitmap
      StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+2*Width/3, YOrigin, Width/3, Height-1,
		 MemDC, 0, 23, 13, 23, SRCCOPY);

      // Now draw in the average departure delay box
      XOrigin = 0.3*TheRect.right;
      YOrigin = 0.86*TheRect.bottom;
      Width = 0.075*TheRect.right;
      Height = 0.094*TheRect.bottom;

      // Stretch a bitmap
      StretchBlt(TheDC, XOrigin, YOrigin, Width/3, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+Width/3, YOrigin, Width/3, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
      StretchBlt(TheDC, XOrigin+2*Width/3, YOrigin, Width/3, Height-1,
	   	 MemDC, 0, 23, 13, 23, SRCCOPY);
    }
  }

  // Delete memory DC
  DeleteDC(MemDC);

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::DrawSection(HDC TheDC, int SectNum)
{
  // This routine is responsible for drawing a section
  // on the screen.

  BOOL   MadeDC;          // Did we make a DC?
  POINT  MSetPoint[4];    // Temporary point array
  HBRUSH OldBrush;        // Old DC brush handle

  // Check to see that the pointer is valid
  if (PSectionInfo[SectNum] == 0)  return;

  // Do we need to create a new DC?
  if (TheDC == 0)
  {
    // ...yes we do 
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
  {
    // ...no we don't
    MadeDC = FALSE;
  }


  // Select pen for drawing outlines
  SelectObject(TheDC, GetStockObject(BLACK_PEN));

  // Decide which brush to use
  if (PSectionInfo[SectNum]->IsOccupied())
  {
    OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->RedBrush);
  }
  else
  {
    OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->GreenBrush);
  } 

  // Get data and draw it
  PSectionInfo[SectNum]->GetSection(MSetPoint);
  Polygon(TheDC, MSetPoint, sizeof(MSetPoint) / sizeof(POINT));

  // Reselect the old brush
  SelectObject(TheDC, OldBrush);

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::DrawPlatform(HDC TheDC, int PlatNum)
{
  // This routine is responsible for drawing a platform status indicator
  // on the screen.

  BOOL   MadeDC;          // Did we make a DC?
  POINT  MSetPoint[4];    // Temporary point array
  HBRUSH OldBrush;        // Old DC brush handle

  // Check to see that the pointer is valid
  if (PPlatDataInfo[PlatNum] == 0)  return;

  // Do we need to create a new DC?
  if (TheDC == 0)
  {
    // ...yes we do 
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
  {
    // ...no we don't
    MadeDC = FALSE;
  }

  // Select pen for drawing outlines
  SelectObject(TheDC, GetStockObject(BLACK_PEN));

  // Decide which brush to use
  if (PMainWindow(Parent)->GameInProgress)
  {
    if (PPlatDataInfo[PlatNum]->GetTimetable() > 0)
    {
      switch (PTimetableInfo[PPlatDataInfo[PlatNum]->GetTimetable()]->GetStatus())
      {
	case ST_INPLAT:   OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->RedBrush); break;
	case ST_RELEASE:  OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->YellowBrush); break;
	case ST_STOCKOK:  OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->RedBrush); break;
	case ST_READYDEP: OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->BlueBrush); break;
	default: OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->GreenBrush); break;
      }
    }
    else
    {
      if (PPlatDataInfo[PlatNum]->GetLoco() > 0)
      {
	OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->RedBrush);
      }
      else
      {
	OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->GreenBrush);
      }
    }
  }
  else
  {
    OldBrush = (HBRUSH) SelectObject(TheDC, PMainWindow(Parent)->GreenBrush);
  }

  // Get platform indicator position data and draw it
  PPlatDataInfo[PlatNum]->GetSection(MSetPoint);
  Polygon(TheDC, MSetPoint, sizeof(MSetPoint) / sizeof(POINT));

  // Reselect the old brush
  SelectObject(TheDC, OldBrush);

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::SizeSelectors()
{
  int   i;
  float SCX, SCY;
  RECT  WinRect;

  // Get parent window client coordinates
  GetClientRect(HWindow, &WinRect);

  // Calculate scaling factors
  SCX = WinRect.right/850.0;
  SCY = WinRect.bottom/550.0;

  for (i=0; i<50; i++)
  {
    // Move each selector window
    if (PSelectorInfo[i] != 0)
    {
      MoveWindow(PSelectorInfo[i]->HWindow,
	         SCX*PSelectorInfo[i]->GetDXPos(),
	         SCY*PSelectorInfo[i]->GetDYPos(),
	         SCX*PSelectorInfo[i]->GetDWidth(),
		 SCY*PSelectorInfo[i]->GetDHeight(),
		 TRUE);
    }    
  }
}


void TLayout::HandleSelectors(RTMessage)
{
  // This function is called every time a selector changes its position from
  // either in to out or vice-versa. When a selector changes position there
  // are two possible states: either the selector is the first to be
  // selected, or it is the second. In either case this procedure disables
  // any conflicting selectors helping to prevent invalid routes being
  // selected.

  // There are now three possibilities - 0, 1 or 2 selectors have been
  // selected.  I should point out at this stage that I fully appreciate
  // the error of my ways when I use the phrase "selectors selected" - the
  // grammatical nastiness of this is realised but for reasons of clarity
  // (and lethargy) I have not changed this. Sorry.
  //
  // When this routine is called there are three possible actions:
  //
  //   1: A single selector has been selected
  //   2: A second selector has been selected
  //   3: A single selector which was selected has been deselected
  //
  // In the first case (1) the field <Selector1> is updated and no further
  // action taken. In the second case (2) a possible route has been
  // selected: this route is then checked and updated. In the third case
  // the selector value stored in <Selector1> is cleared.

  int    a, b, i;
  char   TextString[50];

  // Check to see if we are in a case 1 scenario.
  if (Selector1 == 0)
  {
    // Yes, this is a case 1 scenario. We now search the selectors to find
    // which one has been selected.
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
      {
	if (!PSelectorInfo[i]->IsSelected())  Selector1 = i;
      }
    }
  }
  else
  {
    // Now check to see if this is a case 2 scenario.
    a = 0;
    b = 0;
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
      {
	if (!PSelectorInfo[i]->IsSelected())
        {
    	  if ((a != 0) && (b == 0)) b = i;
      	  if (a == 0) a = i;
	}
      }
    }

    if ((a != 0) && (b != 0))
    {
      // This is a case 2 scenario. Ensure that the first pressed
      // selector is in Selector1
      if (a == Selector1)
      {
        Selector2 = b;
      }
      else
      {
        Selector2 = a;
      }

      // Check to see if the selected route is valid
      if (!IsRouteValid(Selector1, Selector2))
      {
	// Update the status bar
	strcpy(TextString, "Invalid route");
        SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
      }
      PSelectorInfo[Selector1]->Release();
      PSelectorInfo[Selector2]->Release();
      Selector1 = 0;
      Selector2 = 0;
    }

    if ((a == 0) && (b == 0))
    {
      // This is a case 3 scenario.
      Selector1 = 0;
      Selector2 = 0;
    }
  }

  // Now update all the selectors
  UpdateSelectors();
}


void TLayout::UpdateSelectors()
{
  // This routine examines all the selectors and decides which ones can
  // be enabled and which cannot
  int   i, j, ArrivalStatus;
  BOOL  SelectEnable[50], Possible, Found;
  char  TextString[100];

  if (PMainWindow(Parent)->GameInProgress)
  {
    // All selectors are initially enabled
    for (i=1; i<50; i++)  SelectEnable[i] = TRUE;

    // PART 1: Checks to be carried out when an input selector is selected
    if (Selector1 != 0)
    {
      if (PSelectorInfo[Selector1]->GetType() == 1)
      {
	// Determine the status flag of the arriving train/loco
	Found = FALSE;
	for (i=0; i<4; i++)
	{
	  if ((Expect[i] > 0) && !(Found))
	  {
	    if (PTimetableInfo[Expect[i]]->GetArrPnt() == Selector1)
	    {
	      // We have found what we are looking for
	      ArrivalStatus = PTimetableInfo[Expect[i]]->GetArrCode();

	      // Update <Found> as we have found it
	      Found = TRUE;
	    }
	  }
	}

	// If the train arriving is not a light engine then disable the LY
	if ((ArrivalStatus != SC_LIGHTECS) &&
	    (ArrivalStatus != SC_LIGHTNORM) &&
	    (ArrivalStatus != SC_LIGHTDELT) &&
	    (ArrivalStatus != SC_LIGHTRLF) &&
	    (ArrivalStatus != SC_LIGHT37))

	{
	  SelectEnable[TSelector::GetLocoyard()] = FALSE;
	}

	// If the arriving train is a EMU then disable all platforms
	// except those of type 4.
	if (ArrivalStatus == SC_EMU)
	{
	  for (i=1; i<50; i++)
	  {
	    if (PSelectorInfo[i] != 0)
	    {
	      if ((PSelectorInfo[i]->GetType() != 4) &&
		  (PSelectorInfo[i]->GetType() != 1))
		 SelectEnable[i] = FALSE;
	    }
	  }
	}

	// If a input selector is already selected then disable any
	// occupied platforms
	for (i=1; i<50; i++)
	{
	  // Is there a train in the platform?
	  if (PPlatDataInfo[i] != 0)
	  {
	    if (PPlatDataInfo[i]->GetTimetable() > 0)
	    {
	      Possible = FALSE;
	      switch (PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus())
	      {
		  case ST_INPLAT:   SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
		  case ST_RELEASE:  Possible = TRUE; break;
		  case ST_STOCKOK:  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
		  case ST_READYDEP: SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
	      }

	      if (Possible)
	      {
		// Only enable the selector if the arriving loco is a light loco
		if ((ArrivalStatus != SC_LIGHTECS) &&
		    (ArrivalStatus != SC_LIGHTNORM) &&
		    (ArrivalStatus != SC_LIGHTDELT) &&
		    (ArrivalStatus != SC_LIGHTRLF) &&
		    (ArrivalStatus != SC_LIGHT37))
		{
		  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
		}
	      }
	    }

	    // Is there just a locomotive in the platform?
	    if ((PPlatDataInfo[i]->GetTimetable() == 0) && (PPlatDataInfo[i]->GetLoco() != 0))
	    {
	      SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
	    }
	  }

	  // Disable any other input lines
	  if (PSelectorInfo[i] != 0)
	  {
	    if ((PSelectorInfo[i]->GetType() == 1) &&
		(i != Selector1))
	      SelectEnable[i] = FALSE;
          }
	}

	// Disable the hold point
	SelectEnable[TSelector::GetHoldPoint()] = FALSE;
      }


      // If a plaform selector has been selected then disable all other platforms
      if ((PSelectorInfo[Selector1]->GetType() >= 3) &&
	  (PSelectorInfo[Selector1]->GetType() <= 4))
      {
	for (i=1; i<50; i++)
	{
	  if (PSelectorInfo[i] != 0)
	  {
	    if ((i != Selector1) &&
		((PSelectorInfo[i]->GetType() == 1) ||
		 (PSelectorInfo[i]->GetType() == 3) ||
		 (PSelectorInfo[i]->GetType() == 4)))
	      SelectEnable[i] = FALSE;
	  }
	}

	// We may also wish to disable the hold point if the train is not a light
	// locomotive
	if (Selector1 != TSelector::GetLocoyard())
	{
	  if ((PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetTimetable() == 0) &&
	      (PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetLoco() != 0))
	  {
	    if (PLocosInfo[PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetLoco()]->GetFlag() == LF_MAINTAIN)
	    {
	      // Disable the hold point
	      SelectEnable[TSelector::GetHoldPoint()] = FALSE;
	    }
	    else
	    {
	      // Disable any output lines
	      for (i=1; i<50; i++)
	      {
		if (PSelectorInfo[i] != 0)
		{
		  if (PSelectorInfo[i]->GetType() == 2)  SelectEnable[i] = FALSE;
		}
	      }
	    }
	  }
	  else
	  {
	    // Disable the hold point
	    SelectEnable[TSelector::GetHoldPoint()] = FALSE;
	  }
	}
	else
	{
	  // Disable any output lines
	  for (i=1; i<50; i++)
	  {
	    if (PSelectorInfo[i] != 0)
	    {
	      if (PSelectorInfo[i]->GetType() == 2)  SelectEnable[i] = FALSE;
	    }
	  }
	}
      }
      else
      {
	// Disable any output lines
	for (i=1; i<50; i++)
	{
	  if (PSelectorInfo[i] != 0)
	  {
	    if (PSelectorInfo[i]->GetType() == 2)  SelectEnable[i] = FALSE;
	  }
	}
      }

      // If the selector selected is of type Locoyard then disable all
      // selectors apart from the hold point
      if (PSelectorInfo[Selector1]->GetType() == 6)
      {
	for (i=1; i<50; i++)
	{
	  if (PSelectorInfo[i] != 0)
	  {
	    if (i != Selector1)
	    {
	      // Disable input/output selectors
	      if ((PSelectorInfo[i]->GetType() == 1) ||
		  (PSelectorInfo[i]->GetType() == 2))
		SelectEnable[i] = FALSE;

	      // Disable any platform which has a train in it which is not in
	      // the state of requiring a loco
	      if ((PSelectorInfo[i]->GetType() == 3) ||
		  (PSelectorInfo[i]->GetType() == 4))
              {
		if ((PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetLoco() != 0) &&
		    (PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetTimetable() == 0))
  		  SelectEnable[i] = FALSE;

		j = PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetTimetable();
		if ((j > 0) && (j < 200))
		{
		  if (PTimetableInfo[j] != 0)
		  {
		    if (PTimetableInfo[j]->GetStatus() != ST_RELEASE)
		    SelectEnable[i] = FALSE;
		  }
		}
	      }
	    }
	  }
	}
      }
    }


    // PART 2: No selector has been selected
    if (Selector1 == 0)
    {
      // Disable the hold point if the hold point is empty
      if (HoldLoco == 0)
	SelectEnable[TSelector::GetHoldPoint()] = FALSE;

      for (i=1; i<50; i++)
      {
	if (PSelectorInfo[i] != 0)
	{
          // If no selector has been selected then disable the input selectors
	  // ie (UF and US) if no train is due.
	  if (PSelectorInfo[i]->GetType() == 1)  SelectEnable[i] = FALSE;

	  // Disable any output lines
	  if (PSelectorInfo[i]->GetType() == 2)  SelectEnable[i] = FALSE;
	}

	if (PPlatDataInfo[i] != 0)
        {
	  // If no selector has been selected yet then disable all empty platforms
	  if ((PPlatDataInfo[i]->GetTimetable() == 0) &&
	      (PPlatDataInfo[i]->GetLoco() == 0) &&
	      (PPlatDataInfo[i]->GetSelectorRef() != 0))
	  {
	    SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
	  }

	  // If no selector has yet been selected then only disable any platforms
	  // where the train is in the ST_STATUSOK, ST_RELEASE, ST_INPLAT
	  if (PPlatDataInfo[i]->GetTimetable() > 0)
          {
	    j = PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus();
            if ((j == ST_STOCKOK) ||
                (j == ST_RELEASE) || 
		(j == ST_INPLAT))
            {
	      SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
	    }
	  }
	}
      }


      for (i=0; i<4; i++)
      {
	if (Expect[i] > 0)
	{
	  // Check the UF
	  if ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
	      (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD) ||
	      (PTimetableInfo[Expect[i]]->GetStatus() == ST_HOLBY))
	  {
            SelectEnable[PTimetableInfo[Expect[i]]->GetArrPnt()] = TRUE;
	  }
	}
      }
    }

    // If the H line has been selected then disable the UF, US, DF and DS
    if (Selector1 == TSelector::GetHoldPoint())
    {
      for (i=1; i<50; i++)
      {
	if (PSelectorInfo[i] != 0)
	{
	  if (PSelectorInfo[i]->GetType() == 1)  SelectEnable[i] = FALSE;
	  if (PSelectorInfo[i]->GetType() == 2)  SelectEnable[i] = FALSE;
	}
      }

      // Check the platforms to ensure that only valid destinations
      // are allowed
      for (i=1; i<50; i++)
      {
	if (PPlatDataInfo[i] != 0)
        {
	  // Is there a train in the platform?
	  if (PPlatDataInfo[i]->GetTimetable() > 0)
	  {
	    switch (PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus())
	    {
	      case ST_INPLAT:   SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
	      case ST_STOCKOK:  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
	      case ST_READYDEP: SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
	    }
	  }
	  else
	  {
	    // Is there just a loco on its own in the platform?
	    if (PPlatDataInfo[i]->GetLoco() > 0)  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
	  }
	}
      }
    
      // Check to see if refueling is enforced
      if ((PMainWindow(Parent)->LocoRefuel) && (HoldLoco > 0))
      {
        if ((PLocosInfo[HoldLoco]->GetFlag() == LF_NEEDFUEL) ||
	    (PLocosInfo[HoldLoco]->GetFlag() == LF_REFUEL))
	{
	  // Disable platform selectors
	  for (i=1; i<50; i++)
	  {
	    if (PSelectorInfo[i] != 0)
	    {
	      if (PSelectorInfo[i]->GetType() == 3)  SelectEnable[i] = FALSE;
	      if (PSelectorInfo[i]->GetType() == 4)  SelectEnable[i] = FALSE;
	    }
          }
	}
      }
    }

    // Now set the selectors appropriately based on the above information
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
      {
	if (IsWindowEnabled(PSelectorInfo[i]->HWindow) != SelectEnable[i])
        {
          EnableWindow(PSelectorInfo[i]->HWindow, SelectEnable[i]);
	  PSelectorInfo[i]->PaintButton(0);
	}
      }
    }
  }
  else
  {
    // Disable all the selectors
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
	EnableWindow(PSelectorInfo[i]->HWindow, FALSE);
    }
  }
}


BOOL TLayout::IsRouteValid(int Start, int End)
{
  // This function determines if the specified route is valid - if it
  // is then the route is set and TRUE returned. Otherwise FALSE is
  // returned.

  int  i, j, a, b, RouteNo;
  BOOL Available, NotFound;
  char TextString[50];

  // Run through the list of routes to see if the route is valid
  i = 1;
  Available = FALSE;
  while ((i < 1000) && !(Available))
  {
    if (PRoutesInfo[i] != 0)
    {
      if ((Start == PRoutesInfo[i]->GetFrom()) && (End == PRoutesInfo[i]->GetTo()))
      {
        // We have a match - now check to see if this route is available
        // In the case of a route starting from the hold point the route is
        // only valid if the section next to the hold point is occupied.
        // In all other cases all sections have to be clear for the route
        // to be valid.

	Available = TRUE;

	// Check all the sections to see if the route is available
	for (j=0; j<6; j++)
	{
	  PRoutesInfo[i]->GetClear(j, a, b);
	  if (a != 0)
	  {
	    if (PSectionInfo[a]->IsOccupied())  Available = FALSE;
	  }
	  if (b != 0)
	  {
	    if (PSectionInfo[b]->IsOccupied())  Available = FALSE;
	  }
	}
      }
    }

    // Increment i
    i++;
  }

  RouteNo = i - 1;

  // If found then select the route and repaint it
  if (Available)
  {
    BOOL UseLightLoco = FALSE;

    // Decide if we should use a light loco
    if (Start == TSelector::GetHoldPoint())  UseLightLoco = TRUE;
    if (End == TSelector::GetHoldPoint())  UseLightLoco = TRUE;
    if (Start == TSelector::GetLocoyard())  UseLightLoco = TRUE;

    // Does this route start from a platform?
    if ((PSelectorInfo[Start]->GetType() == 3) ||
	(PSelectorInfo[Start]->GetType() == 4))
    {
      // Is there no train in this platform and just a locomotive? If this is
      // the case then we need to create a temporary timetable record to move
      // this locomotive.
      if ((PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetTimetable() == 0) &&
	  (PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco() != 0))
      {
	UseLightLoco = TRUE;
      }
    }

    if (UseLightLoco)
    {
      if (Start == TSelector::GetHoldPoint())
      {
	// Get a light loco
	if (HoldLoco > 0)
	{
	  // Clear the hold point
	  LightLoco(HoldLoco, 0, RouteNo, ST_ARRA);
	}
	else
	{
	  MessageBox(HWindow,
		     "Internal error number ERR000 detected. Please note carefully\n how this error arose and report it to MDSoft.",
		     APPNAME,
		     MB_OK);
	}

	// Reset the hold loco
	HoldLoco = 0;
	HoldLocoChng = TRUE;
      }

      // Does this route start from a platform?
      if ((PSelectorInfo[Start]->GetType() == 3) ||
	  (PSelectorInfo[Start]->GetType() == 4))
      {
	// Is there no train in this platform and just a locomotive? If this is
	// the case then we need to create a temporary timetable record to move
	// this locomotive.
	if ((PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetTimetable() == 0) &&
	    (PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco() != 0))
	{
	  // Get a light loco and clear loco from platform
	  LightLoco(PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco(), 0, RouteNo, ST_ARRA);
	  PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->SetLoco(0);
	  PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->SetRedraw(TRUE);
	  DrawPlatform(0, PSelectorInfo[Start]->GetPlatRef());

	  // Set the hold section if to point is the hold point
	  if (End == TSelector::GetHoldPoint())
	  {
	    PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(TRUE);
	    DrawSection(0, TSelector::GetHoldSection());
	  }
	}
      }

      // Did this route start at the locoyard? - if so then we need to determine
      // which loco to use.
      if (Start == TSelector::GetLocoyard())
      {
	// Firstly ensure that the locoyard window exists
	if (PMainWindow(Parent)->LocoyardHan == NULL)
	  SendMessage(PMainWindow(Parent)->HWindow, WM_COMMAND, CM_WINLOCOYA, 0);

	// Then ensure that it is maximized
	if (PMainWindow(Parent)->LocoyardIcon)
	  OpenIcon(PMainWindow(Parent)->LocoyardHan->HWindow);

	// Check that there is at least one loco which is not refuelling
	// in the locoyard
	i=0;
	NotFound = TRUE;
	while ((i<16) && NotFound)
	{
	  // Find a filled locoyard slot
	  if (LocoyardLoco[i] != 0)
	  {
	    if (PLocosInfo[LocoyardLoco[i]]->GetFlag() == LF_LOCOYARD)
	    {
	      NotFound = FALSE;
	    }
	  }
	  i++;
	}

	if (NotFound)
	{
	  // Update the status bar
	  strcpy(TextString, "No locos are currently available");
	  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	  // Prevent route being drawn
	  return TRUE;
	}
	else
	{
	  // Update the main window status bar
	  if (PMainWindow(Parent)->StatbarHan != NULL)
	  {
	    strcpy(TextString, "Select a locomotive from the loco-yard");
	    SendMessage(PMainWindow(Parent)->StatbarHan->HWindow,
			SB_SETTEXT,
			0,
			(LPARAM) ((LPSTR) TextString));
	  }

	  // Now set focus to the locoyard window
	  SetFocus(PMainWindow(Parent)->LocoyardHan->HWindow);

	  // Now run the get loco routine
	  PMainWindow(Parent)->LocoyardHan->WhichLoco(RouteNo);

	  // Set the hold point section as being occupied if a route has
	  // been found
	  if (End == TSelector::GetHoldPoint())
	  {
	    PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(TRUE);
	    DrawSection(0, TSelector::GetHoldSection());
	  }
	}
      }

    }
    else
    {
      // Update the <RoutesSet> array but only if the route did not
      // start from the hold point
      i = 0;
      while ((RoutesSet[i] > 0) && (i < 3)) i++;
      if (RoutesSet[3] != 0)
      {
	MessageBox(HWindow,
		   "Internal error number ERR001 detected. Please note carefully\n how this error arose and report it to MDSoft.",
		   APPNAME,
		   MB_OK);

	char  RDump[100];
	wsprintf(RDump, "Route dump - 0:%d 1:%d 2:%d 3:%d", RoutesSet[0], RoutesSet[1],
		 RoutesSet[2], RoutesSet[3]);
	MessageBox(HWindow, RDump, APPNAME, MB_OK);
      }
      else
      {
	RoutesSet[i] = RouteNo;
      }
    }

    // Repaint the route
    for (j=0; j<6; j++)
    {
      PRoutesInfo[RouteNo]->GetClear(j, a, b);
      if (a != 0)
      {
	PSectionInfo[a]->SetOccupied(TRUE);
	DrawSection(0, a);
      }
      if (b != 0)
      {
	PSectionInfo[b]->SetOccupied(TRUE);
	DrawSection(0, b);
      }
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


BOOL TLayout::ReadDataFile()
{
  // This routine reads an input datafile and copies the data to the
  // appropriate structures
  fstream   infile;                // Input filestream
  char      szInput[100],          // Input string
	    szText1[200],          // Temporary string 1
	    szText2[200];          // Temporary string 2
  int       IndexRef, i;           // General usage
  int       TempX[4], TempY[4];    // Temporary array
  long      TempClear[6];          // Another temporary array
  int       TempTime[10];          // Yet another temporary array
  BOOL      Error;		   // Has an error occured?


  // Open the file
  infile.open(PMainWindow(Parent)->DataFileName, ios::in);
  if (!infile)
  {
    MessageBeep(MB_ICONHAND);
    MessageBox(HWindow,
	       "Cannot open datafile",
	       APPNAME,
	       MB_OK | MB_ICONSTOP);
    return FALSE;
  }
  else
  {
    // We have opened the file. The first thing that we look for is section
    // information ie [SECTIONS].
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[SECTIONS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[SECTIONS]") != 0)
    {
      // No information in the datafile of the [SECTIONS] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[SECTIONS]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the section information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 1000))
	{
	  if (PSectionInfo[IndexRef] == 0)
	  {
	    // Now get point information
            for (i=0; i<4; i++)
            {
  	      ins.getline(szInput, 100, ',');
	      TempX[i] = atoi(szInput);
      	      ins.getline(szInput, 100, ',');
	      TempY[i] = atoi(szInput);
            }

	    // Insert the new section
	    PSectionInfo[IndexRef] = new TSection(IndexRef, TempX, TempY);
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[SECTIONS]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
  	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[SECTIONS]", "1", "999");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }


    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [PLATFORMS] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[PLATFORMS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[PLATFORMS]") != 0)
    {
      // No information in the datafile of the [PLATFORMS] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[PLATFORMS]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 50))
	{
	  if (PPlatDataInfo[IndexRef] == 0)
	  {
	    // Now get point information
            for (i=0; i<4; i++)
            {
	      ins.getline(szInput, 100, ',');
	      TempX[i] = atoi(szInput);
	      ins.getline(szInput, 100, ',');
	      TempY[i] = atoi(szInput);
	    }

	    // Insert the new section
	    PPlatDataInfo[IndexRef] = new TPlatData(TempX, TempY);
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[PLATFORM]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
  	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[PLATFORM]", "1", "49");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }


    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [SELECTOR] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[SELECTOR]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[SELECTOR]") != 0)
    {
      // No information in the datafile of the [SELECTOR] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[SELECTOR]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 50))
	{
	  if (PSelectorInfo[IndexRef] == 0)
	  {
	    // Now get point information
            for (i=0; i<4; i++)
            {
  	      ins.getline(szInput, 100, ',');
	      TempX[i] = atoi(szInput);
	    }
	    for (i=0; i<2; i++)
            {
	      ins.getline(szInput, 100, ', ');
	      TempY[i] = atoi(szInput);
            }

	    if ((TempY[0] == 3) || (TempY[0] == 4))
	    {
	      if ((TempY[1] < 1) || (TempY[1] > 49))
	      {
		// Ensure that the platform index reference value is within limits
                ClearData();
		LoadString(GetApplication()->hInstance, 10010, szText1, 200);
                MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	      }
	      else
	      {
		if (PPlatDataInfo[TempY[1]] == 0)
		{
		  // Selector item references an undefined platform
                  ClearData();
		  LoadString(GetApplication()->hInstance, 10011, szText1, 200);
                  MessageBeep(MB_ICONEXCLAMATION);
		  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
		  return FALSE;
		}
		else
		{
		  // Set the platform selector reference
		  PPlatDataInfo[TempY[1]]->SetSelectorRef(IndexRef);
                }
	      }
	    }

	    // Insert the new selector
	    PSelectorInfo[IndexRef] = new TSelector(this, TempX[0], TempX[1], TempX[2], TempX[3], IndexRef, TempY[0], TempY[1], CM_SELECT, 0, 1);
	    GetApplication()->MakeWindow(PSelectorInfo[IndexRef]);
	    ins.getline(szInput, 100, ',');
	    PSelectorInfo[IndexRef]->SetText(szInput);
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[SELECTOR]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
  	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[SELECTOR]", "1", "49");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }


    // Now check the platform information to ensure that each platform
    // has been associated with a selector
    for (i=1; i<50; i++)
    {
      if (PPlatDataInfo[i] != 0)
      {
	if (PPlatDataInfo[i]->GetSelectorRef() == 0)
	{
	  // Platform <i> is not associated with a selector
          ClearData();
	  LoadString(GetApplication()->hInstance, 10012, szText2, 200);
	  wsprintf(szText1, szText2, i);
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}
      }
    }

    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [ROUTES] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[ROUTES]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[ROUTES]") != 0)
    {
      // No information in the datafile of the [ROUTES] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[ROUTES]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 1000))
	{
	  if (PRoutesInfo[IndexRef] == 0)
	  {
	    // Now get point information
            for (i=0; i<2; i++)
            {
	      ins.getline(szInput, 100, ',');
	      TempX[i] = atoi(szInput);
	    }
	    for (i=0; i<6; i++)
            {
	      ins.getline(szInput, 100, ',');
	      TempClear[i] = atol(szInput);
	    }

	    // Insert the new section
	    PRoutesInfo[IndexRef] = new TRoutes(TempX[0], TempX[1], TempClear);

	    // Now check the section data
	    if ((PSelectorInfo[TempX[0]] == 0) ||
		(PSelectorInfo[TempX[1]] == 0))
	    {
	      // OK, there is a problem so stop
	      ClearData();
	      LoadString(GetApplication()->hInstance, 10005, szText2, 200);
	      wsprintf(szText1, szText2, IndexRef);
              MessageBeep(MB_ICONEXCLAMATION);
	      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	      return FALSE;
	    }

	    for (i=0; i<6; i++)
	    {
	      PRoutesInfo[IndexRef]->GetClear(i, TempX[0], TempX[1]);
	      if (((TempX[0] != 0) &&
		   (PSectionInfo[TempX[0]] == 0)) ||
		  ((TempX[1] != 0) &&
		   (PSectionInfo[TempX[1]] == 0)))
	      {
		// OK there is a problem so stop
		ClearData();
		LoadString(GetApplication()->hInstance, 10004, szText2, 200);
		wsprintf(szText1, szText2, IndexRef, i+1);
                MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	      }
	    }
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[ROUTES]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
  	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[ROUTES]", "1", "999");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }

    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [LOCOS] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[LOCOS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[LOCOS]") != 0)
    {
      // No information in the datafile of the [LOCOS] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[LOCOS]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafi  strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 500))
	{
	  if (PLocosInfo[IndexRef] == 0)
	  {
	ÐÿÐÿÐÿÐÿ    // Now get point information
            for (i=0; i<3; i++)
            {
	      ins.getline(szInput, 100, ',');
	      TempX[i] = atoi(szInput);
	    }

	    // Insert the new loco item
	    PLocosInfo[IndexRef] = new TLocos(TempX[0], TempX[1], TempX[2]);
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[LOCOS]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
  	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[LOCOS]", "1", "499");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }



    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [TIMETABLE] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[TIMETABLE]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[TIMETABLE]") != 0)
    {
      // No information in the datafile of the [TIMETABLE] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[TIMETABLE]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the timetable information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, ',');
	IndexRef = atoi(szInput);

	if ((IndexRef > 0) && (IndexRef < 200))
	{
	  if (PTimetableInfo[IndexRef] == 0)
	  {
	    // Now get timetable information
            ins.getline(szText1, 50, ',');
	    ins.getline(szText2, 50, ',');
	    for (i=0; i<9; i++)
	    {
	      TempTime[i] = 0;
	      ins.getline(szInput, 100, ',');
	      TempTime[i] = atoi(szInput);
	    }

	    // Check to ensure that the selector number is an input selector
            Error = TRUE;
	    if ((TempTime[0] > 0) && (TempTime[0] < 50))
            {
	      if (PSelectorInfo[TempTime[0]] != 0)
	      {
		if (PSelectorInfo[TempTime[0]]->GetType() == 1)  Error = FALSE;
	      }
            }
	    if (Error)
	    {
	      // A selector is not of the required input type
	      ClearData();
	      LoadString(GetApplication()->hInstance, 10006, szText2, 200);
	      wsprintf(szText1, szText2, IndexRef);
	      MessageBeep(MB_ICONEXCLAMATION);
	      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	      return FALSE;
	    }

	    // Check to ensure that the value of the reassignable loco field is valid
	    if ((TempTime[8] < 0) || (TempTime[8] >= 200))
	    {
	      ClearData();
	      LoadString(GetApplication()->hInstance, 10007, szText2, 200);
	      wsprintf(szText1, szText2, "1", "199", IndexRef);
	      MessageBeep(MB_ICONEXCLAMATION);
	      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	      return FALSE;
	    }

	    // Insert the new timetable item
	    PTimetableInfo[IndexRef] = new TTimetable(szText1, szText2, TempTime);


	    // Now check to see if the loco is still attached to train
  	    if (PTimetableInfo[IndexRef]->GetStatus() == ST_INPLAT)
	    {
  	      // Assign a loco and update its status
	      if (!AssignLoco(IndexRef))
	      {
                ClearData();
		return FALSE;
              }
	      PLocosInfo[PTimetableInfo[IndexRef]->GetLoco()]->SetFlag(LF_NEEDFUEL);

	      // Which platform is the train in?
	      if ((PTimetableInfo[IndexRef]->GetLocoPlatFlag() > 0) &&
	 	  (PTimetableInfo[IndexRef]->GetLocoPlatFlag() < 15))
		 PPlatDataInfo[PTimetableInfo[IndexRef]->GetLocoPlatFlag()]->SetTimetable(IndexRef);

	      // Calculate departure time
    	      PTimetableInfo[IndexRef]->SetRelTime(NormTime(PackTime(PTimetableInfo[IndexRef]->GetRelTime()) + PackTime(PTimetableInfo[IndexRef]->GetArrTime())));
	    }

	    // Loco is no longer attached to train and train is ready to depart
	    if (PTimetableInfo[IndexRef]->GetStatus() == ST_STOCKOK)
	    {
	      // Initially assign a loco to the train
	      if (!AssignLoco(IndexRef))
	      {
                ClearData();
		return FALSE;
              }
	      if ((PTimetableInfo[IndexRef]->GetLocoPlatFlag() > 0) &&
		  (PTimetableInfo[IndexRef]->GetLocoPlatFlag() < 15))
              {
	        PPlatDataInfo[PTimetableInfo[IndexRef]->GetLocoPlatFlag()]->SetTimetable(IndexRef);

	        if ((PTimetableInfo[IndexRef]->GetDepCode() != SC_EMU) &&
		    (PTimetableInfo[IndexRef]->GetDepCode() != SC_HST) &&
		    (PTimetableInfo[IndexRef]->GetDepCode() != SC_PUSHPULL) &&
		    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU1))
                {
		  PPlatDataInfo[PTimetableInfo[IndexRef]->GetLocoPlatFlag()]->SetLoco(PTimetableInfo[IndexRef]->GetLoco());
		  PLocosInfo[PTimetableInfo[IndexRef]->GetLoco()]->SetFlag(LF_NEEDFUEL);
		  PTimetableInfo[IndexRef]->SetLoco(0);

	          // Now assign a second loco to the train
		  PTimetableInfo[IndexRef]->SetArrCode(PTimetableInfo[IndexRef]->GetDepCode());
		  if (!AssignLoco(IndexRef))
		  {
		    ClearData();
		    return FALSE;
		  }
		}
	      }
	    }
	  }
	  else
	  {
	    // Duplicate reference number
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10003, szText2, 200);
	    wsprintf(szText1, szText2, "[TIMETABLE]", IndexRef);
	    MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	    return FALSE;
	  }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[TIMETABLE]", "1", "199");
	  MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }

    // Now check all the reassignable loco fields
    for (i=1;i<200;i++)
    {
      if (PTimetableInfo[i] != 0)
      {
	if (PTimetableInfo[i]->GetNextTimeTabPos() != 0)
        {
	  if (PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()] == 0)
	  {
	    ClearData();
	    LoadString(GetApplication()->hInstance, 10008, szText2, 200);
	    wsprintf(szText1, szText2, i);
	    MessageBeep(MB_ICONEXCLAMATION);
	    MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	    return FALSE;
	  }
	}
      }
    }

    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [GENERAL] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[GENERAL]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[GENERAL]") != 0)
    {
      // No information in the datafile of the [GENERAL] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[GENERAL]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the general information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first line of text
	istrstream ins(szInput, strlen(szInput));
	ins.getline(szInput, 100, '=');

	if (strcmp(szInput, "StartTime") == 0)
	{
	  // Found start time
	  ins.getline(szInput, 20, ',');
	  WorkTime = PackTime(atoi(szInput));
	}
	if (strcmp(szInput, "StopTime") == 0)
	{
	  // Found stop time
	  ins.getline(szInput, 20, ',');
	  StopTime = PackTime(atoi(szInput));
	}
	if (strcmp(szInput, "StartText1") == 0)
	{
	  // Found <StartText[0]>
	  ins.getline(szInput, 100, ',');
	  strcpy(StartText[0], szInput);
        }
	if (strcmp(szInput, "StartText2") == 0)
	{
	  // Found <StartText[1]>
	  ins.getline(szInput, 100, ',');
	  strcpy(StartText[1], szInput);
        }
	if (strcmp(szInput, "StartText3") == 0)
	{
	  // Found <StartText[2]>
	  ins.getline(szInput, 100, ',');
	  strcpy(StartText[2], szInput);
        }

	// Get the next line of data
	infile.getline(szInput, 100);
      }

      // Check that we achieved something
      if ((WorkTime == 0) ||
	  (StopTime == 0) ||
	  (WorkTime >= StopTime))
      {
	// Generate an error - data is invalid as some is missing
        ClearData();
	LoadString(GetApplication()->hInstance, 10000, szText2, 200);
	wsprintf(szText1, szText2, "[GENERAL]");
        MessageBeep(MB_ICONEXCLAMATION);
	MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	return FALSE;
      }
      else
      {
	// Calculate the length of the shift (in hours)
	ShiftLength = (NormTime(StopTime)-NormTime(WorkTime))/100;
      }
    }



    // Reset the file
    infile.close();
    infile.open(PMainWindow(Parent)->DataFileName, ios::in);

    // Now we look for [LOCOYARD] information
    strcpy(szInput, "");
    while ((strcmpi(szInput, "[LOCOYARD]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, 100);
    }

    if (strcmpi(szInput, "[LOCOYARD]") != 0)
    {
      // No information in the datafile of the [LOCOYARD] type
      ClearData();
      LoadString(GetApplication()->hInstance, 10001, szText2, 200);
      wsprintf(szText1, szText2, "[LOCOYARD]");
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      strcpy(szInput, "");
      infile.getline(szInput, 100);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
	     (szInput[0] != 0) && (!infile.eof()))
      {     
	// Get first part of string - index reference
	istrstream ins(szInput, strlen(szInput));
	IndexRef = 0;

	if (IndexRef < 15)
	{
	  // Now get locoyard information
	  for (i=0; i<2; i++)
	  {
	    ins.getline(szInput, 100, ',');
	    TempX[i] = atoi(szInput);
	  }
	  if (((TempX[0] > 0) && (TempX[0] < 7)) ||
	      ((TempX[0] > 9) && (TempX[0] < 13)))
	  {
	    if ((TempX[1] >= 0) && (TempX[1] < 60))
	    {
	      // Assign the loco
	      AssignYardLoco(TempX[0], WorkTime+TempX[1]);
	      IndexRef++;
	    }
          }
	}
	else
	{
	  // Out of range
	  ClearData();
	  LoadString(GetApplication()->hInstance, 10002, szText2, 200);
	  wsprintf(szText1, szText2, "[LOCOYARD]", "1", "20");
          MessageBeep(MB_ICONEXCLAMATION);
	  MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
	  return FALSE;
	}

	// Get the next line of data
	infile.getline(szInput, 100);
      }
    }
  }

  // If we get here then all is well (hopefully!)
  return TRUE;
}


void TLayout::AssignYardLoco(int LocoFlag, int RefuelTime)
{
  int  FreeLocos[500];
  int  a, b, i;
  BOOL Found, NotFound;

  // This routine assigns locos to the locoyard

  a = 0;
  Found = FALSE;
  for (i=1; i<500; i++)
  {
    if (PLocosInfo[i] != 0)
    {
      if ((PLocosInfo[i]->GetType() == LocoFlag) && (PLocosInfo[i]->GetFlag() == LF_UNASSIGN))
      {
        FreeLocos[a] = i;
        if (a < 100) a++;
        Found = TRUE;
      }
    }
  }

  // We have found at least one locomotive
  if (Found)
  {
    // Select one of the locos
    b = FreeLocos[random(a)];

    // Is the loco refueling and is refueling enabled?
    if (PMainWindow(Parent)->LocoRefuel)
    {
      if (RefuelTime == 0)
      {
	PLocosInfo[b]->SetFlag(LF_LOCOYARD);  // LF_LOCOYARD (7)
      }
      else
      {
	PLocosInfo[b]->SetFlag(LF_REFUEL);   // LF_REFUEL (6)
	PLocosInfo[b]->SetRefuelTime(RefuelTime);
      }
    }
    else
    {
      PLocosInfo[b]->SetFlag(LF_LOCOYARD);
    }

    // Update the <LocoyardLoco> array
    i=0;
    NotFound = TRUE;
    while ((i<16) && NotFound)
    {
      // Find an empty locoyard slot
      if (LocoyardLoco[i] == 0)  NotFound = FALSE;
      i++;
    }

    if ((i == 16) && (LocoyardLoco[15] != 0))
    {
      MessageBox(HWindow,
		 "Internal error number ERR002 detected. Please note carefully\n how this error arose and report it to MDSoft.",
		 APPNAME,
		 MB_OK);
    }
    else
    {
      // Assign the loco to the <LocoyardLoco> array
      i--;
      LocoyardLoco[i] = b;
    }
  }
}


BOOL TLayout::AssignLoco(int TimetabPos)
{
  // This routine is responsible for assigning locomotives to trains
  int  FreeLoco[100];
  int  GivenType;
  int  a, z;
  char Str1[100], Str2[100];

  // Translate loco types
  GivenType = PTimetableInfo[TimetabPos]->GetArrCode();
  if (GivenType == SC_LIGHTECS)   GivenType = SC_ECS;
  if (GivenType == SC_LIGHTNORM)  GivenType = SC_NORMAL;
  if (GivenType == SC_LIGHTDELT)  GivenType = SC_DELTIC;
  if (GivenType == SC_LIGHTRLF)   GivenType = SC_RELIEF;
  if (GivenType == SC_LIGHT37)    GivenType = SC_CLASS37;

  // Work out which locos of the appropriate type are free.
  a = 0;
  for (z=1;z<500;z++)
  {
    if (PLocosInfo[z] != 0)
    {
      if ((PLocosInfo[z]->GetType() == GivenType) && (PLocosInfo[z]->GetFlag() == LF_UNASSIGN))
      {
	FreeLoco[a] = z;
	if (a < 100) a++;
      }
    }
  }

  // Select a loco from the array
  if (a > 0)
  {
    PTimetableInfo[TimetabPos]->SetLoco(FreeLoco[random(a)]);

    // Update the fact that the loco is now assigned
    PLocosInfo[PTimetableInfo[TimetabPos]->GetLoco()]->SetFlag(LF_ASSIGNED);

    return TRUE;
  }
  else
  {
    // Stop
    SendMessage(PMainWindow(Parent)->HWindow, WM_COMMAND, CM_MNUFISTOP, 0);

    // ...display error box...
    strcpy(Str1, "An error has occurred: Rail Control was unable to\nassign a loco to a train. The loco type required is: ");
    itoa(GivenType, Str2, 10);
    strcat(Str1, Str2);
    strcat(Str1, ".");
    MessageBox(HWindow, Str1, APPNAME, MB_ICONHAND | MB_OK);

    return FALSE;
  }
}


void TLayout::GetExpecteds()
{
  // This routine retrieves the timetable numbers of the next four expected
  // trains.

  int   i, mintime, OldExpect[4];

  // Reset everything
  for (i=0; i<4; i++)
  {
    OldExpect[i] = Expect[i];
    Expect[i] = 0;
    ExpChng[i] = FALSE;
  }

  // Read the timetable - 
  // run through the timetable data looking for first train expected...
  mintime = 2500;
  for (i=1; i<200; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      if ((CalcArrivalTime(i) <= mintime) &&
	  (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	  (PTimetableInfo[i]->GetStatus() <= ST_ARRB))
      {
        // Get new minimum value
	mintime = CalcArrivalTime(i);
	Expect[0] = i;
      }
    }
  }

  // ...and now the second... 
  if ((Expect[0] > 0) && (Expect[0] < 200))
  {
    mintime = 2500;
    for (i=1; i<200; i++)
    {
      if (PTimetableInfo[i] != 0)
      {
	if ((CalcArrivalTime(i) <= mintime) &&
            (i != Expect[0]) &&
	    (CalcArrivalTime(i) >= CalcArrivalTime(Expect[0])) &&
	    (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	    (PTimetableInfo[i]->GetStatus() <= ST_ARRB))
	{
          // Get new minimum value
	  mintime = CalcArrivalTime(i);
          Expect[1] = i;
	}
      }
    }

    if ((Expect[1] > 0) && (Expect[1] < 200))
    {
      // ...and the third...
      mintime = 2500;
      for (i=1; i<200; i++)
      {
	if (PTimetableInfo[i] != 0)
        {
	  if ((CalcArrivalTime(i) <= mintime) &&
	      (i != Expect[0]) &&
              (i != Expect[1]) &&
	      (CalcArrivalTime(i) >= CalcArrivalTime(Expect[1])) &&
	      (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	      (PTimetableInfo[i]->GetStatus() <= ST_ARRB))
	  {
	    // Get new minimum value
	    mintime = CalcArrivalTime(i);
	    Expect[2] = i;
	  }
	}
      }

      if ((Expect[2] > 0) && (Expect[2] < 200))
      {
        // ...and finally the fourth.
        mintime = 2500;
	for (i=1; i<200; i++)
	{
	  if (PTimetableInfo[i] != 0)
          {
	    if ((CalcArrivalTime(i) <= mintime) &&
	        (i != Expect[0]) &&
	        (i != Expect[1]) &&
	        (i != Expect[2]) &&
		(CalcArrivalTime(i) >= CalcArrivalTime(Expect[2])) &&
		(PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
		(PTimetableInfo[i]->GetStatus() <= ST_ARRB))
	    {
	      // Get new minimum value
	      mintime = CalcArrivalTime(i);
	      Expect[3] = i;
	    }
          }
        }
      }
    }
  }

  // Check to see if value has changed since previous time...
  if (Expect[0] != OldExpect[0]) ExpChng[0] = TRUE;
  if (Expect[1] != OldExpect[1]) ExpChng[1] = TRUE;
  if (Expect[2] != OldExpect[2]) ExpChng[2] = TRUE;
  if (Expect[3] != OldExpect[3]) ExpChng[3] = TRUE;

  // Now we can calculate whether the train is due, approaching, or held.
  // The following codes are used:
  //
  //  1: Due on UF
  //  2: Approaching on UF
  //  3: Held on UF
  //  4: Held at Holby (UF)
  //  5: Set to platform
  //  6: Stage A of approach
  //  7: Stage B of approach
  //  8: Stage C of approach
  //  9: Stage D of approach
  // 10: Stage E of approach
  // 11: Stage F of approach
  // 12: In platform
  // 13: Stock released and needing new loco
  // 14: Stock ready to depart
  // 15: Stage A of departure
  // 16: Stage B of departure
  // 17: Stage C of departure
  // 18: Stage D of departure
  // 19: Stage E of departure
  // 20: Stage F of departure
  //
  // A train will begin approaching the station 6 minutes before it is due
  // in the platform. It will then move from state 1 to 2. If it has not
  // been set to a platform it will be held three minutes before it is
  // due to arrive in the platform.
  //
  // Also note that all times used in the following routine have the same
  // format as that of WorkTime, ie Hours*2*60 + Minutes*2 + Halfminute.
  // The half minute flag is either 0 (corresponding to the upper timer
  // block illuminated on the main window display) or 1 (corresponding to
  // the lower timer block illuminated on the main window display)

  for (i=1; i<50; i++)
  {
    if (PSelectorInfo[i] != 0)
    {
      if (PSelectorInfo[i]->GetType() == 1) TimeCheck(i);
    }
  }
}


void TLayout::TimeCheck(int GivArrPnt)
{
  int   i, j, StateChange;
  int   TempMinute, TempHour, ActArrivalTime;
  int   TimeMinusSix, TimeMinusThree, CurrentTime;
  BOOL  States[50];

  // Now carry out our time checks
  for (i=0; i<4; i++)
  {
    if ((Expect[i] > 0) && (Expect[i] < 200))
    {                                 
      // Calculate arrival time minus 6 minutes
      TimeMinusSix = 120*(CalcArrivalTime(Expect[i])/100);
      TimeMinusSix += 2*(CalcArrivalTime(Expect[i])-
			 100*int(CalcArrivalTime(Expect[i])/100));
      TimeMinusSix -= 12;
      if (TimeMinusSix < 0)  TimeMinusSix += 2880;

      // Calculate arrival time minus 3 minutes
      TimeMinusThree = TimeMinusSix + 6;
      if (TimeMinusThree >= 2880)  TimeMinusThree -= 2880;

      // Calculate current time
      CurrentTime = WorkTime;
      StateChange = 0;

      // Find out the states of the other waiting trains - all other trains
      // are automatically excluded.
      for (j=1;j<=40;j++) States[j] = FALSE;
      for (j=0;j<=3;j++)
      {
	if (Expect[j] > 0)
	{
	  if (PTimetableInfo[Expect[j]]->GetArrPnt() == GivArrPnt)
	    States[PTimetableInfo[Expect[j]]->GetStatus()] = TRUE;
        }
      }

      // Handle the trains
      if (PTimetableInfo[Expect[i]]->GetArrPnt() == GivArrPnt)
      {
	// Check for status ST_DUE - "Expected"
        if ((CurrentTime < TimeMinusSix) &&
      	    (PTimetableInfo[Expect[i]]->GetStatus() != ST_DUE) &&
            (StateChange == 0))
        {
	  if (PTimetableInfo[Expect[i]]->GetLoco() == 0)
          {
	    if (!AssignLoco(Expect[i])) return;
	  }
	  StateChange = ST_DUE;
	  ExpChng[i] = TRUE;
	}

	// Check for status ST_APPROACH - "Approaching"
        if ((CurrentTime >= TimeMinusSix) &&
	    (CurrentTime < TimeMinusThree) &&
	    (!States[ST_APPROACH]) &&
	    (!States[ST_HOLBY]) &&
	    (StateChange == 0) &&
	    ((PTimetableInfo[Expect[i]]->GetStatus() == ST_DUE) ||
	     (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD)))
        {
	  StateChange = ST_APPROACH;
	  ExpChng[i] = TRUE;
        }

	// Check for status ST_SETPLAT - "Set to platform X"
	if ((PTimetableInfo[Expect[i]]->GetRoute() == 0) &&
	    (StateChange == 0) &&
	    (!States[ST_SETPLAT]) &&
	    ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
	     (PTimetableInfo[Expect[i]]->GetStatus() == ST_HOLBY)))
        {
	  for (j=0;j<=3;j++)
          {
      	    // Check to see if any of the set routes start at the appropriate
	    // section...
	    if (RoutesSet[j] > 0)
	    {
	      if (PRoutesInfo[RoutesSet[j]]->GetFrom() == PTimetableInfo[Expect[i]]->GetArrPnt())
	      {
	        // OK - we have a match
	        PTimetableInfo[Expect[i]]->SetRoute(RoutesSet[j]);
		RoutesSet[j] = 0;
	        ExpChng[i] = TRUE;
		StateChange = ST_SETPLAT;
	      }
	    } 
	  }
        }

	// Check for status ST_HELD - "Held on"
        if ((CurrentTime >= TimeMinusSix) &&
	    (CurrentTime < TimeMinusThree) &&
	    (PTimetableInfo[Expect[i]]->GetStatus() == ST_DUE) &&
	    (StateChange == 0) &&
	    ((States[ST_HOLBY]) || (States[ST_APPROACH])))
        {
	  StateChange = ST_HELD;
	  ExpChng[i] = TRUE;
	}

	// Check for status ST_HELD if train is past its arrival time
	// minus three minutes and does not yet have a locomotive attached
	if ((CurrentTime >= TimeMinusThree) &&
	    (PTimetableInfo[Expect[i]]->GetStatus() == ST_NONE) &&
	    (PTimetableInfo[Expect[i]]->GetLoco() == 0) &&
	    (StateChange == 0))
	{
          // What is the next allowable state?
	  if (States[ST_HOLBY])
          {
	    StateChange = ST_HELD;
	  }
	  else
	  {
	    StateChange = ST_HOLBY;
	  }
	  ExpChng[i] = TRUE;

	  // Ensure that a loco has been assigned to the train
	  if (!AssignLoco(Expect[i])) return;
        }

        // Check for status ST_HOLBY - "Held at Holby"
        if ((CurrentTime >= TimeMinusThree) &&
	    (!States[ST_HOLBY]) &&
	    (StateChange == 0) &&
	    ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
	     (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD)))
        {
	  StateChange = ST_HOLBY;
	  ExpChng[i] = TRUE;
        }

	// Check for status ST_ARRA - "Section A of approach"
        if ((CurrentTime > TimeMinusThree) &&
	    (PTimetableInfo[Expect[i]]->GetStatus() == ST_SETPLAT) &&
            (StateChange == 0))
        {
	  ExpChng[i] = TRUE;
	  StateChange = ST_ARRA;

	  // Calculate average arrival delay
	  ArrDelay += WorkTime+5-PackTime(PTimetableInfo[Expect[i]]->GetArrTime());
	  ArrNum ++;

	  if (PMainWindow(Parent)->DelayEnable)
	  {
	    ArrDelay -= 2*(PTimetableInfo[Expect[i]]->GetMinLate());
          }
	}
      }

      // Update status field
      if (StateChange > 0)
	PTimetableInfo[Expect[i]]->SetStatus(StateChange);
    }
  }
}


void TLayout::GetDepartures()
{
  // This routine gets the timetable numbers of the next four departures

  int   i, mintime;
  int   OldDepart[4];
  int   OldStatus[4];
  BOOL  NotFound;

  // Reset everything
  for (i=0; i<4; i++)
  {
    OldDepart[i] = Depart[i];
    if (Depart[i] > 0)  OldStatus[i] = PTimetableInfo[Depart[i]]->GetStatus();
    Depart[i] = 0;
    DepChng[i] = FALSE;
  }

  // Run through the timetable data looking for first train to depart...
  mintime = 2500;
  for (i=1; i<200; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      if ((PTimetableInfo[i]->GetDepTime() <= mintime) &&
          (PTimetableInfo[i]->GetDepTime() > 0) &&
	  (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	  (PTimetableInfo[i]->GetStatus() <= ST_STARTDEP))
      {
        // Get new minimum value
	mintime = PTimetableInfo[i]->GetDepTime();
	Depart[0] = i;
      }
    }
  }

  // ...and now the second... 
  if ((Depart[0] > 0) && (Depart[0] < 200))
  {
    mintime = 2500;
    for (i=1; i<200; i++)
    {
      if (PTimetableInfo[i] != 0)
      {
	if ((PTimetableInfo[i]->GetDepTime() <= mintime) &&
	    (i != Depart[0]) &&
	    (PTimetableInfo[i]->GetDepTime() >= PTimetableInfo[Depart[0]]->GetDepTime()) &&
	    (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	    (PTimetableInfo[i]->GetStatus() <= ST_STARTDEP))
	{
          // Get new minimum value
	  mintime = PTimetableInfo[i]->GetDepTime();
	  Depart[1] = i;
	}
      }
    }

    if ((Depart[1] > 0) && (Depart[1] < 200))
    {
      // ...and the third...
      mintime = 2500;
      for (i=1; i<200; i++)
      {
	if (PTimetableInfo[i] != 0)
        {
	  if ((PTimetableInfo[i]->GetDepTime() <= mintime) &&
	      (i != Depart[0]) &&
              (i != Depart[1]) &&
	      (PTimetableInfo[i]->GetDepTime() >= PTimetableInfo[Depart[1]]->GetDepTime()) &&
	      (PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
	      (PTimetableInfo[i]->GetStatus() <= ST_STARTDEP))
	  {
	    // Get new minimum value
	    mintime = PTimetableInfo[i]->GetDepTime();
	    Depart[2] = i;
	  }
	}
      }

      if ((Depart[2] > 0) && (Depart[2] < 200))
      {
        // ...and finally the fourth.
        mintime = 2500;
	for (i=1; i<200; i++)
	{
	  if (PTimetableInfo[i] != 0)
          {
	    if ((PTimetableInfo[i]->GetDepTime() <= mintime) &&
	        (i != Depart[0]) &&
	        (i != Depart[1]) &&
	        (i != Depart[2]) &&
		(PTimetableInfo[i]->GetDepTime() >= PTimetableInfo[Depart[2]]->GetDepTime()) &&
		(PTimetableInfo[i]->GetStatus() >= ST_NONE) &&
		(PTimetableInfo[i]->GetStatus() <= ST_STARTDEP))
	    {
	      // Get new minimum value
	      mintime = PTimetableInfo[i]->GetDepTime();
	      Depart[3] = i;
	    }
          }
        }
      }
    }
  }

  // Check to see if value has changed since previous time...
  if (Depart[0] != OldDepart[0])  DepChng[0] = TRUE;
  if (Depart[1] != OldDepart[1])  DepChng[1] = TRUE;
  if (Depart[2] != OldDepart[2])  DepChng[2] = TRUE;
  if (Depart[3] != OldDepart[3])  DepChng[3] = TRUE;

  // Also check to see if status has changed...
  if ((Depart[0] == OldDepart[0]) && (Depart[0] > 0))
  {
    if (PTimetableInfo[Depart[0]]->GetStatus() != OldStatus[0])  DepChng[0] = TRUE;
    if (WorkTime == PackTime(PTimetableInfo[Depart[0]]->GetDepTime())+1)  DepChng[0] = TRUE;
  }

  if ((Depart[1] == OldDepart[1]) && (Depart[1] > 0))
  {
    if (PTimetableInfo[Depart[1]]->GetStatus() != OldStatus[1])  DepChng[1] = TRUE;
    if (WorkTime == PackTime(PTimetableInfo[Depart[1]]->GetDepTime())+1)  DepChng[1] = TRUE;
  }

  if ((Depart[2] == OldDepart[2]) && (Depart[2] > 0))
  {
    if (PTimetableInfo[Depart[2]]->GetStatus() != OldStatus[2])  DepChng[2] = TRUE;
    if (WorkTime == PackTime(PTimetableInfo[Depart[2]]->GetDepTime())+1)  DepChng[2] = TRUE;
  }

  if ((Depart[3] == OldDepart[3]) && (Depart[3] > 0))
  {
    if (PTimetableInfo[Depart[3]]->GetStatus() != OldStatus[3])  DepChng[3] = TRUE;
    if (WorkTime == PackTime(PTimetableInfo[Depart[3]]->GetDepTime())+1)  DepChng[3] = TRUE;
  }
}


void TLayout::HandleTracking()
{
  // The purpose of this routine is to "track" trains across the station
  // layout. Each train in the timetable has an associated route field.
  // This field refers to a route in the <Routes> array and is used to
  // determine the tracking. A train is only accepted into this section
  // when it has a status value of 5 or higher.

  int   a, b, i, j, k, Temp[16];
  int   ReleaseTime, OldStatus, DepartTime;
  BOOL  NotFound;

  // SECTION 1:
  // Select trains which are arriving and clear track sections for them
  for (i=1; i<200; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      // Check for trains
      if ((PTimetableInfo[i]->GetStatus() >= ST_ARRA) &&
	  (PTimetableInfo[i]->GetStatus() <= ST_ARRF))
      {
	PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetClear(PTimetableInfo[i]->GetStatus()-ST_ARRA, a, b);

	if (a > 0)
	{
	  PSectionInfo[a]->SetOccupied(FALSE);
	  DrawSection(0, a);
	}
	if (b > 0)
	{
	  PSectionInfo[b]->SetOccupied(FALSE);
	  DrawSection(0, b);
	}

	// Is this the last section?
	if ((PTimetableInfo[i]->GetStatus()-ST_ARRA+1) == (ST_ARRF-ST_ARRA+1))
	{
	  if ((PTimetableInfo[i]->GetArrCode() == SC_LIGHTECS) ||
	      (PTimetableInfo[i]->GetArrCode() == SC_LIGHTNORM) ||
	      (PTimetableInfo[i]->GetArrCode() == SC_LIGHTDELT) ||
	      (PTimetableInfo[i]->GetArrCode() == SC_LIGHTRLF) ||
	      (PTimetableInfo[i]->GetArrCode() == SC_LIGHT37))
	  {
	    // Ensure that the end point is not the locoyard
	    if (PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard())
	    {
	      // OK, so the loco is headed for the locoyard
	      PTimetableInfo[i]->SetStatus(ST_INPLAT);

	      // Does the loco need refuelling?
	      if (PLocosInfo[PTimetableInfo[i]->GetLoco()]->GetFlag() == LF_NEEDFUEL)
	      {
		PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_REFUEL);
		PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetRefuelTime(WorkTime + 80);
	      }
	      else
	      {
		PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_LOCOYARD);
	      }

	      // Compact the locoyard array
	      for (j=0; j<16; j++)  Temp[j] = LocoyardLoco[j];
	      for (j=0; j<16; j++)  LocoyardLoco[j] = 0;

	      k=0;
	      for (j=0; j<16; j++)
	      {
		if (Temp[j] > 0)
		{
		  LocoyardLoco[k] = Temp[j];
		  k++;
		}
	      }

	      // Add a loco to the locoyard array
	      if (k<16)
	      {
		// Assign the loco to the <LocoyardLoco> array
		LocoyardLoco[k] = PTimetableInfo[i]->GetLoco();
	      }
	      else
	      {
		MessageBox(HWindow,
			   "Internal error number ERR003 detected. Please note carefully\n how this error arose and report it to MDSoft.",
			   APPNAME,
			   MB_OK);
	      }
	    }
	    else
	    {
	      // Handle a light loco
	      if (PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable() > 0)
	      {
		// There is already a train in the platform
		PTimetableInfo[PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable()]->SetLoco(PTimetableInfo[i]->GetLoco());
		PTimetableInfo[PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable()]->SetStatus(ST_STOCKOK);
		PTimetableInfo[i]->SetStatus(ST_STOCKOK);
	      }
	      else
	      {
		// There is not a train in the platform
		PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetLoco(PTimetableInfo[i]->GetLoco());
		PTimetableInfo[i]->SetStatus(ST_STOCKOK);
	      }
	    }
	  }
	  else
	  {
	    // This is a normal train
	    PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetTimetable(i);
	    PTimetableInfo[i]->SetStatus(ST_INPLAT);
	    PTimetableInfo[i]->SetRelTime(NormTime(PackTime(PTimetableInfo[i]->GetRelTime()) + WorkTime));

	    // The loco needs to be refuelled/maintained
	    if ((PTimetableInfo[i]->GetArrCode() == SC_HST) ||
		(PTimetableInfo[i]->GetArrCode() == SC_DELTIC) ||
		(PTimetableInfo[i]->GetArrCode() == SC_NORMAL) ||
		(PTimetableInfo[i]->GetArrCode() == SC_RELIEF) ||
		(PTimetableInfo[i]->GetArrCode() == SC_CLASS37))
	    {
	      switch (PTimetableInfo[i]->GetLocoPlatFlag())
	      {
		case 1:  PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_NEEDFUEL); break;
		case 2:  PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_MAINTAIN); break;
		default: PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_ASSIGNED); break;
	      }
	    }
	  }

	  // Check to see if this train is in the departures list
	  for (j=0;j<=3;j++)
	  {
	    if (Depart[j] == i) Depart[j] = 0;
	  }

	  if (PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()] != 0)
          {
  	    // Update the platforms display window
	    PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetRedraw(TRUE);

	    // Update platform indicator
	    DrawPlatform(0, PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef());
	  }
	}
	else
	{
	  // Increment status
	  PTimetableInfo[i]->SetStatus(PTimetableInfo[i]->GetStatus()+1);
	}
      }
    }
  }

  // SECTION 1B:
  // Handle locos which are tracking across the station
  for (i=0; i<10; i++)
  {
    if (PTrackLoco[i] != 0)
    {

      // Check for trains
      if ((PTrackLoco[i]->GetStatus() >= ST_ARRA) &&
	  (PTrackLoco[i]->GetStatus() <= ST_ARRF))
      {
	PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetClear(PTrackLoco[i]->GetStatus()-ST_ARRA, a, b);

	if ((PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetFrom() == TSelector::GetHoldPoint()) &&
	    (PTrackLoco[i]->GetStatus()-ST_ARRA == 0))
        {
	  PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(FALSE);
	  DrawSection(0, TSelector::GetHoldSection());
	}

	if (a > 0)
	{
	  PSectionInfo[a]->SetOccupied(FALSE);
	  DrawSection(0, a);
	}
	if (b > 0)
	{
	  PSectionInfo[b]->SetOccupied(FALSE);
	  DrawSection(0, b);
	}

	// Is this the last section?
	if ((PTrackLoco[i]->GetStatus()-ST_ARRA+1) == (ST_ARRF-ST_ARRA+1))
	{
	  // Check to see if the end point is the hold point or the locoyard
	  if ((PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetHoldPoint()) ||
	      (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard()))
	  {
	    if (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard())
	    {
	      // OK, so the loco is headed for the locoyard
	      PTrackLoco[i]->SetStatus(ST_INPLAT);

	      // Does the loco need refuelling?
	      if (PLocosInfo[PTrackLoco[i]->GetLoco()]->GetFlag() == LF_NEEDFUEL)
	      {
		PLocosInfo[PTrackLoco[i]->GetLoco()]->SetFlag(LF_REFUEL);
		PLocosInfo[PTrackLoco[i]->GetLoco()]->SetRefuelTime(WorkTime + 80);
	      }
	      else
	      {
		PLocosInfo[PTrackLoco[i]->GetLoco()]->SetFlag(LF_LOCOYARD);
	      }

	      // Compact the locoyard array
	      for (j=0; j<16; j++)  Temp[j] = LocoyardLoco[j];
	      for (j=0; j<16; j++)  LocoyardLoco[j] = 0;

	      k=0;
	      for (j=0; j<16; j++)
	      {
		if (Temp[j] > 0)
		{
		  LocoyardLoco[k] = Temp[j];
		  k++;
		}
	      }

	      // Add a loco to the locoyard array
	      if (k<16)
	      {
		// Assign the loco to the <LocoyardLoco> array
		LocoyardLoco[k] = PTrackLoco[i]->GetLoco();
	      }
	      else
	      {
		MessageBox(HWindow,
			   "Internal error number ERR004 detected. Please note carefully\n how this error arose and report it to MDSoft.",
			   APPNAME,
			   MB_OK);
	      }
	    }

	    if (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetHoldPoint())
	    {
	      // Loco is headed for the hold point
	      PTrackLoco[i]->SetStatus(ST_INPLAT);
	      HoldLoco = PTrackLoco[i]->GetLoco();
	      HoldLocoChng = TRUE;
	    }
	  }
	  else
	  {
	    // Handle a light loco
	    j = PSelectorInfo[PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo()]->GetPlatRef();
	    if (PPlatDataInfo[j] != 0)
	    {
	      if (PPlatDataInfo[j]->GetTimetable() > 0)
              {
		// There is already a train in the platform
		PTimetableInfo[PPlatDataInfo[j]->GetTimetable()]->SetLoco(PTrackLoco[i]->GetLoco());
		PTimetableInfo[PPlatDataInfo[j]->GetTimetable()]->SetStatus(ST_STOCKOK);
		PTrackLoco[i]->SetStatus(ST_STOCKOK);
	      }
	      else
	      {
		// There is not a train in the platform
		PPlatDataInfo[j]->SetLoco(PTrackLoco[i]->GetLoco());
		PTrackLoco[i]->SetStatus(ST_STOCKOK);
	      }

	      // Update the display
	      PPlatDataInfo[j]->SetRedraw(TRUE);
	      DrawPlatform(0, j);
	    }
	    else
	    {
	      PTrackLoco[i]->SetStatus(ST_STOCKOK);
	    }
	  }

	  // Release the loco
	  PTrackLoco[i]->SetLocoPlatFlag(0);
	}
	else
	{
	  // Increment status
	  PTrackLoco[i]->SetStatus(PTrackLoco[i]->GetStatus()+1);
	}


      }
    }
  }


  // SECTION 2:
  // We now check for trains which have reached their release time and those
  // which are candidates for departure
  for (i=1; i<50; i++)
  {
    if (PPlatDataInfo[i] != 0)
    {
      // Is there a train in the platform?
      a = PPlatDataInfo[i]->GetTimetable();
      if (a > 0)
      {
	// Store current status flag
	OldStatus = PTimetableInfo[a]->GetStatus();

	// Calculate release time
	ReleaseTime = 120*(PTimetableInfo[a]->GetRelTime()/100);
	ReleaseTime += 2*(PTimetableInfo[a]->GetRelTime() -
			   100*int(PTimetableInfo[a]->GetRelTime()/100));
	if (ReleaseTime < 0) ReleaseTime += 2880;

	if ((ReleaseTime <= WorkTime) &&
	    (PTimetableInfo[a]->GetStatus() == ST_INPLAT))
	{
          // Time for stock to be released - is the stock an HST or EMU or DMU?
	  if ((PTimetableInfo[a]->GetArrCode() == SC_HST) ||
	      (PTimetableInfo[a]->GetArrCode() == SC_EMU) ||
	      (PTimetableInfo[a]->GetArrCode() == SC_PUSHPULL) ||
	      (PTimetableInfo[a]->GetArrCode() == SC_DMU1))
	  {
	    // Yes it is, so...
	    if (PTimetableInfo[a]->GetStatus() == ST_INPLAT)
	      PTimetableInfo[a]->SetStatus(ST_STOCKOK);
	  }
	  else
	  {
	    // Just a normal train
	    if (PTimetableInfo[a]->GetStatus() == ST_INPLAT)
	      PTimetableInfo[a]->SetStatus(ST_RELEASE);

	    // Swap locomotive about
	    PPlatDataInfo[i]->SetLoco(PTimetableInfo[a]->GetLoco());
	    PTimetableInfo[a]->SetLoco(0);
	  }

	  // Update platform information if the status has changed
	  if (PTimetableInfo[a]->GetStatus() != OldStatus)
	  {
	    DrawPlatform(0, i);
	    PPlatDataInfo[i]->SetRedraw(TRUE);
	  }

	  // Check to see if this train is in the departures list
	  for (j=0;j<=3;j++)
	  {
	    if (Depart[j] == i) Depart[j] = 0;
	  }
	}

	// Now check to see if the train in this platform can be departed
	if ((PTimetableInfo[a]->GetStatus() == OldStatus) &&
	    ((OldStatus == ST_STOCKOK) ||
	     (OldStatus == ST_READYDEP)))
	{
	  // Calculate departure time
	  DepartTime = 120*(PTimetableInfo[a]->GetDepTime()/100);
	  DepartTime += 2*(PTimetableInfo[a]->GetDepTime() -
			   100*int(PTimetableInfo[a]->GetDepTime()/100));
	  if (DepartTime < 0) DepartTime += 2880;

	  if ((DepartTime-6) <= WorkTime)
	  {
	    // Train is now in a position that it can potentially depart
	    // This change of state changes the platform indicator from
	    // red to blue
	    if (PTimetableInfo[a]->GetStatus() == ST_STOCKOK)
	    {
	      PTimetableInfo[a]->SetStatus(ST_READYDEP);
	      DrawPlatform(0, i);
	    }
	  }

	  if (DepartTime <= WorkTime)
	  {
	    // Check to see if there is a route available
	    for (j=0;j<=3;j++)
	    {
	      // Check to see if any of the set routes start at the appropriate
	      // section...
	      if (RoutesSet[j] > 0)
	      {
		if (PRoutesInfo[RoutesSet[j]]->GetFrom() == PPlatDataInfo[i]->GetSelectorRef())
		{
		  // OK - we have a match
		  PTimetableInfo[a]->SetRoute(RoutesSet[j]);
		  if (PTimetableInfo[a]->GetStatus() == ST_READYDEP)  PTimetableInfo[a]->SetStatus(ST_STARTDEP);

		  // Clear the platform flag
		  RoutesSet[j] = 0;
		  PPlatDataInfo[i]->SetTimetable(0);
		  DrawPlatform(0, i);
		  PPlatDataInfo[i]->SetRedraw(TRUE);

		  // Calculate average departure delay
		  if ((PTimetableInfo[a]->GetDepCode() == SC_EMU) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_HST) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_DELTIC) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_NORMAL) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_RELIEF) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_DMU1) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_PUSHPULL) ||
		      (PTimetableInfo[a]->GetDepCode() == SC_CLASS37))
		  {
		    DepDelay += WorkTime-PackTime(PTimetableInfo[a]->GetDepTime());
		    DepNum ++;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }


  // SECTION 3:
  // Select trains which are departing and clear track sections behind them
  for (i=1; i<200; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      // Check for trains on the UF
      if ((PTimetableInfo[i]->GetStatus() >= ST_DEPA) &&
	  (PTimetableInfo[i]->GetStatus() <= ST_DEPF))
      {
	// Firstly update the display
	PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetClear(PTimetableInfo[i]->GetStatus()-ST_DEPA, a, b);

	if (a > 0)
	{
	  PSectionInfo[a]->SetOccupied(FALSE);
	  DrawSection(0, a);
	}
	if (b > 0)
	{
	  PSectionInfo[b]->SetOccupied(FALSE);
	  DrawSection(0, b);
	}

	// Increment status
	PTimetableInfo[i]->SetStatus(PTimetableInfo[i]->GetStatus()+1);

        // Check to see if we have reached the last section
	if ((PTimetableInfo[i]->GetStatus()-ST_DEPA+1) == (ST_DEPF-ST_DEPA+1))
	{
	  // If this is an ECS train then we should recover the loco
	  if (PTimetableInfo[i]->GetDepCode() == SC_ECS)
	  {
	    // Recover the loco
	    if (PLocosInfo[PTimetableInfo[i]->GetLoco()]->GetType() == SC_ECS)
            {
	      PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_UNASSIGN);
	    }
	  }

	  // Alternatively we may want to recover the loco if it is to be reassigned
	  if (PTimetableInfo[i]->GetNextTimeTabPos() != 0)
	  {
	    // Let's recover the loco and allocate it to another train -
	    // Now assign it to the next train (if it doesn't already have a loco)
	    if (PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->GetLoco() == 0)
	    {
	      PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->SetLoco(PTimetableInfo[i]->GetLoco());
	      PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_ASSIGNED);

	      // Update the delay as well
	      PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->AddMinLate( ( WorkTime-6-PackTime(PTimetableInfo[i]->GetDepTime()) ) /2);
            }

	    PTimetableInfo[i]->SetLoco(0);
	  }
	}

      }

      // Check to see if train has just started a departure (this is AFTER the
      // above code for a good reason - it and the ST_STARTDEP state prevent
      // a train clearing the platform section and that immedately preceeding
      // it at the same time when it is departing)
      if (PTimetableInfo[i]->GetStatus() == ST_STARTDEP)  PTimetableInfo[i]->SetStatus(ST_DEPA);
    }

    // Now check through the locos in the locoyard to see if any are refueling
    // and update them
    for (j=0; j<16; j++)
    {
      if (LocoyardLoco[j] > 0)
      {
	if ((PLocosInfo[LocoyardLoco[j]]->GetFlag() == LF_REFUEL) &&
	    (WorkTime >= PLocosInfo[LocoyardLoco[j]]->GetRefuelTime()))
	{
	  // Loco has completed refueling
	  PLocosInfo[LocoyardLoco[j]]->SetFlag(LF_LOCOYARD);

	  // Update the locoyard window
	  if (PMainWindow(Parent)->LocoyardHan != NULL)
	    PMainWindow(Parent)->LocoyardHan->UpdateDisplay(0, TRUE);
	}
      }
    }
  }
}


void TLayout::LightLoco(int LocoNum, int ArrPnt, int Route, int Status)
{
  int   i;
  BOOL  NotFound;
 
  i=0;
  NotFound = TRUE;

  while (NotFound && i<5)
  {
    // Look for an empty slot
    if (PTrackLoco[i]->GetLocoPlatFlag() == 0)  NotFound = FALSE;
    i++;
  }
  i--;

  // Error message
  if (NotFound)
  {
    MessageBox(HWindow,
	       "Internal error number ERR005 detected. Please note carefully\n how this error arose and report it to MDSoft.",
	       APPNAME,
	       MB_OK);
    i = 2;
  }

  // Attach a selected loco to a timetable position
  PTrackLoco[i]->SetArrPnt(ArrPnt);
  PTrackLoco[i]->SetRoute(Route);
  PTrackLoco[i]->SetLoco(LocoNum);
  PTrackLoco[i]->SetArrCode(SC_LIGHTECS);
  PTrackLoco[i]->SetStatus(Status);
  PTrackLoco[i]->SetLocoPlatFlag(1);
}


int TLayout::NormTime(int GivenTime)
{
  int A, TheTime;

  // This routine takens the value <GivenTime> which is in packed format
  // (ie 2*(60*hrs+minutes)) and expands it into a "normal" time (ie
  // hhmm) and returns this value.

  A = int(GivenTime/1200);                            
  TheTime = 1000*A;                                           // First hour digit
  TheTime += 100*int(GivenTime/120 - 10*A);                   // Second hour digit
             
  A = int((GivenTime - 120*int(GivenTime/120))/20);           
  TheTime += 10*A;                                            // 1st minute digit
  TheTime += ((GivenTime - 120*int(GivenTime/120)) - 20*A)/2; // 2nd minute digit

  return TheTime;
}


int TLayout::PackTime(int GivenTime)
{
  int TheTime;

  // This routine takes a normal time value and returns it as a packed time
  // value

  TheTime = 2*(GivenTime - 40*int(GivenTime/100));
  return TheTime;
}


int TLayout::CalcArrivalTime(int TimetableNo)
{
  int A, B;

  // This routine, given a timetable number calculates the actual
  // train arrival time

  if (PMainWindow(Parent)->DelayEnable)
  {
    // Get delay time and actual time
    A = PTimetableInfo[TimetableNo]->GetMinLate();
    B = PTimetableInfo[TimetableNo]->GetArrTime() + A;

    if (((B - 100*int(B/100)) >= 60) && (A > 0))  B += 40;
    if (((B - 100*int(B/100)) >= 60) && (A < 0))  B -= 40;

    return B;
  }
  else
  {
    // Don't bother taking the delay into account
    A = PTimetableInfo[TimetableNo]->GetArrTime();
    return A;
  }
}


int TLayout::CodeCalculate(char* TextString)
{
  // This routine takes a <TextString> and returns the appropriate ST_XXXX
  // or SC_XXXX code 

  char InternText[100], ErrorText[100];
  int  i, j;

  // Trim the string first of all - trailing blanks...
  i = strlen(TextString) - 1;
  while (i >= 0 && TextString[i] == ' ')  i--;
  TextString[i+1] = '\0';

  // ...and then leading blanks
  i = 0;
  j = 0;
  while ((TextString[i] != '\0') && (TextString[i] == ' '))  i++;
  while (TextString[i] != '\0')
  {
    InternText[j] = TextString[i];
    i++;
    j++;
  }
  InternText[j] = '\0';


  // SC_XXXX codes
  if (strcmpi(InternText, "SC_ECS") == 0)        return SC_ECS;
  if (strcmpi(InternText, "SC_HST") == 0)        return SC_HST;
  if (strcmpi(InternText, "SC_EMU") == 0)        return SC_EMU;
  if (strcmpi(InternText, "SC_DELTIC") == 0)     return SC_DELTIC;
  if (strcmpi(InternText, "SC_NORMAL") == 0)     return SC_NORMAL;
  if (strcmpi(InternText, "SC_RELIEF") == 0)     return SC_RELIEF;
  if (strcmpi(InternText, "SC_LIGHTECS") == 0)   return SC_LIGHTECS;
  if (strcmpi(InternText, "SC_LIGHTNORM") == 0)  return SC_LIGHTNORM;
  if (strcmpi(InternText, "SC_LIGHTDELT") == 0)  return SC_LIGHTDELT;
  if (strcmpi(InternText, "SC_PUSHPULL") == 0)   return SC_PUSHPULL;
  if (strcmpi(InternText, "SC_DMU1") == 0)       return SC_DMU1;
  if (strcmpi(InternText, "SC_CLASS37") == 0)    return SC_CLASS37;
  if (strcmpi(InternText, "SC_LIGHTRLF") == 0)   return SC_LIGHTRLF;
  if (strcmpi(InternText, "SC_LIGHT37") == 0)    return SC_LIGHT37;

  // ST_XXXX codes
  if (strcmpi(InternText, "ST_NONE") == 0)    return ST_NONE;
  if (strcmpi(InternText, "ST_INPLAT") == 0)  return ST_INPLAT;
  if (strcmpi(InternText, "ST_STOCKOK") == 0)  return ST_STOCKOK;

  strcpy(ErrorText, "Incorrect timetable code error\n\n");
  strcat(ErrorText, "Arrival description: \n");
 
  MessageBox(HWindow, ErrorText, APPNAME, MB_OK);
  return 1;
}






