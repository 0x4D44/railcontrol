// OWLCVT 05/11/95 22:34:01
/* RAILC.CPP
*  =========
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

//#include "owl\compat.h"
#include "classdef.h"

/*************************************************************************/
/* Methods of TManager follow...*/


void TManager::InitMainWindow()
{
  MainWindow = new TMainWindow(NULL, Name);
}


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPSTR lpCmdLine, int nCmdShow)
{
  // Load the 3D custom controls
  Ctl3dRegister(hInstance);
  Ctl3dAutoSubclass(hInstance);

  TManager Manager(APPNAME, hInstance, hPrevInstance,
                 lpCmdLine, nCmdShow);
  Manager.Run();

  // Unload the 3D custom controls
  Ctl3dUnregister(hInstance);
  return Manager.Status;
}


/*************************************************************************/
/* Methods of TMainWindow follow...*/


TMainWindow::TMainWindow(TWindow * AParent, LPSTR ATitle)
  : TFrameWindow(AParent, ATitle)
{
  LOGFONT    lFont = {0}; 

#ifdef MDDEBUG
  // Initialize debug strings.
  lDbgStates[0] = "ST_NONE";
  lDbgStates[1] = "ST_DUE";
  lDbgStates[2] = "ST_APPROACH";
  lDbgStates[3] = "ST_HELD";
  lDbgStates[4] = "ST_HOLBY";
  lDbgStates[5] = "ST_SETPLAT";
  lDbgStates[6] = "ST_ARRA";
  lDbgStates[7] = "ST_ARRB";
  lDbgStates[8] = "ST_ARRC";
  lDbgStates[9] = "ST_ARRD";
  lDbgStates[10] = "ST_ARRE";
  lDbgStates[11] = "ST_ARRF";
  lDbgStates[12] = "ST_INPLAT";
  lDbgStates[13] = "ST_RELEASE";
  lDbgStates[14] = "ST_STOCKOK";
  lDbgStates[15] = "ST_READYDEP";
  lDbgStates[16] = "ST_STARTDEP";
  lDbgStates[17] = "ST_DEPA";
  lDbgStates[18] = "ST_DEPB";
  lDbgStates[19] = "ST_DEPC";
  lDbgStates[20] = "ST_DEPD";
  lDbgStates[21] = "ST_DEPE";
  lDbgStates[22] = "ST_DEPF";
  lDbgStates[30] = "ST_TWINASSOC";
#endif

  // Display startup title window
  TitleWin = GetApplication()->MakeWindow(new TStartup(this));
  ::ShowWindow(TitleWin->HWindow, SW_SHOW);

  TRC_NRM((TB, "RAIL CONTROL STARTING"));
  TRC_NRM((TB, "====================="));

  // Assign menu to the window
  AssignMenu("MAIN_MENU");

  // Window attributes
  Attr.X = GetPrivateProfileInt("Main Window", "X position", 0, INIFILENAME);
  Attr.Y = GetPrivateProfileInt("Main Window", "Y position", 0, INIFILENAME);
  Attr.W = GetPrivateProfileInt("Main Window", "Width", 600, INIFILENAME);
  Attr.H = GetPrivateProfileInt("Main Window", "Height", 400, INIFILENAME);
  MainWinX = Attr.X;
  MainWinY = Attr.Y;

  // Reset pointers
  ToolbarHan = NULL;
  DisplayHan = NULL;
  StatbarHan = NULL;

  // Note that all brushes/pens etc are created in the main application window
  // (ie this one) and all child windows use the resources stored here.

  // Create brushes
  RedBrush    = CreateSolidBrush(RGB(255, 0, 0));
  GreenBrush  = CreateSolidBrush(RGB(0, 255, 0));
  DkGrayBrush = CreateSolidBrush(RGB(128, 128, 128));
  BlueBrush   = CreateSolidBrush(RGB(0, 128, 255));
  YellowBrush = CreateSolidBrush(RGB(255, 255, 0));
  WhiteBrush  = CreateSolidBrush(RGB(255, 255, 255));

  // Create pens
  RedPen    = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
  GreenPen  = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
  YellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
  DkGrayPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
  LtGrayPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));

  // Text font record
  lFont.lfHeight = 16;
  lFont.lfWidth = 0;
  lFont.lfEscapement = 0;
  lFont.lfOrientation = 0;
  lFont.lfWeight = FW_NORMAL;
  lFont.lfItalic = 0;
  lFont.lfUnderline = 0;
  lFont.lfStrikeOut = 0;
  lFont.lfCharSet = ANSI_CHARSET;
  lFont.lfOutPrecision = OUT_STROKE_PRECIS;
  lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lFont.lfQuality = PROOF_QUALITY;
  lFont.lfPitchAndFamily = FF_DONTCARE;
  strcpy(lFont.lfFaceName, "Arial");
  mTextFont = CreateFontIndirect(&lFont);

   // Header font record
  lFont.lfHeight = 14;
  lFont.lfWidth = 0;
  lFont.lfEscapement = 0;
  lFont.lfOrientation = 0;
  lFont.lfWeight = FW_BOLD;
  lFont.lfItalic = 0;
  lFont.lfUnderline = 0;
  lFont.lfStrikeOut = 0;
  lFont.lfCharSet = ANSI_CHARSET;
  lFont.lfOutPrecision = OUT_STROKE_PRECIS;
  lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lFont.lfQuality = PROOF_QUALITY;
  lFont.lfPitchAndFamily = FF_DONTCARE;
  strcpy(lFont.lfFaceName, "Arial");
  mHeaderFont = CreateFontIndirect(&lFont);

   // Small font record
  lFont.lfHeight = 12;
  lFont.lfWidth = 0;
  lFont.lfEscapement = 0;
  lFont.lfOrientation = 0;
  lFont.lfWeight = FW_BOLD;
  lFont.lfItalic = 0;
  lFont.lfUnderline = 0;
  lFont.lfStrikeOut = 0;
  lFont.lfCharSet = ANSI_CHARSET;
  lFont.lfOutPrecision = OUT_STROKE_PRECIS;
  lFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  lFont.lfQuality = PROOF_QUALITY;
  lFont.lfPitchAndFamily = FF_DONTCARE;
  strcpy(lFont.lfFaceName, "Arial");
  mSmallFont = CreateFontIndirect(&lFont);

  // Initialize variables
  ArrivalHan = NULL;
  DeparturHan = NULL;
  PlatformHan = NULL;
  LocoyardHan = NULL;
  MainWinIcon = FALSE;
  GamePaused = FALSE;
  GameInProgress = FALSE;
  LocoyardEnabled = TRUE;
}


