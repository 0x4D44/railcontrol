// OWLCVT 05/11/95 22:36:23
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
#include <cstdio>

/*************************************************************************/
/* Methods of TArrivals follow...*/

TArrivals::TArrivals(TWindow * AParent, int XPos, int YPos)
  : TFrameWindow(AParent, "Arrivals")
{
  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 720;
  Attr.H = 192;
  Attr.Style = WS_DLGFRAME;
}


TArrivals::~TArrivals()
{
}


void TArrivals::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  WndClass.hIcon = ::LoadIcon(GetModule()->GetHandle(), TEXT("ARRIVAL"));
}


auto TArrivals::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Arrival Window"));
}


void TArrivals::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = ::GetSystemMenu(GetHandle(), FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MINIMIZE, MF_BYCOMMAND);
}


bool TArrivals::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = ::GetMenu(Parent->GetHandle());
  CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_UNCHECKED);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  frame->ArrivalHan = NULL;

  // Return value
  return TRUE;
}


void TArrivals::EvSize(owl::uint sizeType, const TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check the iconized status of the window
  if (IsIconic())
  {
    frame->ArrivalIcon = TRUE;
  }
  else
  {
    frame->ArrivalIcon = FALSE;
  }
}


void TArrivals::EvMove(const TPoint& point)
{
  // Call ancestor method
  TFrameWindow::EvMove(point);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  frame->ArrivalX = Attr.X;
  frame->ArrivalY = Attr.Y;
}


void TArrivals::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Call main painting routine
  UpdateDisplay(dc, TRUE);
}


void TArrivals::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL     MadeDC;
  HPEN     hOldPen, hPen;
  RECT     TheRect;
  int      Exp;
  int      lii = 0;
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
  ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));

  // Now draw timetable header information
  if (Redraw)
  {
    CopyString(TextString, "Due:");
    TextOut(TheDC, ARR_OFFSET_DUE, 5, TextString, strlen(TextString));
    CopyString(TextString, "On:");
    TextOut(TheDC, ARR_OFFSET_ON, 5, TextString, strlen(TextString));
    CopyString(TextString, "+:");
    TextOut(TheDC, ARR_OFFSET_LATE, 5, TextString, strlen(TextString));
    CopyString(TextString, "Loco / MU:");
    TextOut(TheDC, ARR_OFFSET_LOCO, 5, TextString, strlen(TextString));
    CopyString(TextString, "Description:");
    TextOut(TheDC, ARR_OFFSET_ARRDESC, 5, TextString, strlen(TextString));
    CopyString(TextString, "Status:");
    TextOut(TheDC, ARR_OFFSET_STATUS, 5, TextString, strlen(TextString));
    CopyString(TextString, "Dep:");
    TextOut(TheDC, ARR_OFFSET_DEPTIME, 5, TextString, strlen(TextString));
    CopyString(TextString, "Description:");
    TextOut(TheDC, ARR_OFFSET_DEPDESC, 5, TextString, strlen(TextString));
  }

  SelectObject(TheDC, frame->mTextFont);
  if (frame->GameInProgress)
  {
   // Now we draw in the timetable information
    for (lii = 0; lii < DISP_NUM_ARRIVAL; lii++)
    {
      if (Redraw | frame->DisplayHan->ExpChng[lii])
      {
        Exp = frame->DisplayHan->Expect[lii];
        TimeTabInfo(TheDC, Exp, 25 + (lii * 17));
      }
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);

  return;
}


