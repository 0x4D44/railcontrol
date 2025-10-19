// OWLCVT 05/11/95 22:34:34
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

TDepartur::TDepartur(TWindow * AParent, int XPos, int YPos)
  : TFrameWindow(AParent, "Departures")
{
  // Set up window attributes
  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = 300;
  Attr.H = 192;
  Attr.Style = WS_DLGFRAME;
}


TDepartur::~TDepartur()
{
}


void TDepartur::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  WndClass.hIcon = ::LoadIcon(GetModule()->GetHandle(), TEXT("DEPART"));
}


auto TDepartur::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Departure Window"));
}


void TDepartur::SetupWindow()
{
  HMENU  hSysMenu;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = ::GetSystemMenu(GetHandle(), FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
}


bool TDepartur::CanClose()
{
  HMENU  TheMenu;

  // Check main menu appropriately
  TheMenu = ::GetMenu(Parent->GetHandle());
  CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_UNCHECKED);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  if (frame)  frame->DeparturHan = NULL;

  // Return value
  return TRUE;
}


void TDepartur::EvSize(owl::uint sizeType, const TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check the iconized status of the window
  if (::IsIconic(GetHandle()))
  {
   if (frame)  frame->DeparturIcon = TRUE;
  }
  else
  {
   if (frame)  frame->DeparturIcon = FALSE;
  }
}


void TDepartur::EvMove(const TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvMove(pnt);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  if (frame)  frame->DeparturX = Attr.X;
  if (frame)  frame->DeparturY = Attr.Y;
}


void TDepartur::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Call ancestor method
  TFrameWindow::Paint(dc, erase, rect);

  // Call main painting routine
  UpdateDisplay(dc, TRUE);
}


