// OWLCVT 05/11/95 22:35:59
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

TLocoyard::TLocoyard(TWindow * AParent, int XPos, int YPos)
  : TFrameWindow(AParent, "Loco yard")
{
  int i;

  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 200;
  Attr.H = 120;
  Attr.Style = WS_DLGFRAME;

  // Initialize variables
  InWhichLoco = FALSE;
  for (i=0; i<16; i++)  OldYardLoco[i] = 0;
  OldSelect = 100;
 }


TLocoyard::~TLocoyard()
{
}


void TLocoyard::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  WndClass.hIcon = LoadIcon(*GetApplication(), "LOCOYARD");
}


LPSTR TLocoyard::GetClassName()
{
  return "Locoyard Window";
}


void TLocoyard::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = ::GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}


bool TLocoyard::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = ::GetMenu(Parent->HWindow);
  CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_UNCHECKED);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  frame->LocoyardHan = NULL;

  // If we have the mouse capture then release it
  if (HWindow == ::GetCapture())  ReleaseCapture();

  // Return value
  return TRUE;
}


void TLocoyard::EvSize(UINT sizeType, TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check the iconized status of the window
  if (::IsIconic(HWindow))
  {
   frame->LocoyardIcon = TRUE;
  }
  else
  {
   frame->LocoyardIcon = FALSE;
  }
}


void TLocoyard::EvMove(TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvMove(pnt);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  frame->LocoyardX = Attr.X;
  frame->LocoyardY = Attr.Y;
}


void TLocoyard::EvLButtonDown(UINT modKeys, TPoint& point)
{
  // Call ancestor method
  TFrameWindow::EvLButtonDown(modKeys, point);
}


void TLocoyard::EvLButtonUp(UINT, TPoint&)
{

  HDC     TheDC;
  char    TextString[40];
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (InWhichLoco)
  {
   // Firstly check to see that we have a loco selected
   if (OldSelect < 16)
   {
    if ((frame->DisplayHan->LocoyardLoco[OldSelect] > 0) &&
       (frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL))
    {
      // Deselect the locomotive
      TheDC = GetDC(HWindow);
      SelectObject(TheDC, frame->mTextFont);
      SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
      SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));

      // Get loco number as a character array
      frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
      TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
      ReleaseDC(HWindow, TheDC);

      // Now set the loco to LF_LIGHT
      frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->SetFlag(LF_LIGHT);
      frame->DisplayHan->LightLoco(frame->DisplayHan->LocoyardLoco[OldSelect], TSelector::GetLocoyard(), TheRoute, ST_ARRA);
      frame->DisplayHan->LocoyardLoco[OldSelect] = 0;


      // Release mouse capture and reset window text
      ReleaseCapture();
      ::SetWindowText(HWindow, "Loco yard");
      ::InvalidateRect(HWindow, NULL, TRUE);

      InWhichLoco = FALSE;
      OldSelect = 100;
      OldXPos = 0;
       OldYPos = 0;
    }
   }
  }
}


