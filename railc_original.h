/* RAILC.H
*  =======
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

/* Header file for <RailControl> class definitions */

#if !defined(_RAILC_H_)
#define _RAILC_H_

// Define application class
class TManager : public TApplication
{
public:
  TManager(LPSTR AName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
          LPSTR lpCmdLine, int nCmdShow)
	  : TApplication(AName, hInstance, hPrevInstance, lpCmdLine,
	  nCmdShow) {};
  virtual void InitMainWindow();
};


// Define main window class
class TMainWindow;
typedef TMainWindow* PMainWindow;

class TMainWindow : public TWindow
{
private:
  int             SectNum;		       // Section information position
  int             ArrivalX,                    // Arrivals X window position
		  ArrivalY;	               // Arrivals Y window position
  int             DeparturX,                   // Departure X window position
		  DeparturY;	               // Departure Y window position
  int             PlatformX,                   // Platform X window position
		  PlatformY;	               // Platform Y window position
  int             LocoyardX,                   // Locoyard X window position
		  LocoyardY;	               // Locoyard Y window position
  BOOL            ArrivalIcon,                 // Is the arrivals window iconized?
		  DeparturIcon,                // Is the departure window iconized?
                  PlatformIcon,                // Is the platform window iconized?
		  LocoyardIcon,                // Is the locoyard window iconized?
		  MainWinIcon;                 // Is the main window iconized?
  HBRUSH          RedBrush,	               // Handle to a red brush
		  GreenBrush,                  // Handle to a green brush
		  BlueBrush,	               // Handle to a blue brush
		  YellowBrush;                 // Handle to a yellow brush
  HPEN	          RedPen,                      // Handle to a red pen
		  GreenPen,                    // Handle to a green pen
		  YellowPen,                   // Handle to a yellow pen
		  DkGrayPen,                   // Handle to a dark gray pen
		  LtGrayPen;                   // Handle to a light gray pen
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
  HBITMAP	  ToolbarBitmaps;	       // Handle to toolbar bitmaps
  Toolbuttondata  ToolButtData;	               // X positions of toolbuttons
  PTWindowsObject TitleWin;                    // Handle to title window (splash window)

public:
  int             TimerSpeed;                  // What is the current timer speed
  BOOL            SaveOnExit,                  // Do we save on exit?
		  StartOptim,                  // Do we optimize the display on startup?
		  DelayEnable,                 // Are delays enabled?
		  SoundEnable,                 // Is sound enabled?
		  LocoRefuel;                  // Must locos be refuelled?
  char            DataFileName[100];           // Data file name
  PLayout         DisplayHan;		       // Handle to the layout window

  TMainWindow(PTWindowsObject AParent, LPSTR ATitle);
  ~TMainWindow();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow(void);
  virtual BOOL CanClose();
  virtual void QuitProgram();
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
  virtual void WMMenuSelect(RTMessage Msg) = [WM_FIRST + WM_MENUSELECT];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void RedoChildren();
  virtual void CM_FileExit(RTMessage Msg) = [CM_FIRST + CM_MNUFIEXIT]
  {
    CloseWindow();
  }
  virtual void CMMnuFilNew(RTMessage Msg) = [CM_FIRST + CM_MNUFILNEW];
  virtual void CMMnuFiPaus(RTMessage Msg) = [CM_FIRST + CM_MNUFIPAUS];
  virtual void CMMnuFiStop(RTMessage Msg) = [CM_FIRST + CM_MNUFISTOP];
  virtual void CMMnuFiSetD(RTMessage Msg) = [CM_FIRST + CM_MNUFISETD];
  virtual void CMOptOptimi(RTMessage Msg) = [CM_FIRST + CM_OPTOPTIMI];
  virtual void CMOptConfig(RTMessage Msg) = [CM_FIRST + CM_OPTCONFIG];
  virtual void CMWinArriva(RTMessage Msg) = [CM_FIRST + CM_WINARRIVA];
  virtual void CMWinDepart(RTMessage Msg) = [CM_FIRST + CM_WINDEPART];
  virtual void CMWinPlatfo(RTMessage Msg) = [CM_FIRST + CM_WINPLATFO];
  virtual void CMWinLocoya(RTMessage Msg) = [CM_FIRST + CM_WINLOCOYA];
  virtual void CMMnuHeCtnt(RTMessage Msg) = [CM_FIRST + CM_MNUHECTNT];
  virtual void CMMnuHeHelp(RTMessage Msg) = [CM_FIRST + CM_MNUHEHELP];
  virtual void CMMnuHeAbot(RTMessage Msg) = [CM_FIRST + CM_MNUHEABOT];

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
};


#endif // _RAILC_H_