void TDepartur::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL     MadeDC;
  HPEN     hOldPen, hPen;
  RECT     TheRect;
  int      i;
  int      Dep;
  char     TextString[40];
  COLORREF loldBackColour;
  RECT     lrect;

  TRC_NRM((TB, "Enter TDepartur::UpdateDisplay (Redraw:%s)", 
           Redraw ? "TRUE" : "FALSE"));

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

  ::GetClientRect(HWindow, &TheRect);
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Draw a bounding box (3D effect)
  if (Redraw)
  {
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
    CopyString(TextString, "Dep:");
    TextOut(TheDC, 10, 5, TextString, strlen(TextString));
    CopyString(TextString, "Type:");
    TextOut(TheDC, 50, 5, TextString, strlen(TextString));
    CopyString(TextString, "Plat:");
    TextOut(TheDC, 90, 5, TextString, strlen(TextString));
    CopyString(TextString, "Description:");
    TextOut(TheDC, 130, 5, TextString, strlen(TextString));
  }

  SelectObject(TheDC, frame->mTextFont);
  if (frame->GameInProgress)
  {
    // Now we draw in the timetable information
    for (i = 0; i < DISP_NUM_DEPART; i++)
    {
      Dep = frame->DisplayHan->Depart[i];
      if (Dep > 0)
      {
        if (Redraw || frame->DisplayHan->DepChng[i] ||
            (frame->DisplayHan->PTimetableInfo[Dep]->GetStatus() != OldStatus[i]))
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
    for (i = 0; i < DISP_NUM_DEPART; i++)
    {
      Dep = frame->DisplayHan->Depart[i];
      if (Dep > 0)
      {
        OldStatus[i] = frame->DisplayHan->PTimetableInfo[Dep]->GetStatus();
      }
      else
      {
        OldStatus[i] = 0;
      }
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);

  return;
}


void TDepartur::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  // This routine draws in the timetable information as specified in
  // <Timetable[TimetabVal]>. It is called exclusively by
  // <UpdateDisplay>

  int      a, i;
  char     TextString[21], PlatName[10];
  HBRUSH   hBrush, hOldBrush;
  HPEN     hPen, hOldPen;
  RECT     lRect;
  int      lStatus;
  COLORREF lColour;
  BOOL     NotFound;
  BOOL     lDelayed;

  if (TheDC == 0) return;
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  /**************************************************************************/
  /* Blank out the previous data.                                           */
  /**************************************************************************/
  SetBkColor(TheDC, RGB(0, 0, 0));
  lRect.left   = 9;
  lRect.top    = (YPos);
  lRect.right  = 246;
  lRect.bottom = YPos + 17;
  ExtTextOut(TheDC, 9, YPos, ETO_OPAQUE, &lRect, "", 0, NULL);
  ::SetTextColor(TheDC, RGB(255, 255, 255));

  if (TabVal > 0)
  {
    // Decide which colour text is to be
    lStatus = frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus();

    // Departure platform
    NotFound = TRUE;
    i = 1;
    while (i < 50 && NotFound)
    {
      if (frame->DisplayHan->PPlatDataInfo[i] != 0)
      {
        if ((frame->DisplayHan->PPlatDataInfo[i]->GetTimetable(0) == TabVal) ||
            (frame->DisplayHan->PPlatDataInfo[i]->GetTimetable(1) == TabVal))
        {
          // Get platform name text
          a = frame->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
          frame->DisplayHan->PSelectorInfo[a]->GetText(PlatName, 4);
          NotFound = FALSE;
        }
      }
      i++;
    }

    /************************************************************************/
    /* Figure out if the departure has been delayed.                        */
    /************************************************************************/
    a = frame->DisplayHan->PTimetableInfo[TabVal]->GetDepTime();
    if (frame->DisplayHan->WorkTime > 2*(a - 40*int(a/100)))
    {
      lDelayed = TRUE;
    }
    else
    {
      lDelayed = FALSE;
    }

    if (!NotFound)
    {
      /**********************************************************************/
      /* Use center text alignment.                                         */
      /**********************************************************************/
      SetTextAlign(TheDC, TA_CENTER);

      if (lStatus == ST_READYDEP)
      {
        if (lDelayed)
        {
          lColour = RGB(255, 0, 0);
        }
        else
        {
          lColour = RGB(0, 255, 0);
        }

        /********************************************************************/
        /* Create and select pen and brush.                                 */
        /********************************************************************/
        hBrush = CreateSolidBrush(lColour);
        hPen   = CreatePen(PS_SOLID, 1, lColour);

        hOldBrush = (HBRUSH) SelectObject(TheDC, hBrush);
        hOldPen   = (HPEN) SelectObject(TheDC, hPen);

        /********************************************************************/
        /* Draw the circle.                                                 */
        /********************************************************************/
        Ellipse(TheDC, 91, (YPos), 109, (YPos + 17));

        /********************************************************************/
        /* Tidy up.                                                         */
        /********************************************************************/
        SelectObject(TheDC, hOldBrush);
        SelectObject(TheDC, hOldPen);

        DeleteObject(hBrush);
        DeleteObject(hPen);

        SetBkColor(TheDC, lColour);
        ::SetTextColor(TheDC, RGB(0, 0, 0));
        SelectObject(TheDC, frame->mSmallFont);

        TextOut(TheDC, 100, (YPos + 2), PlatName, strlen(PlatName));
      }
      else
      {
      if (lStatus == ST_STOCKOK)
      {
        // Train is ready to depart
        ::SetTextColor(TheDC, RGB(255, 255, 255));
        CopyString(TextString, PlatName);
      }
      else
      {
          /******************************************************************/
          /* Figure the colour to use to display the train.                 */
          /******************************************************************/
          if (lDelayed)
          {
            ::SetTextColor(TheDC, RGB(255, 0, 0));
          }
          else
          {
            if (lStatus == ST_RELEASE)
            {
              ::SetTextColor(TheDC, RGB(225, 225, 0));
            }
            else
            {
              ::SetTextColor(TheDC, RGB(255, 255, 255));
            }
          }

          // Train is not ready to depart
          CopyString(TextString, "(");
          AppendString(TextString, PlatName);
          AppendString(TextString, ")");
        }

        TextOut(TheDC, 100, YPos, TextString, strlen(TextString));
      }

      /**********************************************************************/
      /* Reset text alignment.                                              */
      /**********************************************************************/
      SetTextAlign(TheDC, TA_LEFT);
    }

    if (NotFound)
    {
      ::SetTextColor(TheDC, RGB(128, 128, 128));
    }
    else
    {
      ::SetTextColor(TheDC, RGB(255, 255, 255));
    }

    /************************************************************************/
    /* Ensure background colour is black.                                   */
    /************************************************************************/
    SetBkColor(TheDC, RGB(0, 0, 0));
    SelectObject(TheDC, frame->mTextFont);

    // Departure time
    ConvertTime(frame->DisplayHan->PTimetableInfo[TabVal]->GetDepTime(),
                TextString);
    TextOut(TheDC, 10, YPos, TextString, strlen(TextString));

    // Departure code
    a = frame->DisplayHan->PTimetableInfo[TabVal]->GetDepCode();
    if (a == SC_HST) TextOut(TheDC, 55, YPos, "HST", 3);
    if (a == SC_EMU) TextOut(TheDC, 55, YPos, "EMU", 3);
    if (a == SC_DELTIC) TextOut(TheDC, 55, YPos, "DEL", 3);
    if (a == SC_NORMAL) TextOut(TheDC, 55, YPos, "NRM", 3);
    if (a == SC_RELIEF) TextOut(TheDC, 55, YPos, "RLF", 3);
    if (a == SC_DMU1) TextOut(TheDC, 55, YPos, "DMU", 3);
    if (a == SC_DMU150) TextOut(TheDC, 55, YPos, "SPR", 3);
    if (a == SC_PUSHPULL) TextOut(TheDC, 55, YPos, "P/P", 3);
    if (a == SC_CLASS37) TextOut(TheDC, 55, YPos, "37/4", 4);
    if (a == SC_DMU156) TextOut(TheDC, 55, YPos, "156", 3);
    if (a == SC_DMU158) TextOut(TheDC, 55, YPos, "158", 3);
    if (a == SC_DMU170) TextOut(TheDC, 55, YPos, "170", 3);
    if (a == SC_TWIN156) TextOut(TheDC, 55, YPos, "T156", 4);
    if (a == SC_TWIN158) TextOut(TheDC, 55, YPos, "T158", 4);
    if (a == SC_TWIN170) TextOut(TheDC, 55, YPos, "T170", 4);

    // Departure description
    frame->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString, sizeof(TextString));
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
    CopyString(TextString, "XX:XX");
  }
  else
  { 
    Hr = TimeVal / 100;
  
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




DEFINE_RESPONSE_TABLE1(TDepartur, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
