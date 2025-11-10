// OWLCVT 05/11/95 22:34:00
/* RAILC.H
*  =======
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

/* Header file for <RailControl> class definitions */

#if !defined(_RAILC_H_)
#define _RAILC_H_

struct StageTelemetryUpdate;

// Define application class
class TManager : public TApplication
{
public:
  TManager();
  virtual void InitMainWindow();
};


// Define main window class
class TMainWindow;
typedef TMainWindow* PMainWindow;

class TMainWindow : public TFrameWindow
{
private:
  int             SectNum;                     // Section information position
  int             MainWinX,                    // Main window X position
                  MainWinY;                    // Main window Y position
  int             ArrivalX,                    // Arrivals X window position
                  ArrivalY;                    // Arrivals Y window position
  int             DeparturX,                   // Departure X window position
                  DeparturY;                   // Departure Y window position
  int             PlatformX,                   // Platform X window position
                  PlatformY;                   // Platform Y window position
  int             LocoyardX,                   // Locoyard X window position
                  LocoyardY;                   // Locoyard Y window position
  BOOL            ArrivalIcon,                 // Is the arrivals window iconized?
                  DeparturIcon,                // Is the departure window iconized?
                  PlatformIcon,                // Is the platform window iconized?
                  LocoyardIcon,                // Is the locoyard window iconized?
                  MainWinIcon;                 // Is the main window iconized?
  TBrushGuard     RedBrush;                    // Handle to a red brush
  TBrushGuard     GreenBrush;                  // Handle to a green brush
  TBrushGuard     DkGrayBrush;                 // Handle to a dark gray brush
  TBrushGuard     BlueBrush;                   // Handle to a blue brush
  TBrushGuard     YellowBrush;                 // Handle to a yellow brush
  TBrushGuard     WhiteBrush;                  // Handle to a white brush
  TPenGuard       RedPen;                      // Handle to a red pen
  TPenGuard       GreenPen;                    // Handle to a green pen
  TPenGuard       YellowPen;                  // Handle to a yellow pen
  TPenGuard       DkGrayPen;                   // Handle to a dark gray pen
  TPenGuard       LtGrayPen;                   // Handle to a light gray pen
  BOOL            mLayoutInProgress;           // Are we currently adjusting layout?
  BOOL            mOptimizedLayoutActive;      // Should child resizes trigger restacking?
  BOOL            mResizeCoalescePending;      // Is a coalesced resize pending?
  PStatbar        StatbarHan;                  // Handle to the status bar
  PToolbar        ToolbarHan;                  // Handle to the tool bar
  PArrivals       ArrivalHan;                  // Handle to the arrivals window
  PDepartur       DeparturHan;                 // Handle to the departure window
  PPlatform       PlatformHan;                 // Handle to the platform window
  PLocoyard       LocoyardHan;                 // Handle to the locoyard window
  BOOL            DisplayExist,                // Does the layout window exist?
                  StatbarExist,                // Does the status bar exist?
                  ToolbarExist,                // Does the tool bar exist?
                  GamePaused,                  // Is the game paused?
                  GameInProgress;              // Is there a game in progress?
  TBitmapGuard    ToolbarBitmaps;              // Handle to toolbar bitmaps
  Toolbuttondata  ToolButtData;                // X positions of toolbuttons
  TWindow*        TitleWin;                    // Handle to title window (splash window)

public:
  int             TimerSpeed;                  // What is the current timer speed
  BOOL            SaveOnExit,                  // Do we save on exit?
                  StartOptim,                  // Do we optimize the display on startup?
                  DelayEnable,                 // Are delays enabled?
                  SoundEnable,                 // Is sound enabled?
                  LocoRefuel,                  // Must locos be refuelled?
                  LocoyardEnabled;             // Is the locoyard enabled?
  char            DataFileName[100];           // Data file name
  PLayout         DisplayHan;                  // Handle to the layout window
  TFontGuard      mTextFont;                   // Handle to the main window font
  TFontGuard      mHeaderFont;                 // Header font handle
  TFontGuard      mSmallFont;                  // Small font handle

#ifdef MDDEBUG
  char*           lDbgStates[90];
#endif


  TMainWindow(TWindow* AParent, LPCTSTR ATitle);
  ~TMainWindow();
  void GetWindowClass(WNDCLASS& WndClass);
  auto GetWindowClassName() -> owl::TWindowClassName override;
  void SetupWindow(void);
  virtual bool CanClose();
  void QuitProgram();
  void EvSize(owl::uint sizeType, const TSize& size);
  void EvMove(const TPoint &clientOrigin);
  void EvMenuSelect(owl::uint menuId, owl::uint flags, HMENU);
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
  void RedoChildren();
  void CM_FileExit();
  void CMMnuFilNew();
  void CMMnuFiPaus();
  void CMMnuFiStop();
  void CMMnuFiSetD();
  void CMOptOptimi();
  void CMOptConfig();
  void CMWinArriva();
  void CMWinDepart();
  void CMWinPlatfo();
  void CMWinLocoya();
  void CMMnuHeCtnt();
  void CMMnuHeHelp();
  void CMMnuHeAbot();
  void HandleChildResize(TWindow* child, const RECT& oldRect, const RECT& newRect);
  void QueueStageTelemetryUpdate(const StageTelemetryUpdate& update);
  void EvTimer(owl::uint timerId);

  // Define friend classes
  friend class TLayout;
  friend class TSelector;
  friend class TArrivals;
  friend class TDepartur;
  friend class TPlatform;
  friend class TLocoyard;
  friend class TToolbutt;
  friend class TToolbar;
  friend class TStatbar;

  DECLARE_RESPONSE_TABLE(TMainWindow);
};

#endif // _RAILC_H_
