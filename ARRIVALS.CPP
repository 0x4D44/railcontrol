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

namespace
{
  int MeasureStringWidth(HDC dc, const char* text)
  {
    if ((dc == 0) || (text == 0))
    {
      return 0;
    }

    int length = lstrlenA(text);
    if (length <= 0)
    {
      return 0;
    }

    SIZE extent = {0, 0};
    if (::GetTextExtentPoint32(dc, text, length, &extent))
    {
      return extent.cx;
    }

    return 0;
  }
}

/*************************************************************************/
/* Methods of TArrivals follow...*/

TArrivals::TArrivals(TWindow * AParent, int XPos, int YPos)
  : TFrameWindow(AParent, "Arrivals"),
    mRowHeight(16),
    mRowSpacing(18),
    mClientWidth(0),
    mClientHeight(0),
    mContentRight(0),
    mContentBottom(0)
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


void TArrivals::ResetContentExtents()
{
  mContentRight = 0;
  mContentBottom = 0;
}


void TArrivals::UpdateMetrics(HDC dc)
{
  if (dc == 0)
  {
    return;
  }

  TEXTMETRIC metrics = {0};
  if (::GetTextMetrics(dc, &metrics))
  {
    mRowHeight = metrics.tmHeight;
    int gap = metrics.tmExternalLeading;
    if (gap < 2)  gap = 2;
    mRowSpacing = mRowHeight + gap;
  }
}


void TArrivals::TrackContent(int right, int bottom)
{
  if (right > mContentRight)  mContentRight = right;
  if (bottom > mContentBottom)  mContentBottom = bottom;
}


void TArrivals::EnsureContentFits()
{
  RECT client = {0, 0, 0, 0};
  ::GetClientRect(HWindow, &client);
  const int currentWidth = client.right - client.left;
  const int currentHeight = client.bottom - client.top;

  int desiredWidth = mContentRight + 5;
  int desiredHeight = mContentBottom + 5;
  if (desiredWidth < 0)  desiredWidth = 0;
  if (desiredHeight < 0)  desiredHeight = 0;

  if ((desiredWidth <= currentWidth) && (desiredHeight <= currentHeight))
  {
    return;
  }

  const int targetWidth = (desiredWidth > currentWidth) ? desiredWidth : currentWidth;
  const int targetHeight = (desiredHeight > currentHeight) ? desiredHeight : currentHeight;
  RECT windowRect = {0, 0, targetWidth, targetHeight};

  DWORD style = ::GetWindowLong(HWindow, GWL_STYLE);
  DWORD exStyle = ::GetWindowLong(HWindow, GWL_EXSTYLE);
  BOOL hasMenu = (::GetMenu(HWindow) != 0) ? TRUE : FALSE;
  if (!::AdjustWindowRectEx(&windowRect, style, hasMenu, exStyle))
  {
    return;
  }

  const int windowWidth = windowRect.right - windowRect.left;
  const int windowHeight = windowRect.bottom - windowRect.top;
  ::SetWindowPos(HWindow, 0, 0, 0, windowWidth, windowHeight,
                 SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
  Attr.W = windowWidth;
  Attr.H = windowHeight;
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
  TReleaseDcGuard dcGuard;
  if (TheDC == 0)
  {
    dcGuard.Attach(HWindow, GetDC(HWindow));
    TheDC = dcGuard.Get();
  }
  if (!TheDC)
  {
    return;
  }

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  SelectObject(TheDC, frame->mTextFont);
  UpdateMetrics(TheDC);
  ResetContentExtents();

  RECT TheRect = {0, 0, 0, 0};
  ::GetClientRect(HWindow, &TheRect);
  mClientWidth = TheRect.right;
  mClientHeight = TheRect.bottom;

  if (Redraw)
  {
    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      DrawRaisedPanel(TheDC, TheRect, 23);
    }

    RECT panelRect;
    panelRect.left   = 4;
    panelRect.right  = TheRect.right - 5;
    panelRect.top    = 24;
    panelRect.bottom = TheRect.bottom - 5;
    FillRectOpaque(TheDC, panelRect, RGB(0, 0, 0));
    TrackContent(panelRect.right, panelRect.bottom);
  }

  if (Redraw)
  {
    static const HeaderLabel kArrivalHeaders[] =
    {
      {ARR_OFFSET_DUE, "Due:"},
      {ARR_OFFSET_ON, "On:"},
      {ARR_OFFSET_LATE, "+:"},
      {ARR_OFFSET_LOCO, "Loco / MU:"},
      {ARR_OFFSET_ARRDESC, "Description:"},
      {ARR_OFFSET_STATUS, "Status:"},
      {ARR_OFFSET_DEPTIME, "Dep:"},
      {ARR_OFFSET_DEPDESC, "Description:"}
    };

    HFONT oldFont = (HFONT) SelectObject(TheDC, frame->mHeaderFont);
    TEXTMETRIC headerMetrics = {0};
    if (::GetTextMetrics(TheDC, &headerMetrics))
    {
      const int headerBottom = 5 + headerMetrics.tmHeight;
      for (size_t idx = 0; idx < sizeof(kArrivalHeaders) / sizeof(kArrivalHeaders[0]); ++idx)
      {
        const HeaderLabel& label = kArrivalHeaders[idx];
        int width = MeasureStringWidth(TheDC, label.text);
        TrackContent(label.x + width, headerBottom);
      }
    }
    SelectObject(TheDC, oldFont);

    DrawHeaderLabels(TheDC,
                     frame->mHeaderFont,
                     GetSysColor(COLOR_WINDOWTEXT),
                     GetSysColor(COLOR_BTNFACE),
                     5,
                     kArrivalHeaders,
                     sizeof(kArrivalHeaders) / sizeof(kArrivalHeaders[0]));
    SelectObject(TheDC, frame->mTextFont);
  }

  ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));

  if (frame->GameInProgress)
  {
    for (int lii = 0; lii < DISP_NUM_ARRIVAL; lii++)
    {
      if (Redraw || frame->DisplayHan->ExpChng[lii])
      {
        int Exp = frame->DisplayHan->Expect[lii];
        const int rowTop = 25 + (lii * mRowSpacing);
        TimeTabInfo(TheDC, Exp, rowTop);
      }
    }
  }

  EnsureContentFits();
}


