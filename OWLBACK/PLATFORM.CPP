/* PLATFORM.CPP
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
/* Methods of TPlatform follow...*/

TPlatform::TPlatform(PTWindowsObject AParent, int XPos, int YPos)
  : TWindow(AParent, "Platforms")
{
  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 250;
  Attr.H = 253;
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


TPlatform::~TPlatform()
{
  // Free resources
  DeleteObject(HFont);
}


void TPlatform::GetWindowClass(WNDCLASS& WndClass)
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, "PLATFORM");
}


LPSTR TPlatform::GetClassName()
{
  return "Platform Window";
}


void TPlatform::SetupWindow()
{
  HMENU  hSysMenu;
  int    i;  

  // Call ancestor method
  TWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);

  // Intialize variables
  for (i=1;i<=14;i++)
    Timetab[i] = 0;
}


BOOL TPlatform::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = GetMenu(PMainWindow(Parent)->HWindow);
  CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_UNCHECKED);
  PMainWindow(Parent)->PlatformHan = NULL;

  // Return value
  return TRUE;
}


void TPlatform::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);

  // Check the iconized status of the window
  if (IsIconic(HWindow))
  {
    PMainWindow(Parent)->PlatformIcon = TRUE;
  }
  else
  {
    PMainWindow(Parent)->PlatformIcon = FALSE;
  }
}


void TPlatform::WMMove(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMMove(Msg);

  // Update parent fields
  PMainWindow(Parent)->PlatformX = Attr.X;
  PMainWindow(Parent)->PlatformY = Attr.Y;
}


void TPlatform::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call default paint routine
  TWindow::Paint(PaintDC, PaintInfo);

  // Call main painting routine
  UpdateDisplay(PaintDC, TRUE);
}


