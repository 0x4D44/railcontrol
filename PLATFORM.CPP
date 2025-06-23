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

/*************************************************************************/
/* Methods of TPlatform follow...*/

TPlatform::TPlatform(TWindow* parent, int XPos, int YPos)
  : TFrameWindow(parent, "Platforms")
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


void TPlatform::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  WndClass.hIcon = LoadIcon(*GetApplication(), "PLATFORM");
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
  TFrameWindow::SetupWindow();

  // Get rid of unnecessary system menu commands
  hSysMenu = ::GetSystemMenu(HWindow, FALSE);
  RemoveMenu(hSysMenu, SC_SIZE, MF_BYCOMMAND);
  RemoveMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);

  // Intialize variables
  for (i=1;i<=14;i++) Timetab[i] = 0;
}


BOOL TPlatform::CanClose()
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


void TPlatform::EvSize(UINT sizeType, TSize& size)
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


void TPlatform::EvMove(TPoint& pnt)
{
  // Call ancestor method
  TFrameWindow::EvMove(pnt);

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Update parent fields
  frame->PlatformX = Attr.X;
  frame->PlatformY = Attr.Y;
}


void TPlatform::Paint(TDC& dc, BOOL, TRect&)
{
  // Call main painting routine
  UpdateDisplay(dc, TRUE);
}


