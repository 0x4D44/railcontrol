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

  template <size_t BufferSize>
  bool FormatStageStatusString(TLayout* layout,
                               int timetableId,
                               char (&buffer)[BufferSize])
  {
    if ((layout == NULL) || (BufferSize == 0))
    {
      return false;
    }

    int stage = -1;
    int stageIndex = -1;
    int stagePrimary = 0;
    int stageSecondary = 0;
    long progressMs = -1;
    if (!layout->GetStageTelemetry(timetableId,
                                   stage,
                                   stageIndex,
                                   stagePrimary,
                                   stageSecondary,
                                   progressMs))
    {
      return false;
    }

    bool hasText = false;
    buffer[0] = 0;

    if (stage >= 0)
    {
      FormatBuffer(buffer, "Stage %d", stage);
      hasText = true;
    }

    if (stageIndex >= 0)
    {
      if (hasText)
      {
        AppendString(buffer, " (");
        char tmp[32];
        FormatBuffer(tmp, "bucket %d", stageIndex + 1);
        AppendString(buffer, tmp);
        AppendString(buffer, ")");
      }
      else
      {
        FormatBuffer(buffer, "Bucket %d", stageIndex + 1);
      }
      hasText = true;
    }

    if ((stagePrimary != 0) || (stageSecondary != 0))
    {
      char tmp[48];
      if (stageSecondary != 0)
      {
        FormatBuffer(tmp, "sec %d->%d", stagePrimary, stageSecondary);
      }
      else
      {
        FormatBuffer(tmp, "sec %d", stagePrimary);
      }

      if (hasText)
      {
        AppendString(buffer, " ");
      }
      AppendString(buffer, tmp);
      hasText = true;
    }

    return hasText;
  }
}

/*************************************************************************/
/* Methods of TDepartur follow...*/

TDepartur::TDepartur(TWindow * AParent, int XPos, int YPos)
  : TFrameWindow(AParent, "Departures"),
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
  Attr.W = 300;
  Attr.H = 192;
  Attr.Style = WS_DLGFRAME;

  for (int index = 0; index < DISP_NUM_DEPART; index++)
  {
    OldStatus[index] = 0;
  }
}


TDepartur::~TDepartur()
{
}


void TDepartur::ResetContentExtents()
{
  mContentRight = 0;
  mContentBottom = 0;
}


void TDepartur::UpdateMetrics(HDC dc)
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


void TDepartur::TrackContent(int right, int bottom)
{
  if (right > mContentRight)  mContentRight = right;
  if (bottom > mContentBottom)  mContentBottom = bottom;
}


