// OWLCVT 05/11/95 22:38:21
/* LAYOUT.CPP
*  ==========
*
*  PROGRAM DESCRIPTION
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
#include <cstring>
#include <cmath>
#include <vector>

namespace {
inline int RoundToInt(double value)
{
  return static_cast<int>(std::lround(value));
}

#if defined(DIAG_SETUP_TRACE)
bool ShouldWriteDiagnosticsLog()
{
  static int sLogFlag = -1;
  if (sLogFlag == -1)
  {
    sLogFlag = GetPrivateProfileInt("Debug", "EnableDiagnosticsLog", 0, INIFILENAME);
  }
  return sLogFlag != 0;
}

FILE* OpenDiagnosticsLog()
{
  if (!ShouldWriteDiagnosticsLog())
  {
    return 0;
  }

  const char* logPath = GetLogFilePath();
  if ((logPath == 0) || (logPath[0] == '\0'))
  {
    return 0;
  }

  return fopen(logPath, "a");
}

void LogLayoutEvent(const char* phase, const TLayout* layout, const char* detail = nullptr)
{
  if (!phase)
  {
    return;
  }

  char buffer[512];
  HWND hwnd = layout ? layout->GetHandle() : HWND(0);
  const char* notePrefix = (detail && detail[0] != '\0') ? " note=" : "";
  const char* noteText = detail ? detail : "";
  FormatBuffer(buffer,
               "LayoutDiag:%s this=%p hwnd=%p%s%s",
               phase,
               layout,
               hwnd,
               notePrefix,
               noteText);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");

  if (FILE* logFile = OpenDiagnosticsLog())
  {
    fputs(buffer, logFile);
    fputc('\n', logFile);
    fclose(logFile);
  }
}

void LogLayoutCounter(const char* phase,
                      const TLayout* layout,
                      const char* label,
                      int value)
{
  if (!phase || !label)
  {
    return;
  }

  char detail[256];
  FormatBuffer(detail, "%s=%d", label, value);
  LogLayoutEvent(phase, layout, detail);
}
#endif
#if !defined(DIAG_SETUP_TRACE)
static FILE* OpenDiagnosticsLog()
{
  return 0;
}
#endif
}

/*************************************************************************/
/* Methods of TLayout follow...*/


TLayout::TLayout(owl::TWindow * AParent, int XPos, int YPos, int Width, int Height)
  : TFrameWindow(AParent, "")
{
  int  i;

  Attr.X = XPos;
  Attr.Y = YPos;
  Attr.W = Width;
  Attr.H = Height;
  Attr.Style = WS_CHILD | WS_VISIBLE;

  // Load the digital time symbols (ie 1-2-3-4-5-6-7-8-9 etc )
  HTimeSym = ::LoadBitmap(GetModule()->GetHandle(), MAKEINTRESOURCE(HR_DIGITAL));

  // Seed the random number generator
  ::srand(static_cast<unsigned int>(::time(0)));

  // Clear all section etc data
  for (i=0; i<1000; i++)  PSectionInfo[i] = 0;
  for (i=0; i<50; i++)    PSelectorInfo[i] = 0;
  for (i=0; i<50; i++)    POverlapDataInfo[i] = 0;
  for (i=0; i<50; i++)    PPlatDataInfo[i] = 0;
  for (i=0; i<1000; i++)  PRoutesInfo[i] = 0;
  for (i=0; i<500; i++)   PLocosInfo[i] = 0;
  for (i=0; i<MAX_TIMETABLE; i++)   PTimetableInfo[i] = 0;
  for (i=0; i<10; i++)    PTrackLoco[i] = 0;

  // Clear text
  for (i=0; i<3; i++)  CopyString(StartText[i], "");
}


TLayout::~TLayout()
{
#if defined(DIAG_SETUP_TRACE)
  LogLayoutEvent("TLayout:dtor:begin", this);
#endif
  // Clear any loaded data
  ClearData();

  // Delete time symbol bitmap
  DeleteObject(HTimeSym);
#if defined(DIAG_SETUP_TRACE)
  LogLayoutEvent("TLayout:dtor:end", this);
#endif
}