void TLocoyard::EvMouseMove(UINT, TPoint& pnt)
{
  HDC     TheDC;
  int     LookXPos, LookYPos;
  int     a;
  char    TextString[40];
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (InWhichLoco)
  {
   // Highlight the loco the mouse is currently over
   if ((pnt.x > 9) && (pnt.x < 191) && (pnt.y > 24) && (pnt.y < 93))
   {
    a = 0;

    LookXPos = 10;
    LookYPos = 25;
    while (((pnt.x < LookXPos) || (pnt.x > LookXPos+45) ||
          (pnt.y < LookYPos) || (pnt.y > LookYPos+17)) &&
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
      SelectObject(TheDC, frame->mTextFont);
      SetTextColor(TheDC, RGB(255, 255, 255));
      SetBkColor(TheDC, RGB(0, 0, 0));

      if (OldSelect < 16)
      {
       if (frame->DisplayHan->LocoyardLoco[OldSelect] > 0)
       {
        if (frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL)
        {
          // Get loco number as a character array
          frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
          TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
        }
       }
      }

      if (frame->DisplayHan->LocoyardLoco[a] > 0)
      {
       if (frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[a]]->GetFlag() != LF_REFUEL)
       {
        // Get new loco number and output it
        SetTextColor(TheDC, RGB(255, 255, 0));
        frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[a]]->LocoNumStr(TextString);
        TextOut(TheDC, LookXPos, LookYPos, TextString, strlen(TextString));
       }
      }

      // Update "old" positions
      OldSelect = a;
      OldXPos = LookXPos;
      OldYPos = LookYPos;

      // Release the DC
      ReleaseDC(HWindow, TheDC);
    }
   }
   else
   {
    if (OldSelect < 16)
    {
      if ((frame->DisplayHan->LocoyardLoco[OldSelect] > 0) &&
        (frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->GetFlag() != LF_REFUEL))
      {
       // Get old loco number and reset it
       TheDC = GetDC(HWindow);
       SelectObject(TheDC, frame->mTextFont);
       SetTextColor(TheDC, RGB(255, 255, 255));
       SetBkColor(TheDC, RGB(0, 0, 0));

       // Get loco number as a character array
       frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
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


void TLocoyard::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Call main painting routine
  UpdateDisplay(dc, TRUE);
}


void TLocoyard::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL     MadeDC, Update;
  RECT     TheRect;
  HPEN     hOldPen, hPen;
  int      a, i, XPos, YPos;
  char     TextString[40];
  COLORREF loldBackColour;
  RECT     lrect;
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

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

  if (Redraw)
  {
    // Draw a bounding box (3D effect)
    ::GetClientRect(HWindow, &TheRect);

    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
      hOldPen = (HPEN) SelectObject(TheDC, hPen);
      MoveToEx(TheDC, TheRect.right-4, 23, 0);
      LineTo(TheDC, TheRect.right-4, TheRect.bottom-4);
      LineTo(TheDC, 3, TheRect.bottom-4);
      SelectObject(TheDC, hOldPen);
      DeleteObject(hPen);

      hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
      hOldPen = (HPEN) SelectObject(TheDC, hPen);
      LineTo(TheDC, 3, 23);
      LineTo(TheDC, TheRect.right-4, 23);
      SelectObject(TheDC, hOldPen);
      DeleteObject(hPen);
    }

    /************************************************************************/
    /* Draw a black background to the data box.                             */
    /************************************************************************/
    loldBackColour = SetBkColor(TheDC, RGB(0, 0, 0));

    lrect.left   = 4;
    lrect.right  = TheRect.right - 5;
    lrect.top    = 24;
    lrect.bottom = TheRect.bottom - 5;
    ExtTextOut(TheDC, 0, 0, ETO_OPAQUE, &lrect, NULL, 0, NULL);

    SetBkColor(TheDC, loldBackColour);
  }

  // Set up text attributes
  SelectObject(TheDC, frame->mHeaderFont);
  SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));

  // Now draw timetable header information
  if (Redraw)
  {
    strcpy(TextString, "Available locos:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
  }

  SetTextColor(TheDC, RGB(255, 255, 255));
  SetBkColor(TheDC, RGB(0, 0, 0));

  if (frame->GameInProgress)
  {
    // Do we get new loco information?
    Update = FALSE;
    for (i=0; i<16; i++)
    {
      if (OldYardLoco[i] != frame->DisplayHan->LocoyardLoco[i])  Update = TRUE;
    }

    // Draw in loco information
    if (Update || Redraw)
    {
      a = 0;
      XPos = 10;
      YPos = 25;
      for (i=0; i<16; i++)
      {
        a = frame->DisplayHan->LocoyardLoco[i];
        if ((a > 0) && (a < 500))
        {
          // Get loco number as a character array
          frame->DisplayHan->PLocosInfo[a]->LocoNumStr(TextString);

          if (frame->DisplayHan->PLocosInfo[a]->GetFlag() == LF_REFUEL)
          {
            SetTextColor(TheDC, RGB(128, 128, 128));
          }
          else
          {
            SetTextColor(TheDC, RGB(255, 255, 255));
          }

          SelectObject(TheDC, frame->mTextFont);
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
    for (i=0; i<16; i++)  OldYardLoco[i] = frame->DisplayHan->LocoyardLoco[i];
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLocoyard::WhichLoco(int RouteNo)
{
  int i, j, Temp[16];
  
  // Modify window text
  TheRoute = RouteNo;
  ::SetWindowText(HWindow, "Which Loco?");
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);


  // Compress the <LocoyardLoco> array (held in the TLayout class)
  for (i=0; i<16; i++)  Temp[i] = frame->DisplayHan->LocoyardLoco[i];
  for (i=0; i<16; i++)  frame->DisplayHan->LocoyardLoco[i] = 0;

  j = 0;
  for (i=0; i<16; i++)
  {
    if (Temp[i] > 0)
    {
      frame->DisplayHan->LocoyardLoco[j] = Temp[i];
      j++;
    }
  }

  // Get the mouse
  ::SetCapture(HWindow);

  InWhichLoco = TRUE;
  OldSelect = 100;
}




DEFINE_RESPONSE_TABLE1(TLocoyard, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
    EV_WM_LBUTTONDOWN,
    EV_WM_LBUTTONUP,
    EV_WM_MOUSEMOVE,
END_RESPONSE_TABLE;