void TArrivals::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  // This routine draws in the timetable information as specified in
  // <Timetable[TimetabVal]>. It is called exclusively by
  // <UpdateDisplay>

  long    a           = 0;
  int     lii         = 0;
  int     llocoid     = 0;
  RECT    lRect;
  char    TextString[100], Str2[21], ArrPntName[4], lTempStr[100];

  if (TheDC == 0) return;

  /**************************************************************************/
  /* Blank out the previous data.                                           */
  /**************************************************************************/
  SetBkColor(TheDC, RGB(0, 0, 0));
  lRect.left   = 9;
  lRect.top    = YPos;
  lRect.right  = 709;
  lRect.bottom = YPos + 16;
  ExtTextOut(TheDC, 9, YPos, ETO_OPAQUE, &lRect, "", 0, NULL);

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);


  if (TabVal > 0)
  {
   ::SetTextColor(TheDC, RGB(255, 255, 255));

   // Arrival time
   ConvertTime(frame->DisplayHan->PTimetableInfo[TabVal]->GetArrTime(),
               TextString);
   TextOut(TheDC, ARR_OFFSET_DUE, YPos, TextString, strlen(TextString));

   // Arrival line
   a = frame->DisplayHan->PTimetableInfo[TabVal]->GetArrPnt();
   frame->DisplayHan->PSelectorInfo[a]->GetText(ArrPntName, 4);
   TextOut(TheDC, ARR_OFFSET_ON, YPos, ArrPntName, strlen(ArrPntName));

   // Minutes late
   if (frame->DelayEnable)
   {
     _itoa_s(frame->DisplayHan->PTimetableInfo[TabVal]->GetMinLate(), 
         TextString, 
         sizeof(TextString), 
         10);
   }
   else
   {
     CopyString(TextString, "");
   }
   TextOut(TheDC, ARR_OFFSET_LATE, YPos, TextString, strlen(TextString));

   // Loco attached
   CopyString(TextString, "XXXXX");
   for (lii = 0; lii < 2; lii++)
   {
     llocoid = frame->DisplayHan->PTimetableInfo[TabVal]->GetLoco(lii);

     if (llocoid != 0)
     {
       // Get loco number as a character array
       frame->DisplayHan->PLocosInfo[llocoid]->LocoNumStr(lTempStr);

       if (lii == 0)
       {
       FormatBuffer(TextString, "%s", lTempStr);
      }
      else
      {
        AppendString(TextString, "+");
        AppendString(TextString, lTempStr);
      }

      if (frame->DisplayHan->PTimetableInfo[TabVal]->GetDelayFlag() == DLF_DELAYS_MAINT)
      {
        ::SetTextColor(TheDC, RGB(255, 128, 0));
       }
     }
   }
   TextOut(TheDC, ARR_OFFSET_LOCO, YPos, TextString, strlen(TextString));

   // Arrival description
   ::SetTextColor(TheDC, RGB(255, 255, 255));
   frame->DisplayHan->PTimetableInfo[TabVal]->GetArrDesc(TextString);
   TextOut(TheDC, ARR_OFFSET_ARRDESC, YPos, TextString, strlen(TextString));

   // Arrival status
   if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_DUE)
   {
     ::SetTextColor(TheDC, RGB(255, 255, 255));
    FormatBuffer(TextString, "Expected on %s", ArrPntName);
     TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
   }
   if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_APPROACH)
   {
     ::SetTextColor(TheDC, RGB(192, 192, 0));
    FormatBuffer(TextString, "Approaching on %s", ArrPntName);
     TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
   }
   if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_HELD)
   {
     ::SetTextColor(TheDC, RGB(255, 0, 0));
    FormatBuffer(TextString, "Held on %s", ArrPntName);
     TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
   }
   if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_FIRSTHELD)
   {
     ::SetTextColor(TheDC, RGB(255, 0, 0));
    FormatBuffer(TextString, "First held on %s", ArrPntName);
     TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
   }
   if ((frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() >= ST_SETPLAT) &&
       (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() <= ST_ARRC))
   {
     ::SetTextColor(TheDC, RGB(0, 127, 0));
     a = frame->DisplayHan->PTimetableInfo[TabVal]->GetRoute();
     frame->DisplayHan->PSelectorInfo[frame->DisplayHan->PRoutesInfo[a]->GetTo()]->GetText(Str2, 20);
     CopyString(TextString, "Set to platform ");
     AppendString(TextString, Str2);
     TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    }

    // Departure time
    ::SetTextColor(TheDC, RGB(255, 255, 255));

    if (frame->DisplayHan->PTimetableInfo[TabVal]->GetDelayFlag() == DLF_DELAYS_THRU)
    {
      // Through train, so display as such.
      CopyString(TextString, "THRU");
    }
    else
    {
      // Regular train.
      ConvertTime(frame->DisplayHan->PTimetableInfo[TabVal]->GetDepTime(), TextString);
    }

    TextOut(TheDC, ARR_OFFSET_DEPTIME, YPos, TextString, strlen(TextString));

    // Departure description
    frame->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString);
    TextOut(TheDC, ARR_OFFSET_DEPDESC, YPos, TextString, strlen(TextString));
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
    CopyString(TextString, "XX:XX");
  }
  else
  { 
    Hr = TimeVal /100;
  
    _itoa_s(Hr, TextStrtwo, sizeof(TextStrtwo), 10);
    if (Hr < 10)
    {
      CopyString(TextString, "0");
      AppendString(TextString, TextStrtwo);
    }
    else
    {
      CopyString(TextString, TextStrtwo);
    }
    AppendString(TextString, ":");

    Mn = TimeVal - 100*Hr;

    if (Mn < 10) AppendString(TextString, "0");
    _itoa_s(Mn, TextStrtwo, sizeof(TextStrtwo), 10);
    AppendString(TextString, TextStrtwo); 
  }

  // Copy time field back to main field
  CopyString(TimeStr, 7, TextString);
}






DEFINE_RESPONSE_TABLE1(TArrivals, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
