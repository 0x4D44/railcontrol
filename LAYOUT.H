// OWLCVT 05/11/95 22:34:00
/* LAYOUT.H
*  ========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.1
*  Language : C++ (BORLAND v3.1)
*
*/

/* Header file for Class definitions */

#if !defined(_LAYOUT_H_)
#define _LAYOUT_H_

#define FILE_INPUT_BUFSIZE  400
#define MAX_TIMETABLE       500

//  Define layout window class
class TLayout;
typedef TLayout* PLayout;

class TLayout : public TFrameWindow
{
private:
  PSection       PSectionInfo[1000];              // Pointers to section information
  PTimetable     PTrackLoco[10];                  // Pointers to loco track information
  int            TimeNum;                         // Total number of timetable items
  char           StockCode[5][20];                // Stock code translation array
  BOOL           SelectOut[NOSELECTORS+1];        // Selector positions up/down
  HBITMAP        HTimeSym;                        // Handle to the digital time symbols
  int            SkipCount;                       // Count of timer messages

  int            RoutesSet[5];                    // Which routes (if any) are currently set?
  int            Selector1;                       // The number of the first selector selected
  int            Selector2;                       // The number of the second selector selected
  int            TrackLocoNum;                    // Number of locos which are tracking through the station
  float          mOldAvArrDelay, mOldAvDepDelay;  // Old average arrival/dep delays
  int            OldWorkTime,                     // Old work time
                 StopTime;                        // Stop time
  BOOL           mDrawSectionNumber;              // Should the section number be drawn on top of the section?
  BOOL           mDrawSelectorNumber;             // Should the actual selector number be drawn (rather than the textual name)?


public:
  BOOL           ExpChng[DISP_NUM_ARRIVAL];       // Have the <Expect> values changed since the last time period?
  int            Expect[DISP_NUM_ARRIVAL];        // Timetable numbers of next n expected trains
  BOOL           DepChng[DISP_NUM_DEPART];        // Have the <Depart> values changed since the last time period?
  int            Depart[DISP_NUM_DEPART];         // Timetable numbers of next n departures

  PTimetable     PTimetableInfo[MAX_TIMETABLE];   // Handles to timetable information
  PPlatData      PPlatDataInfo[50];               // Handles to platform information
  POverlapData   POverlapDataInfo[50];            // Handles to overlapping information
  PLocos         PLocosInfo[500];                 // Handles to loco information
  PRoutes        PRoutesInfo[1000];               // Pointers to routes information
  PSelector      PSelectorInfo[50];               // Pointers to selector windows

  int            LocoyardLoco[16];                // Locos that are in the locoyard
  int            WorkTime,                        // Current work time
                 ShiftLength;                     // Shift length (in hours)
  int            ArrDelay, DepDelay,              // Arrival/dep delay (total)
                 ArrNum, DepNum;                  // Arrival/dep number
  int            HoldLoco;                        // Loco held at hold point
  BOOL           HoldLocoChng;                    // Has the hold loco changed?
  char           StartText[3][100];               // Start text descriptions



  TLayout(owl::TWindow * AParent, int XPos, int YPos, int Width, int Height);
  ~TLayout();
  void ClearData();
  void GetWindowClass(WNDCLASS& WndClass);
  auto GetWindowClassName() -> owl::TWindowClassName override;
  void SetupWindow(void);
  virtual bool CanClose();
  void QuitProgram();
  void EvSize(owl::uint sizeType, const TSize& size);
  void EvTimer(owl::uint timerId);
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
  void UpdateDisplay(HDC TheDC);
  void IniVariables();
  BOOL StartNew();
  void HandleTimeChange();
  void DrawClock(HDC TheDC, BOOL xiRedraw);
  void DrawDelayBox(RECT xiRect, float xiDelay);
  void DrawSection(HDC xiDC, int xiSectNum);
  void DrawPlatform(HDC TheDC, int PlatNum);
  void SizeSelectors();
  void HandleSelectors();
  void UpdateSelectors();
  BOOL IsRouteValid(int Start, int End);
  BOOL ReadDataFile();
  void AssignYardLoco(int LocoFlag, int RefuelTime);
  BOOL AssignLoco(int TimetabPos);
  void GetExpecteds();
  void TimeCheck(int GivArrPnt);
  void GetDepartures();
  void HandleTracking();
  void LightLoco(int LocoNum, int ArrPnt, int Route, int Status);
  int  NormTime(int GivenTime);
  int  PackTime(int GivenTime);
  int  CalcArrivalTime(int TimetableNo);
  int  CodeCalculate(char* TextString);
#ifdef MDDEBUG
  void DebugTrainInfo(char* xiStr, int xiIndex, int xiExpect);
#endif


  DECLARE_RESPONSE_TABLE(TLayout);
};


#endif // _LAYOUT_H_