void TDepartur::EnsureContentFits()
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

  RECT beforeRect = {0, 0, 0, 0};
  ::GetWindowRect(HWindow, &beforeRect);

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
  const BOOL resized = ::SetWindowPos(HWindow, 0, 0, 0, windowWidth, windowHeight,
                                      SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
  if (!resized)
  {
    return;
  }

  RECT afterRect = beforeRect;
  ::GetWindowRect(HWindow, &afterRect);

  const int actualWidth = afterRect.right - afterRect.left;
  const int actualHeight = afterRect.bottom - afterRect.top;
  Attr.W = actualWidth;
  Attr.H = actualHeight;

  const bool sizeChanged = ((actualWidth != (beforeRect.right - beforeRect.left)) ||
                            (actualHeight != (beforeRect.bottom - beforeRect.top)));
  if (!sizeChanged)
  {
    return;
  }

  ::InvalidateRect(HWindow, NULL, TRUE);

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  if (frame != NULL)
  {
    frame->HandleChildResize(this, beforeRect, afterRect);
  }
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

  RECT     TheRect;
  int      i;
  int      Dep;
  RECT     lrect;

  TRC_NRM((TB, "Enter TDepartur::UpdateDisplay (Redraw:%s)", 
           Redraw ? "TRUE" : "FALSE"));

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  SelectObject(TheDC, frame->mTextFont);
  UpdateMetrics(TheDC);
  ResetContentExtents();

  const int baselineRowBottom = 25 + ((DISP_NUM_DEPART - 1) * mRowSpacing) + mRowHeight;
  int baselineRight = 10 + MeasureStringWidth(TheDC, "23:59");
  int candidateRight = 55 + MeasureStringWidth(TheDC, "T170");
  if (candidateRight > baselineRight)  baselineRight = candidateRight;
  candidateRight = 130 + MeasureStringWidth(TheDC, "Express to King's Cross (non-stop)");
  if (candidateRight > baselineRight)  baselineRight = candidateRight;
  TrackContent(baselineRight, baselineRowBottom);

  ::GetClientRect(HWindow, &TheRect);
  mClientWidth = TheRect.right;
  mClientHeight = TheRect.bottom;

  // Compute content rect always; draw only on redraw
  lrect.left   = 4;
  lrect.right  = TheRect.right - 5;
  lrect.top    = 24;
  lrect.bottom = TheRect.bottom - 5;
  TrackContent(lrect.right, lrect.bottom);
  if (Redraw)
  {
    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      DrawRaisedPanel(TheDC, TheRect, 23);
    }
    FillRectOpaque(TheDC, lrect, RGB(0, 0, 0));
  }

  static const HeaderLabel kDepartHeaders[] =
  {
    {10, "Dep:"},
    {50, "Type:"},
    {90, "Plat:"},
    {130, "Description:"}
  };

  // Measure header extents always
  {
    HFONT oldFont = (HFONT) SelectObject(TheDC, frame->mHeaderFont);
    TEXTMETRIC headerMetrics = {0};
    if (::GetTextMetrics(TheDC, &headerMetrics))
    {
      const int headerBottom = 5 + headerMetrics.tmHeight;
      for (size_t idx = 0; idx < sizeof(kDepartHeaders) / sizeof(kDepartHeaders[0]); ++idx)
      {
        const HeaderLabel& label = kDepartHeaders[idx];
        int width = MeasureStringWidth(TheDC, label.text);
        TrackContent(label.x + width, headerBottom);
      }
    }
    SelectObject(TheDC, oldFont);
  }

  if (Redraw)
  {
    DrawHeaderLabels(TheDC,
                     frame->mHeaderFont,
                     GetSysColor(COLOR_WINDOWTEXT),
                     GetSysColor(COLOR_BTNFACE),
                     5,
                     kDepartHeaders,
                     sizeof(kDepartHeaders) / sizeof(kDepartHeaders[0]));
    SelectObject(TheDC, frame->mTextFont);
  }

  ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
  SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));
  if (frame->GameInProgress)
  {
    for (i = 0; i < DISP_NUM_DEPART; i++)
    {
      Dep = frame->DisplayHan->Depart[i];
      if (Dep > 0)
      {
        if (Redraw || frame->DisplayHan->DepChng[i] ||
            (frame->DisplayHan->PTimetableInfo[Dep]->GetStatus() != OldStatus[i]))
        {
          TimeTabInfo(TheDC, Dep, 25 + (i * mRowSpacing));
        }
      }
      else
      {
        TimeTabInfo(TheDC, Dep, 25 + (i * mRowSpacing));
      }
    }

    for (i = 0; i < DISP_NUM_DEPART; i++)
    {
      Dep = frame->DisplayHan->Depart[i];
      OldStatus[i] = (Dep > 0) ? frame->DisplayHan->PTimetableInfo[Dep]->GetStatus() : 0;
    }
  }

  EnsureContentFits();
}