void TPlatform::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL    MadeDC;
  RECT    TheRect;
  int     a, b, c, i, XPos, YPos, MaxVal;
  char    TrainName[100], LocoName[100], TextString[100], MGD[100];
  WORD    wTxtWid;

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
    strcpy(TextString, "No:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
    strcpy(TextString, "Description:");
    TextOut(TheDC, 60, 5, TextString, strlen(TextString));
  }

  // Set Text align to the right
  SetTextAlign(TheDC, TA_RIGHT);

  if (PMainWindow(Parent)->GameInProgress)
  {
    // Now we draw in the timetable information - we deal with platforms
    // first
    MaxVal = 0;
    for (i=1; i<50; i++)
    {
      if (PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i] != 0)
      {
	MaxVal = i;
	if (Redraw || (PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetRedraw()))
	{
	  PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->SetRedraw(FALSE);
	  strcpy(LocoName, "");
	  strcpy(TrainName, "");

	  strcpy(MGD, "PLT STRT");
	  SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	  // Draw main platform name text
	  a = PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
	  if (a != 0)
          {
	    PMainWindow(Parent)->DisplayHan->PSelectorInfo[a]->GetText(TextString, 20);
	    TextOut(TheDC, 33, 8+17*i, TextString, strlen(TextString));
	  }
	  else
	  {
	    MessageBox(HWindow, "Zero selector ref", APPNAME, MB_OK);
	  }

	  strcpy(MGD, "PLT CASE 1");
          SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	  // CASE 1: Single loco in the platform
	  if ((PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetLoco() > 0) &&
	      (PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetTimetable() == 0))
	  {
	    // Get loco number
	    a = PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetLoco();
	    if (a > 0)
	    {
	      PMainWindow(Parent)->DisplayHan->PLocosInfo[a]->LocoNumStr(LocoName);
	      strcpy(TrainName, "");

	      // Select <LocoName> text colour
	      switch (PMainWindow(Parent)->DisplayHan->PLocosInfo[a]->GetFlag())
	      {
		case LF_NEEDFUEL: SetTextColor(TheDC, RGB(128, 128, 128)); break;
		case LF_MAINTAIN: SetTextColor(TheDC, RGB(225, 225, 0)); break;
		default: SetTextColor(TheDC, RGB(0, 0, 0)); break;
	      }
	    }
	  }

	  strcpy(MGD, "PLT CASE 2&3");
          SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	  // CASE 2 & 3: There is a train in the platform with 1 or 2 locos
	  a = PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetTimetable();
	  if (a > 0)
	  {
	    // Get timetable status
	    b = PMainWindow(Parent)->DisplayHan->PTimetableInfo[a]->GetStatus();

	    if (b == ST_INPLAT)
	    {
 	      strcpy(MGD, "PLT ST_INPLAT");
	      SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	      // Train has just arrived and loco is still connected
	      PMainWindow(Parent)->DisplayHan->PTimetableInfo[a]->GetArrDesc(TrainName);
	      strcat(TrainName, " + ");

	      c = PMainWindow(Parent)->DisplayHan->PTimetableInfo[a]->GetLoco();
	      if (c > 0)
	      {
		PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);

		// Select <LocoName> text colour
		switch (PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->GetFlag())
		{
		  case LF_NEEDFUEL: SetTextColor(TheDC, RGB(128, 128, 128)); break;
		  case LF_MAINTAIN: SetTextColor(TheDC, RGB(225, 225, 0)); break;
		  default: SetTextColor(TheDC, RGB(0, 0, 0)); break;
		}
	      }
	    }

	    if ((b == ST_RELEASE) ||
		(b == ST_STOCKOK) ||
		(b == ST_READYDEP))
	    {
	      strcpy(MGD, "PLT ST_RELEA");
	      SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	      // Loco is disconnected from the train
	      c = PMainWindow(Parent)->DisplayHan->PTimetableInfo[a]->GetLoco();
	      if (c > 0)
	      {
		PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);
		strcpy(TrainName, LocoName);
		strcat(TrainName, " + ");
	      }
	      else
	      {
		strcpy(TrainName, "");
	      }

	      PMainWindow(Parent)->DisplayHan->PTimetableInfo[a]->GetDepDesc(LocoName);
	      strcat(TrainName, LocoName);

	      strcpy(MGD, "PLT CHOICE");
	      SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	      c = PMainWindow(Parent)->DisplayHan->PPlatDataInfo[i]->GetLoco();
	      if (c > 0)
	      {
		PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);
		strcat(TrainName, " . ");

		// Select <LocoName> text colour
		switch (PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->GetFlag())
		{
		  case LF_NEEDFUEL: SetTextColor(TheDC, RGB(128, 128, 128)); break;
		  case LF_MAINTAIN: SetTextColor(TheDC, RGB(225, 225, 0)); break;
		  default: SetTextColor(TheDC, RGB(0, 0, 0)); break;
		}
	      }
	      else
	      {
		strcpy(LocoName, "");
	      }
	    }
	  }

	  strcpy(MGD, "PLT DRAW");
          SendMessage(PMainWindow(Parent)->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

	  // Now draw information on the screen
	  Rectangle(TheDC, 35, 8+17*i, 241, 24+17*i);
	  TextOut(TheDC, 240, 8+17*i, LocoName, strlen(LocoName));
	  wTxtWid = LOWORD(GetTextExtent(TheDC, LocoName, strlen(LocoName)));
	  SetTextColor(TheDC, RGB(0, 0, 0));
	  TextOut(TheDC, (240-wTxtWid), 8+17*i, TrainName, strlen(TrainName));
	}
      }
    }

    // Now update the hold point
    if (TSelector::GetHoldPoint() > 0)
    {
      if (Redraw || PMainWindow(Parent)->DisplayHan->HoldLocoChng)
      {
	PMainWindow(Parent)->DisplayHan->HoldLocoChng = FALSE;
	strcpy(TextString, "Hold");
	TextOut(TheDC, 33, 25+17*MaxVal, TextString, strlen(TextString));
	if (PMainWindow(Parent)->DisplayHan->HoldLoco > 0)
	{
	  c = PMainWindow(Parent)->DisplayHan->HoldLoco;
	  if (c > 0)
	  {
	    PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->LocoNumStr(TextString);

	    // Select <LocoName> text colour
	    switch (PMainWindow(Parent)->DisplayHan->PLocosInfo[c]->GetFlag())
	    {
	      case LF_NEEDFUEL: SetTextColor(TheDC, RGB(128, 128, 128)); break;
	      case LF_MAINTAIN: SetTextColor(TheDC, RGB(225, 225, 0)); break;
	      default: SetTextColor(TheDC, RGB(0, 0, 0)); break;
	    }
	  }
	  else
	  {
	    strcpy(TextString, "");
	  }
	}
	else
	{
	  strcpy(TextString, "");
	}

	Rectangle(TheDC, 35, 25+17*MaxVal, 241, 41+17*MaxVal);
	TextOut(TheDC, 240, 25+17*MaxVal, TextString, strlen(TextString));
	strcpy(Currently[MaxVal+1], TextString);
      }
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}