TMainWindow::~TMainWindow()
{
  // Delete brushes etc
  DeleteObject(RedBrush);
  DeleteObject(GreenBrush);
  DeleteObject(DkGrayBrush);
  DeleteObject(BlueBrush);
  DeleteObject(YellowBrush);
  DeleteObject(WhiteBrush);
  DeleteObject(RedPen);
  DeleteObject(GreenPen);
  DeleteObject(YellowPen);
  DeleteObject(DkGrayPen);
  DeleteObject(LtGrayPen);
  DeleteObject(ToolbarBitmaps);
  DeleteObject(mTextFont);
  DeleteObject(mHeaderFont);
  DeleteObject(mSmallFont);
}


void TMainWindow::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hIcon = LoadIcon(*GetApplication(), "AMAIN");
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


LPSTR TMainWindow::GetClassName()
{
  return "Main_Window_Class";
}


void TMainWindow::SetupWindow()
{
  RECT  WinRect;
  int   YPos, Height, Width;
  HMENU TheMenu;

  // Call ancestor method
  TFrameWindow::SetupWindow();

  // Get client window size
  ::GetClientRect(HWindow, &WinRect);

  // Create status bar
  StatbarHan = new TStatbar(this);
  GetApplication()->MakeWindow(StatbarHan);
  StatbarExist = TRUE;

  /**************************************************************************/
  /* Get the menu handle.                                                   */
  /**************************************************************************/
  TheMenu = ::GetMenu(HWindow);

  // Set up the tool button X positions
  ToolButtData.XPos[1] = 10;
  ToolButtData.WParam[1] = CM_MNUFILNEW;
  ToolButtData.LParam[1] = 0;
  ToolButtData.SentWindow[1] = HWindow;
  ToolButtData.StatWindow[1] = StatbarHan->HWindow;
  strcpy(ToolButtData.UpText[1], "");
  strcpy(ToolButtData.DownText[1], "Start a new session");

  ToolButtData.XPos[2] = 34;
  ToolButtData.WParam[2] = CM_MNUFIPAUS;
  ToolButtData.LParam[2] = 0;
  ToolButtData.SentWindow[2] = HWindow;
  ToolButtData.StatWindow[2] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[2], "");
  strcpy(ToolButtData.DownText[2], "Restart the current session");

  ToolButtData.XPos[3] = 34;
  ToolButtData.WParam[3] = CM_MNUFIPAUS;
  ToolButtData.LParam[3] = 0;
  ToolButtData.SentWindow[3] = HWindow;
  ToolButtData.StatWindow[3] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[3], "");
  strcpy(ToolButtData.DownText[3], "Pause the current session");

  ToolButtData.XPos[4] = 58;
  ToolButtData.WParam[4] = CM_MNUFISTOP;
  ToolButtData.LParam[4] = 0;
  ToolButtData.SentWindow[4] = HWindow;
  ToolButtData.StatWindow[4] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[4], "");
  strcpy(ToolButtData.DownText[4], "Stop the current session");

  ToolButtData.XPos[5] = 90;
  ToolButtData.WParam[5] = CM_OPTOPTIMI;
  ToolButtData.LParam[5] = 0;
  ToolButtData.SentWindow[5] = HWindow;
  ToolButtData.StatWindow[5] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[5], "");
  strcpy(ToolButtData.DownText[5], "Optimize display of windows");

  ToolButtData.XPos[6] = 114;
  ToolButtData.WParam[6] = CM_OPTCONFIG;
  ToolButtData.LParam[6] = 0;
  ToolButtData.SentWindow[6] = HWindow;
  ToolButtData.StatWindow[6] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[6], "");
  strcpy(ToolButtData.DownText[6], "Set program configuration");

  ToolButtData.XPos[7] = 146;
  ToolButtData.WParam[7] = CM_WINARRIVA;
  ToolButtData.LParam[7] = 0;
  ToolButtData.SentWindow[7] = HWindow;
  ToolButtData.StatWindow[7] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[7], "");
  strcpy(ToolButtData.DownText[7], "Display/hide the Arrivals window");

  ToolButtData.XPos[8] = 170;
  ToolButtData.WParam[8] = CM_WINDEPART;
  ToolButtData.LParam[8] = 0;
  ToolButtData.SentWindow[8] = HWindow;
  ToolButtData.StatWindow[8] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[8], "");
  strcpy(ToolButtData.DownText[8], "Display/hide the Departures window");

  ToolButtData.XPos[9] = 194;
  ToolButtData.WParam[9] = CM_WINPLATFO;
  ToolButtData.LParam[9] = 0;
  ToolButtData.SentWindow[9] = HWindow;
  ToolButtData.StatWindow[9] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[9], "");
  strcpy(ToolButtData.DownText[9], "Display/hide the Platforms window");

  ToolButtData.XPos[10] = 218;
  ToolButtData.WParam[10] = CM_WINLOCOYA;
  ToolButtData.LParam[10] = 0;
  ToolButtData.SentWindow[10] = HWindow;
  ToolButtData.StatWindow[10] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[10], "");
  strcpy(ToolButtData.DownText[10], "Display/hide the Locoyard window");

  ToolButtData.XPos[11] = 250;
  ToolButtData.WParam[11] = CM_MNUHECTNT;
  ToolButtData.LParam[11] = 0;
  ToolButtData.SentWindow[11] = HWindow;
  ToolButtData.StatWindow[11] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[11], "");
  strcpy(ToolButtData.DownText[11], "Display the help file contents page");

  ToolButtData.XPos[12] = 274;
  ToolButtData.WParam[12] = CM_MNUHEABOT;
  ToolButtData.LParam[12] = 0;
  ToolButtData.SentWindow[12] = HWindow;
  ToolButtData.StatWindow[12] = PStatbar(StatbarHan)->HWindow;
  strcpy(ToolButtData.UpText[12], "");
  strcpy(ToolButtData.DownText[12], "Display information about Rail Control");

  // Load toolbar button bitmaps
  ToolbarBitmaps = LoadBitmap(*GetApplication(), "TB_MAINWIN");

  // Then create the toolbar
  ToolbarHan = new TToolbar(this, 12, ToolButtData, ToolbarBitmaps);
  GetApplication()->MakeWindow(ToolbarHan);
  ToolbarExist = TRUE;

  // Modify the buttons - hide button 2
  ToolbarHan->PToolbutt[2]->Enable(FALSE);
  ToolbarHan->PToolbutt[3]->Enable(FALSE);
  ToolbarHan->PToolbutt[4]->Enable(FALSE);
  ::ShowWindow(ToolbarHan->PToolbutt[2]->HWindow, FALSE);

  // Create the main display window
  if (ToolbarExist)
  {
    YPos = 30;
  }
  else
  {
    YPos = 0;
  }

  Height = (WinRect.bottom-WinRect.top);
  if (ToolbarExist) Height -= 30;
  if (StatbarExist) Height -= 22;

  Width = (WinRect.right-WinRect.left);

  DisplayHan = new TLayout(this, 0, YPos, Width, Height);
  GetApplication()->MakeWindow(DisplayHan);
  DisplayExist = TRUE;

  // Read initialisation file
  SaveOnExit = BOOL(GetPrivateProfileInt("Main Window", "Save on exit", 1, INIFILENAME));
  StartOptim = BOOL(GetPrivateProfileInt("Main Window", "Optimize on start", 1, INIFILENAME));
  DelayEnable = BOOL(GetPrivateProfileInt("Main Window", "Enable delay", 1, INIFILENAME));
  SoundEnable = BOOL(GetPrivateProfileInt("Main Window", "Enable sound", 1, INIFILENAME));
  LocoRefuel = BOOL(GetPrivateProfileInt("Main Window", "Loco refuel", 1, INIFILENAME));
  TimerSpeed = GetPrivateProfileInt("Main Window", "Timer speed", 2, INIFILENAME);
  GetPrivateProfileString("Main Window", "Data file name", "", DataFileName, 100, INIFILENAME);
  ArrivalX = GetPrivateProfileInt("Arrival Window", "X position", 0, INIFILENAME);
  ArrivalY = GetPrivateProfileInt("Arrival Window", "Y position", 0, INIFILENAME);
  DeparturX = GetPrivateProfileInt("Departure Window", "X position", 0, INIFILENAME);
  DeparturY = GetPrivateProfileInt("Departure Window", "Y position", 0, INIFILENAME);
  PlatformX = GetPrivateProfileInt("Platform Window", "X position", 0, INIFILENAME);
  PlatformY = GetPrivateProfileInt("Platform Window", "Y position", 0, INIFILENAME);
  LocoyardX = GetPrivateProfileInt("Locoyard Window", "X position", 0, INIFILENAME);
  LocoyardY = GetPrivateProfileInt("Locoyard Window", "Y position", 0, INIFILENAME);

  // Check to see if we need to create an arrivals window
  if (BOOL(GetPrivateProfileInt("Arrival Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new arrivals window
    CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_UNCHECKED);
    CMWinArriva();
  }

  // Check to see if we need to create a departure window
  if (BOOL(GetPrivateProfileInt("Departure Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new departure window
    CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_UNCHECKED);
    CMWinDepart();
  }

  // Check to see if we need to create a platform window
  if (BOOL(GetPrivateProfileInt("Platform Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new platform window
    CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_UNCHECKED);
    CMWinPlatfo();
  }

  // Check to see if we need to create a locoyard window
  if (BOOL(GetPrivateProfileInt("Locoyard Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new locoyard window
    CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_UNCHECKED);
    CMWinLocoya();
  }

  // Check to see if we should optimize the display
  if (StartOptim)
  {
    ::SendMessage(HWindow, WM_COMMAND, CM_OPTOPTIMI, 0);
  }

  // Disable the menu items
  EnableMenuItem(TheMenu, CM_MNUFIPAUS, MF_GRAYED);
  EnableMenuItem(TheMenu, CM_MNUFISTOP, MF_GRAYED);

  // Close the title window
  TitleWin->CloseWindow();
}


BOOL TMainWindow::CanClose()
{
  // Call file saving routine
  QuitProgram();

  // Call ancestor method
  TFrameWindow::CanClose();

  // Pass return value
  return TRUE;
}


void TMainWindow::QuitProgram()
{
  char  TextString[50];

  // This is the intialization file saving routine
  if (SaveOnExit)
  {
    // Main window size/position
    itoa(Attr.X, TextString, 10);
    WritePrivateProfileString("Main Window", "X position", TextString, INIFILENAME);
    itoa(Attr.Y, TextString, 10);
    WritePrivateProfileString("Main Window", "Y position", TextString, INIFILENAME);
    itoa(Attr.W, TextString, 10);
    WritePrivateProfileString("Main Window", "Width", TextString, INIFILENAME);
    itoa(Attr.H, TextString, 10);
    WritePrivateProfileString("Main Window","Height", TextString, INIFILENAME);

    // Arrival window details
    if (ArrivalHan != NULL)
    {
      WritePrivateProfileString("Arrival Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Arrival Window", "Exists", "0", INIFILENAME);
    }

    itoa(ArrivalX, TextString, 10);
    WritePrivateProfileString("Arrival Window","X position", TextString, INIFILENAME);
    itoa(ArrivalY, TextString, 10);
    WritePrivateProfileString("Arrival Window","Y position", TextString, INIFILENAME);

    // Departure window details
    if (DeparturHan != NULL)
    {
      WritePrivateProfileString("Departure Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Departure Window", "Exists", "0", INIFILENAME);
    }

    itoa(DeparturX, TextString, 10);
    WritePrivateProfileString("Departure Window","X position", TextString, INIFILENAME);
    itoa(DeparturY, TextString, 10);
    WritePrivateProfileString("Departure Window","Y position", TextString, INIFILENAME);

    // Platform window details
    if (PlatformHan != NULL)
    {
      WritePrivateProfileString("Platform Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Platform Window", "Exists", "0", INIFILENAME);
    }

    itoa(PlatformX, TextString, 10);
    WritePrivateProfileString("Platform Window","X position", TextString, INIFILENAME);
    itoa(PlatformY, TextString, 10);
    WritePrivateProfileString("Platform Window","Y position", TextString, INIFILENAME);

    // Locoyard window details
    if (LocoyardHan != NULL)
    {
      WritePrivateProfileString("Locoyard Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Locoyard Window", "Exists", "0", INIFILENAME);
    }

    itoa(LocoyardX, TextString, 10);
    WritePrivateProfileString("Locoyard Window","X position", TextString, INIFILENAME);
    itoa(LocoyardY, TextString, 10);
    WritePrivateProfileString("Locoyard Window","Y position", TextString, INIFILENAME);

    // Other information
    itoa(int(StartOptim), TextString, 10);
    WritePrivateProfileString("Main Window", "Optimize on start", TextString, INIFILENAME);
    itoa(int(DelayEnable), TextString, 10);
    WritePrivateProfileString("Main Window", "Enable delay", TextString, INIFILENAME);
    itoa(int(SoundEnable), TextString, 10);
    WritePrivateProfileString("Main Window", "Enable sound", TextString, INIFILENAME);
    itoa(int(LocoRefuel), TextString, 10);
    WritePrivateProfileString("Main Window", "Loco refuel", TextString, INIFILENAME);
    itoa(TimerSpeed, TextString, 10);
    WritePrivateProfileString("Main Window", "Timer speed", TextString, INIFILENAME);
    WritePrivateProfileString("Main Window", "Data file name", DataFileName, INIFILENAME);

    // Yes, we did save the options on exit
    WritePrivateProfileString("Main Window", "Save on exit", "1", INIFILENAME);
  }
  else
  {
    WritePrivateProfileString("Main Window", "Save on exit", "0", INIFILENAME);
  }
}


void TMainWindow::EvSize(UINT sizeType, TSize& size)
{
  // Call ancestor method - update window
  TFrameWindow::EvSize(sizeType, size);
  Invalidate(TRUE);

  // Redraw children
  RedoChildren();

  // If this window has just been maximized then restore the sub-windows to their
  // original state
  if (!::IsIconic(HWindow) && MainWinIcon)
  {
    // If the Arrival window exists...
    if (ArrivalHan != NULL)
    {
      if (ArrivalIcon)
      {
        ArrivalHan->Show(SW_SHOWMINIMIZED);
      }
      else
      {
        ArrivalHan->Show(SW_SHOWNORMAL);
      }
   }

   // If the Departure window exists...
   if (DeparturHan != NULL)
   {
     if (DeparturIcon)
     {
       PDepartur(DeparturHan)->Show(SW_SHOWMINIMIZED);
     }
     else
     {
       PDepartur(DeparturHan)->Show(SW_SHOWNORMAL);
     }
   }

   // If the Platform window exists...
   if (PlatformHan != NULL)
   {
    if (PlatformIcon)
    {
      PPlatform(PlatformHan)->Show(SW_SHOWMINIMIZED);
    }
    else
    {
      PPlatform(PlatformHan)->Show(SW_SHOWNORMAL);
    }
   }

   // If the Locoyard window exists...
   if (LocoyardHan != NULL)
   {
    if (LocoyardIcon)
    {
      PLocoyard(LocoyardHan)->Show(SW_SHOWMINIMIZED);
    }
    else
    {
      PLocoyard(LocoyardHan)->Show(SW_SHOWNORMAL);
    }
   }
  }

  // Get current windows state
  MainWinIcon = ::IsIconic(HWindow);
}

void TMainWindow::EvMove(TPoint &clientOrigin)
{
  int   OffsetX, OffsetY;
  RECT  lWinRect;

  // Call ancestor method - update window
  TFrameWindow::EvMove(clientOrigin);

  /**************************************************************************/
  /* Work out the offset.                                                   */
  /**************************************************************************/
  ::GetWindowRect(HWindow, &lWinRect);
  OffsetX  = lWinRect.left - MainWinX;
  OffsetY  = lWinRect.top - MainWinY;
  MainWinX = lWinRect.left;
  MainWinY = lWinRect.top;

  /**************************************************************************/
  /* Move the child windows as well.                                        */
  /**************************************************************************/
  ArrivalX += OffsetX;
  ArrivalY += OffsetY;
  if (ArrivalHan != NULL)
  {
    ::MoveWindow(ArrivalHan->HWindow, ArrivalX, ArrivalY, ArrivalHan->Attr.W, ArrivalHan->Attr.H, TRUE);
  }

  // Move departure window
  DeparturX += OffsetX;
  DeparturY += OffsetY;
  if (DeparturHan != NULL)
  {
    ::MoveWindow(DeparturHan->HWindow, DeparturX, DeparturY, DeparturHan->Attr.W, DeparturHan->Attr.H, TRUE);
  }

  // Move platform window
  PlatformX += OffsetX;
  PlatformY += OffsetY;
  if (PlatformHan != NULL)
  {
    ::MoveWindow(PlatformHan->HWindow, PlatformX, PlatformY, PlatformHan->Attr.W, PlatformHan->Attr.H, TRUE);
  }

  // Move locoyard window
  LocoyardX += OffsetX;
  LocoyardY += OffsetY;
  if (LocoyardHan != NULL)
  {
    ::MoveWindow(LocoyardHan->HWindow, LocoyardX, LocoyardY, LocoyardHan->Attr.W, LocoyardHan->Attr.H, TRUE);
  }
}


void TMainWindow::EvMenuSelect( UINT, UINT, HMENU )
{
/*
  HMENU  hMenu1, hMenu2, hMenu3, hMenu4;
  char   OldText[100], TextString[80];

  // Handle menu selections by describing menu item's function in the status bar
  // - if it exists
  hMenu1 = GetSubMenu(::GetMenu(HWindow), 0);
  hMenu2 = GetSubMenu(::GetMenu(HWindow), 1);
  hMenu3 = GetSubMenu(::GetMenu(HWindow), 2);
  hMenu4 = GetSubMenu(::GetMenu(HWindow), 3);

  // Get the current status bar text
  strcpy(OldText, StatbarHan->GetText());

  switch (Msg.WParam)
  {
   case CM_MNUFILNEW       : strcpy(TextString, "Start a new session"); break;
   case CM_MNUFIPAUS       : {
        if (GamePaused)
                      {
          strcpy(TextString, "Continue the current session");
        }
        else
        {
          strcpy(TextString, "Pause the current session");
        }
                      break;
                    }
   case CM_MNUFISTOP       : strcpy(TextString, "Stop the current session"); break;
   case CM_MNUFISETD       : strcpy(TextString, "Set the RCD data file name"); break;
   case CM_MNUFIEXIT       : strcpy(TextString, "Exit from Rail Control"); break;
   case CM_OPTOPTIMI       : strcpy(TextString, "Optimize the display of all windows"); break;
   case CM_OPTCONFIG       : strcpy(TextString, "Configure game and program options"); break;
   case CM_WINARRIVA       : strcpy(TextString, "Display or hide the Arrivals window"); break;
   case CM_WINDEPART       : strcpy(TextString, "Display or hide the Departures window"); break;
   case CM_WINPLATFO       : strcpy(TextString, "Display or hide the Platform window"); break;
   case CM_WINLOCOYA       : strcpy(TextString, "Display or hide the Locoyard window"); break;
   case CM_MNUHECTNT       : strcpy(TextString, "Display the help contents page"); break;
   case CM_MNUHEHELP       : strcpy(TextString, "Help on using Windows help"); break;
   case CM_MNUHEABOT       : strcpy(TextString, "Display information about Rail Control"); break;

   default:
    if (::IsIconic(HWindow))
    {
  strcpy(TextString, OldText);
    }
    else
    {
      strcpy(TextString, "");
    }
  }

  // Check for main menu selections
  if ((HMENU(Msg.WParam) == hMenu1) && (hMenu1 != 0))  strcpy(TextString, "Game menu");
  if ((HMENU(Msg.WParam) == hMenu2) && (hMenu2 != 0))  strcpy(TextString, "Options menu");
  if ((HMENU(Msg.WParam) == hMenu3) && (hMenu3 != 0))  strcpy(TextString, "Window menu");
  if ((HMENU(Msg.WParam) == hMenu4) && (hMenu4 != 0))  strcpy(TextString, "Help menu");

  // Update the status bar
  ::SendMessage(StatbarHan->HWindow,
      SB_SETTEXT,
          0,
      (LPARAM) ((LPSTR) TextString));
         */
}


void TMainWindow::Paint(TDC& dc, BOOL erase, TRect& rect)
{
  TFrameWindow::Paint(dc, erase, rect);
}


void TMainWindow::RedoChildren()
{
  int  YPos, Width, Height;
  RECT WinRect;

  // Get client window size
  ::GetClientRect(HWindow, &WinRect);

  // Reset the position of the Toolbar window
  if (ToolbarExist)
  {
    ::MoveWindow(ToolbarHan->HWindow, 0, 0, (WinRect.right-WinRect.left), 30, TRUE);
  }

  if (StatbarExist)
  {
    ::MoveWindow(StatbarHan->HWindow, 0, (WinRect.bottom-22),
         (WinRect.right-WinRect.left), 22, TRUE);
  }

  // Move the layout window
  if (DisplayExist)
  {
    if (ToolbarExist)
    {
      YPos = 30;
    }
    else
    {
      YPos = 0;
    }

    Height = (WinRect.bottom-WinRect.top);
    if (ToolbarExist) Height -= 30;
    if (StatbarExist) Height -= 22;

    Width = (WinRect.right-WinRect.left);

    ::MoveWindow(DisplayHan->HWindow, 0, YPos, Width, Height, TRUE);
  }
}


void TMainWindow::CMMnuFilNew()
{

  HMENU  TheMenu;
  MSG    msg;

  // Firstly set up a new timer and kill any old ones in the process
  if (GameInProgress)
    ::KillTimer(DisplayHan->HWindow, ID_TIMER);

  // Reset the time for the window
  if (!DisplayHan->StartNew())
  {
    GameInProgress = FALSE;
    return;
  }

  /**************************************************************************/
  /* Decide whether to enable or disable the locoyard toolbar button.       */
  /**************************************************************************/
  if (LocoyardEnabled)
  {
    ToolbarHan->PToolbutt[10]->Enable(TRUE);
  }
  else
  {
    CMWinLocoya();
  }

  // Start a new game
  GameInProgress = TRUE;
  GamePaused = FALSE;

  // Update the display
  DisplayHan->SizeSelectors();
  ::InvalidateRect(DisplayHan->HWindow, 0, TRUE);

  while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Disable the menu items
  TheMenu = ::GetMenu(HWindow);
  EnableMenuItem(TheMenu, CM_MNUFIPAUS, MF_ENABLED);
  EnableMenuItem(TheMenu, CM_MNUFISTOP, MF_ENABLED);

  // Update the toolbar
  ToolbarHan->PToolbutt[2]->Enable(TRUE);
  ToolbarHan->PToolbutt[3]->Enable(TRUE);
  ToolbarHan->PToolbutt[4]->Enable(TRUE);
  ::ShowWindow(ToolbarHan->PToolbutt[2]->HWindow, FALSE);
  ::ShowWindow(ToolbarHan->PToolbutt[3]->HWindow, TRUE);

  // Update any child windows as well
  if (ArrivalHan != NULL)
    ::InvalidateRect(ArrivalHan->HWindow, 0, TRUE);
  if (DeparturHan != NULL)
    ::InvalidateRect(DeparturHan->HWindow, 0, TRUE);
  if (PlatformHan != NULL)
    ::InvalidateRect(PlatformHan->HWindow, 0, TRUE);
  if (LocoyardHan != NULL)
    ::InvalidateRect(LocoyardHan->HWindow, 0, TRUE);

  // Display startup info
  GetApplication()->ExecDialog(new TStart(this, "START"));


  // Create a new timer
  ::SetTimer(DisplayHan->HWindow, ID_TIMER, 100, NULL);
}


void TMainWindow::CMMnuFiPaus()
{

  HMENU  TheMenu;

  // Pause the current session
  TheMenu = ::GetMenu(HWindow);
  if (GamePaused)
  {   
    ::SetTimer(DisplayHan->HWindow, ID_TIMER, 100, NULL);
    GamePaused = FALSE;

    // Modify the menu as well
    ModifyMenu(TheMenu, CM_MNUFIPAUS, MF_BYCOMMAND | MF_STRING, CM_MNUFIPAUS, "&Pause");
    ::DrawMenuBar(HWindow);
    ::ShowWindow(ToolbarHan->PToolbutt[2]->HWindow, FALSE);
    ::ShowWindow(ToolbarHan->PToolbutt[3]->HWindow, TRUE);
  }
  else
  {
    ::KillTimer(DisplayHan->HWindow, ID_TIMER);
    GamePaused = TRUE;

    // Modify the menu as well
    ModifyMenu(TheMenu, CM_MNUFIPAUS, MF_BYCOMMAND | MF_STRING, CM_MNUFIPAUS, "&Restart");
    ::DrawMenuBar(HWindow);
    ::ShowWindow(ToolbarHan->PToolbutt[2]->HWindow, TRUE);
    ::ShowWindow(ToolbarHan->PToolbutt[3]->HWindow, FALSE);
  }
}


void TMainWindow::CMMnuFiStop()
{

  HMENU TheMenu;

  // Stop a session currently in progress
  GameInProgress = FALSE;
  ::KillTimer(DisplayHan->HWindow, ID_TIMER);

  // Disable the pause option
  TheMenu = ::GetMenu(HWindow);
  EnableMenuItem(TheMenu, CM_MNUFIPAUS, MF_GRAYED);
  EnableMenuItem(TheMenu, CM_MNUFISTOP, MF_GRAYED);

  // Update the toolbar
  ToolbarHan->PToolbutt[2]->Enable(FALSE);
  ToolbarHan->PToolbutt[3]->Enable(FALSE);
  ToolbarHan->PToolbutt[4]->Enable(FALSE);
  ::ShowWindow(ToolbarHan->PToolbutt[2]->HWindow, FALSE);
  ::ShowWindow(ToolbarHan->PToolbutt[3]->HWindow, TRUE);

  // Clear all the data
  DisplayHan->IniVariables();
  DisplayHan->ClearData();

  // Redraw the screen
  DisplayHan->UpdateSelectors();
  ::InvalidateRect(DisplayHan->HWindow, 0, TRUE);

  // Update any child windows as well
  if (ArrivalHan != NULL)
    ::InvalidateRect(ArrivalHan->HWindow, 0, TRUE);
  if (DeparturHan != NULL)
    ::InvalidateRect(DeparturHan->HWindow, 0, TRUE);
  if (PlatformHan != NULL)
    ::InvalidateRect(PlatformHan->HWindow, 0, TRUE);
  if (LocoyardHan != NULL)
    ::InvalidateRect(LocoyardHan->HWindow, 0, TRUE);
}


void TMainWindow::CMMnuFiSetD()
{

  // Set the datafile name
  OPENFILENAME ofn;             // File structure
  char         szName[256];     // File name
  char         buf[5];          // Error buffer
  char         szTemp[] = "All Files (*.*)\0*.*\0Rail Control files (*.rcd)\0*.rcd\0";
  DWORD        Errval;    // Error value


  if (GameInProgress)
  {
    if (!GamePaused)  ::SendMessage(HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);
    if (::MessageBox(HWindow,
       "A game is currently in progress. Are you sure?",
       APPNAME,
       MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
    {
      ::SendMessage(HWindow, WM_COMMAND, CM_MNUFISTOP, 0);
    }
    else
    {
      ::SendMessage(HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);
      return;
    }
  }

  // Update the status bar
  ::SendMessage(StatbarHan->HWindow, SB_SETTEXT, 0,
        (LPARAM) ((LPSTR) "Please select a filename..."));

  // Set all structure members to zero
  memset(&ofn, 0, sizeof(OPENFILENAME));
  strcpy(szName, DataFileName);

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = HWindow;
  ofn.hInstance = 0;
  ofn.lpstrFilter = (LPSTR)szTemp;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 2;
  ofn.lpstrFile = (LPSTR)szName;
  ofn.nMaxFile = sizeof(szName);
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = "*";
  ofn.lCustData = NULL;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
  ofn.lpstrTitle = LPSTR("Rail Control data file");

  // Get the filename
  if (GetOpenFileName(&ofn) != TRUE)
  {
    char Errstr[50]="Internal error: GetOpenFileName returned error #";

    Errval=CommDlgExtendedError();
    if (Errval != 0)   // 0 value means user selected Cancel
    {
      wsprintf(buf, "%ld", Errval);
      strcat(Errstr, buf);
      ::MessageBox(HWindow, Errstr, APPNAME, MB_OK|MB_ICONSTOP);
    }

    // Update the status bar
    ::SendMessage(StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) ""));
  }
  else
  {
    // Everything is OK
    strcpy(DataFileName, szName);

    // Update the status bar
    ::SendMessage(StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) ""));
  }
}


void TMainWindow::CMOptOptimi()
{
  RECT TheRect;
  RECT lWinRect;
  int  lWidth, lHeight;

  ::GetWindowRect(::GetDesktopWindow(), &TheRect);

  switch (TheRect.right)
  {
    case 640:
       // Optimize the main window display
       MainWinX = 0;
       MainWinY = 0;
       ::MoveWindow(HWindow, MainWinX, MainWinY, 405, 360, TRUE);

       // Move arrival window
       if (ArrivalHan != NULL)
       {
         if (ArrivalIcon) OpenIcon(ArrivalHan->HWindow);
         ArrivalX = 0;
         ArrivalY = 360;
         ::MoveWindow(ArrivalHan->HWindow, 0, 360, ArrivalHan->Attr.W, ArrivalHan->Attr.H, TRUE);
       }

       // Move departure window
       if (DeparturHan != NULL)
       {
         if (DeparturIcon) OpenIcon(DeparturHan->HWindow);
         DeparturX = 405;
         DeparturY = -13;
         ::MoveWindow(DeparturHan->HWindow, 405, -13, DeparturHan->Attr.W, DeparturHan->Attr.H, TRUE);
       }

       // Move platform window
       if (PlatformHan != NULL)
       {
         if (PlatformIcon) OpenIcon(PlatformHan->HWindow);
         PlatformX = 405;
         PlatformY = 107;
         ::MoveWindow(PlatformHan->HWindow, 405, 107, PlatformHan->Attr.W, PlatformHan->Attr.H, TRUE);
       }

       // Move locoyard window
       if (LocoyardHan != NULL)
       {
         if (LocoyardIcon) OpenIcon(LocoyardHan->HWindow);
         LocoyardX = 455;
         LocoyardY = 360;
         ::MoveWindow(LocoyardHan->HWindow, LocoyardX, LocoyardY, LocoyardHan->Attr.W, LocoyardHan->Attr.H, TRUE);
       }
       break;

    case 800:
       // Optimize the main window display - 800x600 and above
       MainWinX = 0;
       MainWinY = 0;
       ::MoveWindow(HWindow, MainWinX, MainWinY, 550, 373, TRUE);

       // Move arrival window
       if (ArrivalHan != NULL)
       {
         if (ArrivalIcon) OpenIcon(ArrivalHan->HWindow);
         ArrivalX = 0;
         ArrivalY = 373;
         ::MoveWindow(ArrivalHan->HWindow, ArrivalX, ArrivalY, ArrivalHan->Attr.W, ArrivalHan->Attr.H, TRUE);
       }

       // Move departure window
       if (DeparturHan != NULL)
       {
         if (DeparturIcon) OpenIcon(DeparturHan->HWindow);
         DeparturX = 550;
         DeparturY = 0;
         ::MoveWindow(DeparturHan->HWindow, DeparturX, DeparturY, DeparturHan->Attr.W, DeparturHan->Attr.H, TRUE);
       }

       // Move platform window
       if (PlatformHan != NULL)
       {
         if (PlatformIcon) OpenIcon(PlatformHan->HWindow);
         PlatformX = 550;
         PlatformY = 120;
         ::MoveWindow(PlatformHan->HWindow, PlatformX, PlatformY, PlatformHan->Attr.W, PlatformHan->Attr.H, TRUE);
       }

       // Move locoyard window
       if (LocoyardHan != NULL)
       {
         if (LocoyardIcon) OpenIcon(LocoyardHan->HWindow);
         LocoyardX = 600;
         LocoyardY = 373;
         ::MoveWindow(LocoyardHan->HWindow, LocoyardX, LocoyardY, LocoyardHan->Attr.W, LocoyardHan->Attr.H, TRUE);
       }
       break;

    case 1024:
    default:
    {
      if (TheRect.bottom == 600)
      {
        /********************************************************************/
        /* Optimize the main window display - 1024x600.                     */
        /********************************************************************/
        MainWinX = 0;
        MainWinY = 0;
        lWidth  = 720;
        lHeight = (600 - 192);
      }
      else
      {
        // Optimize the main window display - 1024x768 and above
        lWidth  = 720;
        lHeight = 480;

        ::GetWindowRect(HWindow, &lWinRect);
        MainWinX = lWinRect.left;
        MainWinY = lWinRect.top;
      }

      ::MoveWindow(HWindow, MainWinX, MainWinY, lWidth, lHeight, TRUE);

      // Move arrival window
      if (ArrivalHan != NULL)
      {
        if (ArrivalIcon) OpenIcon(ArrivalHan->HWindow);
        ArrivalX = MainWinX;
        ArrivalY = MainWinY + lHeight;
        ::MoveWindow(ArrivalHan->HWindow, ArrivalX, ArrivalY, ArrivalHan->Attr.W, ArrivalHan->Attr.H, TRUE);
      }

      // Move departure window
      if (DeparturHan != NULL)
      {
        if (DeparturIcon) OpenIcon(DeparturHan->HWindow);
        DeparturX = MainWinX + lWidth;
        DeparturY = MainWinY + 259;
        ::MoveWindow(DeparturHan->HWindow, DeparturX, DeparturY, DeparturHan->Attr.W, DeparturHan->Attr.H, TRUE);
      }

      // Move platform window
      if (PlatformHan != NULL)
      {
        if (PlatformIcon) OpenIcon(PlatformHan->HWindow);
        PlatformX = MainWinX + lWidth;
        PlatformY = MainWinY;
        ::MoveWindow(PlatformHan->HWindow, PlatformX, PlatformY, PlatformHan->Attr.W, PlatformHan->Attr.H, TRUE);
      }

      // Move locoyard window
      if (LocoyardHan != NULL)
      {
        if (LocoyardIcon) OpenIcon(LocoyardHan->HWindow);
        LocoyardX = MainWinX + lWidth;
        LocoyardY = MainWinY + 259 + 192;
        ::MoveWindow(LocoyardHan->HWindow, LocoyardX, LocoyardY, LocoyardHan->Attr.W, LocoyardHan->Attr.H, TRUE);
      }
    }
    break;
  }
}


void TMainWindow::CMOptConfig()
{

  BOOL CurrentlyPaused;

  // Pause the game
  CurrentlyPaused = GamePaused;
  if (GameInProgress && !GamePaused)
  {
      ::SendMessage(HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);
  }

  // Execute the Configuration dialog box
  GetApplication()->ExecDialog(new TConfigur(this, "CONFIG"));

  // Un-pause the game
  if (GameInProgress && !CurrentlyPaused)
  {
      ::SendMessage(HWindow, WM_COMMAND, CM_MNUFIPAUS, 0);
  }
}


void TMainWindow::CMWinArriva()
{

  HMENU TheMenu;

  TheMenu = ::GetMenu(HWindow);
  if (GetMenuState(TheMenu, CM_WINARRIVA, MF_BYCOMMAND) == MF_CHECKED)
  {
    // Get rid of the Arrivals window
    if (ArrivalHan != NULL)
    {
      ArrivalHan->CloseWindow();
      ArrivalHan = NULL;
    }
    CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_UNCHECKED);
  }
  else
  {
    // Create a new arrivals window
    if (ArrivalHan == NULL)
    {
      ArrivalHan = new TArrivals(this, ArrivalX, ArrivalY);
      GetApplication()->MakeWindow(ArrivalHan);
      ArrivalHan->Show(SW_SHOWNORMAL);
      SetFocus();
    }
    CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_CHECKED);
  }
}


