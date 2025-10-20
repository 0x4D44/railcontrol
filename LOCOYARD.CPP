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
  WndClass.hIcon = ::LoadIcon(GetModule()->GetHandle(), TEXT("LOCOYARD"));
}


auto TLocoyard::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Locoyard Window"));
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


void TLocoyard::EvSize(owl::uint sizeType, const TSize& size)
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


void TLocoyard::EvMove(const TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvMove(pnt);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  frame->LocoyardX = Attr.X;
  frame->LocoyardY = Attr.Y;
}


void TLocoyard::EvLButtonDown(owl::uint modKeys, const TPoint& point)
{
  // Call ancestor method
  TFrameWindow::EvLButtonDown(modKeys, point);
}


void TLocoyard::EvLButtonUp(owl::uint, const TPoint&)
{

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
      TReleaseDcGuard dcGuard(HWindow, GetDC(HWindow));
      if (HDC TheDC = dcGuard.Get())
      {
        SelectObject(TheDC, frame->mTextFont);
        ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));

        frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
        TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
      }

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


void TLocoyard::EvMouseMove(owl::uint, const TPoint& pnt)
{
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
      TReleaseDcGuard dcGuard(HWindow, GetDC(HWindow));
      if (HDC TheDC = dcGuard.Get())
      {
        SelectObject(TheDC, frame->mTextFont);
        ::SetTextColor(TheDC, RGB(255, 255, 255));
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
        ::SetTextColor(TheDC, RGB(255, 255, 0));
        frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[a]]->LocoNumStr(TextString);
        TextOut(TheDC, LookXPos, LookYPos, TextString, strlen(TextString));
       }
      }

      // Update "old" positions
      OldSelect = a;
      OldXPos = LookXPos;
      OldYPos = LookYPos;
      }
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
       TReleaseDcGuard dcGuard(HWindow, GetDC(HWindow));
       if (HDC TheDC = dcGuard.Get())
       {
         SelectObject(TheDC, frame->mTextFont);
         ::SetTextColor(TheDC, RGB(255, 255, 255));
         SetBkColor(TheDC, RGB(0, 0, 0));

         frame->DisplayHan->PLocosInfo[frame->DisplayHan->LocoyardLoco[OldSelect]]->LocoNumStr(TextString);
         TextOut(TheDC, OldXPos, OldYPos, TextString, strlen(TextString));
       }

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
  TReleaseDcGuard dcGuard;
  BOOL     Update;
  RECT     TheRect;
  int      a, i, XPos, YPos;
  char     TextString[40];
  RECT     lrect;
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (TheDC == 0)
  {
    dcGuard.Attach(HWindow, GetDC(HWindow));
    TheDC = dcGuard.Get();
  }
  if (!TheDC)
  {
    return;
  }

  if (Redraw)
  {
    // Draw a bounding box (3D effect)
    ::GetClientRect(HWindow, &TheRect);

    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      DrawRaisedPanel(TheDC, TheRect, 23);
    }

    lrect.left   = 4;
    lrect.right  = TheRect.right - 5;
    lrect.top    = 24;
    lrect.bottom = TheRect.bottom - 5;
    FillRectOpaque(TheDC, lrect, RGB(0, 0, 0));
  }

  // Set up text attributes
  if (Redraw)
  {
    static const HeaderLabel kYardHeaders[] =
    {
      {10, "Available locos:"}
    };

    DrawHeaderLabels(TheDC,
                     frame->mHeaderFont,
                     GetSysColor(COLOR_WINDOWTEXT),
                     GetSysColor(COLOR_BTNFACE),
                     5,
                     kYardHeaders,
                     sizeof(kYardHeaders) / sizeof(kYardHeaders[0]));
  }

  ::SetTextColor(TheDC, RGB(255, 255, 255));
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
            ::SetTextColor(TheDC, RGB(128, 128, 128));
          }
          else
          {
            ::SetTextColor(TheDC, RGB(255, 255, 255));
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
  (void)dcGuard;
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