void TArrivals::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  if (TheDC == 0)
  {
    return;
  }

  RECT lRect;
  SetBkColor(TheDC, RGB(0, 0, 0));
  lRect.left   = 9;
  lRect.top    = YPos;
  lRect.right  = (mClientWidth > 5) ? (mClientWidth - 5) : (YPos + mRowHeight);
  lRect.bottom = YPos + mRowHeight;
  ExtTextOut(TheDC, 9, YPos, ETO_OPAQUE, &lRect, "", 0, NULL);
  TrackContent(lRect.right, lRect.bottom);

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  if (TabVal <= 0)
  {
    return;
  }

  ::SetTextColor(TheDC, RGB(255, 255, 255));

  char TextString[100];
  char Str2[21];
  char ArrPntName[4];
  char lTempStr[100];
  long a = 0;

  // Suppress unused-variable warnings in diagnostic builds that
  // short-circuit before the formatting logic runs.
  (void)TextString;

  // Arrival time
  ConvertTime(frame->DisplayHan->PTimetableInfo[TabVal]->GetArrTime(), TextString);
  TextOut(TheDC, ARR_OFFSET_DUE, YPos, TextString, strlen(TextString));
  TrackContent(ARR_OFFSET_DUE + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  // Arrival line
  a = frame->DisplayHan->PTimetableInfo[TabVal]->GetArrPnt();
  frame->DisplayHan->PSelectorInfo[a]->GetText(ArrPntName, 4);
  TextOut(TheDC, ARR_OFFSET_ON, YPos, ArrPntName, strlen(ArrPntName));
  TrackContent(ARR_OFFSET_ON + MeasureStringWidth(TheDC, ArrPntName), YPos + mRowHeight);

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
    CopyBuffer(TextString, "");
  }
  TextOut(TheDC, ARR_OFFSET_LATE, YPos, TextString, strlen(TextString));
  TrackContent(ARR_OFFSET_LATE + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  // Loco attached
  CopyBuffer(TextString, "XXXXX");
  for (int lii = 0; lii < 2; lii++)
  {
    int llocoid = frame->DisplayHan->PTimetableInfo[TabVal]->GetLoco(lii);
    if (llocoid != 0)
    {
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
  TrackContent(ARR_OFFSET_LOCO + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  ::SetTextColor(TheDC, RGB(255, 255, 255));
  frame->DisplayHan->PTimetableInfo[TabVal]->GetArrDesc(TextString, sizeof(TextString));
  TextOut(TheDC, ARR_OFFSET_ARRDESC, YPos, TextString, strlen(TextString));
  TrackContent(ARR_OFFSET_ARRDESC + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  int width = 0;
  if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_DUE)
  {
    ::SetTextColor(TheDC, RGB(255, 255, 255));
    FormatBuffer(TextString, "Expected on %s", ArrPntName);
    TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    width = MeasureStringWidth(TheDC, TextString);
    TrackContent(ARR_OFFSET_STATUS + width, YPos + mRowHeight);
  }
  if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_APPROACH)
  {
    ::SetTextColor(TheDC, RGB(192, 192, 0));
    FormatBuffer(TextString, "Approaching on %s", ArrPntName);
    TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    width = MeasureStringWidth(TheDC, TextString);
    TrackContent(ARR_OFFSET_STATUS + width, YPos + mRowHeight);
  }
  if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_HELD)
  {
    ::SetTextColor(TheDC, RGB(255, 0, 0));
    FormatBuffer(TextString, "Held on %s", ArrPntName);
    TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    width = MeasureStringWidth(TheDC, TextString);
    TrackContent(ARR_OFFSET_STATUS + width, YPos + mRowHeight);
  }
  if (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() == ST_FIRSTHELD)
  {
    ::SetTextColor(TheDC, RGB(255, 0, 0));
    FormatBuffer(TextString, "First held on %s", ArrPntName);
    TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    width = MeasureStringWidth(TheDC, TextString);
    TrackContent(ARR_OFFSET_STATUS + width, YPos + mRowHeight);
  }
  if ((frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() >= ST_SETPLAT) &&
      (frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus() <= ST_ARRC))
  {
    ::SetTextColor(TheDC, RGB(0, 127, 0));
    a = frame->DisplayHan->PTimetableInfo[TabVal]->GetRoute();
    frame->DisplayHan->PSelectorInfo[frame->DisplayHan->PRoutesInfo[a]->GetTo()]->GetText(Str2, 20);
    CopyBuffer(TextString, "Set to platform ");
    AppendString(TextString, Str2);
    TextOut(TheDC, ARR_OFFSET_STATUS, YPos, TextString, strlen(TextString));
    width = MeasureStringWidth(TheDC, TextString);
    TrackContent(ARR_OFFSET_STATUS + width, YPos + mRowHeight);
  }

  ::SetTextColor(TheDC, RGB(255, 255, 255));

  if (frame->DisplayHan->PTimetableInfo[TabVal]->GetDelayFlag() == DLF_DELAYS_THRU)
  {
    CopyBuffer(TextString, "THRU");
  }
  else
  {
    ConvertTime(frame->DisplayHan->PTimetableInfo[TabVal]->GetDepTime(), TextString);
  }
  TextOut(TheDC, ARR_OFFSET_DEPTIME, YPos, TextString, strlen(TextString));
  TrackContent(ARR_OFFSET_DEPTIME + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  frame->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString, sizeof(TextString));
  TextOut(TheDC, ARR_OFFSET_DEPDESC, YPos, TextString, strlen(TextString));
  TrackContent(ARR_OFFSET_DEPDESC + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);
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
    CopyBuffer(TextString, "XX:XX");
  }
  else
  { 
    Hr = TimeVal /100;
  
    _itoa_s(Hr, TextStrtwo, sizeof(TextStrtwo), 10);
    if (Hr < 10)
    {
      CopyBuffer(TextString, "0");
      AppendString(TextString, TextStrtwo);
    }
    else
    {
      CopyBuffer(TextString, TextStrtwo);
    }
    AppendString(TextString, ":");

    Mn = TimeVal - 100*Hr;

    if (Mn < 10) AppendString(TextString, "0");
    _itoa_s(Mn, TextStrtwo, sizeof(TextStrtwo), 10);
    AppendString(TextString, TextStrtwo); 
  }

  // Copy time field back to main field
  CopyBuffer(TimeStr, 7, TextString);
}






DEFINE_RESPONSE_TABLE1(TArrivals, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