void TMainWindow::CMWinDepart()
{

  HMENU TheMenu;

  TheMenu = ::GetMenu(HWindow);
  if (GetMenuState(TheMenu, CM_WINDEPART, MF_BYCOMMAND) == MF_CHECKED)
  {
    // Get rid of the Departure window
    if (DeparturHan != NULL)
    {
      DeparturHan->CloseWindow();
      DeparturHan = NULL;
    }
    CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_UNCHECKED);
  }
  else
  {
    // Create a new Departure window
    if (DeparturHan == NULL)
    {
      DeparturHan = new TDepartur(this, DeparturX, DeparturY);
      GetApplication()->MakeWindow(DeparturHan);
      DeparturHan->Show(SW_SHOWNORMAL);
      SetFocus();
    }
    CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_CHECKED);
  }
}


void TMainWindow::CMWinPlatfo()
{

  HMENU TheMenu;

  TheMenu = ::GetMenu(HWindow);
  if (GetMenuState(TheMenu, CM_WINPLATFO, MF_BYCOMMAND) == MF_CHECKED)
  {
    // Get rid of the Platform window
    if (PlatformHan != NULL)
    {
      PlatformHan->CloseWindow();
      PlatformHan = NULL;
    }
    CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_UNCHECKED);
  }
  else
  {
    // Create a new platform window
    if (PlatformHan == NULL)
    {
      PlatformHan = new TPlatform(this, PlatformX, PlatformY);
      GetApplication()->MakeWindow(PlatformHan);
      PlatformHan->Show(SW_SHOWNORMAL);
      SetFocus();
    }
    CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_CHECKED);
  }
}


