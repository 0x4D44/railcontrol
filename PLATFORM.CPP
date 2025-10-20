// OWLCVT 05/11/95 22:39:08
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
/* Methods of TPlatform follow...*/

TPlatform::TPlatform(TWindow* parent, int XPos, int YPos)
  : TFrameWindow(parent, "Platforms"),
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
  Attr.H = 259;
  Attr.Style = WS_DLGFRAME;
 }


TPlatform::~TPlatform()
{
}


void TPlatform::ResetContentExtents()
{
  mContentRight = 0;
  mContentBottom = 0;
}


void TPlatform::UpdateMetrics(HDC dc)
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


void TPlatform::TrackContent(int right, int bottom)
{
  if (right > mContentRight)  mContentRight = right;
  if (bottom > mContentBottom)  mContentBottom = bottom;
}


void TPlatform::EnsureContentFits()
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


void TPlatform::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  WndClass.hIcon = ::LoadIcon(GetModule()->GetHandle(), TEXT("PLATFORM"));
}


auto TPlatform::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Platform Window"));
}


void TPlatform::SetupWindow()
{
  HMENU  hSysMenu;
  int    i;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = ::GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);

  // Intialize variables
  for (i=1;i<=14;i++) Timetab[i] = 0;
}


bool TPlatform::CanClose()
{
  HMENU  TheMenu;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check main menu appropriately
  TheMenu = ::GetMenu(frame->HWindow);
  CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_UNCHECKED);
  frame->PlatformHan = NULL;

  // Return value
  return TRUE;
}


void TPlatform::EvSize(owl::uint sizeType, const TSize& size)
{
  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check the iconized status of the window
  if (::IsIconic(HWindow))
  {
   frame->PlatformIcon = TRUE;
  }
  else
  {
   frame->PlatformIcon = FALSE;
  }
}


void TPlatform::EvMove(const TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvMove(pnt);

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  frame->PlatformX = Attr.X;
  frame->PlatformY = Attr.Y;
}


void TPlatform::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Call main painting routine
  UpdateDisplay(dc, TRUE);
}