void TDepartur::TimeTabInfo(HDC TheDC, int TabVal, int YPos)
{
  // This routine draws in the timetable information as specified in
  // <Timetable[TimetabVal]>. It is called exclusively by
  // <UpdateDisplay>

  int      a = 0;
  int      i = 0;
  char     TextString[100];
  char     PlatName[10];
  RECT     lRect;
  COLORREF lColour = RGB(255, 255, 255);

  // Suppress unused-variable warnings in diagnostic-only code paths.
  (void)TextString;

  if (TheDC == 0) return;
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  SetBkColor(TheDC, RGB(0, 0, 0));
  lRect.left   = 9;
  lRect.top    = YPos;
  lRect.right  = (mClientWidth > 5) ? (mClientWidth - 5) : (YPos + mRowHeight);
  lRect.bottom = YPos + mRowHeight;
  ExtTextOut(TheDC, 9, YPos, ETO_OPAQUE, &lRect, "", 0, NULL);
  TrackContent(lRect.right, lRect.bottom);
  ::SetTextColor(TheDC, RGB(255, 255, 255));

  if (TabVal <= 0)
  {
    return;
  }

  const int lStatus = frame->DisplayHan->PTimetableInfo[TabVal]->GetStatus();

  BOOL NotFound = TRUE;
  i = 1;
  while (i < 50 && NotFound)
  {
    if (frame->DisplayHan->PPlatDataInfo[i] != 0)
    {
      if ((frame->DisplayHan->PPlatDataInfo[i]->GetTimetable(0) == TabVal) ||
          (frame->DisplayHan->PPlatDataInfo[i]->GetTimetable(1) == TabVal))
      {
        a = frame->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
        frame->DisplayHan->PSelectorInfo[a]->GetText(PlatName, 4);
        NotFound = FALSE;
      }
    }
    i++;
  }

  const int depTime = frame->DisplayHan->PTimetableInfo[TabVal]->GetDepTime();
  BOOL lDelayed = (frame->DisplayHan->WorkTime > 2*(depTime - 40*int(depTime/100))) ? TRUE : FALSE;

  if (!NotFound)
  {
    SetTextAlign(TheDC, TA_CENTER);

    if (lStatus == ST_READYDEP)
    {
      lColour = lDelayed ? RGB(255, 0, 0) : RGB(0, 255, 0);

      // SECURITY FIX (MEM-02): Use RAII guards for automatic GDI object cleanup
      TBrushGuard hBrush(CreateSolidBrush(lColour));
      TPenGuard hPen(CreatePen(PS_SOLID, 1, lColour));

      HBRUSH hOldBrush = (HBRUSH) SelectObject(TheDC, hBrush);
      HPEN hOldPen = (HPEN) SelectObject(TheDC, hPen);

      const int circleLeft = 91;
      const int circleRight = circleLeft + mRowHeight;
      Ellipse(TheDC, circleLeft, YPos, circleRight, YPos + mRowHeight);
      TrackContent(circleRight, YPos + mRowHeight);

      SelectObject(TheDC, hOldBrush);
      SelectObject(TheDC, hOldPen);
      // DeleteObject calls removed - RAII guards handle cleanup automatically

      SetBkColor(TheDC, lColour);
      ::SetTextColor(TheDC, RGB(0, 0, 0));
      SelectObject(TheDC, frame->mSmallFont);

      TEXTMETRIC smallMetrics = {0};
      int textY = YPos + 2;
      int textBottom = YPos + mRowHeight;
      if (::GetTextMetrics(TheDC, &smallMetrics))
      {
        textY = YPos + ((mRowHeight - smallMetrics.tmHeight) / 2);
        textBottom = textY + smallMetrics.tmHeight;
      }

      TextOut(TheDC, 100, textY, PlatName, strlen(PlatName));
      int platWidth = MeasureStringWidth(TheDC, PlatName);
      int rightEdge = 100 + ((platWidth + 1) / 2);
      TrackContent(rightEdge, textBottom);

      SetBkColor(TheDC, RGB(0, 0, 0));
      SelectObject(TheDC, frame->mTextFont);
    }
    else
    {
      if (lStatus == ST_STOCKOK)
      {
        ::SetTextColor(TheDC, RGB(255, 255, 255));
        CopyBuffer(TextString, PlatName);
      }
      else
      {
        if (lDelayed)
        {
          ::SetTextColor(TheDC, RGB(255, 0, 0));
        }
        else if (lStatus == ST_RELEASE)
        {
          ::SetTextColor(TheDC, RGB(225, 225, 0));
        }
        else
        {
          ::SetTextColor(TheDC, RGB(255, 255, 255));
        }

        CopyBuffer(TextString, "(");
        AppendString(TextString, PlatName);
        AppendString(TextString, ")");
      }

      TextOut(TheDC, 100, YPos, TextString, strlen(TextString));
      TrackContent(100 + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);
    }

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

  SetBkColor(TheDC, RGB(0, 0, 0));
  SelectObject(TheDC, frame->mTextFont);

  ConvertTime(depTime, TextString);
  TextOut(TheDC, 10, YPos, TextString, strlen(TextString));
  TrackContent(10 + MeasureStringWidth(TheDC, TextString), YPos + mRowHeight);

  a = frame->DisplayHan->PTimetableInfo[TabVal]->GetDepCode();
  const char* codeText = "";
  switch (a)
  {
    case SC_HST:      codeText = "HST"; break;
    case SC_EMU:      codeText = "EMU"; break;
    case SC_DELTIC:   codeText = "DEL"; break;
    case SC_NORMAL:   codeText = "NRM"; break;
    case SC_RELIEF:   codeText = "RLF"; break;
    case SC_DMU1:     codeText = "DMU"; break;
    case SC_DMU150:   codeText = "SPR"; break;
    case SC_PUSHPULL: codeText = "P/P"; break;
    case SC_CLASS37:  codeText = "37/4"; break;
    case SC_DMU156:   codeText = "156"; break;
    case SC_DMU158:   codeText = "158"; break;
    case SC_DMU170:   codeText = "170"; break;
    case SC_TWIN156:  codeText = "T156"; break;
    case SC_TWIN158:  codeText = "T158"; break;
    case SC_TWIN170:  codeText = "T170"; break;
    default:          codeText = ""; break;
  }

  if (codeText[0] != '\0')
  {
    TextOut(TheDC, 55, YPos, codeText, int(strlen(codeText)));
    TrackContent(55 + MeasureStringWidth(TheDC, codeText), YPos + mRowHeight);
  }

  frame->DisplayHan->PTimetableInfo[TabVal]->GetDepDesc(TextString, sizeof(TextString));
  const int depDescWidth = MeasureStringWidth(TheDC, TextString);
  TextOut(TheDC, 130, YPos, TextString, strlen(TextString));
  TrackContent(130 + depDescWidth, YPos + mRowHeight);

  char stageBuffer[64];
  if (FormatStageStatusString(frame->DisplayHan, TabVal, stageBuffer))
  {
    ::SetTextColor(TheDC, RGB(192, 192, 192));
    const int stageX = 130 + depDescWidth + 8;
    TextOut(TheDC, stageX, YPos, stageBuffer, strlen(stageBuffer));
    TrackContent(stageX + MeasureStringWidth(TheDC, stageBuffer), YPos + mRowHeight);
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
    CopyBuffer(TextString, "XX:XX");
  }
  else
  { 
    Hr = TimeVal / 100;
  
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




DEFINE_RESPONSE_TABLE1(TDepartur, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