void TLayout::ClearData()
{
  int i;
#if defined(DIAG_SETUP_TRACE)
  int deletedSections = 0;
  int deletedOverlaps = 0;
  int deletedSelectors = 0;
  int deletedPlatData = 0;
  int deletedRoutes = 0;
  int deletedLocos = 0;
  int deletedTimetable = 0;
  int deletedTrackLoco = 0;
  LogLayoutEvent("TLayout:ClearData:begin", this);
#endif

  // Clear any data that may be currently in the structures
  // Clear section information
  for (i=0; i<1000; i++)
  {
    if (PSectionInfo[i] != 0)
    {
      delete((PSection) PSectionInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedSections;
#endif
    }
    PSectionInfo[i] = 0;
  }

  // Clear overlapping information
  for (i=0; i<50; i++)
  {
    if (POverlapDataInfo[i] != 0)
    {
      delete((POverlapData) POverlapDataInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedOverlaps;
#endif
    }
    POverlapDataInfo[i] = 0;
  }

  // Clear selector information
  for (i=0; i<50; i++)
  {
    if (PSelectorInfo[i] != 0)
    {
      delete((PSelector) PSelectorInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedSelectors;
#endif
    }
    PSelectorInfo[i] = 0;
  }

  // Clear platform information
  for (i=0; i<50; i++)
  {
    if (PPlatDataInfo[i] != 0)
    {
      delete((PPlatData) PPlatDataInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedPlatData;
#endif
    }
    PPlatDataInfo[i] = 0;
  }

  // Clear route information
  for (i=0; i<1000; i++)
  {
    if (PRoutesInfo[i] != 0)
    {
      delete((PRoutes) PRoutesInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedRoutes;
#endif
    }
    PRoutesInfo[i] = 0;
  }

  // Clear loco information
  for (i=0; i<500; i++)
  {
    if (PLocosInfo[i] != 0)
    {
      delete((PLocos) PLocosInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedLocos;
#endif
    }
    PLocosInfo[i] = 0;
  }

  // Clear timetable information
  for (i=0; i<MAX_TIMETABLE; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      delete((PTimetable) PTimetableInfo[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedTimetable;
#endif
    }
    PTimetableInfo[i] = 0;
  }

  // Clear track loco information
  for (i=0; i<10; i++)
  {
    if (PTrackLoco[i] != 0)
    {
      delete((PTimetable) PTrackLoco[i]);
#if defined(DIAG_SETUP_TRACE)
      ++deletedTrackLoco;
#endif
    }
    PTrackLoco[i] = 0;
  }
#if defined(DIAG_SETUP_TRACE)
  LogLayoutCounter("TLayout:ClearData:sections", this, "deleted", deletedSections);
  LogLayoutCounter("TLayout:ClearData:overlaps", this, "deleted", deletedOverlaps);
  LogLayoutCounter("TLayout:ClearData:selectors", this, "deleted", deletedSelectors);
  LogLayoutCounter("TLayout:ClearData:platforms", this, "deleted", deletedPlatData);
  LogLayoutCounter("TLayout:ClearData:routes", this, "deleted", deletedRoutes);
  LogLayoutCounter("TLayout:ClearData:locos", this, "deleted", deletedLocos);
  LogLayoutCounter("TLayout:ClearData:timetable", this, "deleted", deletedTimetable);
  LogLayoutCounter("TLayout:ClearData:trackloco", this, "deleted", deletedTrackLoco);
  LogLayoutEvent("TLayout:ClearData:end", this);
#endif
}


void TLayout::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


auto TLayout::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Layout Class"));
}


void TLayout::SetupWindow()
{
  int i;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  for (i=1; i<50; i++)
  {
    if (PSelectorInfo[i] != 0)  PSelectorInfo[i]->Release();
  }

  // Update the selectors
  UpdateSelectors();
}


bool TLayout::CanClose()
{
  return TRUE;
}


void TLayout::QuitProgram()
{
  // This is the intialization file saving routine
  ;
}


void TLayout::EvSize(owl::uint sizeType, const TSize& size)
{
  TRC_NRM((TB, "Enter EvSize"));

  // Call ancestor method
  TFrameWindow::EvSize(sizeType, size);

  // Resize the selectors
  SizeSelectors();

  TRC_NRM((TB, "Exit EvSize"));
}


void TLayout::EvTimer(owl::uint timerID)
{
  int SkipInt;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Handle timer messages
  if (timerID == ID_TIMER)
  {
    // Increment skip count
    SkipCount++;

    switch (frame->TimerSpeed)
    {
      case 1:  SkipInt = 30; break;
      case 2:  SkipInt = 20; break;
      case 3:  SkipInt = 10; break;
      default: SkipInt = 20;
    }

    if (SkipCount >= SkipInt)
    {
      WorkTime++;
      if (WorkTime >= 2880) WorkTime = 0;

      // Check to see if we have finished
      if (WorkTime >= StopTime)
      {
        // Pause...
        ::SendMessage(frame->HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);

        // ...create the dialog and display it...
        {
          TFinish dlg(this, TEXT("FINISH"));
          dlg.Execute();
        }

        // ...and then stop
        ::SendMessage(frame->HWindow, WM_COMMAND, CM_MNUFISTOP, 0);
      }
      else
      {
        // Call the 'main' routine - this does a lot of work
        HandleTimeChange();

        // Reset the skip counter
        SkipCount = 0;
      }
    }
  }
}


void TLayout::Paint(TDC& dc, bool erase, TRect& rect)
{
  // Update the display
  UpdateDisplay(dc);
}


void TLayout::UpdateDisplay(HDC TheDC)
{
  BOOL    MadeDC;
  HPEN    hOldPen, hPen;
  RECT    TheRect;
  int     i;
  HDC     MemDC;                    // Copied DC
  HBITMAP HOldBitmap, HLoco1;       // Handle to a bitmap

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

  // Draw a bounding box (3D effect)
  ::GetClientRect(HWindow, &TheRect);

  if ((TheRect.right > 3) && (TheRect.bottom > 3))
  {
    hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
    hOldPen = (HPEN) SelectObject(TheDC, hPen);
    MoveToEx(TheDC, TheRect.right-2, 1, 0);
    LineTo(TheDC, TheRect.right-2, TheRect.bottom-1);
    LineTo(TheDC, 3, TheRect.bottom-1);
    SelectObject(TheDC, hOldPen);
    DeleteObject(hPen);

    hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
    hOldPen = (HPEN) SelectObject(TheDC, hPen);
    LineTo(TheDC, 3, 1);
    LineTo(TheDC, TheRect.right-2, 1);
    SelectObject(TheDC, hOldPen);
    DeleteObject(hPen);

    if (frame->GameInProgress)
    {
      // We run through the values in the main <PSectionInfo> array and draw them.
      // However we first scale them all so that they will fit on the screen.
      const float sectionXScale = static_cast<float>(TheRect.right) / 850.0f;
      const float sectionYScale = static_cast<float>(TheRect.bottom) / 550.0f;
      TSection::SetXScaleFactor(sectionXScale);
      TSection::SetYScaleFactor(sectionYScale);

      // Draw the sections
      for (i = 0; i < 1000; i++)
      {
        DrawSection(TheDC, i);
      }

      // Draw the platform status indicators
      const float platformXScale = static_cast<float>(TheRect.right) / 850.0f;
      const float platformYScale = static_cast<float>(TheRect.bottom) / 550.0f;
      TPlatData::SetXScaleFactor(platformXScale);
      TPlatData::SetYScaleFactor(platformYScale);
      for (i = 1; i < 50; i++)
      {
        DrawPlatform(TheDC, i);
      }

      DrawClock(TheDC, TRUE);
    }
    else
    {
      // Stretch the <loco> bitmap to fill the screen
      HLoco1 = ::LoadBitmap(GetModule()->GetHandle(), TEXT("LOCO"));
      ::GetClientRect(HWindow, &TheRect);

      MemDC = CreateCompatibleDC(TheDC);
      HOldBitmap = (HBITMAP) SelectObject(MemDC, HLoco1);

      // StretchBlt it
      StretchBlt(TheDC, 5, 5, (TheRect.right-TheRect.left-10), (TheRect.bottom-TheRect.top-10),
                 MemDC, 0, 0, 150, 130, SRCCOPY);
      SelectObject(MemDC, HOldBitmap);

      // Delete compatible DC
      DeleteDC(MemDC);
      DeleteObject(HLoco1);
    }
  }

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::IniVariables()
{
  int i;

  // Initialize all variables
  WorkTime = 0;
  StopTime = 0;
  ShiftLength = 0;
  OldWorkTime = 0;
  TimeNum = 1;
  SkipCount = 0;
  Selector1 = 0;
  Selector2 = 0;
  HoldLoco = 0;
  TrackLocoNum = 0;
  ArrDelay = 0;
  ArrNum = 0;
  DepDelay = 0;
  DepNum = 0;
  mOldAvArrDelay = 100;
  mOldAvDepDelay = 100;

  // Clear all locos in the locoyard
  for (i = 0; i < 16; i++)  LocoyardLoco[i] = 0;

  // Set all expected/depart trains and changed flags
  for (i = 0; i < DISP_NUM_ARRIVAL; i++)
  {
    Expect[i] = 0;
    ExpChng[i] = TRUE;
  }

  for (i = 0; i < DISP_NUM_DEPART; i++)
  {
    Depart[i] = 0;
    DepChng[i] = TRUE;
  }

  // No routes currently set
  for (i=0; i < 4; i++)
  {
    RoutesSet[i] = 0;
  }

  // Clear all occupied platforms
  for (i = 1; i < 50; i++)
  {
    if (PPlatDataInfo[i] != 0)
    {
      PPlatDataInfo[i]->SetTimetable(0, 0);
      PPlatDataInfo[i]->SetTimetable(1, 0);
      PPlatDataInfo[i]->SetLoco(0);
    }
  }

  // Clear all section occupied flags
  for (i=1; i<1000; i++)
  {
    if (PSectionInfo[i] != 0)  PSectionInfo[i]->SetOccupied(FALSE);
  }

  // Clear text
  for (i=0; i<3; i++)  CopyString(StartText[i], "");

  // Debug controls
  mDrawSelectorNumber = FALSE;
  mDrawSectionNumber  = FALSE;
}


BOOL TLayout::StartNew()
{
  int  i, TempArray[9];

  // Start a new game thing
#if defined(DIAG_SETUP_TRACE)
  LogLayoutEvent("TLayout:StartNew:begin", this);
#endif
  TSelector::Reset();
  ClearData();
  IniVariables();
  
#if defined(DIAG_SETUP_TRACE)
  // DIAGNOSTIC: Log startup
  if (FILE* logFile = OpenDiagnosticsLog())
  {
    fprintf(logFile, "=== RAILCONTROL STARTUP ===\n");
    fprintf(logFile, "Initializing PTrackLoco array with 10 elements...\n");
    fclose(logFile);
  }
#endif
  
  for (i=0; i<9; i++)  TempArray[i] = 0;
  for (i=0; i<10; i++)
  {
    PTrackLoco[i] = new TTimetable("", "", TempArray, "");
    PTrackLoco[i]->SetStatus(ST_STOCKOK);
  }

  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);
  bool dataLoaded = ReadDataFile();
  if (!dataLoaded && frame != nullptr && frame->DataFileName[0] == '\0')
  {
    char fallbackPath[MAX_PATH] = {0};
    if (::GetModuleFileNameA(nullptr, fallbackPath, MAX_PATH) != 0)
    {
      char* lastSlash = strrchr(fallbackPath, '\\');
      if (lastSlash != nullptr)
      {
        ++lastSlash;
        *lastSlash = '\0';
      }
      lstrcatA(fallbackPath, "FAST.RCD");
      CopyString(frame->DataFileName, sizeof(frame->DataFileName), fallbackPath);
      dataLoaded = ReadDataFile();
    }
  }

  if (!dataLoaded)
  {
#if defined(DIAG_SETUP_TRACE)
    LogLayoutEvent("TLayout:StartNew:ReadDataFile:fail", this);
#endif
    ::MessageBox(HWindow,
        "The datafile is either incomplete or inconsistent.\n\nEnsure that all the necessary data sections are\nincluded. Refer to the online help for more\ninformation.",
        APPNAME,
        MB_ICONEXCLAMATION | MB_OK);
    return FALSE;
  }

  // Pass control over to the main running routine
#if defined(DIAG_SETUP_TRACE)
  LogLayoutEvent("TLayout:StartNew:end", this);
#endif
  return TRUE;
}


void TLayout::HandleTimeChange()
{
  char       TextString[100];

  // DIAGNOSTIC: Track PTrackLoco array usage every 10 time units
#if defined(DIAG_SETUP_TRACE)
  if ((WorkTime % 10) == 0)
  {
    int UsedCount = 0;
    FILE* logFile = OpenDiagnosticsLog();
    if (logFile != NULL)
    {
      fprintf(logFile, "=== TIME %d: PTrackLoco Array State ===\n", WorkTime);
      
      for (int i = 0; i < 10; i++)
      {
        if (PTrackLoco[i]->GetInitialLocoPlat() != 0)
        {
          UsedCount++;
          fprintf(logFile, "[%d] USED: Loco=%d, Status=%d\n", 
                  i, PTrackLoco[i]->GetLoco(), PTrackLoco[i]->GetStatus());
        }
        else
        {
          fprintf(logFile, "[%d] FREE\n", i);
        }
      }
      
      fprintf(logFile, "Total Usage: %d/10 slots\n", UsedCount);
      
      if (UsedCount >= 8)  // Warning when approaching limit
      {
        fprintf(logFile, "WARNING: Array nearly full!\n");
      }
      else if (UsedCount >= 5)  // Info when light loco slots are full
      {
        fprintf(logFile, "INFO: Light loco slots (0-4) may be full\n");
      }
      
      fprintf(logFile, "======================================\n\n");
      fclose(logFile);
    }
  }
#endif

  // This routine is called every time the time changes and is responsible
  // for ensuring that everything that should be done is done.

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Draw the time
  TRC_NRM((TB, "AT DrawClock"));
  if (frame->GameInProgress)  DrawClock(0, FALSE);

  TRC_NRM((TB, "AT GetExpecteds"));
  if (frame->GameInProgress)  GetExpecteds();

  TRC_NRM((TB, "AT HandleTracking"));
  if (frame->GameInProgress)  HandleTracking();

  TRC_NRM((TB, "AT GetDepartures"));
  if (frame->GameInProgress)  GetDepartures();

  TRC_NRM((TB, "AT UpdateSelectors"));
  if (frame->GameInProgress)  UpdateSelectors();

  // Update various windows
  TRC_NRM((TB, "ATR Arrival"));
  if ((frame->ArrivalHan != NULL) && (frame->GameInProgress))
  {
    frame->ArrivalHan->UpdateDisplay(0, FALSE);
  }

  TRC_NRM((TB, "ATR Departure"));
  if ((frame->DeparturHan != NULL) && (frame->GameInProgress))
  {
    frame->DeparturHan->UpdateDisplay(0, FALSE);
  }

  TRC_NRM((TB, "ATR Platform"));
  if ((frame->PlatformHan != NULL) && (frame->GameInProgress))
  {
    frame->PlatformHan->UpdateDisplay(0, FALSE);
  }

  TRC_NRM((TB, "ATR Locoyard"));
  if ((frame->LocoyardHan != NULL) && (frame->GameInProgress))
  {
    frame->LocoyardHan->UpdateDisplay(0, FALSE);
  }

  TRC_NRM((TB, "ATP Sound"));

  // Load the resource and then play the sound if sound is enabled
  if (frame->SoundEnable && frame->GameInProgress)
  {
    const HINSTANCE module = GetModule()->GetHandle();
    if (!::PlaySound(TEXT("BEEP"), module, SND_ASYNC | SND_RESOURCE | SND_NODEFAULT))
    {
      TRC_ALT((TB, "PlaySound failed for resource 'BEEP' (err:%lu)", ::GetLastError()));
      MessageBeep(MB_ICONEXCLAMATION);
    }
  }

  TRC_NRM((TB, "Cycle complete"));

  CopyString(TextString, "Ready");
  ::SendMessage(frame->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
}


void TLayout::DrawClock(HDC TheDC, BOOL xiRedraw)
{
  BOOL    MadeDC;
  HPEN    hOldPen, hPen;
  HDC     MemDC;
  RECT    lClientRect;
  RECT    lTmpRect;
  float   lDelay;
  int     Val1, Val2, Val3, Val4, Val5;
  int     OldVal1, OldVal2, OldVal3, OldVal4, OldVal5;
  int     XOrigin, YOrigin, Width, Height;
  HFONT   HTheFont, HOldFont;


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

  // Draw the clock panel and fill it with the appropriate time
  ::GetClientRect(HWindow, &lClientRect);

  // Create DC and get ready for some bitblt'ing...
  MemDC = CreateCompatibleDC(TheDC);
  SelectObject(MemDC, HTimeSym);

  if (frame->GameInProgress)
  {
    if ((lClientRect.right > 3) && (lClientRect.bottom > 3))
    {
      // Now we draw the clock time display - firstly draw its bounding box
      XOrigin = RoundToInt(0.2 * lClientRect.right);
      YOrigin = RoundToInt(0.06 * lClientRect.bottom);
      Width = RoundToInt(0.15 * lClientRect.right);
      Height = RoundToInt(0.125 * lClientRect.bottom);

      // Only draw the background/box if redraw is TRUE
      if (xiRedraw)
      {
        hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
        hOldPen = (HPEN) SelectObject(TheDC, hPen);
        MoveToEx(TheDC, XOrigin, YOrigin+Height, 0);
        LineTo(TheDC, XOrigin, YOrigin);
        LineTo(TheDC, XOrigin+Width, YOrigin);
        SelectObject(TheDC, hOldPen);
        DeleteObject(hPen);

        hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
        hOldPen = (HPEN) SelectObject(TheDC, hPen);
        LineTo(TheDC, XOrigin+Width, YOrigin+Height);
        LineTo(TheDC, XOrigin, YOrigin+Height);
        SelectObject(TheDC, hOldPen);
        DeleteObject(hPen);

        // Fill in the background
        SelectObject(TheDC, GetStockObject(BLACK_BRUSH));
        SelectObject(TheDC, GetStockObject(BLACK_PEN));
        Rectangle(TheDC, XOrigin, YOrigin, XOrigin+Width, YOrigin+Height);
      }

      // Now we get the time from the system...
      Val1 = WorkTime/1200;
      Val2 = WorkTime/120 - 10*Val1;

      Val3 = (WorkTime - 120*int(WorkTime/120))/20;
      Val4 = ((WorkTime - 120*int(WorkTime/120)) - 20*Val3)/2;
      Val5 = WorkTime - 2*(WorkTime/2);

      // ...and do the same for the old time values
      OldVal1 = OldWorkTime/1200;
      OldVal2 = OldWorkTime/120 - 10*OldVal1;

      OldVal3 = (OldWorkTime - 120*int(OldWorkTime/120))/20;
      OldVal4 = ((OldWorkTime - 120*int(OldWorkTime/120)) - 20*OldVal3)/2;
      OldVal5 = OldWorkTime -2*(OldWorkTime/2);

      // Update old time values
      OldWorkTime = WorkTime;

      // Val1 and Val2
      if (xiRedraw || (Val1 != OldVal1))
      {
        const int digitWidth = RoundToInt(static_cast<double>(Width) / 4.5);
        if (Val1 != 0)
        {
          StretchBlt(TheDC, XOrigin, YOrigin, digitWidth, Height-1,
                     MemDC, 0, 252-Val1*23, 13, 23, SRCCOPY);
        }
        else
        {
          StretchBlt(TheDC, XOrigin, YOrigin, digitWidth, Height-1,
                     MemDC, 0, 23, 13, 23, SRCCOPY);
        }
      }
      if (xiRedraw || (Val2 != OldVal2))
      {
        const int digitWidth = RoundToInt(static_cast<double>(Width) / 4.5);
        StretchBlt(TheDC, XOrigin + RoundToInt(2.0 * Width / 9.0), YOrigin, digitWidth, Height-1,
                   MemDC, 0, 252-Val2*23, 13, 23, SRCCOPY);
      }

      // Now stick the double dot seperator on the screen
      if (xiRedraw || (OldVal5 != Val5))
      {
        const int separatorWidth = RoundToInt(static_cast<double>(Width) / 9.0);
        StretchBlt(TheDC, XOrigin + RoundToInt(4.0 * Width / 9.0), YOrigin, separatorWidth, Height-1,
                   MemDC, 3, 275+23*Val5, 6, 23, SRCCOPY);
      }

      // Val3 and Val4
      if (xiRedraw || (OldVal3 != Val3))
      {
        const int digitWidth = RoundToInt(static_cast<double>(Width) / 4.5);
        StretchBlt(TheDC, XOrigin + RoundToInt(5.0 * Width / 9.0), YOrigin, digitWidth, Height-1,
                   MemDC, 0, 252-Val3*23, 13, 23, SRCCOPY);
      }
      if (xiRedraw || (OldVal4 != Val4))
      {
        const int digitWidth = RoundToInt(static_cast<double>(Width) / 4.5);
        StretchBlt(TheDC, XOrigin + RoundToInt(7.0 * Width / 9.0), YOrigin, digitWidth, Height-1,
                   MemDC, 0, 252-Val4*23, 13, 23, SRCCOPY);
      }

                           
      if (xiRedraw)
      {
        // Redraw the font items
        HTheFont = CreateFont(RoundToInt(0.06 * lClientRect.bottom), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
              CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, (LPSTR)"Arial");

        HOldFont = (HFONT) SelectObject(TheDC, HTheFont);
        ::SetTextColor(TheDC, GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor(TheDC, GetSysColor(COLOR_BTNFACE));
        SetTextAlign(TheDC, TA_CENTER);

        TextOut(TheDC, RoundToInt(0.1375 * lClientRect.right), RoundToInt(0.8 * lClientRect.bottom), "Arrival delay:", 14);
        TextOut(TheDC, RoundToInt(0.3375 * lClientRect.right), RoundToInt(0.8 * lClientRect.bottom), "Departure delay:", 16);
        SelectObject(TheDC, HOldFont);

        DeleteObject(HTheFont);
      }

      // Delete memory DC
      DeleteDC(MemDC);

      // Tidy up
      if (MadeDC) ReleaseDC(HWindow, TheDC);

      /**********************************************************************/
      /* Now draw in the average arrival delay box.                         */
      /**********************************************************************/
      const int delayBoxWidth = RoundToInt(0.11 * lClientRect.right);
      const int delayBoxHeight = RoundToInt(0.094 * lClientRect.bottom);
      lTmpRect.left   = RoundToInt(0.08 * lClientRect.right);
      lTmpRect.right  = lTmpRect.left + delayBoxWidth;
      lTmpRect.top    = RoundToInt(0.86 * lClientRect.bottom);
      lTmpRect.bottom = lTmpRect.top + delayBoxHeight;

      if (ArrNum == 0)
      {
        lDelay = 0;
      }
      else
      {
        lDelay = static_cast<float>(ArrDelay) / static_cast<float>(2 * ArrNum);
        TRC_NRM((TB, "ArrDelay:%d ArrNum:%d => lDelay:%.2f", ArrDelay, ArrNum, lDelay));
      }

      if ((mOldAvArrDelay != lDelay) || (xiRedraw))
      {
        mOldAvArrDelay = lDelay;
        DrawDelayBox(lTmpRect, lDelay);
      }

      /**********************************************************************/
      /* Now draw in the average departure delay box.                       */
      /**********************************************************************/
      lTmpRect.left   = RoundToInt(0.28 * lClientRect.right);
      lTmpRect.right  = lTmpRect.left + delayBoxWidth;
      lTmpRect.top    = RoundToInt(0.86 * lClientRect.bottom);
      lTmpRect.bottom = lTmpRect.top + delayBoxHeight;

      if (DepNum == 0)
      {
        lDelay = 0;
      }
      else
      {
        lDelay = static_cast<float>(DepDelay) / static_cast<float>(2 * DepNum);
        TRC_NRM((TB, "DepDelay:%d DepNum:%d => lDelay:%.2f", DepDelay, DepNum, lDelay));
      }

      if ((mOldAvDepDelay != lDelay) || (xiRedraw))
      {
        mOldAvDepDelay = lDelay;
        DrawDelayBox(lTmpRect, lDelay);
      }
    }
  }
}

/****************************************************************************/
/* FUNCTION: DrawDelayBox                                                   */
/****************************************************************************/
void TLayout::DrawDelayBox(RECT xiRect, float xiDelay)
{
  HDC     lDC                     = NULL;
  HDC     lMemDC                  = NULL;
  HPEN    lOldPen                 = NULL;
  HPEN    lPen                    = NULL;
  int     lBmpOffset              = 0;
  int     lVal[4]                 = {0};
  int     lCharWidth              = 0;
  int     lCharHeight             = 0;
  int     lii                     = 0;

  /**************************************************************************/
  /* Create a new DC.                                                       */
  /**************************************************************************/
  lDC = GetDC(HWindow);

  /**************************************************************************/
  /* Create DC and get ready for some bitblt'ing...                         */
  /**************************************************************************/
  lMemDC = CreateCompatibleDC(lDC);
  SelectObject(lMemDC, HTimeSym);

  lPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
  lOldPen = (HPEN) SelectObject(lDC, lPen);
  MoveToEx(lDC, xiRect.left, xiRect.bottom, 0);
  LineTo(lDC, xiRect.left, xiRect.top);
  LineTo(lDC, xiRect.right, xiRect.top);
  SelectObject(lDC, lOldPen);
  DeleteObject(lPen);

  lPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
  lOldPen = (HPEN) SelectObject(lDC, lPen);
  LineTo(lDC, xiRect.right, xiRect.bottom);
  LineTo(lDC, xiRect.left, xiRect.bottom);
  SelectObject(lDC, lOldPen);
  DeleteObject(lPen);

  // Fill in the background
  SelectObject(lDC, GetStockObject(BLACK_BRUSH));
  SelectObject(lDC, GetStockObject(BLACK_PEN));
  Rectangle(lDC, xiRect.left, xiRect.top, xiRect.right, xiRect.bottom);

  /**************************************************************************/
  /* Calculate individual digit values.  Add 0.05 to the total for          */
  /* rounding.                                                              */
  /**************************************************************************/
  xiDelay += 0.05f;
  const int roundedDelay = static_cast<int>(xiDelay);
  lVal[0] = roundedDelay / 100;
  lVal[1] = (roundedDelay % 100) / 10;
  lVal[2] = roundedDelay % 10;
  lVal[3] = (static_cast<int>(std::lround(10.0 * xiDelay))) % 10;

  TRC_NRM((TB, "xiDelay:%f lVal:%d.%d.%d.%d", xiDelay, lVal[0], lVal[1], lVal[2], lVal[3]));

  /************************************************************************/
  /* Calculate bitmap X offset - ie whether we use green or red 'LED's.   */
  /************************************************************************/
  if (xiDelay < 5)
  {
    /**********************************************************************/
    /* Use green 'LED's                                                   */
    /**********************************************************************/
    lBmpOffset = 13;
  }
  else
  {
    /**********************************************************************/
    /* Use red 'LED's.                                                    */
    /**********************************************************************/
    lBmpOffset = 0;
  }

  const double availableWidth = static_cast<double>(xiRect.right - xiRect.left);
  lCharWidth  = RoundToInt(availableWidth / 4.25);
  lCharHeight = (xiRect.bottom - xiRect.top) - 2;

  for (lii = 0; lii < 3; lii++)
  {
    /************************************************************************/
    /* Don't draw leading zeros - rather use the "not illuminated"          */
    /* character.                                                           */
    /************************************************************************/
    if ((lii < 2) && (xiDelay < 100) && (lVal[lii] == 0))
    {
      StretchBlt(lDC, 
                 xiRect.left + lCharWidth * lii, xiRect.top + 1, 
                 lCharWidth, lCharHeight,
                 lMemDC, 
                 lBmpOffset, 23, 
                 13, 23, 
                 SRCCOPY);
    }
    else 
    {
      StretchBlt(lDC, 
                 xiRect.left + lCharWidth * lii, xiRect.top + 1, 
                 lCharWidth, lCharHeight,
                 lMemDC, 
                 lBmpOffset, 253 - (lVal[lii]*23),
                 13, 23, 
                 SRCCOPY);
    }
  }

  /**************************************************************************/
  /* Draw the decimal point.                                                */
  /**************************************************************************/
  StretchBlt(lDC,
             xiRect.left + lCharWidth * 3, xiRect.top + 1,
             lCharWidth, lCharHeight,
             lMemDC,
             lBmpOffset, 322,
             13, 23,
             SRCCOPY);

  /**************************************************************************/
  /* Draw the final digit.                                                  */
  /**************************************************************************/
  StretchBlt(lDC,
             xiRect.left + RoundToInt(static_cast<double>(lCharWidth) * 3.25), xiRect.top + 1,
             lCharWidth, lCharHeight,
             lMemDC,
             lBmpOffset, 253 - (lVal[3]*23),
             13, 23,
             SRCCOPY);


  /**************************************************************************/
  /* Delete memory DC.                                                      */
  /**************************************************************************/
  DeleteDC(lMemDC);

  /**************************************************************************/
  /* Free the DC.                                                           */
  /**************************************************************************/
  ReleaseDC(HWindow, lDC);
}


void TLayout::DrawSection(HDC xiDC, int xiSectNum)
{
  // This routine is responsible for drawing a section
  // on the screen.

  BOOL   lMadeDC;          // Did we make a DC?
  POINT  lSetPoint[4];     // Temporary point array
  POINT  lTmpPoint[4];     // Temporary point array
  HBRUSH lOldBrush;        // Old DC brush handle
  HFONT  lFont, lOldFont;
  SIZE   lTextSize;
  char   lSectionText[10];

  // Check to see that the pointer is valid
  if (PSectionInfo[xiSectNum] == 0)  return;

  // Do we need to create a new DC?
  if (xiDC == 0)
  {
    // ...yes we do
    xiDC = GetDC(HWindow);
    lMadeDC = TRUE;
  }
  else
  {
    // ...no we don't
    lMadeDC = FALSE;
  }

  // Get handle to main frame window
  TMainWindow* lFrame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Select pen for drawing outlines
  SelectObject(xiDC, GetStockObject(BLACK_PEN));

  // Decide which brush to use
  if (PSectionInfo[xiSectNum]->IsOccupied())
  {
    lOldBrush = (HBRUSH) SelectObject(xiDC, lFrame->RedBrush);
  }
  else
  {
    lOldBrush = (HBRUSH) SelectObject(xiDC, lFrame->DkGrayBrush);
  }

  // Get data and draw it
  PSectionInfo[xiSectNum]->GetSection(lSetPoint);
  Polygon(xiDC, lSetPoint, sizeof(lSetPoint) / sizeof(POINT));

  if (mDrawSectionNumber)
  {
    /************************************************************************/
    /* Draw the section number.  First step is to create a new font.        */
    /************************************************************************/
    TSection* lLabelSection = PSectionInfo[1];
    if (lLabelSection == 0)
    {
      TRC_ALT((TB, "DrawSection missing base geometry; using section %d as fallback", xiSectNum));
      lLabelSection = PSectionInfo[xiSectNum];
    }
    lLabelSection->GetSection(lTmpPoint);
    lFont = CreateFont((2 * (lTmpPoint[3].y - lTmpPoint[0].y)),
                       0, 0, 0, 
                       FW_NORMAL, 
                       0, 0, 0, 
                       ANSI_CHARSET, 
                       OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, 
                       DEFAULT_QUALITY, 
                       FF_DONTCARE, 
                       (LPSTR)"Arial");
                          
    lOldFont = (HFONT) SelectObject(xiDC, lFont);

    /************************************************************************/
    /* Format the section number.                                           */
    /************************************************************************/
    FormatBuffer(lSectionText, "%d", xiSectNum);
    GetTextExtentPoint(xiDC, 
                       lSectionText, 
                       strlen(lSectionText), 
                       &lTextSize);

    TextOut(xiDC, 
            ((lSetPoint[0].x + lSetPoint[2].x - lTextSize.cx) / 2),
            ((lSetPoint[0].y + lSetPoint[3].y - lTextSize.cy) / 2),
            lSectionText,
            strlen(lSectionText));

    /************************************************************************/
    /* Re-select the old font and delete the one we used to draw with.      */
    /************************************************************************/
    SelectObject(xiDC, lOldFont);
    DeleteObject(lFont);
    lFont = NULL;
  }

  // Reselect the old brush
  SelectObject(xiDC, lOldBrush);

  // Tidy up
  if (lMadeDC) ReleaseDC(HWindow, xiDC);
}


void TLayout::DrawPlatform(HDC TheDC, int PlatNum)
{
  // This routine is responsible for drawing a platform status indicator
  // on the screen.

  int    a;
  int    lState;
  BOOL   MadeDC;          // Did we make a DC?
  POINT  MSetPoint[4];    // Temporary point array
  HBRUSH OldBrush;        // Old DC brush handle

  // Check to see that the pointer is valid
  if (PPlatDataInfo[PlatNum] == 0)  return;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Do we need to create a new DC?
  if (TheDC == 0)
  {
    // ...yes we do
    TheDC = GetDC(HWindow);
    MadeDC = TRUE;
  }
  else
  {
    // ...no we don't
    MadeDC = FALSE;
  }

  // Select pen for drawing outlines
  SelectObject(TheDC, GetStockObject(BLACK_PEN));

  // Decide which brush to use
  if (frame->GameInProgress)
  {
    if (PPlatDataInfo[PlatNum]->GetTimetable() > 0)
    {
      lState = PTimetableInfo[PPlatDataInfo[PlatNum]->GetTimetable()]->GetStatus(); 
      switch (lState)
      {
        case ST_INPLAT:   OldBrush = (HBRUSH) SelectObject(TheDC, frame->WhiteBrush); break;
        case ST_RELEASE:  OldBrush = (HBRUSH) SelectObject(TheDC, frame->YellowBrush); break;
        case ST_STOCKOK:  OldBrush = (HBRUSH) SelectObject(TheDC, frame->WhiteBrush); break;
        case ST_READYDEP: OldBrush = (HBRUSH) SelectObject(TheDC, frame->GreenBrush); break;
        default: OldBrush = (HBRUSH) SelectObject(TheDC, frame->DkGrayBrush); break;
      }

      /**********************************************************************/
      /* Check for delayed departure.                                       */
      /**********************************************************************/
      if (lState == ST_READYDEP)
      {
        a = PTimetableInfo[PPlatDataInfo[PlatNum]->GetTimetable()]->GetDepTime();
        if (a == 0)
        {
          /******************************************************************/
          /* Train is an ECS train.                                         */
          /******************************************************************/
          SelectObject(TheDC, frame->BlueBrush);
        }
        else if (WorkTime > 2*(a - 40*int(a/100)))
        {
          /******************************************************************/
          /* Train is delayed.                                              */
          /******************************************************************/
          SelectObject(TheDC, frame->RedBrush);
        }
      }
    }
    else
    {
      if (PPlatDataInfo[PlatNum]->GetLoco() > 0)
      {
        OldBrush = (HBRUSH) SelectObject(TheDC, frame->WhiteBrush);
      }
      else
      {
        OldBrush = (HBRUSH) SelectObject(TheDC, frame->DkGrayBrush);
      }
    }
  }
  else
  {
    OldBrush = (HBRUSH) SelectObject(TheDC, frame->DkGrayBrush);
  }

  // Get platform indicator position data and draw it
  PPlatDataInfo[PlatNum]->GetSection(MSetPoint);
  Polygon(TheDC, MSetPoint, sizeof(MSetPoint) / sizeof(POINT));

  // Reselect the old brush
  SelectObject(TheDC, OldBrush);

  // Tidy up
  if (MadeDC) ReleaseDC(HWindow, TheDC);
}


void TLayout::SizeSelectors()
{
  int   i;
  RECT  WinRect;

  // Get parent window client coordinates
  ::GetClientRect(HWindow, &WinRect);

  // Calculate scaling factors
  const double scaleX = static_cast<double>(WinRect.right) / 850.0;
  const double scaleY = static_cast<double>(WinRect.bottom) / 550.0;

  for (i=0; i<50; i++)
  {
    // Move each selector window
    if (PSelectorInfo[i] != 0)
    {
      const int newX = RoundToInt(scaleX * PSelectorInfo[i]->GetDXPos());
      const int newY = RoundToInt(scaleY * PSelectorInfo[i]->GetDYPos());
      const int newWidth = RoundToInt(scaleX * PSelectorInfo[i]->GetDWidth());
      const int newHeight = RoundToInt(scaleY * PSelectorInfo[i]->GetDHeight());
      ::MoveWindow(PSelectorInfo[i]->HWindow,
                   newX,
                   newY,
                   newWidth,
                   newHeight,
                   TRUE);
    }
  }
}


void TLayout::HandleSelectors()
{
  // This function is called every time a selector changes its position from
  // either in to out or vice-versa. When a selector changes position there
  // are two possible states: either the selector is the first to be
  // selected, or it is the second. In either case this procedure disables
  // any conflicting selectors helping to prevent invalid routes being
  // selected.

  // There are now three possibilities - 0, 1 or 2 selectors have been
  // selected.  I should point out at this stage that I fully appreciate
  // the error of my ways when I use the phrase "selectors selected" - the
  // grammatical nastiness of this is realised but for reasons of clarity
  // (and lethargy) I have not changed this. Sorry.
  //
  // When this routine is called there are three possible actions:
  //
  //   1: A single selector has been selected
  //   2: A second selector has been selected
  //   3: A single selector which was selected has been deselected
  //
  // In the first case (1) the field <Selector1> is updated and no further
  // action taken. In the second case (2) a possible route has been
  // selected: this route is then checked and updated. In the third case
  // the selector value stored in <Selector1> is cleared.

  int    a, b, i;
  char   TextString[50];

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Check to see if we are in a case 1 scenario.
  if (Selector1 == 0)
  {
    // Yes, this is a case 1 scenario. We now search the selectors to find
    // which one has been selected.
    for (i = 1; i < 50; i++)
    {
      if (PSelectorInfo[i] != 0)
      {
        if (!PSelectorInfo[i]->IsSelected())  Selector1 = i;
      }
    }
  }
  else
  {
   // Now check to see if this is a case 2 scenario.
   a = 0;
   b = 0;
   for (i = 1; i < 50; i++)
   {
     if (PSelectorInfo[i] != 0)
     {
       if (!PSelectorInfo[i]->IsSelected())
       {
         if ((a != 0) && (b == 0)) b = i;
         if (a == 0) a = i;
       }
     }
   }

   if ((a != 0) && (b != 0))
   {
    // This is a case 2 scenario. Ensure that the first pressed
    // selector is in Selector1
    if (a == Selector1)
    {
      Selector2 = b;
    }
    else
    {
      Selector2 = a;
    }

    // Check to see if the selected route is valid
    if (!IsRouteValid(Selector1, Selector2))
    {
      // Update the status bar
      CopyString(TextString, "Invalid route");
      ::SendMessage(frame->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));
    }
    PSelectorInfo[Selector1]->Release();
    PSelectorInfo[Selector2]->Release();
    Selector1 = 0;
    Selector2 = 0;
   }

   if ((a == 0) && (b == 0))
   {
     // This is a case 3 scenario.
     Selector1 = 0;
     Selector2 = 0;
   }
  }

  // Now update all the selectors
  UpdateSelectors();
}


void TLayout::UpdateSelectors()
{
  // This routine examines all the selectors and decides which ones can
  // be enabled and which cannot
  int   i, j, ArrivalStatus;
  BOOL  SelectEnable[50], Possible;
  int   lidx = 0;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (frame->GameInProgress)
  {
    // All selectors are initially enabled
    for (i=1; i<50; i++)  SelectEnable[i] = TRUE;

    // PART 1: Checks to be carried out when an input selector is selected
    if (Selector1 != 0)
    {
      if (PSelectorInfo[Selector1]->GetType() == SEL_INPUT)
      {
        // Determine the status flag of the arriving train/loco
        TRC_NRM((TB, "Determine arriving status flag"));
        for (i = 0; i < 4; i++)
        {
          if (Expect[i] > 0)
          {
            if (PTimetableInfo[Expect[i]]->GetArrPnt() == Selector1)
            {
              // We have found what we are looking for
              ArrivalStatus = PTimetableInfo[Expect[i]]->GetArrCode();
              lidx = i;

              break;
            }
          }
        }

        if (PTimetableInfo[Expect[lidx]]->GetDelayFlag() == DLF_DELAYS_THRU)
        {
          // If this is a through train then only enable output selectors.
          for (i = 1; i < 50; i++)
          {
            if (PSelectorInfo[i] != 0)
            {
              if ((PSelectorInfo[i]->GetType() != SEL_OUTPUT) &&
                  (i != Selector1))
              {
                SelectEnable[i] = FALSE;
              }
            }
          }
        }
        else
        {
          // If the train arriving is not a light engine then disable the LY
          if ((ArrivalStatus != SC_LIGHTECS) &&
              (ArrivalStatus != SC_LIGHTNORM) &&
              (ArrivalStatus != SC_LIGHTDELT) &&
              (ArrivalStatus != SC_LIGHTRLF) &&
              (ArrivalStatus != SC_LIGHT37))

          {
            SelectEnable[TSelector::GetLocoyard()] = FALSE;
          }

          // If the arriving train is a EMU then disable all platforms
          // except those of type 4.
          if (ArrivalStatus == SC_EMU)
          {
            for (i = 1; i < 50; i++)
            {
              if (PSelectorInfo[i] != 0)
              {
                if ((PSelectorInfo[i]->GetType() != SEL_ELECTRIC_PLAT) &&
                    (PSelectorInfo[i]->GetType() != SEL_INPUT))
                {
                  SelectEnable[i] = FALSE;
                }
              }
            }
          }

          // If a input selector is already selected then disable any
          // occupied platforms
          for (i = 1; i < 50; i++)
          {
            // Is there a train in the platform?
            if (PPlatDataInfo[i] != 0)
            {
              if (PPlatDataInfo[i]->GetTimetable() > 0)
              {
                Possible = FALSE;
                switch (PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus())
                {
                  case ST_INPLAT:   SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
                  case ST_RELEASE:  Possible = TRUE; break;
                  case ST_STOCKOK:  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
                  case ST_READYDEP: SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
                }

                if (Possible)
                {
                  // Only enable the selector if the arriving loco is a light loco
                  if ((ArrivalStatus != SC_LIGHTECS) &&
                      (ArrivalStatus != SC_LIGHTNORM) &&
                      (ArrivalStatus != SC_LIGHTDELT) &&
                      (ArrivalStatus != SC_LIGHTRLF) &&
                      (ArrivalStatus != SC_LIGHT37))
                  {
                    SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
                  }
                }
              }

              // Is there just a locomotive in the platform?
              if ((PPlatDataInfo[i]->GetTimetable() == 0) && (PPlatDataInfo[i]->GetLoco() != 0))
              {
                SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
              }
            }

            // Disable any other input lines
            if (PSelectorInfo[i] != 0)
            {
              if ((PSelectorInfo[i]->GetType() == SEL_INPUT) && (i != Selector1))
              {
                SelectEnable[i] = FALSE;
              }
            }
          }

          // Disable the hold point
          SelectEnable[TSelector::GetHoldPoint()] = FALSE;
        }
      }


      // If a plaform selector has been selected then disable all other platforms
      if ((PSelectorInfo[Selector1]->GetType() == SEL_PLAT) || 
          (PSelectorInfo[Selector1]->GetType() == SEL_ELECTRIC_PLAT))
      {
        for (i = 1; i < 50; i++)
        {
          if (PSelectorInfo[i] != 0)
          {
            if ((i != Selector1) &&
                ((PSelectorInfo[i]->GetType() == SEL_INPUT) ||
                 (PSelectorInfo[i]->GetType() == SEL_PLAT) ||
                 (PSelectorInfo[i]->GetType() == SEL_ELECTRIC_PLAT)))
            {
              SelectEnable[i] = FALSE;
            }
          }
        }

        /********************************************************************/
        /* We may also wish to disable the hold point if the train is not a */
        /* light locomotive.                                                */
        /********************************************************************/
        if ((PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetTimetable() == 0) &&
            (PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetLoco() != 0))
        {
          /******************************************************************/
          /* This is a light loco.                                          */
          /******************************************************************/
          if (PLocosInfo[PPlatDataInfo[PSelectorInfo[Selector1]->GetPlatRef()]->GetLoco()]->GetFlag() == LF_MAINTAIN)
          {
            /****************************************************************/
            /* The loco needs maintenance - disable the hold point.         */
            /****************************************************************/
            SelectEnable[TSelector::GetHoldPoint()] = FALSE;
          }
          else if (frame->LocoyardEnabled)
          {
            /****************************************************************/
            /* The locoyard is enabled - so disable all the output lines.   */
            /****************************************************************/
            for (i = 1; i < 50; i++)
            {
              if (PSelectorInfo[i] != 0)
              {
                if (PSelectorInfo[i]->GetType() == SEL_OUTPUT)  SelectEnable[i] = FALSE;
              }
            }
          }
        }
        else
        {
          /******************************************************************/
          /* This is not a light loco - disable the hold point.             */
          /******************************************************************/
          SelectEnable[TSelector::GetHoldPoint()] = FALSE;
        }
      }
      else if (PTimetableInfo[Expect[lidx]]->GetDelayFlag() != DLF_DELAYS_THRU)
      {
        // Disable any output lines
        for (i = 1; i < 50; i++)
        {
          if (PSelectorInfo[i] != 0)
          {
            if (PSelectorInfo[i]->GetType() == SEL_OUTPUT)  SelectEnable[i] = FALSE;
          }
        }
      }

      // If the selector selected is of type Locoyard then disable all
      // selectors apart from the hold point
      if (PSelectorInfo[Selector1]->GetType() == SEL_LOCOYARD)
      {
        for (i=1; i<50; i++)
        {
          if (PSelectorInfo[i] != 0)
          {
            if (i != Selector1)
            {
              // Disable input/output selectors
              if ((PSelectorInfo[i]->GetType() == SEL_INPUT) || (PSelectorInfo[i]->GetType() == SEL_OUTPUT))
              {
                SelectEnable[i] = FALSE;
              }

              // Disable any platform which has a train in it which is not in
              // the state of requiring a loco
              if ((PSelectorInfo[i]->GetType() == SEL_PLAT) ||
                  (PSelectorInfo[i]->GetType() == SEL_ELECTRIC_PLAT))
              {
                if ((PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetLoco() != 0) &&
                    (PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetTimetable() == 0))
                {
                  SelectEnable[i] = FALSE;
                }

                j = PPlatDataInfo[PSelectorInfo[i]->GetPlatRef()]->GetTimetable();
                if ((j > 0) && (j < MAX_TIMETABLE))
                {
                  if (PTimetableInfo[j] != 0)
                  {
                    if (PTimetableInfo[j]->GetStatus() != ST_RELEASE)
                    SelectEnable[i] = FALSE;
                  }
                }
              }
            }
          }
        }
      }
    }


    // PART 2: No selector has been selected
    if (Selector1 == 0)
    {
      // Disable the hold point if the hold point is empty
      if (HoldLoco == 0)
      {
        SelectEnable[TSelector::GetHoldPoint()] = FALSE;
      }

      for (i=1; i<50; i++)
      {
        if (PSelectorInfo[i] != 0)
        {
          // If no selector has been selected then disable the input selectors
          // ie (UF and US) if no train is due.
          if (PSelectorInfo[i]->GetType() == SEL_INPUT)  SelectEnable[i] = FALSE;

          // Disable any output lines
          if (PSelectorInfo[i]->GetType() == SEL_OUTPUT)  SelectEnable[i] = FALSE;
        }

        if (PPlatDataInfo[i] != 0)
        {
          // If no selector has been selected yet then disable all empty platforms
          if ((PPlatDataInfo[i]->GetTimetable() == 0) &&
              (PPlatDataInfo[i]->GetLoco() == 0) &&
              (PPlatDataInfo[i]->GetSelectorRef() != 0))
          {
            SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
          }

          // If no selector has yet been selected then only disable any platforms
          // where the train is in the ST_STATUSOK, ST_RELEASE, ST_INPLAT
          if (PPlatDataInfo[i]->GetTimetable() > 0)
          {
            j = PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus();
            if ((j == ST_STOCKOK) ||
                (j == ST_RELEASE) ||
                (j == ST_INPLAT))
            {
              SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
            }
          }
        }
      }


      for (i=0; i<4; i++)
      {
        if (Expect[i] > 0)
        {
          // Check the UF
          if ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
              (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD) ||
              (PTimetableInfo[Expect[i]]->GetStatus() == ST_FIRSTHELD))
          {
            SelectEnable[PTimetableInfo[Expect[i]]->GetArrPnt()] = TRUE;
          }
        }
      }
    }

    // If the H line has been selected then disable the UF, US, DF and DS
    if (Selector1 == TSelector::GetHoldPoint())
    {
      for (i=1; i<50; i++)
      {
        if (PSelectorInfo[i] != 0)
        {
          if (PSelectorInfo[i]->GetType() == SEL_INPUT)  SelectEnable[i] = FALSE;
          if (PSelectorInfo[i]->GetType() == SEL_OUTPUT)  SelectEnable[i] = FALSE;
        }
      }

      // Check the platforms to ensure that only valid destinations
      // are allowed
      for (i=1; i<50; i++)
      {
        if (PPlatDataInfo[i] != 0)
        {
          // Is there a train in the platform?
          if (PPlatDataInfo[i]->GetTimetable() > 0)
          {
            switch (PTimetableInfo[PPlatDataInfo[i]->GetTimetable()]->GetStatus())
            {
              case ST_INPLAT:   SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
              case ST_STOCKOK:  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
              case ST_READYDEP: SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE; break;
            }
          }
          else
          {
            // Is there just a loco on its own in the platform?
            if (PPlatDataInfo[i]->GetLoco() > 0)  SelectEnable[PPlatDataInfo[i]->GetSelectorRef()] = FALSE;
          }
        }
      }

      // Check to see if refueling is enforced
      if ((frame->LocoRefuel) && (HoldLoco > 0))
      {
        if ((PLocosInfo[HoldLoco]->GetFlag() == LF_NEEDFUEL) ||
            (PLocosInfo[HoldLoco]->GetFlag() == LF_REFUEL))
        {
          // Disable platform selectors
          for (i = 1; i < 50; i++)
          {
            if (PSelectorInfo[i] != 0)
            {
              if (PSelectorInfo[i]->GetType() == SEL_PLAT)           SelectEnable[i] = FALSE;
              if (PSelectorInfo[i]->GetType() == SEL_ELECTRIC_PLAT)  SelectEnable[i] = FALSE;
            }
          }
        }
      }
    }

    // Now set the selectors appropriately based on the above information
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
      {
        if (::IsWindowEnabled(PSelectorInfo[i]->HWindow) != SelectEnable[i])
        {
          ::EnableWindow(PSelectorInfo[i]->HWindow, SelectEnable[i]);
          PSelectorInfo[i]->PaintButton(0);
        }
      }
    }
  }
  else
  {
    // Disable all the selectors
    for (i=1; i<50; i++)
    {
      if (PSelectorInfo[i] != 0)
      ::EnableWindow(PSelectorInfo[i]->HWindow, FALSE);
    }
  }
}


BOOL TLayout::IsRouteValid(int Start, int End)
{
  // This function determines if the specified route is valid - if it
  // is then the route is set and TRUE returned. Otherwise FALSE is
  // returned.

  int  lii, ljj, lkk, laa, lbb, lxx, lyy, RouteNo;
  BOOL lAvailable, NotFound;
  char TextString[50];

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Run through the list of routes to see if the route is valid
  lii = 1;
  lAvailable = FALSE;
  while ((lii < 1000) && !(lAvailable))
  {
    if (PRoutesInfo[lii] != 0)
    {
      if ((Start == PRoutesInfo[lii]->GetFrom()) && 
          (End == PRoutesInfo[lii]->GetTo()))
      {
        // We have a match - now check to see if this route is available
        // In the case of a route starting from the hold point the route is
        // only valid if the section next to the hold point is occupied.
        // In all other cases all sections have to be clear for the route
        // to be valid.

        lAvailable = TRUE;

        // Check all the sections to see if the route is available
        ljj = 0;
        while ((ljj < 6) && lAvailable)
        {
          PRoutesInfo[lii]->GetClear(ljj, laa, lbb);
          if (PSectionInfo[laa] != 0)
          {
            if (PSectionInfo[laa]->IsOccupied())  lAvailable = FALSE;
          }
          if (PSectionInfo[lbb] != 0)
          {
            if (PSectionInfo[lbb]->IsOccupied())  lAvailable = FALSE;
          }

          // Check for overlapping sections
          lkk = 1;
          while ((POverlapDataInfo[lkk] != 0) && lAvailable)
          {
            lxx = POverlapDataInfo[lkk]->GetSection(0);
            lyy = POverlapDataInfo[lkk]->GetSection(1);

            if ((laa == lxx) || (laa == lyy) || 
                (lbb == lxx) || (lbb == lyy))
            {
              if (PSectionInfo[lxx] != 0)
              {
                if (PSectionInfo[lxx]->IsOccupied())  lAvailable = FALSE;
              }
              if (PSectionInfo[lyy] != 0)
              {
                if (PSectionInfo[lyy]->IsOccupied())  lAvailable = FALSE;
              }
            }

            lkk++;
          }

          ljj++;
        }
      }


    }

    // Increment i
    lii++;
  }

  RouteNo = lii - 1;

  // If found then select the route and repaint it
  if (lAvailable)
  {
    BOOL UseLightLoco = FALSE;

    // Decide if we should use a light loco
    if (Start == TSelector::GetHoldPoint())  UseLightLoco = TRUE;
    if (End == TSelector::GetHoldPoint())  UseLightLoco = TRUE;
    if (Start == TSelector::GetLocoyard())  UseLightLoco = TRUE;

    // Does this route start from a platform?
    if ((PSelectorInfo[Start]->GetType() == SEL_PLAT) ||
        (PSelectorInfo[Start]->GetType() == SEL_ELECTRIC_PLAT))
    {
      // Is there no train in this platform and just a locomotive? If this is
      // the case then we need to create a temporary timetable record to move
      // this locomotive.
      if ((PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetTimetable() == 0) &&
          (PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco() != 0))
      {
        UseLightLoco = TRUE;
      }
    }

    if (UseLightLoco)
    {
      if (Start == TSelector::GetHoldPoint())
      {
        // Get a light loco
        if (HoldLoco > 0)
        {
          // Clear the hold point
          LightLoco(HoldLoco, 0, RouteNo, ST_ARRA);
        }
        else
        {
          ::MessageBox(HWindow,
                       "Internal error number ERR000 detected. Please note carefully\n how this error arose and report it to MDSoft.",
                       APPNAME,
                       MB_OK);
        }

        // Reset the hold loco
        HoldLoco = 0;
        HoldLocoChng = TRUE;
      }

      // Does this route start from a platform?
      if ((PSelectorInfo[Start]->GetType() == SEL_PLAT) ||
         (PSelectorInfo[Start]->GetType() == SEL_ELECTRIC_PLAT))
      {
        // Is there no train in this platform and just a locomotive? If this is
        // the case then we need to create a temporary timetable record to move
        // this locomotive.
        if ((PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetTimetable() == 0) &&
            (PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco() != 0))
        {
          // Get a light loco and clear loco from platform
          LightLoco(PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->GetLoco(), 0, RouteNo, ST_ARRA);
          PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->SetLoco(0);
          PPlatDataInfo[PSelectorInfo[Start]->GetPlatRef()]->SetRedraw(TRUE);
          DrawPlatform(0, PSelectorInfo[Start]->GetPlatRef());

          // Set the hold section if to point is the hold point
          if (End == TSelector::GetHoldPoint())
          {
            PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(TRUE);
            DrawSection(0, TSelector::GetHoldSection());
          }
        }
      }

      // Did this route start at the locoyard? - if so then we need to determine
      // which loco to use.
      if (Start == TSelector::GetLocoyard())
      {
        // Firstly ensure that the locoyard window exists
        if (frame->LocoyardHan == NULL)
        {
          ::SendMessage(frame->HWindow, WM_COMMAND, CM_WINLOCOYA, 0);
        }

        // Then ensure that it is maximized
        if (frame->LocoyardIcon)
        {
          OpenIcon(frame->LocoyardHan->HWindow);
        }

        // Check that there is at least one loco which is not refuelling
        // in the locoyard
        lii=0;
        NotFound = TRUE;
        while ((lii<16) && NotFound)
        {
          // Find a filled locoyard slot
          if (LocoyardLoco[lii] != 0)
          {
            if (PLocosInfo[LocoyardLoco[lii]]->GetFlag() == LF_LOCOYARD)
            {
              NotFound = FALSE;
            }
          }
          lii++;
        }

        if (NotFound)
        {
          // Update the status bar
          CopyString(TextString, "No locos are currently available");
          ::SendMessage(frame->StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) TextString));

          // Prevent route being drawn
          return TRUE;
        }
        else
        {
          // Update the main window status bar
          if (frame->StatbarHan != NULL)
          {
            CopyString(TextString, "Select a locomotive from the loco-yard");
            ::SendMessage(frame->StatbarHan->HWindow,
                          SB_SETTEXT,
                          0,
                          (LPARAM) ((LPSTR) TextString));
          }

          // Now set focus to the locoyard window
          ::SetFocus(frame->LocoyardHan->HWindow);

          // Now run the get loco routine
          frame->LocoyardHan->WhichLoco(RouteNo);

          // Set the hold point section as being occupied if a route has
          // been found
          if (End == TSelector::GetHoldPoint())
          {
            PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(TRUE);
            DrawSection(0, TSelector::GetHoldSection());
          }
        }
      }

    }
    else
    {
      // Update the <RoutesSet> array but only if the route did not
      // start from the hold point
      lii = 0;
      while ((RoutesSet[lii] > 0) && (lii < 3)) lii++;
      if (RoutesSet[3] != 0)
      {
        ::MessageBox(HWindow,
            "Internal error number ERR001 detected. Please note carefully\n how this error arose and report it to MDSoft.",
            APPNAME,
            MB_OK);

        char  RDump[100];
        FormatBuffer(RDump, "Route dump - 0:%d 1:%d 2:%d 3:%d", RoutesSet[0], RoutesSet[1],
          RoutesSet[2], RoutesSet[3]);
        ::MessageBox(HWindow, RDump, APPNAME, MB_OK);
      }
      else
      {
        RoutesSet[lii] = RouteNo;
      }
    }

    // Repaint the route
    for (ljj=0; ljj<6; ljj++)
    {
      PRoutesInfo[RouteNo]->GetClear(ljj, laa, lbb);
      if (PSectionInfo[laa] != 0)
      {
        PSectionInfo[laa]->SetOccupied(TRUE);
        DrawSection(0, laa);
      }
      if (PSectionInfo[lbb] != 0)
      {
        PSectionInfo[lbb]->SetOccupied(TRUE);
        DrawSection(0, lbb);
      }
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


BOOL TLayout::ReadDataFile()
{
  // This routine reads an input datafile and copies the data to the
  // appropriate structures
  std::ifstream infile;                  // Input filestream
  char      szInput[FILE_INPUT_BUFSIZE], // Input string
            szComment[400],              // Comment string
            szArrDesc[30],               // Arrival description
            szText1[400],                // Temporary string 1
            szText2[400];                // Temporary string 2
  int       IndexRef, i;                 // General usage
  int       lNextPos;                    // Next timetable position
  int       TempX[4], TempY[4];          // Temporary array
  long      TempClear[6];                // Another temporary array
  int       TempTime[9];                 // Yet another temporary array
  int       TempSections[2];             // Section array used by [OVERLAPPING] info 
  BOOL      Error;                       // Has an error occured?
  char     *lpstr;                       // String pointer

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Open the file
  infile.open(frame->DataFileName, std::ios::in);
  if (!infile)
  {
    MessageBeep(MB_ICONHAND);
    ::MessageBox(HWindow,
        "Cannot open datafile",
        APPNAME,
        MB_OK | MB_ICONSTOP);
    return FALSE;
  }
  else
  {
    // We have opened the file. The first thing that we look for is section
    // information ie [SECTIONS].
    szInput[0] = 0;
    while ((_stricmp(szInput, "[SECTIONS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[SECTIONS]") != 0)
    {
      // No information in the datafile of the [SECTIONS] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[SECTIONS]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the section information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 1000))
        {
          if (PSectionInfo[IndexRef] == 0)
          {
            // Now get point information
            for (i=0; i<4; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempX[i] = atoi(szInput);
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempY[i] = atoi(szInput);
            }

            // Insert the new section
            PSectionInfo[IndexRef] = new TSection(IndexRef, TempX, TempY);
          }
          else
          {
            // Duplicate reference number
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
            FormatBuffer(szText1, szText2, "[SECTIONS]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[SECTIONS]", "1", "999");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }


    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [GENERAL] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[GENERAL]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[GENERAL]") != 0)
    {
      // No information in the datafile of the [GENERAL] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[GENERAL]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the general information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first line of text
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, '=');

        if (strcmp(szInput, "StartTime") == 0)
        {
          // Found start time
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          WorkTime = PackTime(atoi(szInput));
        }
        else if (strcmp(szInput, "StopTime") == 0)
        {
          // Found stop time
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          StopTime = PackTime(atoi(szInput));
        }
        else if (strcmp(szInput, "StartText1") == 0)
        {
          // Found <StartText[0]>
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          CopyString(StartText[0], szInput);
        }
        else if (strcmp(szInput, "StartText2") == 0)
        {
          // Found <StartText[1]>
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          CopyString(StartText[1], szInput);
        }
        else if (strcmp(szInput, "StartText3") == 0)
        {
          // Found <StartText[2]>
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          CopyString(StartText[2], szInput);
        }
        else if (strcmp(szInput, "DrawSectionNumber") == 0)
        {
          // Found <DrawSectionNumber>
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          lpstr = Trim(szInput);
          if ((lpstr[0] == 'T') || (lpstr[0] == 't'))
          {
            TRC_DBG((TB, "Drawing section numbers"));
            mDrawSectionNumber = TRUE;
          }
        }
        else if (strcmp(szInput, "DrawSelectorNumber") == 0)
        {
          // Found <DrawSelectorNumber>
          ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
          lpstr = Trim(szInput);
          if ((lpstr[0] == 'T') || (lpstr[0] == 't'))
          {
            TRC_DBG((TB, "Drawing selector numbers"));
            mDrawSelectorNumber = TRUE;
          }
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }

      // Check that we achieved something
      if ((WorkTime == 0) ||
         (StopTime == 0) ||
         (WorkTime >= StopTime))
      {
        // Generate an error - data is invalid as some is missing
        ClearData();
        ::LoadString(GetModule()->GetHandle(), 10000, szText2, 400);
        FormatBuffer(szText1, szText2, "[GENERAL]");
        MessageBeep(MB_ICONEXCLAMATION);
        ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
      }
      else
      {
        // Calculate the length of the shift (in hours)
        ShiftLength = (NormTime(StopTime)-NormTime(WorkTime))/100;
      }
    }

    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [OVERLAPPING] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[OVERLAPPING]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[OVERLAPPING]") != 0)
    {
      // No information in the datafile of the [OVERLAPPING] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[OVERLAPPING]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the overlapping information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 50))
        {
          if (POverlapDataInfo[IndexRef] == 0)
          {
            // Now get section information
            for (i=0; i<2; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempSections[i] = atoi(szInput);
            }

            // Insert the new overlap data
            POverlapDataInfo[IndexRef] = new TOverlapData(TempSections);
          }
          else
          {
            // Duplicate reference number
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
            FormatBuffer(szText1, szText2, "[OVERLAPPING]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[OVERLAPPING]", "1", "49");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }

    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [PLATFORMS] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[PLATFORMS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[PLATFORMS]") != 0)
    {
      // No information in the datafile of the [PLATFORMS] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[PLATFORMS]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 50))
        {
          if (PPlatDataInfo[IndexRef] == 0)
          {
            // Now get point information
            for (i=0; i<4; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempX[i] = atoi(szInput);
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempY[i] = atoi(szInput);
            }

            // Insert the new section
            PPlatDataInfo[IndexRef] = new TPlatData(TempX, TempY);
          }
          else
          {
            // Duplicate reference number
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
            FormatBuffer(szText1, szText2, "[PLATFORM]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[PLATFORM]", "1", "49");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }


    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [SELECTOR] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[SELECTOR]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[SELECTOR]") != 0)
    {
      // No information in the datafile of the [SELECTOR] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[SELECTOR]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 50))
        {
         if (PSelectorInfo[IndexRef] == 0)
         {
           // Now get point information
           for (i=0; i<4; i++)
           {
             ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
             TempX[i] = atoi(szInput);
           }
           for (i=0; i<2; i++)
           {
             ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
             TempY[i] = atoi(szInput);
           }

           if ((TempY[0] == 3) || (TempY[0] == 4))
           {
             if ((TempY[1] < 1) || (TempY[1] > 49))
             {
               // Ensure that the platform index reference value is within limits
               ClearData();
               ::LoadString(GetModule()->GetHandle(), 10010, szText1, 400);
               MessageBeep(MB_ICONEXCLAMATION);
               ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
               return FALSE;
             }
             else
             {
               if (PPlatDataInfo[TempY[1]] == 0)
               {
                 // Selector item references an undefined platform
                 ClearData();
                 ::LoadString(GetModule()->GetHandle(), 10011, szText1, 400);
                 MessageBeep(MB_ICONEXCLAMATION);
                 ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
                 return FALSE;
               }
               else
               {
                 // Set the platform selector reference
                 PPlatDataInfo[TempY[1]]->SetSelectorRef(IndexRef);
               }
             }
           }

           // Insert the new selector
           PSelectorInfo[IndexRef] = new TSelector(this, TempX[0], TempX[1], TempX[2], TempX[3], IndexRef, TempY[0], TempY[1], CM_SELECT, 0, 1);
           PSelectorInfo[IndexRef]->Create();
           ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');

           /******************************************************************/
           /* Decide whether to use the selector reference number or the     */
           /* actual selector text.                                          */
           /******************************************************************/
           if (mDrawSelectorNumber)
           {
             FormatBuffer(szInput, "%d", IndexRef);
           }

           /******************************************************************/
           /* Set the text.                                                  */
           /******************************************************************/
           PSelectorInfo[IndexRef]->SetText(szInput);
         }
         else
         {
           // Duplicate reference number
           ClearData();
           ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
           FormatBuffer(szText1, szText2, "[SELECTOR]", IndexRef);
           MessageBeep(MB_ICONEXCLAMATION);
           ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
           return FALSE;
         }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[SELECTOR]", "1", "49");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }


    // Now check the platform information to ensure that each platform
    // has been associated with a selector
    for (i=1; i<50; i++)
    {
      if (PPlatDataInfo[i] != 0)
      {
        if (PPlatDataInfo[i]->GetSelectorRef() == 0)
        {
          // Platform <i> is not associated with a selector
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10012, szText2, 400);
          FormatBuffer(szText1, szText2, i);
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }
      }
    }

    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [ROUTES] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[ROUTES]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[ROUTES]") != 0)
    {
      // No information in the datafile of the [ROUTES] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[ROUTES]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
           (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 1000))
        {
          if (PRoutesInfo[IndexRef] == 0)
          {
            // Now get point information
            for (i=0; i<2; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempX[i] = atoi(szInput);
            }
            for (i=0; i<6; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempClear[i] = atol(szInput);
            }

            // Insert the new section
            PRoutesInfo[IndexRef] = new TRoutes(TempX[0], TempX[1], TempClear);

            // Now check the section data
            if ((PSelectorInfo[TempX[0]] == 0) ||
                (PSelectorInfo[TempX[1]] == 0))
            {
              // OK, there is a problem so stop
              ClearData();
              ::LoadString(GetModule()->GetHandle(), 10005, szText2, 400);
              FormatBuffer(szText1, szText2, IndexRef);
              MessageBeep(MB_ICONEXCLAMATION);
              ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
              return FALSE;
            }

            for (i=0; i<6; i++)
            {
              PRoutesInfo[IndexRef]->GetClear(i, TempX[0], TempX[1]);
              if (((TempX[0] != 0) &&
                  (PSectionInfo[TempX[0]] == 0)) ||
                  ((TempX[1] != 0) &&
                  (PSectionInfo[TempX[1]] == 0)))
              {
                // OK there is a problem so stop
                ClearData();
                ::LoadString(GetModule()->GetHandle(), 10004, szText2, 400);
                FormatBuffer(szText1, szText2, IndexRef, i+1);
                       MessageBeep(MB_ICONEXCLAMATION);
                ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
                return FALSE;
              }
            }
          }
          else
          {
            // Duplicate reference number
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
            FormatBuffer(szText1, szText2, "[ROUTES]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[ROUTES]", "1", "999");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }

    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [LOCOS] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[LOCOS]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[LOCOS]") != 0)
    {
      // No information in the datafile of the [LOCOS] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[LOCOS]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < 500))
        {
          if (PLocosInfo[IndexRef] == 0)
          {
           // Now get point information
           for (i=0; i<3; i++)
           {
             ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
             TempX[i] = atoi(szInput);
           }

           // Insert the new loco item
           PLocosInfo[IndexRef] = new TLocos(TempX[0], TempX[1], TempX[2]);
         }
         else
         {
           // Duplicate reference number
           ClearData();
           ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
           FormatBuffer(szText1, szText2, "[LOCOS]", IndexRef);
           MessageBeep(MB_ICONEXCLAMATION);
           ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
           return FALSE;
         }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[LOCOS]", "1", "499");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }

    // Reset the file
    infile.close();
    infile.clear();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [LOCOYARD] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[LOCOYARD]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[LOCOYARD]") != 0)
    {
      // No information in the datafile of the [LOCOYARD] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[LOCOYARD]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the selector information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);

      /**********************************************************************/
      /* Check for locoyard being disabled.                                 */
      /**********************************************************************/
      if (_stricmp(szInput, "Disabled") == 0)
      {
        /********************************************************************/
        /* Locoyard is disabled.                                            */
        /********************************************************************/
        TRC_NRM((TB, "Locoyard is disabled"));
        frame->LocoyardEnabled = FALSE;
      }
      else
      {
        TRC_NRM((TB, "Locoyard is enabled"));
        frame->LocoyardEnabled = TRUE;

        while ((szInput[0] != '[') && (szInput[0] != ' ') &&
               (szInput[0] != 0) && (!infile.eof()))
        {
          // Get first part of string - index reference
          std::istringstream ins(szInput);
          IndexRef = 0;

          if (IndexRef < 15)
          {
            // Now get locoyard information
            for (i=0; i<2; i++)
            {
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
              TempX[i] = atoi(szInput);
            }
            if (((TempX[0] > 0) && (TempX[0] < 7)) ||
               ((TempX[0] > 9) && (TempX[0] < 13)))
            {
              if ((TempX[1] >= 0) && (TempX[1] < 60))
              {
                // Assign the loco
                AssignYardLoco(TempX[0], WorkTime+TempX[1]);
                IndexRef++;
              }
            }
          }
          else
          {
            // Out of range
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
            FormatBuffer(szText1, szText2, "[LOCOYARD]", "1", "20");
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          // Get the next line of data
          infile.getline(szInput, FILE_INPUT_BUFSIZE);
        }
      }
    }

    // Reset the file
    infile.close();
    infile.open(frame->DataFileName, std::ios::in);

    // Now we look for [TIMETABLE] information
    szInput[0] = 0;
    while ((_stricmp(szInput, "[TIMETABLE]") != 0) && (!infile.eof()))
    {
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
    }

    if (_stricmp(szInput, "[TIMETABLE]") != 0)
    {
      // No information in the datafile of the [TIMETABLE] type
      ClearData();
      ::LoadString(GetModule()->GetHandle(), 10001, szText2, 400);
      FormatBuffer(szText1, szText2, "[TIMETABLE]");
      MessageBeep(MB_ICONEXCLAMATION);
      ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
      return FALSE;
    }
    else
    {
      // Read the timetable information in from the datafile
      szInput[0] = 0;
      infile.getline(szInput, FILE_INPUT_BUFSIZE);
      while ((szInput[0] != '[') && (szInput[0] != ' ') &&
             (szInput[0] != 0) && (!infile.eof()))
      {
        // Get first part of string - index reference
        std::istringstream ins(szInput);
        ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');
        IndexRef = atoi(szInput);

        if ((IndexRef > 0) && (IndexRef < MAX_TIMETABLE))
        {
          if (PTimetableInfo[IndexRef] == 0)
          {
            // Now get timetable information
            ins.getline(szText1, 50, ',');
            ins.getline(szText2, 50, ',');
            for (i = 0; i < 9; i++)
            {
              TempTime[i] = 0;
              ins.getline(szInput, FILE_INPUT_BUFSIZE, ',');

              /**************************************************************/
              /* Check for a comment - copy it across if present.           */
              /**************************************************************/
              if (i == 8)
              {
                szComment[0] = 0;
                lpstr = strchr(szInput, '#');
                if (lpstr != NULL)
                {
                  lpstr++;
                  CopyString(szComment, sizeof(szComment), lpstr);
                  *lpstr = 0;
                }
              }

              /**************************************************************/
              /* Store the value.                                           */
              /**************************************************************/
              TempTime[i] = atoi(szInput);
            }

            // Check to ensure that the selector number is an input selector
            Error = TRUE;
            if ((TempTime[0] > 0) && (TempTime[0] < 50))
            {
              if (PSelectorInfo[TempTime[0]] != 0)
              {
                if (PSelectorInfo[TempTime[0]]->GetType() == SEL_INPUT)  Error = FALSE;
              }
            }
            if (Error)
            {
              // A selector is not of the required input type
              ClearData();
              ::LoadString(GetModule()->GetHandle(), 10006, szText2, 400);
              FormatBuffer(szText1, szText2, IndexRef);
              MessageBeep(MB_ICONEXCLAMATION);
              ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
              return FALSE;
            }

            // Check to ensure that the value of the reassignable loco field is valid
            if ((TempTime[8] < 0) || (TempTime[8] >= MAX_TIMETABLE))
            {
              ClearData();
              ::LoadString(GetModule()->GetHandle(), 10007, szText2, 400);
              FormatBuffer(szText1, szText2, 1, (MAX_TIMETABLE - 1), IndexRef);
              MessageBeep(MB_ICONEXCLAMATION);
              ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
              return FALSE;
            }

            // Insert the new timetable item
            PTimetableInfo[IndexRef] = new TTimetable(szText1, szText2, TempTime, szComment);

            // Now check to see if the loco is still attached to train
            if (PTimetableInfo[IndexRef]->GetStatus() == ST_INPLAT)
            {
              // Assign a loco and update its status
              if (!AssignLoco(IndexRef))
              {
                ClearData();
                return FALSE;
              }

              /**************************************************************/
              /* If the Locoyard is enabled then set the "NEEDFUEL" flag.   */
              /**************************************************************/
              if (frame->LocoyardEnabled)
              {
                PLocosInfo[PTimetableInfo[IndexRef]->GetLoco()]->SetFlag(LF_NEEDFUEL);
              }

              // Which platform is the train in?
              if ((PTimetableInfo[IndexRef]->GetInitialLocoPlat() > 0) &&
                  (PTimetableInfo[IndexRef]->GetInitialLocoPlat() < 15))
              {
                PPlatDataInfo[PTimetableInfo[IndexRef]->GetInitialLocoPlat()]->SetTimetable(IndexRef);
              }

              // Calculate departure time
              PTimetableInfo[IndexRef]->SetRelTime(NormTime(PackTime(PTimetableInfo[IndexRef]->GetRelTime()) + PackTime(PTimetableInfo[IndexRef]->GetArrTime())));
            }

            // Loco is no longer attached to train and train is ready to depart
            if (PTimetableInfo[IndexRef]->GetStatus() == ST_STOCKOK)
            {
              // Initially assign a loco to the train
              if (!AssignLoco(IndexRef))
              {
                ClearData();
                return FALSE;
              }

              if ((PTimetableInfo[IndexRef]->GetInitialLocoPlat() > 0) &&
                  (PTimetableInfo[IndexRef]->GetInitialLocoPlat() < 15))
              {
                PPlatDataInfo[PTimetableInfo[IndexRef]->GetInitialLocoPlat()]->SetTimetable(IndexRef);

                if ((PTimetableInfo[IndexRef]->GetDepCode() != SC_EMU) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_HST) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_PUSHPULL) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU1) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU150) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU156) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU158) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_DMU170) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_TWIN156) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_TWIN158) &&
                    (PTimetableInfo[IndexRef]->GetDepCode() != SC_TWIN170))
                {
                  PPlatDataInfo[PTimetableInfo[IndexRef]->GetInitialLocoPlat()]->SetLoco(PTimetableInfo[IndexRef]->GetLoco());

                  /**********************************************************/
                  /* If the Locoyard is enabled then set the "NEEDFUEL"     */
                  /* flag.                                                  */
                  /**********************************************************/
                  if (frame->LocoyardEnabled)
                  {
                    PLocosInfo[PTimetableInfo[IndexRef]->GetLoco()]->SetFlag(LF_NEEDFUEL);
                  }

                  PTimetableInfo[IndexRef]->SetLoco(0, 0);

                  // Now assign a second loco to the train
                  PTimetableInfo[IndexRef]->SetArrCode(PTimetableInfo[IndexRef]->GetDepCode());
                  if (!AssignLoco(IndexRef))
                  {
                    ClearData();
                    return FALSE;
                  }
                }
              }
            }
          }
          else
          {
            // Duplicate reference number
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10003, szText2, 400);
            FormatBuffer(szText1, szText2, "[TIMETABLE]", IndexRef);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
        else
        {
          // Out of range
          ClearData();
          ::LoadString(GetModule()->GetHandle(), 10002, szText2, 400);
          FormatBuffer(szText1, szText2, "[TIMETABLE]", "1", "499");
          MessageBeep(MB_ICONEXCLAMATION);
          ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
          return FALSE;
        }

        // Get the next line of data
        infile.getline(szInput, FILE_INPUT_BUFSIZE);
      }
    }

    // Now check all the reassignable loco fields
    for (i = 1; i < MAX_TIMETABLE; i++)
    {
      if (PTimetableInfo[i] != 0)
      {
        if (PTimetableInfo[i]->GetNextTimeTabPos() != 0)
        {
          /******************************************************************/
          /* For timetable entries with non-zero reassignable loco fields,  */
          /* check that the linked timetable entry is present.              */
          /******************************************************************/
          lNextPos = PTimetableInfo[i]->GetNextTimeTabPos();
          if ((lNextPos >= 500) || (PTimetableInfo[lNextPos] == 0))
          {
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10008, szText2, 400);
            FormatBuffer(szText1, szText2, i, lNextPos);
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }

          /******************************************************************/
          /* For timetable entries with non-zero reassignable loco fields   */
          /* check that the departure code of the original timetable entry  */
          /* is the same as the arrival code of the linked timetable entry. */
          /******************************************************************/
          if (PTimetableInfo[i]->GetDepCode() != PTimetableInfo[lNextPos]->GetArrCode())
          {
            ::LoadString(GetModule()->GetHandle(), 10009, szText2, 400);
            FormatBuffer(szText1, 
                     szText2, 
                     PTimetableInfo[i]->GetDepCode(), 
                     i, 
                     PTimetableInfo[lNextPos]->GetArrCode(),
                     lNextPos);
            ClearData();
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }

        /********************************************************************/
        /* Check for "Becomes" comment - if it's present then verify that   */
        /* it refers to the same train as the linked timetable index.       */
        /********************************************************************/
        PTimetableInfo[i]->GetComment(szComment, sizeof(szComment));
        if (szComment[0] != 0)
        {
          lpstr = strstr(szComment, "Becomes");

          if (lpstr == NULL)
          {
            continue;
          }

          /******************************************************************/
          /* Skip over the "Becomes" prefix and trim the string.            */
          /******************************************************************/
          lpstr += 7;
          lpstr = Trim(lpstr);

          lNextPos = PTimetableInfo[i]->GetNextTimeTabPos();

          if (lNextPos != 0)
          {
            PTimetableInfo[lNextPos]->GetArrDesc(szArrDesc, sizeof(szArrDesc));
          }
          else
          {
            szArrDesc[0] = 0;
          }


          TRC_NRM((TB, "idx:%d comment:'%s' lnextpos:%d arrdesc:'%s'",
                   i, lpstr, lNextPos, szArrDesc));


          if ((lNextPos == 0) || (strcmp(lpstr, szArrDesc) != 0))
          {
            ClearData();
            ::LoadString(GetModule()->GetHandle(), 10013, szText2, 400);
            FormatBuffer(szText1, 
                     szText2, 
                     i,
                     lNextPos,
                     lpstr ? lpstr : "<null>",
                     (szArrDesc[0] != 0) ? szArrDesc : "<null>");
            MessageBeep(MB_ICONEXCLAMATION);
            ::MessageBox(HWindow, szText1, APPNAME, MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
          }
        }
      }
    }
  }

  // If we get here then all is well (hopefully!)
  return TRUE;
}


void TLayout::AssignYardLoco(int LocoFlag, int RefuelTime)
{
  std::vector<int> freeLocos;
  int  i;
  BOOL NotFound;

  // This routine assigns locos to the locoyard

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  freeLocos.reserve(32);
  for (i = 1; i < 500; i++)
  {
    if (PLocosInfo[i] != 0)
    {
      if ((PLocosInfo[i]->GetType() == LocoFlag) && (PLocosInfo[i]->GetFlag() == LF_UNASSIGN))
      {
        freeLocos.push_back(i);
      }
    }
  }

  // We have found at least one locomotive
  if (!freeLocos.empty())
  {
    // Select one of the locos
    const size_t randomIndex = static_cast<size_t>(::rand() % freeLocos.size());
    const int b = freeLocos[randomIndex];

    // Is the loco refueling and is refueling enabled?
    if (frame->LocoRefuel)
    {
      if (RefuelTime == 0)
      {
        PLocosInfo[b]->SetFlag(LF_LOCOYARD);  // LF_LOCOYARD (7)
      }
      else
      {
        PLocosInfo[b]->SetFlag(LF_REFUEL);   // LF_REFUEL (6)
        PLocosInfo[b]->SetRefuelTime(RefuelTime);
      }
    }
    else
    {
      PLocosInfo[b]->SetFlag(LF_LOCOYARD);
    }

    // Update the <LocoyardLoco> array
    i=0;
    NotFound = TRUE;
    while ((i<16) && NotFound)
    {
      // Find an empty locoyard slot
      if (LocoyardLoco[i] == 0)  NotFound = FALSE;
      i++;
    }

    if ((i == 16) && (LocoyardLoco[15] != 0))
    {
      ::MessageBox(HWindow,
                   "Internal error number ERR002 detected. Please note carefully\n how this error arose and report it to MDSoft.",
                   APPNAME,
                   MB_OK);
    }
    else
    {
      // Assign the loco to the <LocoyardLoco> array
      i--;
      LocoyardLoco[i] = b;
    }
  }
}


BOOL TLayout::AssignLoco(int TimetabPos)
{
  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // This routine is responsible for assigning locomotives to trains
  std::vector<int> freeLocos;
  int  GivenType;
  int  z;
  int  lii;
  int  lToAssign;

  // Get the type of the loco.
  GivenType = PTimetableInfo[TimetabPos]->GetArrCode();

  // Work out how many locos to assign.
  if ((GivenType == SC_TWIN156) ||
      (GivenType == SC_TWIN158) ||
      (GivenType == SC_TWIN170))
  {
    lToAssign = 2;
  }
  else
  {
    lToAssign = 1;
  }

  // Translate loco types
  if (GivenType == SC_LIGHTECS)   GivenType = SC_ECS;
  if (GivenType == SC_LIGHTNORM)  GivenType = SC_NORMAL;
  if (GivenType == SC_LIGHTDELT)  GivenType = SC_DELTIC;
  if (GivenType == SC_LIGHTRLF)   GivenType = SC_RELIEF;
  if (GivenType == SC_LIGHT37)    GivenType = SC_CLASS37;
  if (GivenType == SC_TWIN156)    GivenType = SC_DMU156;
  if (GivenType == SC_TWIN158)    GivenType = SC_DMU158;
  if (GivenType == SC_TWIN170)    GivenType = SC_DMU170;

  freeLocos.reserve(100);

  for (lii = 0; lii < lToAssign; lii++)
  {
    // Work out which locos of the appropriate type are free.
    freeLocos.clear();
    for (z = 1; z < 500; z++)
    {
      if (PLocosInfo[z] != 0)
      {
        if ((PLocosInfo[z]->GetType() == GivenType) && (PLocosInfo[z]->GetFlag() == LF_UNASSIGN))
        {
          freeLocos.push_back(z);
        }
      }
    }

    // Select a loco from the array
    if (!freeLocos.empty())
    {
      const size_t randomIndex = static_cast<size_t>(::rand() % freeLocos.size());
      const int selectedLoco = freeLocos[randomIndex];
      PTimetableInfo[TimetabPos]->SetLoco(lii, selectedLoco);

      // Update the fact that the loco is now assigned
      if (PLocosInfo[selectedLoco] != 0)
      {
        PLocosInfo[selectedLoco]->SetFlag(LF_ASSIGNED);
      }
    }
    else
    {
      /**********************************************************************/
      /* Trace current loco allocations.                                    */
      /**********************************************************************/
      TRC_ALT((TB, "AssignLoco failure timetab:%d type:%d iteration:%d", TimetabPos, GivenType, lii));
      for (z = 1; z < 500; z++)
      {
        if (PLocosInfo[z] != 0)
        {
          if (PLocosInfo[z]->GetType() == GivenType)
          {
            char lLocoNumStr[100];

            PLocosInfo[z]->LocoNumStr(lLocoNumStr);
            TRC_NRM((TB, "Loco idx:%d type:%d flag:%d refueltime:%d ID:%s", 
                         z,
                         PLocosInfo[z]->GetType(),
                         PLocosInfo[z]->GetFlag(),
                         PLocosInfo[z]->GetRefuelTime(),
                         lLocoNumStr));
          }
        }
      }

      // Stop
      if (frame != 0)
      {
        ::SendMessage(frame->HWindow, WM_COMMAND, CM_MNUFISTOP, 0);
      }

      // ...display error box...
      char Str1[200];
      char Str2[32];
      CopyString(Str1, "An error has occurred: Rail Control was unable to\nassign a loco to a train. The loco type required is: ");
      FormatBuffer(Str2, "%d", GivenType);
      AppendString(Str1, Str2);
      AppendString(Str1, ".");
      ::MessageBox(HWindow, Str1, APPNAME, MB_ICONHAND | MB_OK);

      return FALSE;
    }
  }

  return TRUE;
}


void TLayout::GetExpecteds()
{
  // This routine retrieves the timetable numbers of the next four expected
  // trains.

  int  lii                  = 0;
  int  ljj                  = 0;
  int  lkk                  = 0;
  int  lmintime             = 0;
  BOOL lAlreadyFound        = FALSE;
  int  lOldExpect[DISP_NUM_ARRIVAL];

  // Reset everything
  for (lii = 0; lii < DISP_NUM_ARRIVAL; lii++)
  {
    lOldExpect[lii] = Expect[lii];
    Expect[lii] = 0;
    ExpChng[lii] = FALSE;
  }

  // Read the timetable -
  // run through the timetable data looking for first train expected...
  for (ljj = 0; ljj < DISP_NUM_ARRIVAL; ljj++)
  {
    lmintime = 3000;
    for (lii = 1; lii < MAX_TIMETABLE; lii++)
    {
      if ((PTimetableInfo[lii] != 0) &&
          (CalcArrivalTime(lii) <= lmintime) &&
          (PTimetableInfo[lii]->GetStatus() >= ST_NONE) &&
          (PTimetableInfo[lii]->GetStatus() <= ST_ARRB))
      {
        /********************************************************************/
        /* This is a possible candidate, but scan the existing expected     */
        /* array to ensure we've not already found this train.              */
        /********************************************************************/
        lAlreadyFound = FALSE;
        for (lkk = 0; lkk < ljj; lkk++)
        {
          if (Expect[lkk] == lii)
          {
            lAlreadyFound = TRUE;
          }
        }

        if (!lAlreadyFound)
        {
          // Get new minimum value
          lmintime = CalcArrivalTime(lii);
          Expect[ljj] = lii;
        }
      }
    }
  }

  // Check to see if value has changed since previous time...
  for (lii = 0; lii < DISP_NUM_ARRIVAL; lii++)
  {
    if (Expect[lii] != lOldExpect[lii]) ExpChng[lii] = TRUE;
  }

  // Now we can calculate whether the train is due, approaching, or held.
  // The following codes are used:
  //
  //  0: None
  //  1: Due on UF
  //  2: Approaching on UF
  //  3: Held on UF
  //  4: Held at Holby (UF)
  //  5: Set to platform
  //  6: Stage A of approach
  //  7: Stage B of approach
  //  8: Stage C of approach
  //  9: Stage D of approach
  // 10: Stage E of approach
  // 11: Stage F of approach
  // 12: In platform
  // 13: Stock released and needing new loco
  // 14: Stock OK
  // 15: Stock ready to depart
  // 16: Starting to depart
  // 17: Stage A of departure
  // 18: Stage B of departure
  // 19: Stage C of departure
  // 20: Stage D of departure
  // 21: Stage E of departure
  // 22: Stage F of departure
  //
  // A train will begin approaching the station 6 minutes before it is due
  // in the platform. It will then move from state 1 to 2. If it has not
  // been set to a platform it will be held three minutes before it is
  // due to arrive in the platform.
  //
  // Also note that all times used in the following routine have the same
  // format as that of WorkTime, ie Hours*2*60 + Minutes*2 + Halfminute.
  // The half minute flag is either 0 (corresponding to the upper timer
  // block illuminated on the main window display) or 1 (corresponding to
  // the lower timer block illuminated on the main window display)

  for (lii = 1; lii < 50; lii++)
  {
    if (PSelectorInfo[lii] != 0)
    {
      if (PSelectorInfo[lii]->GetType() == SEL_INPUT) TimeCheck(lii);
    }
  }

  return;
}


void TLayout::TimeCheck(int GivArrPnt)
{
  int   i, j, StateChange;
  int   TimeMinusSix, TimeMinusThree, CurrentTime;
  BOOL  States[50];
  BOOL  lRouteSet;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // Now carry out our time checks
  for (i=0; i<4; i++)
  {
   if (Expect[i] > 0)
   {
    // Calculate arrival time minus 6 minutes
    TimeMinusSix = 120*(CalcArrivalTime(Expect[i])/100);
    TimeMinusSix += 2*(CalcArrivalTime(Expect[i])-
               100*int(CalcArrivalTime(Expect[i])/100));
    TimeMinusSix -= 12;
    if (TimeMinusSix < 0)  TimeMinusSix += 2880;

    // Calculate arrival time minus 3 minutes
    TimeMinusThree = TimeMinusSix + 6;
    if (TimeMinusThree >= 2880)  TimeMinusThree -= 2880;

    // Calculate current time
    CurrentTime = WorkTime;
    StateChange = 0;

    // Find out the states of the other waiting trains - all other trains
    // are automatically excluded.
    for (j=1;j<=40;j++) States[j] = FALSE;
    for (j=0;j<=3;j++)
    {
      if (Expect[j] > 0)
      {
       if (PTimetableInfo[Expect[j]]->GetArrPnt() == GivArrPnt)
        States[PTimetableInfo[Expect[j]]->GetStatus()] = TRUE;
      }
    }

    // Handle the trains
    if (PTimetableInfo[Expect[i]]->GetArrPnt() == GivArrPnt)
    {
#ifdef MDDEBUG
      DebugTrainInfo("entry", i, Expect[i]);
#endif
      // Assign a loco
      if (PTimetableInfo[Expect[i]]->GetLoco() == 0)
      {
        if (!AssignLoco(Expect[i])) return;
      }

      /**********************************************************************/
      /* BEGIN STATE ENGINE                                                 */
      /**********************************************************************/
      /**********************************************************************/
      /* Check for a route set from the entry point.                        */
      /**********************************************************************/
      lRouteSet = FALSE;

      for (j=0; j<=3; j++)
      {
        // Check to see if any of the set routes start at the appropriate
        // section...
        if (RoutesSet[j] > 0)
        {
          if (PRoutesInfo[RoutesSet[j]]->GetFrom() == PTimetableInfo[Expect[i]]->GetArrPnt())
          {
            // OK - we have a match
            lRouteSet = TRUE;
            break;
          }                                  
        }
      }

      /**********************************************************************/
      /* CHECK FOR STATUS ST_DUE - "Expected"                               */
      /**********************************************************************/
      if ((CurrentTime < TimeMinusSix) &&
          (PTimetableInfo[Expect[i]]->GetStatus() == ST_NONE))
      {
        StateChange = ST_DUE;
        ExpChng[i] = TRUE;

        TRC_NRM((TB, "StateChange -> ST_DUE"));
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_APPROACH - "Approaching"                       */
      /**********************************************************************/
      else if ((CurrentTime >= TimeMinusSix) &&
               (CurrentTime < TimeMinusThree) &&
               (!States[ST_APPROACH]) &&
               (!States[ST_FIRSTHELD]) &&
               ((PTimetableInfo[Expect[i]]->GetStatus() == ST_NONE) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_DUE) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD)))
      {
        StateChange = ST_APPROACH;
        ExpChng[i] = TRUE;

        TRC_NRM((TB, "StateChange -> ST_APPROACH"));
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_SETPLAT - "Set to platform X"                  */
      /**********************************************************************/
      else if ((PTimetableInfo[Expect[i]]->GetRoute() == 0) &&
               (!States[ST_SETPLAT]) &&
               ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_FIRSTHELD)) &&
               (lRouteSet))
      {
        PTimetableInfo[Expect[i]]->SetRoute(RoutesSet[j]);
        RoutesSet[j] = 0;
        ExpChng[i] = TRUE;
        StateChange = ST_SETPLAT;

        TRC_NRM((TB, "StateChange -> ST_SETPLAT"));
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_HELD - "Held on X"                             */
      /**********************************************************************/
      else if ((CurrentTime >= TimeMinusSix) &&
               (CurrentTime < TimeMinusThree) &&
               ((PTimetableInfo[Expect[i]]->GetStatus() == ST_NONE) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_DUE)) &&
               ((States[ST_FIRSTHELD]) || (States[ST_APPROACH])))
      {
        StateChange = ST_HELD;
        ExpChng[i] = TRUE;

        TRC_NRM((TB, "StateChange -> ST_HELD (1)"));
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_HELD - "Held on X"                             */
      /**********************************************************************/
      else if ((CurrentTime >= TimeMinusThree) &&
               ((PTimetableInfo[Expect[i]]->GetStatus() == ST_NONE) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_DUE)))
      { 
        // What is the next allowable state?
        if (States[ST_FIRSTHELD])
        {
          StateChange = ST_HELD;

          TRC_NRM((TB, "StateChange -> ST_HELD (2)"));
        }
        else
        {
          StateChange = ST_FIRSTHELD;

          TRC_NRM((TB, "StateChange -> ST_FIRSTHELD (2)"));
        }
        ExpChng[i] = TRUE;
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_FIRSTHELD - "First held on X"                  */
      /**********************************************************************/
      else if ((CurrentTime >= TimeMinusThree) &&
               (!States[ST_FIRSTHELD]) &&
               ((PTimetableInfo[Expect[i]]->GetStatus() == ST_APPROACH) ||
                (PTimetableInfo[Expect[i]]->GetStatus() == ST_HELD)))
      {
        StateChange = ST_FIRSTHELD;
        ExpChng[i] = TRUE;

        TRC_NRM((TB, "StateChange -> ST_FIRSTHELD (3)"));
      }
      /**********************************************************************/
      /* CHECK FOR STATUS ST_ARRA - "Section A of approach"                 */
      /**********************************************************************/
      else if ((CurrentTime > TimeMinusThree) &&
               (PTimetableInfo[Expect[i]]->GetStatus() == ST_SETPLAT))
      {
        ExpChng[i] = TRUE;
        StateChange = ST_ARRA;

        // Calculate average arrival delay
        ArrDelay += WorkTime + 5 - PackTime(PTimetableInfo[Expect[i]]->GetArrTime());
        ArrNum ++;

        if (frame->DelayEnable)
        {
          ArrDelay -= 2*(PTimetableInfo[Expect[i]]->GetMinLate());
        }

        TRC_NRM((TB, "StateChange -> ST_ARRA"));
      }
      /**********************************************************************/
      /* NO STATE CHANGE                                                    */
      /**********************************************************************/
      else
      {
        TRC_NRM((TB, "No state change"));
      }
      /**********************************************************************/
      /* END STATE ENGINE                                                   */
      /**********************************************************************/

      // Update status field
      if (StateChange > 0)  
      {
        TRC_NRM((TB, "State change is %d", StateChange));
        PTimetableInfo[Expect[i]]->SetStatus(StateChange);
      }

#ifdef MDDEBUG
      DebugTrainInfo("exit ", i, Expect[i]);
#endif
    }
   }
  }
}


void TLayout::GetDepartures()
{
  // This routine gets the timetable numbers of the next departures

  int   mintime;
  int   lii = 0;
  int   ljj = 0;
  int   OldDepart[DISP_NUM_DEPART];
  int   OldStatus[DISP_NUM_DEPART];

  // Reset everything
  for (lii = 0; lii < DISP_NUM_DEPART; lii++)
  {
    OldDepart[lii] = Depart[lii];
    if (Depart[lii] > 0)  OldStatus[lii] = PTimetableInfo[Depart[lii]]->GetStatus();
    Depart[lii] = 0;
    DepChng[lii] = FALSE;
  }

  // Run through the timetable data looking for first train to depart...
  for (lii = 0; lii < DISP_NUM_DEPART; lii++)
  {
    mintime = 2500;

    for (ljj = 1; ljj < MAX_TIMETABLE; ljj++)
    {
      if (PTimetableInfo[ljj] != 0)
      {
        if ((PTimetableInfo[ljj]->GetDepTime() <= mintime) &&
            (ljj != Depart[0]) &&
            (ljj != Depart[1]) &&
            (ljj != Depart[2]) &&
            (ljj != Depart[3]) &&
            (ljj != Depart[4]) &&
            (ljj != Depart[5]) &&
            (ljj != Depart[6]) &&
            (PTimetableInfo[ljj]->GetDepTime() > 0) &&
            (((PTimetableInfo[ljj]->GetStatus() >= ST_NONE) &&
              (PTimetableInfo[ljj]->GetStatus() <= ST_STARTDEP)) ||
             (PTimetableInfo[ljj]->GetStatus() == ST_TWINASSOC)))
        {
          // Get new minimum value
          mintime = PTimetableInfo[ljj]->GetDepTime();
          Depart[lii] = ljj;
        }
      }
    }
  }

  // Check to see if value has changed since previous time...
  for (lii = 0; lii < DISP_NUM_DEPART; lii++)
  {
    if (Depart[lii] != OldDepart[lii])  DepChng[lii] = TRUE;

    // Also check to see if status has changed...
    if ((Depart[lii] == OldDepart[lii]) && (Depart[lii] > 0))
    {
     if (PTimetableInfo[Depart[lii]]->GetStatus() != OldStatus[lii])  DepChng[lii] = TRUE;
      if (WorkTime == PackTime(PTimetableInfo[Depart[lii]]->GetDepTime())+1)  DepChng[lii] = TRUE;
    }
  }

  return;
}


void TLayout::HandleTracking()
{
  // The purpose of this routine is to "track" trains across the station
  // layout. Each train in the timetable has an associated route field.
  // This field refers to a route in the <Routes> array and is used to
  // determine the tracking. A train is only accepted into this section
  // when it has a status value of 5 or higher.

  int   a, b, i, j, k, Temp[16];
  int   lkk = 0;
  int   ReleaseTime, OldStatus, DepartTime;

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  // SECTION 1:
  // Select trains which are arriving and clear track sections for them
  for (i = 1; i < MAX_TIMETABLE; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      // Check for trains
      if ((PTimetableInfo[i]->GetStatus() >= ST_ARRA) &&
         (PTimetableInfo[i]->GetStatus() <= ST_ARRF))
      {
        PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetClear(PTimetableInfo[i]->GetStatus()-ST_ARRA, a, b);

        if (a > 0)
        {
          PSectionInfo[a]->SetOccupied(FALSE);
          DrawSection(0, a);
        }

        if (b > 0)
        {
          PSectionInfo[b]->SetOccupied(FALSE);
          DrawSection(0, b);
        }

        // Is this the last section?
        if ((PTimetableInfo[i]->GetStatus()-ST_ARRA+1) == (ST_ARRF-ST_ARRA+1))
        {
          if ((PTimetableInfo[i]->GetArrCode() == SC_LIGHTECS) ||
              (PTimetableInfo[i]->GetArrCode() == SC_LIGHTNORM) ||
              (PTimetableInfo[i]->GetArrCode() == SC_LIGHTDELT) ||
              (PTimetableInfo[i]->GetArrCode() == SC_LIGHTRLF) ||
              (PTimetableInfo[i]->GetArrCode() == SC_LIGHT37))
          {
            TRC_NRM((TB, "Last section for arriving train (timetable idx:%d status:%d arrcode:%d)",
                     i,
                     PTimetableInfo[i]->GetStatus(),
                     PTimetableInfo[i]->GetArrCode()));

            // Ensure that the end point is not the locoyard
            if (PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard())
            {
              TRC_NRM((TB, "Loco is headed for the locoyard"));

              // OK, so the loco is headed for the locoyard
              PTimetableInfo[i]->SetStatus(ST_INPLAT);

              // Does the loco need refuelling?
              if (PLocosInfo[PTimetableInfo[i]->GetLoco()]->GetFlag() == LF_NEEDFUEL)
              {
                TRC_NRM((TB, "Loco needs refueling"));

                PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_REFUEL);
                PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetRefuelTime(WorkTime + 80);
              }
              else
              {
                PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_LOCOYARD);
              }

              // Compact the locoyard array
              for (j=0; j<16; j++)  Temp[j] = LocoyardLoco[j];
              for (j=0; j<16; j++)  LocoyardLoco[j] = 0;

              k=0;
              for (j=0; j<16; j++)
              {
                if (Temp[j] > 0)
                {
                  LocoyardLoco[k] = Temp[j];
                  k++;
                }
              }

              // Add a loco to the locoyard array
              if (k<16)
              {
                // Assign the loco to the <LocoyardLoco> array
                LocoyardLoco[k] = PTimetableInfo[i]->GetLoco();
              }
              else
              {
                MessageBox("Internal error number ERR003 detected. Please note carefully\n how this error arose and report it to MDSoft.",
                           APPNAME, MB_ICONSTOP | MB_OK);
              }
            }
            else
            {
              // Handle a light loco
              if (PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable() > 0)
              {
                // There is already a train in the platform
                PTimetableInfo[PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable()]->SetLoco(0, PTimetableInfo[i]->GetLoco());
                PTimetableInfo[PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->GetTimetable()]->SetStatus(ST_STOCKOK);
                PTimetableInfo[i]->SetStatus(ST_STOCKOK);
              }
              else
              {
                // There is not a train in the platform
                PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetLoco(PTimetableInfo[i]->GetLoco());
                PTimetableInfo[i]->SetStatus(ST_STOCKOK);
              }
            }
          }
          else if (PTimetableInfo[i]->GetDelayFlag() == DLF_DELAYS_THRU)
          {
            // This is a through train
            TRC_NRM((TB, "Through train"));

            // Set status to the last valid value.
            PTimetableInfo[i]->SetStatus(ST_DEPF + 1);
          }
          else
          {
            // This is a normal train
            TRC_NRM((TB, "Normal train"));

            PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetTimetable(0, i);
            PTimetableInfo[i]->SetStatus(ST_INPLAT);
            PTimetableInfo[i]->SetRelTime(NormTime(PackTime(PTimetableInfo[i]->GetRelTime()) + WorkTime));

            /****************************************************************/
            /* If the new status is ST_INPLAT, then check to see if there   */
            /* are any associated twin train entries that also need         */
            /* updating.                                                    */
            /****************************************************************/
            for (lkk = 0; lkk < MAX_TIMETABLE; lkk++)
            {
              if ((PTimetableInfo[lkk] != 0) && 
                  (PTimetableInfo[lkk]->GetStatus() == ST_TWINASSOC) &&
                  (PTimetableInfo[lkk]->GetTwinAssoc() == i))
              {
                PTimetableInfo[lkk]->SetStatus(ST_INPLAT);
                PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetTimetable(1, lkk);
                break;
              }
            }

            // The loco needs to be refuelled/maintained
            if ((PTimetableInfo[i]->GetArrCode() == SC_HST) ||
                (PTimetableInfo[i]->GetArrCode() == SC_DELTIC) ||
                (PTimetableInfo[i]->GetArrCode() == SC_NORMAL) ||
                (PTimetableInfo[i]->GetArrCode() == SC_RELIEF) ||
                (PTimetableInfo[i]->GetArrCode() == SC_CLASS37))
            {
              switch (PTimetableInfo[i]->GetDelayFlag())
              {
                case DLF_DELAYS:       
                {
                  /**********************************************************/
                  /* If the Locoyard is enabled then set the "NEEDFUEL"     */
                  /* flag.                                                  */
                  /**********************************************************/
                  if (frame->LocoyardEnabled)
                  {
                    PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_NEEDFUEL);
                  }
                }
                break;

                case DLF_DELAYS_MAINT: 
                {
                  PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_MAINTAIN);
                }
                break;

                default:
                {
                  PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_ASSIGNED);
                }
                break;
              }
            }
          }

          // Check to see if this train is in the departures list
          TRC_NRM((TB, "Check departure status"));
          for (j = 0; j <= 3; j++)
          {
            if (Depart[j] == i) Depart[j] = 0;
          }

          if (PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()] != 0)
          {
            // Update the platforms display window
            PPlatDataInfo[PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef()]->SetRedraw(TRUE);

            // Update platform indicator
            DrawPlatform(0, PSelectorInfo[PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetTo()]->GetPlatRef());
          }
        }
        else
        {
          // Increment status
          PTimetableInfo[i]->SetStatus(PTimetableInfo[i]->GetStatus()+1);
        }
      }
    }
  }

  // SECTION 1B:
  // Handle locos which are tracking across the station
  for (i = 0; i < 10; i++)
  {
    if (PTrackLoco[i] != 0)
    {
      // Check for trains
      if ((PTrackLoco[i]->GetStatus() >= ST_ARRA) &&
          (PTrackLoco[i]->GetStatus() <= ST_ARRF))
      {
        PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetClear(PTrackLoco[i]->GetStatus()-ST_ARRA, a, b);

        if ((PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetFrom() == TSelector::GetHoldPoint()) &&
            (PTrackLoco[i]->GetStatus()-ST_ARRA == 0))
        {
          PSectionInfo[TSelector::GetHoldSection()]->SetOccupied(FALSE);
          DrawSection(0, TSelector::GetHoldSection());
        }

        if (a > 0)
        {
          PSectionInfo[a]->SetOccupied(FALSE);
          DrawSection(0, a);
        }
        if (b > 0)
        {
          PSectionInfo[b]->SetOccupied(FALSE);
          DrawSection(0, b);
        }

        // Is this the last section?
        if ((PTrackLoco[i]->GetStatus()-ST_ARRA+1) == (ST_ARRF-ST_ARRA+1))
        {
          // Check to see if the end point is the hold point or the locoyard
          if ((PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetHoldPoint()) ||
              (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard()))
          {
            if (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetLocoyard())
            {
              // OK, so the loco is headed for the locoyard
              PTrackLoco[i]->SetStatus(ST_INPLAT);

              // Does the loco need refuelling?
              if (PLocosInfo[PTrackLoco[i]->GetLoco()]->GetFlag() == LF_NEEDFUEL)
              {
                PLocosInfo[PTrackLoco[i]->GetLoco()]->SetFlag(LF_REFUEL);
                PLocosInfo[PTrackLoco[i]->GetLoco()]->SetRefuelTime(WorkTime + 80);
              }
              else
              {
                PLocosInfo[PTrackLoco[i]->GetLoco()]->SetFlag(LF_LOCOYARD);
              }

              // Compact the locoyard array
              for (j=0; j<16; j++)  Temp[j] = LocoyardLoco[j];
              for (j=0; j<16; j++)  LocoyardLoco[j] = 0;

              k=0;
              for (j=0; j<16; j++)
              {
                if (Temp[j] > 0)
                {
                  LocoyardLoco[k] = Temp[j];
                  k++;
                }
              }

              // Add a loco to the locoyard array
              if (k<16)
              {
                // Assign the loco to the <LocoyardLoco> array
                LocoyardLoco[k] = PTrackLoco[i]->GetLoco();
              }
              else
              {
                ::MessageBox(HWindow,
                "Internal error number ERR004 detected. Please note carefully\n how this error arose and report it to MDSoft.",
                APPNAME,
                MB_OK);
              }
            }

            if (PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo() == TSelector::GetHoldPoint())
            {
              // Loco is headed for the hold point
              PTrackLoco[i]->SetStatus(ST_INPLAT);
              HoldLoco = PTrackLoco[i]->GetLoco();
              HoldLocoChng = TRUE;
            }
          }
          else
          {
            // Handle a light loco
            j = PSelectorInfo[PRoutesInfo[PTrackLoco[i]->GetRoute()]->GetTo()]->GetPlatRef();
            if (PPlatDataInfo[j] != 0)
            {
              if (PPlatDataInfo[j]->GetTimetable() > 0)
              {
                // There is already a train in the platform
                PTimetableInfo[PPlatDataInfo[j]->GetTimetable()]->SetLoco(0, PTrackLoco[i]->GetLoco());
                PTimetableInfo[PPlatDataInfo[j]->GetTimetable()]->SetStatus(ST_STOCKOK);
                PTrackLoco[i]->SetStatus(ST_STOCKOK);
              }
              else
              {
                // There is not a train in the platform
                PPlatDataInfo[j]->SetLoco(PTrackLoco[i]->GetLoco());
                PTrackLoco[i]->SetStatus(ST_STOCKOK);
              }

              // Update the display
              PPlatDataInfo[j]->SetRedraw(TRUE);
              DrawPlatform(0, j);
            }
            else
            {
              PTrackLoco[i]->SetStatus(ST_STOCKOK);
            }
          }

#if defined(DIAG_SETUP_TRACE)
          // DIAGNOSTIC: Log when releasing PTrackLoco entry
          if (FILE* logFile = OpenDiagnosticsLog())
          {
            fprintf(logFile, "Releasing PTrackLoco[%d]: Loco=%d, Status=%d\n", 
                    i, PTrackLoco[i]->GetLoco(), PTrackLoco[i]->GetStatus());
            fclose(logFile);
          }
#endif

          // Release the loco
          PTrackLoco[i]->SetInitialLocoPlat(0);
        }
        else
        {
          // Increment status
          PTrackLoco[i]->SetStatus(PTrackLoco[i]->GetStatus()+1);
        }
      }
    }
  }


  // SECTION 2:
  // We now check for trains which have reached their release time and those
  // which are candidates for departure
  for (i = 1; i < 50; i++)
  {
    if (PPlatDataInfo[i] != 0)
    {
      // Is there a train in the platform?
      a = PPlatDataInfo[i]->GetTimetable();

      if (a > 0)
      {
        // Store current status flag
        OldStatus = PTimetableInfo[a]->GetStatus();

        // Calculate release time
        ReleaseTime = 120*(PTimetableInfo[a]->GetRelTime()/100);
        ReleaseTime += 2*(PTimetableInfo[a]->GetRelTime() -
                       100*int(PTimetableInfo[a]->GetRelTime()/100));

        if (ReleaseTime < 0)
        {
          ReleaseTime += 2880;
        }

        if ((ReleaseTime <= WorkTime) &&
            (PTimetableInfo[a]->GetStatus() == ST_INPLAT))
        {
          // Time for stock to be released - is the stock an HST or EMU or DMU?
          if ((PTimetableInfo[a]->GetArrCode() == SC_HST) ||
              (PTimetableInfo[a]->GetArrCode() == SC_EMU) ||
              (PTimetableInfo[a]->GetArrCode() == SC_PUSHPULL) ||
              (PTimetableInfo[a]->GetArrCode() == SC_DMU1) ||
              (PTimetableInfo[a]->GetArrCode() == SC_DMU150) ||
              (PTimetableInfo[a]->GetArrCode() == SC_DMU156) ||
              (PTimetableInfo[a]->GetArrCode() == SC_DMU158) ||
              (PTimetableInfo[a]->GetArrCode() == SC_DMU170) ||
              (PTimetableInfo[a]->GetArrCode() == SC_TWIN156) ||
              (PTimetableInfo[a]->GetArrCode() == SC_TWIN158) ||
              (PTimetableInfo[a]->GetArrCode() == SC_TWIN170))
          {
            // Yes it is, so...
            if (PTimetableInfo[a]->GetStatus() == ST_INPLAT)
            {
              PTimetableInfo[a]->SetStatus(ST_STOCKOK);
            }
          }
          else
          {
            // Just a normal train
            if (PTimetableInfo[a]->GetStatus() == ST_INPLAT)
            {
              PTimetableInfo[a]->SetStatus(ST_RELEASE);
            }

            // Swap locomotive about
            PPlatDataInfo[i]->SetLoco(PTimetableInfo[a]->GetLoco());
            PTimetableInfo[a]->SetLoco(0, 0);
          }

          // Update platform information if the status has changed
          if (PTimetableInfo[a]->GetStatus() != OldStatus)
          {
            DrawPlatform(0, i);
            PPlatDataInfo[i]->SetRedraw(TRUE);
          }

          // Check to see if this train is in the departures list
          for (j=0;j<=3;j++)
          {
            if (Depart[j] == i) Depart[j] = 0;
          }
        }

        // Now check to see if the train in this platform can be departed
        if ((PTimetableInfo[a]->GetStatus() == OldStatus) &&
            ((OldStatus == ST_STOCKOK) ||
             (OldStatus == ST_READYDEP)))
        {
          // Calculate departure time
          DepartTime = 120 * (PTimetableInfo[a]->GetDepTime() / 100);
          DepartTime += 2 * (PTimetableInfo[a]->GetDepTime() -
                        100 * int(PTimetableInfo[a]->GetDepTime() / 100));
          if (DepartTime < 0) DepartTime += 2880;

          if ((DepartTime - 6) <= WorkTime)
          {
            // Train is now in a position that it can potentially depart.
            // This change of state changes the platform indicator.
            if (PTimetableInfo[a]->GetStatus() == ST_STOCKOK)
            {
              PTimetableInfo[a]->SetStatus(ST_READYDEP);
              PPlatDataInfo[i]->SetRedraw(TRUE);
              DrawPlatform(0, i);
            }
          }

          if ((DepartTime + 1) == WorkTime)
          {
            PPlatDataInfo[i]->SetRedraw(TRUE);
            DrawPlatform(0, i);
          }

          if (DepartTime <= WorkTime)
          {
            // Check to see if there is a route available
            for (j = 0; j <= 3; j++)
            {
              // Check to see if any of the set routes start at the appropriate
              // section...
              if (RoutesSet[j] > 0)
              {
                if (PRoutesInfo[RoutesSet[j]]->GetFrom() == PPlatDataInfo[i]->GetSelectorRef())
                {
                  // OK - we have a match
                  PTimetableInfo[a]->SetRoute(RoutesSet[j]);
                  if (PTimetableInfo[a]->GetStatus() == ST_READYDEP)  PTimetableInfo[a]->SetStatus(ST_STARTDEP);

                  // Clear the platform flag
                  RoutesSet[j] = 0;
                  PPlatDataInfo[i]->SetTimetable(0);
                  DrawPlatform(0, i);
                  PPlatDataInfo[i]->SetRedraw(TRUE);

                  // Calculate average departure delay
                  if ((PTimetableInfo[a]->GetDepCode() == SC_EMU) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_HST) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_DELTIC) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_NORMAL) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_RELIEF) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_DMU150) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_DMU1) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_PUSHPULL) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_CLASS37) ||
                      (PTimetableInfo[a]->GetDepCode() == SC_HEAVYFREIGHT))
                  {
                    if (PTimetableInfo[a]->GetDepTime() != 0)
                    {
                      DepDelay += WorkTime-PackTime(PTimetableInfo[a]->GetDepTime());
                      DepNum++;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }


  // SECTION 3:
  // Select trains which are departing and clear track sections behind them
  for (i = 1; i < MAX_TIMETABLE; i++)
  {
    if (PTimetableInfo[i] != 0)
    {
      // Check for trains on the UF
      if ((PTimetableInfo[i]->GetStatus() >= ST_DEPA) &&
         (PTimetableInfo[i]->GetStatus() <= ST_DEPF))
      {
        // Firstly update the display
        PRoutesInfo[PTimetableInfo[i]->GetRoute()]->GetClear(PTimetableInfo[i]->GetStatus()-ST_DEPA, a, b);

        if (a > 0)
        {
          PSectionInfo[a]->SetOccupied(FALSE);
          DrawSection(0, a);
        }
        if (b > 0)
        {
          PSectionInfo[b]->SetOccupied(FALSE);
          DrawSection(0, b);
        }

        // Increment status
        PTimetableInfo[i]->SetStatus(PTimetableInfo[i]->GetStatus()+1);

        // Check to see if we have reached the last section
        if ((PTimetableInfo[i]->GetStatus()-ST_DEPA+1) == (ST_DEPF-ST_DEPA+1))
        {
          // If this is an ECS train then we should recover the loco
          if (PTimetableInfo[i]->GetDepCode() == SC_ECS)
          {
            // Recover the loco
            if (PLocosInfo[PTimetableInfo[i]->GetLoco()]->GetType() == SC_ECS)
            {
              PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_UNASSIGN);
            }
          }

          // Alternatively we may want to recover the loco if it is to be reassigned
          if (PTimetableInfo[i]->GetNextTimeTabPos() != 0)
          {
            // Let's recover the loco and allocate it to another train -
            // Now assign it to the next train (if it doesn't already have a loco)
            if (PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->GetLoco() == 0)
            {
              PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->SetLoco(0, PTimetableInfo[i]->GetLoco());
              PLocosInfo[PTimetableInfo[i]->GetLoco()]->SetFlag(LF_ASSIGNED);

              // Update the delay as well if this is not an ECS train 
              if (PTimetableInfo[i]->GetDepTime() != 0)
              {
                PTimetableInfo[PTimetableInfo[i]->GetNextTimeTabPos()]->AddMinLate( ( WorkTime-6-PackTime(PTimetableInfo[i]->GetDepTime()) ) /2);
              }
            }

            PTimetableInfo[i]->SetLoco(0, 0);
          }
        }
      }

      // Check to see if train has just started a departure (this is AFTER the
      // above code for a good reason - it and the ST_STARTDEP state prevent
      // a train clearing the platform section and that immedately preceeding
      // it at the same time when it is departing)
      if (PTimetableInfo[i]->GetStatus() == ST_STARTDEP)  PTimetableInfo[i]->SetStatus(ST_DEPA);
    }

    // Now check through the locos in the locoyard to see if any are refueling
    // and update them
    for (j = 0; j < 16; j++)
    {
      if (LocoyardLoco[j] > 0)
      {
        if ((PLocosInfo[LocoyardLoco[j]]->GetFlag() == LF_REFUEL) &&
            (WorkTime >= PLocosInfo[LocoyardLoco[j]]->GetRefuelTime()))
        {
          // Loco has completed refueling
          PLocosInfo[LocoyardLoco[j]]->SetFlag(LF_LOCOYARD);

          // Update the locoyard window
          if (frame->LocoyardHan != NULL)
          {
            frame->LocoyardHan->UpdateDisplay(0, TRUE);
          }
        }
      }
    }
  }
}


void TLayout::LightLoco(int LocoNum, int ArrPnt, int Route, int Status)
{
  int   i;
  BOOL  NotFound;
#if defined(DIAG_SETUP_TRACE)
  int   UsedCount = 0;
  
  // DIAGNOSTIC: Count current usage of PTrackLoco array
  for (int j = 0; j < 10; j++)
  {
    if (PTrackLoco[j]->GetInitialLocoPlat() != 0) UsedCount++;
  }
  
  // DIAGNOSTIC: Log current array state to file
  if (FILE* logFile = OpenDiagnosticsLog())
  {
    fprintf(logFile, "LightLoco called: Loco=%d, ArrPnt=%d, Route=%d, Status=%d, Usage=%d/10 slots\n", 
            LocoNum, ArrPnt, Route, Status, UsedCount);
    fclose(logFile);
  }
#endif

  i = 0;
  NotFound = TRUE;

  while (NotFound && i < 5)
  {
    // Look for an empty slot
    if (PTrackLoco[i]->GetInitialLocoPlat() == 0)  NotFound = FALSE;
    i++;
  }
  i--;

  // Error message
  if (NotFound)
  {
#if defined(DIAG_SETUP_TRACE)
    // DIAGNOSTIC: Log error to file
    if (FILE* logFile = OpenDiagnosticsLog())
    {
      fprintf(logFile, "ERROR: PTrackLoco overflow! All 5 light loco slots (0-4) occupied. Total usage: %d/10\n", UsedCount);
      fclose(logFile);
    }
#endif
    ::MessageBox(HWindow,
         "Internal error number ERR005 detected. Please note carefully\n how this error arose and report it to MDSoft.",
         APPNAME,
         MB_OK);
    i = 2;
  }

  // Attach a selected loco to a timetable position
  PTrackLoco[i]->SetArrPnt(ArrPnt);
  PTrackLoco[i]->SetRoute(Route);
  PTrackLoco[i]->SetLoco(0, LocoNum);
  PTrackLoco[i]->SetArrCode(SC_LIGHTECS);
  PTrackLoco[i]->SetStatus(Status);
  PTrackLoco[i]->SetInitialLocoPlat(1);
}


int TLayout::NormTime(int GivenTime)
{
  int A, TheTime;

  // This routine takens the value <GivenTime> which is in packed format
  // (ie 2*(60*hrs+minutes)) and expands it into a "normal" time (ie
  // hhmm) and returns this value.

  A = int(GivenTime/1200);                            
  TheTime = 1000*A;                                           // First hour digit
  TheTime += 100*int(GivenTime/120 - 10*A);                   // Second hour digit
             
  A = int((GivenTime - 120*int(GivenTime/120))/20);           
  TheTime += 10*A;                                            // 1st minute digit
  TheTime += ((GivenTime - 120*int(GivenTime/120)) - 20*A)/2; // 2nd minute digit

  return TheTime;
}


int TLayout::PackTime(int GivenTime)
{
  int TheTime;

  // This routine takes a normal time value and returns it as a packed time
  // value (ie 2*(60*hrs+minutes)).  The formula below effectively is 
  // 100 * hours - 40 * hours plus the minutes, all then multiplied by 2.
  TheTime = 2*(GivenTime - 40*int(GivenTime/100));
  return TheTime;
}


int TLayout::CalcArrivalTime(int TimetableNo)
{
  int A, B;

  // This routine, given a timetable number calculates the actual
  // train arrival time

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (frame->DelayEnable)
  {
    // Get delay time and actual time
    A = PTimetableInfo[TimetableNo]->GetMinLate();
    B = PTimetableInfo[TimetableNo]->GetArrTime() + A;

    if (((B - 100*int(B/100)) >= 60) && (A > 0))  B += 40;
    if (((B - 100*int(B/100)) >= 60) && (A < 0))  B -= 40;

    return B;
  }
  else
  {
    // Don't bother taking the delay into account
    A = PTimetableInfo[TimetableNo]->GetArrTime();
    return A;
  }
}


int TLayout::CodeCalculate(char* TextString)
{
  // This routine takes a <TextString> and returns the appropriate ST_XXXX
  // or SC_XXXX code 

  char InternText[100], ErrorText[100];
  int  i, j;

  // Trim the string first of all - trailing blanks...
  i = strlen(TextString) - 1;
  while (i >= 0 && TextString[i] == ' ')  i--;
  TextString[i+1] = '\0';

  // ...and then leading blanks
  i = 0;
  j = 0;
  while ((TextString[i] != '\0') && (TextString[i] == ' '))  i++;
  while (TextString[i] != '\0')
  {
    InternText[j] = TextString[i];
    i++;
    j++;
  }
  InternText[j] = '\0';


  // SC_XXXX codes
  if (_stricmp(InternText, "SC_ECS") == 0)        return SC_ECS;
  if (_stricmp(InternText, "SC_HST") == 0)        return SC_HST;
  if (_stricmp(InternText, "SC_EMU") == 0)        return SC_EMU;
  if (_stricmp(InternText, "SC_DELTIC") == 0)     return SC_DELTIC;
  if (_stricmp(InternText, "SC_NORMAL") == 0)     return SC_NORMAL;
  if (_stricmp(InternText, "SC_RELIEF") == 0)     return SC_RELIEF;
  if (_stricmp(InternText, "SC_LIGHTECS") == 0)   return SC_LIGHTECS;
  if (_stricmp(InternText, "SC_LIGHTNORM") == 0)  return SC_LIGHTNORM;
  if (_stricmp(InternText, "SC_LIGHTDELT") == 0)  return SC_LIGHTDELT;
  if (_stricmp(InternText, "SC_PUSHPULL") == 0)   return SC_PUSHPULL;
  if (_stricmp(InternText, "SC_DMU1") == 0)       return SC_DMU1;
  if (_stricmp(InternText, "SC_DMU150") == 0)     return SC_DMU150;
  if (_stricmp(InternText, "SC_CLASS37") == 0)    return SC_CLASS37;
  if (_stricmp(InternText, "SC_LIGHTRLF") == 0)   return SC_LIGHTRLF;
  if (_stricmp(InternText, "SC_LIGHT37") == 0)    return SC_LIGHT37;

  // ST_XXXX codes
  if (_stricmp(InternText, "ST_NONE") == 0)    return ST_NONE;
  if (_stricmp(InternText, "ST_INPLAT") == 0)  return ST_INPLAT;
  if (_stricmp(InternText, "ST_STOCKOK") == 0)  return ST_STOCKOK;

  CopyString(ErrorText, "Incorrect timetable code error\n\n");
  AppendString(ErrorText, "Arrival description: \n");
 
  ::MessageBox(HWindow, ErrorText, APPNAME, MB_OK);
  return 1;
}

#ifdef MDDEBUG
void TLayout::DebugTrainInfo(char* xiStr, int xiIndex, int xiExpect)
{
  int  lloco = PTimetableInfo[xiExpect]->GetLoco();
  char llocostr[20];

  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  if (lloco == 0)
  {
    CopyString(llocostr, "XXXXX");
  }
  else
  {
    frame->DisplayHan->PLocosInfo[lloco]->LocoNumStr(llocostr);
  }

  TRC_NRM((TB, "EXPECTED (fn:%s) (idx:%d ArrPnt:%d Status:%s Loco:%d '%s')",
           xiStr,
           xiIndex,
           PTimetableInfo[xiExpect]->GetArrPnt(),
           frame->lDbgStates[PTimetableInfo[xiExpect]->GetStatus()],
           lloco,
           llocostr));
}
#endif







DEFINE_RESPONSE_TABLE1( TLayout, TFrameWindow )
    EV_WM_SIZE,
    EV_WM_TIMER,
    EV_COMMAND( 500, HandleSelectors ),
END_RESPONSE_TABLE;



