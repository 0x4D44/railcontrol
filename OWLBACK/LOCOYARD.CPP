/* LOCOYARD.CPP
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
/* Methods of TLocoyard follow...*/

TLocoyard::TLocoyard(PTWindowsObject AParent, int XPos, int YPos)
  : TWindow(AParent, "Loco yard")
{
  int i;

  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 200;
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

  // Initialize variables
  InWhichLoco = FALSE;
  for (i=0; i<16; i++)  OldYardLoco[i] = 0;
  OldSelect = 100;
 }


TLocoyard::~TLocoyard()
{
  // Free resources
  DeleteObject(HFont);
}


void TLocoyard::GetWindowClass(WNDCLASS& WndClass)
{
  TWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
  WndClass.hIcon = LoadIcon(GetApplication()->hInstance, "LOCOYARD");
}


LPSTR TLocoyard::GetClassName()
{
  return "Locoyard Window";
}


void TLocoyard::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}


BOOL TLocoyard::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = GetMenu(PMainWindow(Parent)->HWindow);
  CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_UNCHECKED);
  PMainWindow(Parent)->LocoyardHan = NULL;

  // If we have the mouse capture then release it
  if (HWindow == GetCapture())  ReleaseCapture();

  // Return value
  return TRUE;
}


void TLocoyard::WMSize(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMSize(Msg);

  // Check the iconized status of the window
  if (IsIconic(HWindow))
  {
    PMainWindow(Parent)->LocoyardIcon = TRUE;
  }
  else
  {
    PMainWindow(Parent)->LocoyardIcon = FALSE;
  }
}


void TLocoyard::WMMove(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMMove(Msg);

  // Update parent fields
  PMainWindow(Parent)->LocoyardX = Attr.X;
  PMainWindow(Parent)->LocoyardY = Attr.Y;
}


void TLocoyard::WMLButtonDown(RTMessage Msg)
{
  // Call ancestor method
  TWindow::WMLButtonDown(Msg);
}


void TLocoyard::WMLButtonUp(RTMessage)
{
  HDC     TheDC;
  long    LocoVal;
  char    TextString[40];

  if (InWhichLoco)
  {
    // Firstly check to see that we have a loco selected
    if (OldSelect < 16)
    {
      if ((PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect] > 0) &&
	  (PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL))
      {
        // Deselect the locomotive
	TheDC = GetDC(HWindow);
	SelectObject(TheDC, HFont);
	SetTextColor(TheDC, RGB(0, 0, 0));
	SetBkColor(TheDC, RGB(192, 192, 192));

	// Get loco number as a character array
	PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
	TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
	ReleaseDC(HWindow, TheDC);

	// Now set the loco to LF_LIGHT
	PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->SetFlag(LF_LIGHT);
	PMainWindow(Parent)->DisplayHan->LightLoco(PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect], TSelector::GetLocoyard(), TheRoute, ST_ARRA);
	PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect] = 0;


	// Release mouse capture and reset window text
	ReleaseCapture();
	SetWindowText(HWindow, "Loco yard");
	InvalidateRect(HWindow, NULL, TRUE);

        InWhichLoco = FALSE;
	OldSelect = 100;
        OldXPos = 0;
	OldYPos = 0;
      }
    }
  }
}