void TPlatform::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  TReleaseDcGuard dcGuard;
  RECT     TheRect;
  int      a, b, c, i, MaxVal;
  int      lii = 0;
  int      llocoid = 0;
  int      lLocoState = 0;
  char     TrainName[100], LocoName[100], TextString[100], lTempStr[100];
  SIZE     wTxtWid;
  RECT     lRect;
  COLORREF lLocoColour;
  COLORREF lTrainColour;
  RECT     lrect;
  int      lxx;

  if (TheDC == 0)
  {
    dcGuard.Attach(HWindow, GetDC(HWindow));
    TheDC = dcGuard.Get();
  }
  if (!TheDC)
  {
    return;
  }

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  SelectObject(TheDC, frame->mTextFont);
  UpdateMetrics(TheDC);
  ResetContentExtents();

  ::GetClientRect(HWindow, &TheRect);
  mClientWidth = TheRect.right;
  mClientHeight = TheRect.bottom;

  if (Redraw)
  {
    // Draw a bounding box (3D effect)
    if ((TheRect.right > 3) && (TheRect.bottom > 3))
    {
      DrawRaisedPanel(TheDC, TheRect, 23);
    }

    lrect.left   = 4;
    lrect.right  = TheRect.right - 5;
    lrect.top    = 24;
    lrect.bottom = TheRect.bottom - 5;
    FillRectOpaque(TheDC, lrect, RGB(0, 0, 0));
    TrackContent(lrect.right, lrect.bottom);
  }

  // Set up text attributes
  if (Redraw)
  {
    static const HeaderLabel kPlatformHeaders[] =
    {
      {22, "#:"},
      {43, "Description:"}
    };

    HFONT oldFont = (HFONT) SelectObject(TheDC, frame->mHeaderFont);
    TEXTMETRIC headerMetrics = {0};
    if (::GetTextMetrics(TheDC, &headerMetrics))
    {
      const int headerBottom = 5 + headerMetrics.tmHeight;
      for (size_t idx = 0; idx < sizeof(kPlatformHeaders) / sizeof(kPlatformHeaders[0]); ++idx)
      {
        const HeaderLabel& label = kPlatformHeaders[idx];
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
                     kPlatformHeaders,
                     sizeof(kPlatformHeaders) / sizeof(kPlatformHeaders[0]));
    SelectObject(TheDC, frame->mTextFont);
  }

  // Set Text align to the right
  ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
  SetTextAlign(TheDC, TA_RIGHT);

  if (frame->GameInProgress)
  {
    // Now we draw in the platform information
    MaxVal = 0;
    for (i = 1; i < 50; i++)
    {
      const int rowTop = 8 + (i * mRowSpacing);
      const int rowBottom = rowTop + mRowHeight;
      const int rowRight = (mClientWidth > 5) ? (mClientWidth - 5) : 241;
      lLocoColour  = RGB(255, 255, 255);
      lTrainColour = RGB(255, 255, 255);

      if (frame->DisplayHan->PPlatDataInfo[i] != 0)
      {
        MaxVal = i;
        if (Redraw || (frame->DisplayHan->PPlatDataInfo[i]->GetRedraw()))
        {
          frame->DisplayHan->PPlatDataInfo[i]->SetRedraw(FALSE);
          CopyBuffer(LocoName, "");
          CopyBuffer(TrainName, "");

          // Draw main platform name text
          a = frame->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
          if (a != 0)
          {
            SetBkColor(TheDC, RGB(0, 0, 0));
            ::SetTextColor(TheDC, RGB(255, 255, 255));

            frame->DisplayHan->PSelectorInfo[a]->GetText(TextString, 20);
            TextOut(TheDC, 33, rowTop, TextString, strlen(TextString));
            TrackContent(33, rowBottom);
          }
          else
          {
            ::MessageBox(HWindow, "Zero selector ref", APPNAME, MB_OK);
          }

          // CASE 1: Single loco in the platform
          if ((frame->DisplayHan->PPlatDataInfo[i]->GetLoco() > 0) &&
              (frame->DisplayHan->PPlatDataInfo[i]->GetTimetable() == 0))
          {
            TRC_NRM((TB, "Platform case #1 - single loco in platform"));

            // Get loco number
            a = frame->DisplayHan->PPlatDataInfo[i]->GetLoco();
            if (a > 0)
            {
              frame->DisplayHan->PLocosInfo[a]->LocoNumStr(LocoName);
              CopyBuffer(TrainName, "");

              /**************************************************************/
              /* Store the loco state so we can determine the colour to use */
              /* to draw it with.                                           */
              /**************************************************************/
              lLocoState = frame->DisplayHan->PLocosInfo[a]->GetFlag(); 
            }
          }

          // CASE 2 & 3: There is a train in the platform with 1 or 2 locos
          a = frame->DisplayHan->PPlatDataInfo[i]->GetTimetable();
          if (a > 0)
          {
            // Get timetable status
            b = frame->DisplayHan->PTimetableInfo[a]->GetStatus();

            if (b == ST_INPLAT)
            {
              TRC_NRM((TB, "PLT ST_INPLAT"));

              // Train has just arrived and loco is still connected
              frame->DisplayHan->PTimetableInfo[a]->GetArrDesc(TrainName, sizeof(TrainName));
              AppendString(TrainName, " + ");

              for (lii = 0; lii < 2; lii++)
              {
                llocoid = frame->DisplayHan->PTimetableInfo[a]->GetLoco(lii);

                if (llocoid != 0)
                {
                  // Get loco number as a character array
                  frame->DisplayHan->PLocosInfo[llocoid]->LocoNumStr(lTempStr);

                  if (lii == 0)
                  {
                    FormatBuffer(LocoName, "%s", lTempStr);

                    /********************************************************/
                    /* Store the loco state so we can determine the colour  */
                    /* to use to draw it with.                              */
                    /********************************************************/
                    lLocoState = frame->DisplayHan->PLocosInfo[llocoid]->GetFlag();
                  }
                  else
                  {
                    AppendString(LocoName, "_");
                    AppendString(LocoName, lTempStr);
                  }
                }
              }
            }
            else if ((b == ST_RELEASE) ||
                     (b == ST_STOCKOK) ||
                     (b == ST_READYDEP))
            {
              TRC_NRM((TB, "PLT ST_RELEA"));

              // Loco is disconnected from the train
              c = frame->DisplayHan->PTimetableInfo[a]->GetLoco();
              if (c > 0)
              {
                frame->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);
                CopyBuffer(TrainName, LocoName);
                AppendString(TrainName, " + ");
              }
              else
              {
                CopyBuffer(TrainName, "");
              }

              frame->DisplayHan->PTimetableInfo[a]->GetDepDesc(LocoName, sizeof(LocoName));
              AppendString(TrainName, LocoName);

              TRC_NRM((TB, "PLT CHOICE"));

              c = frame->DisplayHan->PPlatDataInfo[i]->GetLoco();
              if (c > 0)
              {
                frame->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);
                AppendString(TrainName, " . ");

                /************************************************************/
                /* Store the loco state so we can determine the colour to   */
                /* use to draw it with.                                     */
                /************************************************************/
                lLocoState = frame->DisplayHan->PLocosInfo[c]->GetFlag();
              }
              else
              {
                CopyBuffer(LocoName, "");
              }

              /**************************************************************/
              /* Figure out if the train is delayed.                        */
              /**************************************************************/
              if (b == ST_RELEASE)
              {
                lTrainColour = RGB(225, 225, 0);
              }
              else if (b == ST_READYDEP)
              {
                lxx = frame->DisplayHan->PTimetableInfo[a]->GetDepTime();
                if (lxx == 0) 
                {
                  /**********************************************************/
                  /* Train is an ECS train.                                 */
                  /**********************************************************/
                  lTrainColour = RGB(0, 128, 255);

                }
                else if (frame->DisplayHan->WorkTime > 2*(lxx - 40*int(lxx/100)))
                {
                  /**********************************************************/
                  /* Train is delayed.                                      */
                  /**********************************************************/
                  lTrainColour = RGB(255, 0, 0);
                }
                else
                {
                  /**********************************************************/
                  /* Train is awaiting departure.                           */
                  /**********************************************************/
                  lTrainColour = RGB(0, 255, 0);
                }
              }
            }
          }

          // Select <LocoName> text colour
          switch (lLocoState)
          {
            case LF_NEEDFUEL: lLocoColour = RGB(128, 128, 128); break;
            case LF_MAINTAIN: lLocoColour = RGB(255, 128, 0); break;
            default:          lLocoColour = RGB(255, 255, 255); break;
          }

          // Now draw information on the screen
          SetBkColor(TheDC, RGB(0, 0, 0));

          lRect.left   = 35;
          lRect.top    = rowTop;
          lRect.right  = rowRight;
          lRect.bottom = rowBottom;

          /******************************************************************/
          /* Draw loco text at the rightmost of the screen (note that right */
          /* text alignment has already been setup).                        */
          /******************************************************************/
          ::SetTextColor(TheDC, lLocoColour);
          ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
          TrackContent(lRect.right, lRect.bottom);
          TextOut(TheDC, 240, rowTop, LocoName, strlen(LocoName));
          TrackContent(240, rowBottom);

          /******************************************************************/
          /* Draw the train text to the immediate left of the loco text.    */
          /******************************************************************/
          ::SetTextColor(TheDC, lTrainColour);
          GetTextExtentPoint32(TheDC, LocoName, strlen(LocoName), &wTxtWid);
          TextOut(TheDC, (240-wTxtWid.cx), rowTop, TrainName, strlen(TrainName));
        }
      }
    }

    // Now update the hold point
    if (TSelector::GetHoldPoint() > 0)
    {
      if (Redraw || frame->DisplayHan->HoldLocoChng)
      {
        SetBkColor(TheDC, RGB(0, 0, 0));
        ::SetTextColor(TheDC, RGB(255, 255, 255));

        frame->DisplayHan->HoldLocoChng = FALSE;
        CopyBuffer(TextString, "Hold");
        const int holdTop = 25 + (MaxVal * mRowSpacing);
        const int holdBottom = holdTop + mRowHeight;
        const int rowRight = (mClientWidth > 5) ? (mClientWidth - 5) : 241;
        TextOut(TheDC, 33, holdTop, TextString, strlen(TextString));
        TrackContent(33, holdBottom);
        if (frame->DisplayHan->HoldLoco > 0)
        {
          c = frame->DisplayHan->HoldLoco;
          if (c > 0)
          {
            frame->DisplayHan->PLocosInfo[c]->LocoNumStr(TextString);

            // Select <LocoName> text colour
            switch (frame->DisplayHan->PLocosInfo[c]->GetFlag())
            {
              case LF_NEEDFUEL: ::SetTextColor(TheDC, RGB(128, 128, 128)); break;
              case LF_MAINTAIN: ::SetTextColor(TheDC, RGB(225, 225, 0)); break;
              default:          ::SetTextColor(TheDC, RGB(255, 255, 255)); break;
            }
          }
          else
          {
            CopyBuffer(TextString, "");
          }
        }
        else
        {
          CopyBuffer(TextString, "");
        }

        lRect.left   = 35;
        lRect.top    = holdTop;
        lRect.right  = rowRight;
        lRect.bottom = holdBottom;
        ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
        TrackContent(lRect.right, lRect.bottom);
        TextOut(TheDC, 240, holdTop, TextString, strlen(TextString));
        TrackContent(240, holdBottom);
        CopyBuffer(Currently[MaxVal+1], TextString);
      }
    }
  }

  EnsureContentFits();

  // Tidy up
  (void)dcGuard;
}


DEFINE_RESPONSE_TABLE1(TPlatform, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