void TPlatform::UpdateDisplay(HDC TheDC, BOOL Redraw)
{
  BOOL     MadeDC;
  RECT     TheRect;
  HPEN     hOldPen, hPen;
  int      a, b, c, i, MaxVal;
  int      lii = 0;
  int      llocoid = 0;
  int      lLocoState = 0;
  char     TrainName[100], LocoName[100], TextString[100], lTempStr[100];
  SIZE     wTxtWid;
  RECT     lRect;
  COLORREF loldBackColour;
  COLORREF lLocoColour;
  COLORREF lTrainColour;
  RECT     lrect;
  int      lxx;

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

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

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

  // Now draw platform header information
  if (Redraw)
  {
    strcpy(TextString, "#:");
    TextOut(TheDC, 22, 5, TextString, strlen(TextString));
    strcpy(TextString, "Description:");
    TextOut(TheDC, 43, 5, TextString, strlen(TextString));
  }

  // Set Text align to the right
  SelectObject(TheDC, frame->mTextFont);
  SetTextAlign(TheDC, TA_RIGHT);

  if (frame->GameInProgress)
  {
    // Now we draw in the platform information
    MaxVal = 0;
    for (i = 1; i < 50; i++)
    {
      lLocoColour  = RGB(255, 255, 255);
      lTrainColour = RGB(255, 255, 255);

      if (frame->DisplayHan->PPlatDataInfo[i] != 0)
      {
        MaxVal = i;
        if (Redraw || (frame->DisplayHan->PPlatDataInfo[i]->GetRedraw()))
        {
          frame->DisplayHan->PPlatDataInfo[i]->SetRedraw(FALSE);
          strcpy(LocoName, "");
          strcpy(TrainName, "");

          // Draw main platform name text
          a = frame->DisplayHan->PPlatDataInfo[i]->GetSelectorRef();
          if (a != 0)
          {
            SetBkColor(TheDC, RGB(0, 0, 0));
            SetTextColor(TheDC, RGB(255, 255, 255));

            frame->DisplayHan->PSelectorInfo[a]->GetText(TextString, 20);
            TextOut(TheDC, 33, 8+17*i, TextString, strlen(TextString));
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
              strcpy(TrainName, "");

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
              frame->DisplayHan->PTimetableInfo[a]->GetArrDesc(TrainName);
              strcat(TrainName, " + ");

              for (lii = 0; lii < 2; lii++)
              {
                llocoid = frame->DisplayHan->PTimetableInfo[a]->GetLoco(lii);

                if (llocoid != 0)
                {
                  // Get loco number as a character array
                  frame->DisplayHan->PLocosInfo[llocoid]->LocoNumStr(lTempStr);

                  if (lii == 0)
                  {
                    sprintf(LocoName, "%s", lTempStr);

                    /********************************************************/
                    /* Store the loco state so we can determine the colour  */
                    /* to use to draw it with.                              */
                    /********************************************************/
                    lLocoState = frame->DisplayHan->PLocosInfo[llocoid]->GetFlag();
                  }
                  else
                  {
                    strcat(LocoName, "_");
                    strcat(LocoName, lTempStr);
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
                strcpy(TrainName, LocoName);
                strcat(TrainName, " + ");
              }
              else
              {
                strcpy(TrainName, "");
              }

              frame->DisplayHan->PTimetableInfo[a]->GetDepDesc(LocoName);
              strcat(TrainName, LocoName);

              TRC_NRM((TB, "PLT CHOICE"));

              c = frame->DisplayHan->PPlatDataInfo[i]->GetLoco();
              if (c > 0)
              {
                frame->DisplayHan->PLocosInfo[c]->LocoNumStr(LocoName);
                strcat(TrainName, " . ");

                /************************************************************/
                /* Store the loco state so we can determine the colour to   */
                /* use to draw it with.                                     */
                /************************************************************/
                lLocoState = frame->DisplayHan->PLocosInfo[c]->GetFlag();
              }
              else
              {
                strcpy(LocoName, "");
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
          lRect.top    = 8+17*i;
          lRect.right  = 241;
          lRect.bottom = 24+17*i;

          /******************************************************************/
          /* Draw loco text at the rightmost of the screen (note that right */
          /* text alignment has already been setup).                        */
          /******************************************************************/
          SetTextColor(TheDC, lLocoColour);
          ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
          TextOut(TheDC, 240, 8+17*i, LocoName, strlen(LocoName));

          /******************************************************************/
          /* Draw the train text to the immediate left of the loco text.    */
          /******************************************************************/
          SetTextColor(TheDC, lTrainColour);
          GetTextExtentPoint32(TheDC, LocoName, strlen(LocoName), &wTxtWid);
          TextOut(TheDC, (240-wTxtWid.cx), 8+17*i, TrainName, strlen(TrainName));
        }
      }
    }

    // Now update the hold point
    if (TSelector::GetHoldPoint() > 0)
    {
      if (Redraw || frame->DisplayHan->HoldLocoChng)
      {
        SetBkColor(TheDC, RGB(0, 0, 0));
        SetTextColor(TheDC, RGB(255, 255, 255));

        frame->DisplayHan->HoldLocoChng = FALSE;
        strcpy(TextString, "Hold");
        TextOut(TheDC, 33, 25+17*MaxVal, TextString, strlen(TextString));
        if (frame->DisplayHan->HoldLoco > 0)
        {
          c = frame->DisplayHan->HoldLoco;
          if (c > 0)
          {
            frame->DisplayHan->PLocosInfo[c]->LocoNumStr(TextString);

            // Select <LocoName> text colour
            switch (frame->DisplayHan->PLocosInfo[c]->GetFlag())
            {
              case LF_NEEDFUEL: SetTextColor(TheDC, RGB(128, 128, 128)); break;
              case LF_MAINTAIN: SetTextColor(TheDC, RGB(225, 225, 0)); break;
              default:          SetTextColor(TheDC, RGB(255, 255, 255)); break;
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

        lRect.left   = 35;
        lRect.top    = 25+17*MaxVal;
        lRect.right  = 241;
        lRect.bottom = 41+17*MaxVal;
        ExtTextOut(TheDC, lRect.left, lRect.top, ETO_OPAQUE, &lRect, "", 0, NULL);
        TextOut(TheDC, 240, 25+17*MaxVal, TextString, strlen(TextString));
        strcpy(Currently[MaxVal+1], TextString);
      }
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


DEFINE_RESPONSE_TABLE1(TPlatform, TFrameWindow)
    EV_WM_SIZE,
    EV_WM_MOVE,
END_RESPONSE_TABLE;