void TLocoyard::WMMouseMove(RTMessage Msg)
{
  HDC     TheDC;
  int     LookXPos, LookYPos;
  int     XPos, YPos, a;
  long    LocoVal;
  char    TextString[40];

  // Calculate mouse X and Y position
  XPos = LOWORD(Msg.LParam);
  YPos = HIWORD(Msg.LParam);

  if (InWhichLoco)
  {
    // Highlight the loco the mouse is currently over
    if ((XPos > 9) && (XPos < 191) && (YPos > 24) && (YPos < 93))
    {
      a = 0;

      LookXPos = 10;
      LookYPos = 25;
      while (((XPos < LookXPos) || (XPos > LookXPos+45) ||
	      (YPos < LookYPos) || (YPos > LookYPos+17)) &&
	      (a < 16))
      {
        LookXPos += 45;
        if (LookXPos > 180)
        {
          LookXPos = 10;
          LookYPos += 17;
	}

	// Increment counter
	a++;
      }

      // LookXPos and LookYPos should now contain the XY position of the
      // currently highlighted (by the mouse) loco number
      if (OldSelect != a)
      {
        // Get a DC and set it up
        TheDC = GetDC(HWindow);
	SelectObject(TheDC, HFont);
	SetTextColor(TheDC, RGB(0, 0, 0));
	SetBkColor(TheDC, RGB(192, 192, 192));

	if (OldSelect < 16)
        {
	  if (PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect] > 0)
	  {
	    if (PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL)
	    {
              // Get loco number as a character array
	      PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
	      TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
	    }
	  }
	}

	if (PMainWindow(Parent)->DisplayHan->LocoyardLoco[a] > 0)
	{
	  if (PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[a]]->GetFlag() != LF_REFUEL)
	  {
            // Get new loco number and output it
	    SetTextColor(TheDC, RGB(255, 255, 255));
	    PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[a]]->LocoNumStr(TextString);
	    TextOut(TheDC, LookXPos, LookYPos, TextString, strlen(TextString));

	    // Update "old" positions
	    OldSelect = a;
	    OldXPos = LookXPos;
	    OldYPos = LookYPos;
	  }
	}

	// Release the DC
	ReleaseDC(HWindow, TheDC);
      }
    }
    else
    {
      if (OldSelect < 16)
      {
	if ((PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect] > 0) &&
	    (PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL))
	{
	  // Get old loco number and reset it
          TheDC = GetDC(HWindow);
	  SelectObject(TheDC, HFont);
	  SetTextColor(TheDC, RGB(0, 0, 0));
	  SetBkColor(TheDC, RGB(192, 192, 192));

	  // Get loco number as a character array
	  PMainWindow(Parent)->DisplayHan->PLocosInfo[PMainWindow(Parent)->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
	  TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
	  ReleaseDC(HWindow, TheDC);

	  OldSelect = 100;
          OldXPos = 0;
	  OldYPos = 0;
	}
      }
    }
  }
}


void TLocoyard::Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo)
{
  // Call default paint routine
  TWindow::Paint(PaintDC, PaintInfo);

  // Call main painting routine
  UpdateDisplay(PaintDC, TRUE);
}


void TLocoyard::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL    MadeDC, Update;
  RECT    TheRect;
  int     a, i, XPos, YPos;
  long    LocoVal;
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
    strcpy(TextString, "Available locos:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
  }

  if (PMainWindow(Parent)->GameInProgress)
  {
    // Do we get new loco information?
    Update = FALSE;
    for (i=0; i<16; i++)
    {
      if (OldYardLoco[i] != PMainWindow(Parent)->DisplayHan->LocoyardLoco[i])  Update = TRUE;
    }

    // Draw in loco information
    if (Update || Redraw)
    {
      a = 0;
      XPos = 10;
      YPos = 25;
      for (i=0; i<16; i++)
      {
	a = PMainWindow(Parent)->DisplayHan->LocoyardLoco[i];
	if ((a > 0) && (a < 500))
        {
	  // Get loco number as a character array
          PMainWindow(Parent)->DisplayHan->PLocosInfo[a]->LocoNumStr(TextString);

	  if (PMainWindow(Parent)->DisplayHan->PLocosInfo[a]->GetFlag() == LF_REFUEL)
	  {
	    SetTextColor(TheDC, RGB(128, 128, 128));
	  }
	  else
	  {
	    SetTextColor(TheDC, RGB(0, 0, 0));
	  }

	  TextOut(TheDC, XPos, YPos, TextString, strlen(TextString));

          // Update XPos and YPos
          XPos += 45;
  	  if (XPos > 180)
          {
            XPos = 10;
            YPos += 17;
	  }
	}
      }
    }

    // Get current <LocoyardLoco> for the next time
    for (i=0; i<16; i++)  OldYardLoco[i] = PMainWindow(Parent)->DisplayHan->LocoyardLoco[i];
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLocoyard::WhichLoco(int RouteNo)
{
  int i, j, Temp[16];
  
  // Modify window text
  TheRoute = RouteNo;
  SetWindowText(HWindow, "Which Loco?");


  // Compress the <LocoyardLoco> array (held in the TLayout class)
  for (i=0; i<16; i++)  Temp[i] = PMainWindow(Parent)->DisplayHan->LocoyardLoco[i];
  for (i=0; i<16; i++)  PMainWindow(Parent)->DisplayHan->LocoyardLoco[i] = 0;

  j = 0;
  for (i=0; i<16; i++)
  {
    if (Temp[i] > 0)
    {
      PMainWindow(Parent)->DisplayHan->LocoyardLoco[j] = Temp[i];
      j++;
    }
  }

  // Get the mouse
  SetCapture(HWindow);

  InWhichLoco = TRUE;
  OldSelect = 100;
}