void TMainWindow::CMWinLocoya()
{

  HMENU TheMenu;

  TheMenu = ::GetMenu(HWindow);
  if (GetMenuState(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND) == MF_CHECKED)
  {
    // Get rid of the locoyard window
    if (LocoyardHan != NULL)
    {
      LocoyardHan->CloseWindow();
      LocoyardHan = NULL;
    }
    CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_UNCHECKED);
  }
  else if (LocoyardEnabled)
  {
    // Create a new locoyard window
    if (LocoyardHan == NULL)
    {
      LocoyardHan = new TLocoyard(this, LocoyardX, LocoyardY);
      GetApplication()->MakeWindow(LocoyardHan);
      LocoyardHan->Show(SW_SHOWNORMAL);
      SetFocus();
    }
    CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_CHECKED);
  }

  /**************************************************************************/
  /* Decide whether to enable or disable the locoyard toolbar button.       */
  /**************************************************************************/
  if (LocoyardEnabled)
  {
    ToolbarHan->PToolbutt[10]->Enable(TRUE);
  }
  else
  {
    ToolbarHan->PToolbutt[10]->Enable(FALSE);
  }
}


void TMainWindow::CMMnuHeCtnt()
{

  // Display help contents
  ::WinHelp(HWindow, HELPFILENAME, HELP_CONTENTS, 0L);
}


