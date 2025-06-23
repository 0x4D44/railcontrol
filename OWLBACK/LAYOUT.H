/* LAYOUT.H
*  ========
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

/* Header file for Class definitions */

#if !defined(_LAYOUT_H_)
#define _LAYOUT_H_


//  Define layout window class
class TLayout;
typedef TLayout* PLayout;

class TLayout : public TWindow
{
private:
  PSection       PSectionInfo[1000];          // Pointers to section information
  PTimetable     PTrackLoco[10];              // Pointers to loco track information
  int            TimeNum;                     // Total number of timetable items
  char           StockCode[5][20];            // Stock code translation array
  BOOL           SelectOut[NOSELECTORS+1];    // Selector positions up/down
  HBITMAP        HTimeSym;		      // Handle to the digital time symbols
  int            SkipCount;                   // Count of timer messages

  int            RoutesSet[5];                // Which routes (if any) are currently set?
  int            Selector1;                   // The number of the first selector selected
  int            Selector2;                   // The number of the second selector selected
  int            TrackLocoNum;                // Number of locos which are tracking through the station
  int            OldArrDelay, OldDepDelay,    // Old arrival/dep delay (total)
		 OldArrNum, OldDepNum;        // Old arrival/dep number
  int            OldWorkTime,                 // Old work time
		 StopTime;                    // Stop time


public:
  BOOL           ExpChng[4];                  // Have the <Expect> values changed since the last time period?
  int            Expect[4];                   // Timetable numbers of next 4 expected trains
  BOOL           DepChng[4];                  // Have the <Depart> values changed since the last time period?
  int            Depart[4];                   // Timetable numbers of next 4 departures

  PTimetable     PTimetableInfo[200];         // Handles to timetable information
  PPlatData      PPlatDataInfo[50];           // Handles to platform information
  PLocos         PLocosInfo[500];             // Handles to loco information
  PRoutes        PRoutesInfo[1000];           // Pointers to routes information
  PSelector      PSelectorInfo[50];           // Pointers to selector windows

  int            LocoyardLoco[16];            // Locos that are in the locoyard
  int            WorkTime,                    // Current work time
		 ShiftLength;                 // Shift length (in hours)
  int            ArrDelay, DepDelay,          // Arrival/dep delay (total)
		 ArrNum, DepNum;              // Arrival/dep number
  int            HoldLoco;                    // Loco held at hold point
  BOOL           HoldLocoChng;                // Has the hold loco changed?
  char           StartText[3][100];           // Start text descriptions



  TLayout::TLayout(PTWindowsObject AParent, int XPos, int YPos, int Width, int Height);
  ~TLayout();
  virtual void ClearData();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow(void);
  virtual BOOL CanClose();
  virtual void QuitProgram();
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
  virtual void WMTimer(RTMessage Msg) = [WM_FIRST + WM_TIMER];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void UpdateDisplay(HDC TheDC);
  virtual void IniVariables();
  virtual BOOL StartNew();
  virtual void HandleTimeChange();
  virtual void DrawClock(HDC TheDC, BOOL Redraw);
  virtual void DrawSection(HDC TheDC, int SectNum);
  virtual void DrawPlatform(HDC TheDC, int PlatNum);
  virtual void SizeSelectors();
  virtual void HandleSelectors(RTMessage Msg) = [CM_FIRST + CM_SELECT];
  virtual void UpdateSelectors();
  virtual BOOL IsRouteValid(int Start, int End);
  virtual BOOL ReadDataFile();
  virtual void AssignYardLoco(int LocoFlag, int RefuelTime);
  virtual BOOL AssignLoco(int TimetabPos);
  virtual void GetExpecteds();
  virtual void TimeCheck(int GivArrPnt);
  virtual void GetDepartures();
  virtual void HandleTracking();
  virtual void LightLoco(int LocoNum, int ArrPnt, int Route, int Status);
  virtual int  NormTime(int GivenTime);
  virtual int  PackTime(int GivenTime);
  virtual int  CalcArrivalTime(int TimetableNo);
  virtual int  CodeCalculate(char* TextString);
};


#endif // _LAYOUT_H_