void TMainWindow::CMMnuHeHelp()
{

  // Display help contents
  ::WinHelp(HWindow, HELPFILENAME, HELP_HELPONHELP, 0L);
}


void TMainWindow::CMMnuHeAbot()
{

  // Display about box
  GetApplication()->ExecDialog(new TAbout(this, "AboutBox"));
}



DEFINE_RESPONSE_TABLE1( TMainWindow, TFrameWindow )
    EV_WM_SIZE,
    EV_WM_MOVE,
    EV_WM_MENUSELECT,
    EV_COMMAND( 110, CM_FileExit ),
    EV_COMMAND( 100, CMMnuFilNew ),
    EV_COMMAND( 101, CMMnuFiPaus ),
    EV_COMMAND( 102, CMMnuFiStop ),
    EV_COMMAND( 103, CMMnuFiSetD ),
    EV_COMMAND( 200, CMOptOptimi ),
    EV_COMMAND( 201, CMOptConfig ),
    EV_COMMAND( 300, CMWinArriva ),
    EV_COMMAND( 301, CMWinDepart ),
    EV_COMMAND( 302, CMWinPlatfo ),
    EV_COMMAND( 303, CMWinLocoya ),
    EV_COMMAND( 900, CMMnuHeCtnt ),
    EV_COMMAND( 901, CMMnuHeHelp ),
    EV_COMMAND( 999, CMMnuHeAbot ),
END_RESPONSE_TABLE;

/****************************************************************************/
/* Utility functions.                                                       */
/****************************************************************************/
int checkisspace(char c)
{
  return c == ' '  || c == '\t' ||
         c == '\v' || c == '\f';  
}
 
char* lTrim(char* p)
{
  while (p && checkisspace(*p)) ++p;
  return p;
}
 
char* rTrim(char* p)
{
  char * temp;
  if (!p) return p;
  temp = (char *)(p + strlen(p)-1);
  while ((temp>=p) && checkisspace(*temp)) --temp;
  *(temp+1) = '\0';
  return p;
}
 
char* Trim(char* p)
{
  return lTrim(rTrim(p));
}


