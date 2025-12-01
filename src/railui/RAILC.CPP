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
#include <cstdio>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#if defined(_MSC_VER)
#include <windows.h>
#endif
/*************************************************************************/
/* Methods of TManager follow...*/

namespace
{
  void StartupLog(const char* message)
  {
    (void) message;
  }

  void AppendPathLeaf(char* xoPath, size_t xiSize, const char* xiLeaf)
  {
    if ((xoPath == 0) || (xiLeaf == 0) || (xiSize == 0))  return;

    size_t lLength = lstrlenA(xoPath);
    if (lLength >= xiSize)  return;

    if ((lLength > 0) && (xoPath[lLength - 1] != '\\'))
    {
      if (lLength >= (xiSize - 1))  return;
      xoPath[lLength] = '\\';
      xoPath[lLength + 1] = '\0';
      lLength++;
    }

    lstrcpynA(xoPath + lLength, xiLeaf, int(xiSize - lLength));
  }

  bool EnsureDirectoryExists(const char* xiPath, DWORD* xoError)
  {
    if (xoError)  *xoError = ERROR_SUCCESS;
    if (xiPath == 0)  return false;

    DWORD lAttrs = ::GetFileAttributesA(xiPath);
    if (lAttrs == INVALID_FILE_ATTRIBUTES)
    {
      if (::CreateDirectoryA(xiPath, 0) == 0)
      {
        if (xoError)  *xoError = ::GetLastError();
        return false;
      }
      return true;
    }

    if ((lAttrs & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
      if (xoError)  *xoError = ERROR_ALREADY_EXISTS;
      return false;
    }

    return true;
  }

  bool ResolveBaseAppDataPath(char* xoPath, size_t xiSize)
  {
    if ((xoPath == 0) || (xiSize == 0))  return false;

    xoPath[0] = '\0';
    DWORD lLen = ::GetEnvironmentVariableA("LOCALAPPDATA", xoPath, DWORD(xiSize));
    if ((lLen == 0) || (lLen >= xiSize))
    {
      lLen = ::GetEnvironmentVariableA("APPDATA", xoPath, DWORD(xiSize));
    }
    if ((lLen == 0) || (lLen >= xiSize))
  {
    if (::GetWindowsDirectoryA(xoPath, UINT(xiSize)) == 0)
    {
      lstrcpynA(xoPath, ".", int(xiSize));
    }
  }

  return true;
}

  void ClampWindowPosition(int& ioX, int& ioY, int width, int height)
  {
    if (width <= 0)  width = 1;
    if (height <= 0) height = 1;

    int virtualLeft = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    int virtualTop = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    int virtualWidth = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int virtualHeight = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if ((virtualWidth <= 0) || (virtualHeight <= 0))
    {
      virtualLeft = 0;
      virtualTop = 0;
      virtualWidth = ::GetSystemMetrics(SM_CXSCREEN);
      virtualHeight = ::GetSystemMetrics(SM_CYSCREEN);
    }

    const int maxX = virtualLeft + virtualWidth - width;
    const int maxY = virtualTop + virtualHeight - height;

    if (ioX < virtualLeft)  ioX = virtualLeft;
    if (ioY < virtualTop)   ioY = virtualTop;
    if (ioX > maxX)         ioX = maxX;
    if (ioY > maxY)         ioY = maxY;
  }

#if defined(_MSC_VER)
  bool ParseIniBoolean(const char* xiValue, bool xiDefault)
  {
    if (!xiValue)
    {
      return xiDefault;
    }

    if ((lstrcmpiA(xiValue, "true") == 0) ||
        (lstrcmpiA(xiValue, "yes") == 0) ||
        (lstrcmpiA(xiValue, "on") == 0))
    {
      return true;
    }

    if ((lstrcmpiA(xiValue, "false") == 0) ||
        (lstrcmpiA(xiValue, "no") == 0) ||
        (lstrcmpiA(xiValue, "off") == 0))
    {
      return false;
    }

    return GetPrivateProfileIntA("Diagnostics",
                                 "EnableMemoryGuards",
                                 xiDefault ? 1 : 0,
                                 INIFILENAME) != 0;
  }

  bool ReadDiagnosticsGuardFlag(bool* xoWroteDefault)
  {
    const bool kDefaultValue = true;

    if (xoWroteDefault)
    {
      *xoWroteDefault = false;
    }

    const char* kSentinel = "__RC_SENTINEL__";
    char buffer[32] = {0};
    DWORD read = ::GetPrivateProfileStringA("Diagnostics",
                                            "EnableMemoryGuards",
                                            kSentinel,
                                            buffer,
                                            static_cast<DWORD>(sizeof(buffer)),
                                            INIFILENAME);
    bool missing = (read == 0) || (lstrcmpA(buffer, kSentinel) == 0);
    if (missing)
    {
      if (::WritePrivateProfileStringA("Diagnostics",
                                       "EnableMemoryGuards",
                                       kDefaultValue ? "1" : "0",
                                       INIFILENAME) != 0)
      {
        if (xoWroteDefault)
        {
          *xoWroteDefault = true;
        }
      }
      return kDefaultValue;
    }

    bool parsed = ParseIniBoolean(buffer, kDefaultValue);
#if defined(_DEBUG)
    if (!parsed)
    {
      // Force-enable guards in debug builds to guarantee diagnostics.
      ::WritePrivateProfileStringA("Diagnostics", "EnableMemoryGuards", "1", INIFILENAME);
      parsed = true;
      if (xoWroteDefault)
      {
        *xoWroteDefault = true;
      }
    }
#endif
    return parsed;
  }

  bool sMemoryGuardInitialised = false;

  void BootstrapMemoryGuardsFromConfig()
  {
    if (sMemoryGuardInitialised)
    {
      return;
    }

    bool wroteDefault = false;
    const bool guardEnabled = ReadDiagnosticsGuardFlag(&wroteDefault);

    DebugMemoryGuard::Options options = {};
    options.enableGuard = guardEnabled;
    options.maxFileBytes = 2u * 1024u * 1024u;
    options.maxFileCount = 5;

    DebugMemoryGuard::Initialise(options);
    sMemoryGuardInitialised = true;

    if (!DebugMemoryGuard::IsEnabled())
    {
      return;
    }

    DebugMemoryGuard::LogEvent("startup", "memory guard initialised");

    if (wroteDefault)
    {
      DebugMemoryGuard::LogEvent("startup", "guard key missing; default enabled flag persisted");
    }

    if (const char* logDir = DebugMemoryGuard::GetLogDirectory())
    {
      if (logDir[0] != '\0')
      {
        char buffer[256];
        FormatBuffer(buffer, "log directory set to %s", logDir);
        DebugMemoryGuard::LogEvent("startup", buffer);
      }
    }
  }

  void ShutdownMemoryGuards()
  {
    if (!sMemoryGuardInitialised)
    {
      return;
    }

    if (DebugMemoryGuard::IsEnabled())
    {
      DebugMemoryGuard::LogEvent("shutdown", "memory guard shutting down");
    }

    DebugMemoryGuard::Shutdown();
    sMemoryGuardInitialised = false;
  }
#endif

  void ShowHelpPlaceholder(HWND xiWindow, UINT xiCommand)
  {
    TRC_ALT((TB, "WinHelp command %u requested but WinHelp is unavailable on modern Windows", xiCommand));
    ::MessageBoxA(xiWindow,
                  "Help is currently unavailable.\n\nThe legacy WinHelp system is unsupported on modern Windows versions. "
                  "Please refer to the README for interim documentation.",
                  APPNAME,
                  MB_OK | MB_ICONINFORMATION);
  }

#if defined(DIAG_SETUP_TRACE)
  void AppendSetupTrace(const char* xiMessage)
  {
    if (!xiMessage || xiMessage[0] == '\0')
    {
      return;
    }

    static bool sTraceDirectoryInitialised = false;
    if (!sTraceDirectoryInitialised)
    {
      ::CreateDirectoryA("tmp", 0);
      sTraceDirectoryInitialised = true;
    }

    OutputDebugStringA(xiMessage);
    OutputDebugStringA("\n");

    if (FILE* logFile = fopen("tmp\\perform_setup_trace.log", "a"))
    {
      fputs(xiMessage, logFile);
      fputc('\n', logFile);
      fclose(logFile);
    }
  }

  void LogSetupEvent(const char* xiPhase, TWindow* xiWindow, const char* xiDetail = nullptr)
  {
    if (!xiPhase)
    {
      return;
    }

    char buffer[512];
    HWND hwnd = xiWindow ? xiWindow->GetHandle() : HWND(0);
    const char* detailPrefix = (xiDetail && xiDetail[0] != '\0') ? " note=" : "";
    const char* detailText = xiDetail ? xiDetail : "";
    FormatBuffer(buffer,
                 "%s this=%p hwnd=%p%s%s",
                 xiPhase,
                 xiWindow,
                 hwnd,
                 detailPrefix,
                 detailText);
    AppendSetupTrace(buffer);
  }

  void LogSetupPointer(const char* xiPhase, TWindow* xiWindow, const void* xiTarget, const char* xiLabel)
  {
    if (!xiPhase)
    {
      return;
    }

    char buffer[512];
    HWND hwnd = xiWindow ? xiWindow->GetHandle() : HWND(0);
    const char* labelText = xiLabel ? xiLabel : "ptr";
    FormatBuffer(buffer,
                 "%s this=%p hwnd=%p %s=%p",
                 xiPhase,
                 xiWindow,
                 hwnd,
                 labelText,
                 xiTarget);
    AppendSetupTrace(buffer);
  }
#endif
}

const char* GetIniFilePath()
{
  static char gIniFilePath[MAX_PATH] = {0};
  static bool gIniInitialised = false;

  if (gIniInitialised && (gIniFilePath[0] != '\0'))
  {
    return gIniFilePath;
  }

  gIniInitialised = true;

  char lBasePath[MAX_PATH] = {0};
  ResolveBaseAppDataPath(lBasePath, MAX_PATH);

  char lSettingsDir[MAX_PATH];
  lstrcpynA(lSettingsDir, lBasePath, MAX_PATH);
  AppendPathLeaf(lSettingsDir, MAX_PATH, "RailControl");

  DWORD lMkdirError = ERROR_SUCCESS;
  if (!EnsureDirectoryExists(lSettingsDir, &lMkdirError))
  {
    TRC_ALT((TB, "Settings directory %s unavailable (err:%lu); using %s", lSettingsDir, lMkdirError, lBasePath));
    lstrcpynA(lSettingsDir, lBasePath, MAX_PATH);
  }

  lstrcpynA(gIniFilePath, lSettingsDir, MAX_PATH);
  AppendPathLeaf(gIniFilePath, MAX_PATH, "railc.ini");

  if (::GetFileAttributesA(gIniFilePath) == INVALID_FILE_ATTRIBUTES)
  {
    char lLegacyPath[MAX_PATH];
    if (::GetWindowsDirectoryA(lLegacyPath, MAX_PATH) != 0)
    {
      AppendPathLeaf(lLegacyPath, MAX_PATH, "railc.ini");
      if (::GetFileAttributesA(lLegacyPath) != INVALID_FILE_ATTRIBUTES)
      {
        if (!::CopyFileA(lLegacyPath, gIniFilePath, TRUE))
        {
          TRC_ALT((TB, "Failed to migrate legacy INI %s -> %s (err:%lu)", lLegacyPath, gIniFilePath, ::GetLastError()));
        }
        else
        {
          TRC_NRM((TB, "Migrated legacy INI to %s", gIniFilePath));
        }
      }
    }
  }

  TRC_NRM((TB, "Using settings file %s", gIniFilePath));
  return gIniFilePath;
}

const char* GetLogFilePath()
{
  static char gLogFilePath[MAX_PATH] = {0};
  static bool gLogInitialised = false;

  if (gLogInitialised && (gLogFilePath[0] != '\0'))
  {
    return gLogFilePath;
  }

  gLogInitialised = true;

  char lBasePath[MAX_PATH] = {0};
  ResolveBaseAppDataPath(lBasePath, MAX_PATH);

  char lSettingsDir[MAX_PATH];
  lstrcpynA(lSettingsDir, lBasePath, MAX_PATH);
  AppendPathLeaf(lSettingsDir, MAX_PATH, "RailControl");

  DWORD lSettingsError = ERROR_SUCCESS;
  if (!EnsureDirectoryExists(lSettingsDir, &lSettingsError))
  {
    TRC_ALT((TB, "Settings directory %s unavailable for logs (err:%lu); using %s", lSettingsDir, lSettingsError, lBasePath));
    lstrcpynA(lSettingsDir, lBasePath, MAX_PATH);
  }

  char lLogDir[MAX_PATH];
  lstrcpynA(lLogDir, lSettingsDir, MAX_PATH);
  AppendPathLeaf(lLogDir, MAX_PATH, "Logs");

  DWORD lLogError = ERROR_SUCCESS;
  if (!EnsureDirectoryExists(lLogDir, &lLogError))
  {
    TRC_ALT((TB, "Log directory %s unavailable (err:%lu); using %s", lLogDir, lLogError, lSettingsDir));
    lstrcpynA(lLogDir, lSettingsDir, MAX_PATH);
  }

  lstrcpynA(gLogFilePath, lLogDir, MAX_PATH);
  AppendPathLeaf(gLogFilePath, MAX_PATH, "ptrack_debug.log");

  TRC_NRM((TB, "Using diagnostics log %s", gLogFilePath));
  return gLogFilePath;
}


TManager::TManager()
  : TApplication(APPNAME)
{
}


void TManager::InitMainWindow()
{
  MainWindow = new TMainWindow(nullptr, APPNAME);
}


// Forward decl for CLI validation runner (implemented in cli_validate.cpp)
#if defined(_MSC_VER)
extern "C" int RunRcdValidationCli(int argc, wchar_t** argv);
#endif

int OwlMain(int /*argc*/, tchar* /*argv*/[])
{
#if defined(_MSC_VER)
  // Early CLI dispatch: handle --rcd-validate before any UI or guards
  int argcW = 0;
  LPWSTR* argvW = ::CommandLineToArgvW(::GetCommandLineW(), &argcW);
  if (argvW && argcW > 0)
  {
    bool hasValidate = false;
    for (int i = 1; i < argcW; ++i)
    {
      if ((::lstrcmpiW(argvW[i], L"--rcd-validate") == 0) ||
          (::lstrcmpiW(argvW[i], L"-rcd-validate") == 0) ||
          (::lstrcmpiW(argvW[i], L"/rcd-validate") == 0))
      {
        hasValidate = true;
        break;
      }
    }
    if (hasValidate)
    {
      int exitCode = RunRcdValidationCli(argcW, argvW);
      ::LocalFree(argvW);
      return exitCode;
    }
  }
  if (argvW) { ::LocalFree(argvW); }
#endif
#if defined(_MSC_VER)
  BootstrapMemoryGuardsFromConfig();
#endif
  StartupLog("OwlMain: after BootstrapMemoryGuards");
  TManager app;
  StartupLog("OwlMain: before Run");
  int runResult = app.Run();
  char runBuf[64];
  std::snprintf(runBuf, sizeof(runBuf), "OwlMain: Run returned %d", runResult);
  StartupLog(runBuf);
#if defined(_MSC_VER)
  ShutdownMemoryGuards();
#endif
  return runResult;
}


/*************************************************************************/
/* Methods of TMainWindow follow...*/


TMainWindow::TMainWindow(TWindow * AParent, LPCTSTR ATitle)
  : TFrameWindow(AParent, ATitle)
{
  StartupLog("TMainWindow ctor begin");
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

  StartupLog("Creating startup title window");
  // Display startup title window
  TitleWin = new TStartup(this);
  if (!TitleWin->Create())
  {
    StartupLog("Title window creation failed");
  }
  else
  {
    StartupLog("Title window created");
    ::ShowWindow(TitleWin->GetHandle(), SW_SHOW);
  }

  StartupLog("Main window initialised");
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
  CopyBuffer(lFont.lfFaceName, "Arial");
  mTextFont = CreateFontIndirect(&lFont);
  StartupLog("Fonts initialised");

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
  CopyBuffer(lFont.lfFaceName, "Arial");
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
  CopyBuffer(lFont.lfFaceName, "Arial");
  mSmallFont = CreateFontIndirect(&lFont);

  // Initialize variables
  ArrivalHan = NULL;
  DeparturHan = NULL;
  PlatformHan = NULL;
  LocoyardHan = NULL;
  MainWinIcon = FALSE;
  mLayoutInProgress = FALSE;
  mOptimizedLayoutActive = FALSE;
  mResizeCoalescePending = FALSE;
  GamePaused = FALSE;
  GameInProgress = FALSE;
  LocoyardEnabled = TRUE;
}


TMainWindow::~TMainWindow()
{
  // Delete brushes etc
  RedBrush.Reset();
  GreenBrush.Reset();
  DkGrayBrush.Reset();
  BlueBrush.Reset();
  YellowBrush.Reset();
  WhiteBrush.Reset();
  RedPen.Reset();
  GreenPen.Reset();
  YellowPen.Reset();
  DkGrayPen.Reset();
  LtGrayPen.Reset();
  ToolbarBitmaps.Reset();
  mTextFont.Reset();
  mHeaderFont.Reset();
  mSmallFont.Reset();
  ::KillTimer(HWindow, ID_CHILD_RESIZE_COALESCE);
}


void TMainWindow::GetWindowClass(WNDCLASS& WndClass)
{
  TFrameWindow::GetWindowClass(WndClass);
  WndClass.hIcon = ::LoadIcon(GetModule()->GetHandle(), TEXT("AMAIN"));
  WndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
}


auto TMainWindow::GetWindowClassName() -> owl::TWindowClassName
{
  return owl::TWindowClassName(_T("Main_Window_Class"));
}


void TMainWindow::SetupWindow()
{
  StartupLog("TMainWindow::SetupWindow entry");
  RECT  WinRect;
  int   YPos, Height, Width;
  HMENU TheMenu;

#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Start", this);
#endif
  // Call ancestor method
  TFrameWindow::SetupWindow();
  char startupBuffer[128];
  std::snprintf(startupBuffer, sizeof(startupBuffer), "Main window handle after base: %p", GetHandle());
  StartupLog(startupBuffer);
  StartupLog("TMainWindow::SetupWindow base done");
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:AfterBase", this);
#endif

  ::GetClientRect(HWindow, &WinRect);

  // Create status bar
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:CreateStatbar:pre", this);
#endif
  StatbarHan = new TStatbar(this);
  StatbarHan->Create();
  StatbarExist = TRUE;
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:CreateStatbar:post", StatbarHan);
#endif

  /**************************************************************************/
  /* Get the menu handle.                                                   */
  /**************************************************************************/
#if defined(DIAG_SKIP_MENU_COMMANDS)
  TheMenu = NULL;
#else
  TheMenu = ::GetMenu(HWindow);
#if defined(DIAG_SETUP_TRACE)
  LogSetupPointer("SetupWindow:MenuHandle", this, TheMenu, "menu");
#endif
#endif

  // Set up the tool button X positions
  ToolButtData.XPos[1] = 10;
  ToolButtData.WParam[1] = CM_MNUFILNEW;
  ToolButtData.LParam[1] = 0;
  ToolButtData.SentWindow[1] = HWindow;
  ToolButtData.StatWindow[1] = StatbarHan->HWindow;
  CopyBuffer(ToolButtData.UpText[1], "");
  CopyBuffer(ToolButtData.DownText[1], "Start a new session");

  ToolButtData.XPos[2] = 34;
  ToolButtData.WParam[2] = CM_MNUFIPAUS;
  ToolButtData.LParam[2] = 0;
  ToolButtData.SentWindow[2] = HWindow;
  ToolButtData.StatWindow[2] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[2], "");
  CopyBuffer(ToolButtData.DownText[2], "Restart the current session");

  ToolButtData.XPos[3] = 34;
  ToolButtData.WParam[3] = CM_MNUFIPAUS;
  ToolButtData.LParam[3] = 0;
  ToolButtData.SentWindow[3] = HWindow;
  ToolButtData.StatWindow[3] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[3], "");
  CopyBuffer(ToolButtData.DownText[3], "Pause the current session");

  ToolButtData.XPos[4] = 58;
  ToolButtData.WParam[4] = CM_MNUFISTOP;
  ToolButtData.LParam[4] = 0;
  ToolButtData.SentWindow[4] = HWindow;
  ToolButtData.StatWindow[4] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[4], "");
  CopyBuffer(ToolButtData.DownText[4], "Stop the current session");

  ToolButtData.XPos[5] = 90;
  ToolButtData.WParam[5] = CM_OPTOPTIMI;
  ToolButtData.LParam[5] = 0;
  ToolButtData.SentWindow[5] = HWindow;
  ToolButtData.StatWindow[5] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[5], "");
  CopyBuffer(ToolButtData.DownText[5], "Optimize display of windows");

  ToolButtData.XPos[6] = 114;
  ToolButtData.WParam[6] = CM_OPTCONFIG;
  ToolButtData.LParam[6] = 0;
  ToolButtData.SentWindow[6] = HWindow;
  ToolButtData.StatWindow[6] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[6], "");
  CopyBuffer(ToolButtData.DownText[6], "Set program configuration");

  ToolButtData.XPos[7] = 146;
  ToolButtData.WParam[7] = CM_WINARRIVA;
  ToolButtData.LParam[7] = 0;
  ToolButtData.SentWindow[7] = HWindow;
  ToolButtData.StatWindow[7] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[7], "");
  CopyBuffer(ToolButtData.DownText[7], "Display/hide the Arrivals window");

  ToolButtData.XPos[8] = 170;
  ToolButtData.WParam[8] = CM_WINDEPART;
  ToolButtData.LParam[8] = 0;
  ToolButtData.SentWindow[8] = HWindow;
  ToolButtData.StatWindow[8] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[8], "");
  CopyBuffer(ToolButtData.DownText[8], "Display/hide the Departures window");

  ToolButtData.XPos[9] = 194;
  ToolButtData.WParam[9] = CM_WINPLATFO;
  ToolButtData.LParam[9] = 0;
  ToolButtData.SentWindow[9] = HWindow;
  ToolButtData.StatWindow[9] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[9], "");
  CopyBuffer(ToolButtData.DownText[9], "Display/hide the Platforms window");

  ToolButtData.XPos[10] = 218;
  ToolButtData.WParam[10] = CM_WINLOCOYA;
  ToolButtData.LParam[10] = 0;
  ToolButtData.SentWindow[10] = HWindow;
  ToolButtData.StatWindow[10] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[10], "");
  CopyBuffer(ToolButtData.DownText[10], "Display/hide the Locoyard window");

  ToolButtData.XPos[11] = 250;
  ToolButtData.WParam[11] = CM_MNUHECTNT;
  ToolButtData.LParam[11] = 0;
  ToolButtData.SentWindow[11] = HWindow;
  ToolButtData.StatWindow[11] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[11], "");
  CopyBuffer(ToolButtData.DownText[11], "Display the help file contents page");

  ToolButtData.XPos[12] = 274;
  ToolButtData.WParam[12] = CM_MNUHEABOT;
  ToolButtData.LParam[12] = 0;
  ToolButtData.SentWindow[12] = HWindow;
  ToolButtData.StatWindow[12] = PStatbar(StatbarHan)->HWindow;
  CopyBuffer(ToolButtData.UpText[12], "");
  CopyBuffer(ToolButtData.DownText[12], "Display information about Rail Control");

#if !defined(DIAG_SKIP_TOOLBAR)
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Toolbar:loadbitmaps", this);
#endif
  // Load toolbar button bitmaps
  ToolbarBitmaps = ::LoadBitmap(GetModule()->GetHandle(), TEXT("TB_MAINWIN"));

  // Then create the toolbar
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Toolbar:create:pre", this);
#endif
  ToolbarHan = new TToolbar(this, 12, ToolButtData, ToolbarBitmaps);
  ToolbarHan->Create();
  ToolbarExist = TRUE;
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Toolbar:create:post", ToolbarHan);
#endif

  // Modify the buttons - hide button 2
  if (TToolbutton* button = ToolbarHan->GetButton(2))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(3))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(4))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(2))  ::ShowWindow(button->HWindow, FALSE);
#else
  ToolbarExist = FALSE;
  ToolbarBitmaps = NULL;
  ToolbarHan = NULL;
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Toolbar:skipped", this, "DIAG_SKIP_TOOLBAR");
#endif
#endif

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

#if !defined(DIAG_SKIP_LAYOUT)
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Layout:create:pre", this);
#endif
  DisplayHan = new TLayout(this, 0, YPos, Width, Height);
  DisplayHan->Create();
  DisplayExist = TRUE;
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Layout:create:post", DisplayHan);
#endif
#else
  DisplayExist = FALSE;
  DisplayHan = NULL;
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:Layout:skipped", this, "DIAG_SKIP_LAYOUT");
#endif
#endif

#if !defined(DIAG_SKIP_MENU_COMMANDS)
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
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Arrival:pre", this);
#endif
#if !defined(DIAG_SKIP_MENU_COMMANDS)
    CheckMenuItem(TheMenu, CM_WINARRIVA, MF_BYCOMMAND | MF_UNCHECKED);
#endif
    CMWinArriva();
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Arrival:post", ArrivalHan);
#endif
  }

  // Check to see if we need to create a departure window
  if (BOOL(GetPrivateProfileInt("Departure Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new departure window
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Departure:pre", this);
#endif
#if !defined(DIAG_SKIP_MENU_COMMANDS)
    CheckMenuItem(TheMenu, CM_WINDEPART, MF_BYCOMMAND | MF_UNCHECKED);
#endif
    CMWinDepart();
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Departure:post", DeparturHan);
#endif
  }

  // Check to see if we need to create a platform window
  if (BOOL(GetPrivateProfileInt("Platform Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new platform window
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Platform:pre", this);
#endif
#if !defined(DIAG_SKIP_MENU_COMMANDS)
    CheckMenuItem(TheMenu, CM_WINPLATFO, MF_BYCOMMAND | MF_UNCHECKED);
#endif
    CMWinPlatfo();
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Platform:post", PlatformHan);
#endif
  }

  // Check to see if we need to create a locoyard window
  if (BOOL(GetPrivateProfileInt("Locoyard Window", "Exists", 1, INIFILENAME)))
  {
    // Create a new locoyard window
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Locoyard:pre", this);
#endif
#if !defined(DIAG_SKIP_MENU_COMMANDS)
    CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_UNCHECKED);
#endif
    CMWinLocoya();
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoChild:Locoyard:post", LocoyardHan);
#endif
  }

  // Check to see if we should optimize the display
  if (StartOptim)
  {
#if defined(DIAG_SETUP_TRACE)
    LogSetupEvent("SetupWindow:AutoCommand:CM_OPTOPTIMI", this);
#endif
    CMOptOptimi();
  }

  // Disable the menu items
#if !defined(DIAG_SKIP_MENU_COMMANDS)
  EnableMenuItem(TheMenu, CM_MNUFIPAUS, MF_GRAYED);
  EnableMenuItem(TheMenu, CM_MNUFISTOP, MF_GRAYED);
#if defined(DIAG_SETUP_TRACE)
  LogSetupPointer("SetupWindow:MenuDisabled", this, TheMenu, "menu");
#endif
#endif
#else
  TRC_ALT((TB, "SetupWindow DIAG_SKIP_MENU_COMMANDS active; skipping INI load and auto window creation"));
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:AutoChildren:skipped", this, "DIAG_SKIP_MENU_COMMANDS");
#endif
#endif

  // Close the title window
#if !defined(DIAG_SKIP_CLOSE_TITLE)
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:TitleWin:close", this);
  if (TitleWin != NULL)
  {
    LogSetupPointer("SetupWindow:TitleWin:pre-close", this, TitleWin->GetHandle(), "hwnd");
  }
#endif
  TitleWin->CloseWindow();
#if defined(DIAG_SETUP_TRACE)
  if (TitleWin != NULL)
  {
    LogSetupPointer("SetupWindow:TitleWin:closed", this, TitleWin->GetHandle(), "hwnd");
  }
#endif
#else
#if defined(DIAG_SKIP_MENU_COMMANDS)
  TRC_ALT((TB, "SetupWindow DIAG_SKIP_MENU_COMMANDS active; skipping INI load and auto window creation"));
#if defined(DIAG_SETUP_TRACE)
  LogSetupEvent("SetupWindow:TitleWin:skipped", this, "DIAG_SKIP_CLOSE_TITLE");
#endif
#endif
#endif

#if defined(DEBUG_AUTOSTART_NEW_GAME)
  {
    static bool QueuedStart = false;
    if (!QueuedStart)
    {
      QueuedStart = true;
      ::PostMessage(HWindow, WM_COMMAND, CM_MNUFILNEW, 0);
    }
  }
#endif
}


bool TMainWindow::CanClose()
{
  // Call file saving routine
  QuitProgram();

  // Call ancestor method
  TFrameWindow::CanClose();

  // Pass return value
  return TRUE;
}


void TMainWindow::CM_FileExit()
{
  ::SendMessage(GetHandle(), WM_CLOSE, 0, 0);
}
void TMainWindow::QuitProgram()
{
  char  TextString[50];

  // This is the intialization file saving routine
  if (SaveOnExit)
  {
    // Main window size/position
    _itoa_s(Attr.X, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "X position", TextString, INIFILENAME);
    _itoa_s(Attr.Y, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Y position", TextString, INIFILENAME);
    _itoa_s(Attr.W, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Width", TextString, INIFILENAME);
    _itoa_s(Attr.H, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Height", TextString, INIFILENAME);

    // Arrival window details
    if (ArrivalHan != NULL)
    {
      WritePrivateProfileString("Arrival Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Arrival Window", "Exists", "0", INIFILENAME);
    }

    _itoa_s(ArrivalX, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Arrival Window", "X position", TextString, INIFILENAME);
    _itoa_s(ArrivalY, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Arrival Window", "Y position", TextString, INIFILENAME);

    // Departure window details
    if (DeparturHan != NULL)
    {
      WritePrivateProfileString("Departure Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Departure Window", "Exists", "0", INIFILENAME);
    }

    _itoa_s(DeparturX, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Departure Window", "X position", TextString, INIFILENAME);
    _itoa_s(DeparturY, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Departure Window", "Y position", TextString, INIFILENAME);

    // Platform window details
    if (PlatformHan != NULL)
    {
      WritePrivateProfileString("Platform Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Platform Window", "Exists", "0", INIFILENAME);
    }

    _itoa_s(PlatformX, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Platform Window", "X position", TextString, INIFILENAME);
    _itoa_s(PlatformY, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Platform Window", "Y position", TextString, INIFILENAME);

    // Locoyard window details
    if (LocoyardHan != NULL)
    {
      WritePrivateProfileString("Locoyard Window", "Exists", "1", INIFILENAME);
    }
    else
    {
      WritePrivateProfileString("Locoyard Window", "Exists", "0", INIFILENAME);
    }

    const int locoyardWidth = (LocoyardHan != NULL) ? LocoyardHan->Attr.W : 200;
    const int locoyardHeight = (LocoyardHan != NULL) ? LocoyardHan->Attr.H : 120;
    ClampWindowPosition(LocoyardX, LocoyardY, locoyardWidth, locoyardHeight);

    _itoa_s(LocoyardX, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Locoyard Window", "X position", TextString, INIFILENAME);
    _itoa_s(LocoyardY, TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Locoyard Window", "Y position", TextString, INIFILENAME);

    // Other information
    _itoa_s(int(StartOptim), TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Optimize on start", TextString, INIFILENAME);
    _itoa_s(int(DelayEnable), TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Enable delay", TextString, INIFILENAME);
    _itoa_s(int(SoundEnable), TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Enable sound", TextString, INIFILENAME);
    _itoa_s(int(LocoRefuel), TextString, sizeof(TextString), 10);
    WritePrivateProfileString("Main Window", "Loco refuel", TextString, INIFILENAME);
    _itoa_s(TimerSpeed, TextString, sizeof(TextString), 10);
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


void TMainWindow::EvSize(owl::uint sizeType, const TSize& size)
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
        ArrivalHan->ShowWindow(SW_SHOWMINIMIZED);
      }
      else
      {
        ArrivalHan->ShowWindow(SW_SHOWNORMAL);
      }
   }

   // If the Departure window exists...
   if (DeparturHan != NULL)
   {
     if (DeparturIcon)
     {
       PDepartur(DeparturHan)->ShowWindow(SW_SHOWMINIMIZED);
     }
     else
     {
       PDepartur(DeparturHan)->ShowWindow(SW_SHOWNORMAL);
     }
   }

   // If the Platform window exists...
   if (PlatformHan != NULL)
   {
    if (PlatformIcon)
    {
      PPlatform(PlatformHan)->ShowWindow(SW_SHOWMINIMIZED);
    }
    else
    {
      PPlatform(PlatformHan)->ShowWindow(SW_SHOWNORMAL);
    }
   }

   // If the Locoyard window exists...
   if (LocoyardHan != NULL)
   {
    if (LocoyardIcon)
    {
      PLocoyard(LocoyardHan)->ShowWindow(SW_SHOWMINIMIZED);
    }
    else
    {
      PLocoyard(LocoyardHan)->ShowWindow(SW_SHOWNORMAL);
    }
   }
  }

  // Get current windows state
  MainWinIcon = ::IsIconic(HWindow);
}

void TMainWindow::EvMove(const TPoint &clientOrigin)
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


void TMainWindow::EvMenuSelect(owl::uint, owl::uint, HMENU)
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
  CopyBuffer(OldText, StatbarHan->GetText());

  switch (Msg.WParam)
  {
   case CM_MNUFILNEW       : CopyBuffer(TextString, "Start a new session"); break;
   case CM_MNUFIPAUS       : {
        if (GamePaused)
                      {
          CopyBuffer(TextString, "Continue the current session");
        }
        else
        {
          CopyBuffer(TextString, "Pause the current session");
        }
                      break;
                    }
   case CM_MNUFISTOP       : CopyBuffer(TextString, "Stop the current session"); break;
   case CM_MNUFISETD       : CopyBuffer(TextString, "Set the RCD data file name"); break;
   case CM_MNUFIEXIT       : CopyBuffer(TextString, "Exit from Rail Control"); break;
   case CM_OPTOPTIMI       : CopyBuffer(TextString, "Optimize the display of all windows"); break;
   case CM_OPTCONFIG       : CopyBuffer(TextString, "Configure game and program options"); break;
   case CM_WINARRIVA       : CopyBuffer(TextString, "Display or hide the Arrivals window"); break;
   case CM_WINDEPART       : CopyBuffer(TextString, "Display or hide the Departures window"); break;
   case CM_WINPLATFO       : CopyBuffer(TextString, "Display or hide the Platform window"); break;
   case CM_WINLOCOYA       : CopyBuffer(TextString, "Display or hide the Locoyard window"); break;
   case CM_MNUHECTNT       : CopyBuffer(TextString, "Display the help contents page"); break;
   case CM_MNUHEHELP       : CopyBuffer(TextString, "Help on using Windows help"); break;
   case CM_MNUHEABOT       : CopyBuffer(TextString, "Display information about Rail Control"); break;

   default:
    if (::IsIconic(HWindow))
    {
  CopyBuffer(TextString, OldText);
    }
    else
    {
      CopyBuffer(TextString, "");
    }
  }

  // Check for main menu selections
  if ((HMENU(Msg.WParam) == hMenu1) && (hMenu1 != 0))  CopyBuffer(TextString, "Game menu");
  if ((HMENU(Msg.WParam) == hMenu2) && (hMenu2 != 0))  CopyBuffer(TextString, "Options menu");
  if ((HMENU(Msg.WParam) == hMenu3) && (hMenu3 != 0))  CopyBuffer(TextString, "Window menu");
  if ((HMENU(Msg.WParam) == hMenu4) && (hMenu4 != 0))  CopyBuffer(TextString, "Help menu");

  // Update the status bar
  ::SendMessage(StatbarHan->HWindow,
      SB_SETTEXT,
          0,
      (LPARAM) ((LPSTR) TextString));
         */
}


void TMainWindow::Paint(TDC& dc, bool erase, TRect& rect)
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
  if (DisplayExist && DisplayHan != NULL)
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
  // Guard against null display handle
  if (DisplayHan == NULL) return;

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
    if (TToolbutton* button = ToolbarHan->GetButton(10))  button->Enable(TRUE);
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
  // Pre-measure and size child windows to avoid initial size jumps
  if (ArrivalHan != NULL)    ArrivalHan->UpdateDisplay(0, FALSE);
  if (DeparturHan != NULL)   DeparturHan->UpdateDisplay(0, FALSE);
  if (PlatformHan != NULL)   PlatformHan->UpdateDisplay(0, FALSE);
  if (LocoyardHan != NULL)   LocoyardHan->UpdateDisplay(0, FALSE);
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
  if (TToolbutton* button = ToolbarHan->GetButton(2))  button->Enable(TRUE);
  if (TToolbutton* button = ToolbarHan->GetButton(3))  button->Enable(TRUE);
  if (TToolbutton* button = ToolbarHan->GetButton(4))  button->Enable(TRUE);
  if (TToolbutton* button = ToolbarHan->GetButton(2))  ::ShowWindow(button->HWindow, FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(3))  ::ShowWindow(button->HWindow, TRUE);

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
  {
    TStart dlg(this, TEXT("START"));
    dlg.Execute();
  }


  // Create a new timer
  ::SetTimer(DisplayHan->HWindow, ID_TIMER, 100, NULL);
}


void TMainWindow::CMMnuFiPaus()
{
  // Guard against null display handle
  if (DisplayHan == NULL) return;

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
    if (TToolbutton* button = ToolbarHan->GetButton(2))  ::ShowWindow(button->HWindow, FALSE);
    if (TToolbutton* button = ToolbarHan->GetButton(3))  ::ShowWindow(button->HWindow, TRUE);
  }
  else
  {
    ::KillTimer(DisplayHan->HWindow, ID_TIMER);
    GamePaused = TRUE;

    // Modify the menu as well
    ModifyMenu(TheMenu, CM_MNUFIPAUS, MF_BYCOMMAND | MF_STRING, CM_MNUFIPAUS, "&Restart");
    ::DrawMenuBar(HWindow);
    if (TToolbutton* button = ToolbarHan->GetButton(2))  ::ShowWindow(button->HWindow, TRUE);
    if (TToolbutton* button = ToolbarHan->GetButton(3))  ::ShowWindow(button->HWindow, FALSE);
  }
}


void TMainWindow::CMMnuFiStop()
{
  // Guard against null display handle
  if (DisplayHan == NULL) return;

  HMENU TheMenu;

  // Stop a session currently in progress
  GameInProgress = FALSE;
  ::KillTimer(DisplayHan->HWindow, ID_TIMER);

  // Disable the pause option
  TheMenu = ::GetMenu(HWindow);
  EnableMenuItem(TheMenu, CM_MNUFIPAUS, MF_GRAYED);
  EnableMenuItem(TheMenu, CM_MNUFISTOP, MF_GRAYED);

  // Update the toolbar
  if (TToolbutton* button = ToolbarHan->GetButton(2))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(3))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(4))  button->Enable(FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(2))  ::ShowWindow(button->HWindow, FALSE);
  if (TToolbutton* button = ToolbarHan->GetButton(3))  ::ShowWindow(button->HWindow, TRUE);

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
  CopyBuffer(szName, DataFileName);

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
      FormatBuffer(buf, "%ld", Errval);
      AppendString(Errstr, buf);
      ::MessageBox(HWindow, Errstr, APPNAME, MB_OK|MB_ICONSTOP);
    }

    // Update the status bar
    ::SendMessage(StatbarHan->HWindow, SB_SETTEXT, 0, (LPARAM) ((LPSTR) ""));
  }
  else
  {
    // Everything is OK
    CopyBuffer(DataFileName, szName);

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
  ::GetWindowRect(HWindow, &lWinRect);
  MainWinX = lWinRect.left;
  MainWinY = lWinRect.top;

  struct LayoutGuard
  {
    TMainWindow* Frame;
    explicit LayoutGuard(TMainWindow* owner)
      : Frame(owner)
    {
      Frame->mLayoutInProgress = TRUE;
    }

    ~LayoutGuard()
    {
      Frame->mLayoutInProgress = FALSE;
    }
  } layoutGuard(this);

  auto moveWindowAndMeasure = [](HWND handle, BOOL iconFlag, int x, int y, int width, int height, int& outX, int& outY) -> RECT
  {
    RECT result = {x, y, x + width, y + height};
    outX = x;
    outY = y;

    if (handle == NULL)
    {
      return result;
    }

    if (iconFlag)
    {
      ::OpenIcon(handle);
    }

    ::MoveWindow(handle, x, y, width, height, TRUE);

    if (::GetWindowRect(handle, &result) != 0)
    {
      outX = result.left;
      outY = result.top;
    }
    else
    {
      result.left   = x;
      result.top    = y;
      result.right  = x + width;
      result.bottom = y + height;
    }

    return result;
  };

  switch (TheRect.right)
  {
    case 640:
    {
      const int mainWidth = 405;
      const int mainHeight = 360;
      MainWinX = 0;
      MainWinY = 0;
      lWinRect = moveWindowAndMeasure(HWindow, FALSE, MainWinX, MainWinY, mainWidth, mainHeight, MainWinX, MainWinY);

      const int mainRight = lWinRect.right;
      const int mainBottom = lWinRect.bottom;
      int stackX = mainRight;
      int stackY = lWinRect.top;

      // Arrival window sits directly beneath the main window.
      if (ArrivalHan != NULL)
      {
        ArrivalX = MainWinX;
        ArrivalY = mainBottom;
        moveWindowAndMeasure(ArrivalHan->HWindow, ArrivalIcon,
                             ArrivalX, ArrivalY,
                             ArrivalHan->Attr.W, ArrivalHan->Attr.H,
                             ArrivalX, ArrivalY);
      }

      // Right-column windows stacked without gaps.
      if (DeparturHan != NULL)
      {
        DeparturX = stackX;
        DeparturY = stackY;
        RECT departRect = moveWindowAndMeasure(DeparturHan->HWindow, DeparturIcon,
                                               DeparturX, DeparturY,
                                               DeparturHan->Attr.W, DeparturHan->Attr.H,
                                               DeparturX, DeparturY);
        stackY = departRect.bottom;
      }

      if (PlatformHan != NULL)
      {
        PlatformX = stackX;
        PlatformY = stackY;
        RECT platformRect = moveWindowAndMeasure(PlatformHan->HWindow, PlatformIcon,
                                                 PlatformX, PlatformY,
                                                 PlatformHan->Attr.W, PlatformHan->Attr.H,
                                                 PlatformX, PlatformY);
        stackY = platformRect.bottom;
      }

      if (LocoyardHan != NULL)
      {
        LocoyardX = stackX;
        LocoyardY = stackY;
        moveWindowAndMeasure(LocoyardHan->HWindow, LocoyardIcon,
                             LocoyardX, LocoyardY,
                             LocoyardHan->Attr.W, LocoyardHan->Attr.H,
                             LocoyardX, LocoyardY);
      }
    }
    break;

    case 800:
    {
      const int mainWidth = 550;
      const int mainHeight = 373;
      MainWinX = 0;
      MainWinY = 0;
      lWinRect = moveWindowAndMeasure(HWindow, FALSE, MainWinX, MainWinY, mainWidth, mainHeight, MainWinX, MainWinY);

      const int mainRight = lWinRect.right;
      const int mainBottom = lWinRect.bottom;
      int stackX = mainRight;
      int stackY = lWinRect.top;

      if (ArrivalHan != NULL)
      {
        ArrivalX = MainWinX;
        ArrivalY = mainBottom;
        moveWindowAndMeasure(ArrivalHan->HWindow, ArrivalIcon,
                             ArrivalX, ArrivalY,
                             ArrivalHan->Attr.W, ArrivalHan->Attr.H,
                             ArrivalX, ArrivalY);
      }

      if (DeparturHan != NULL)
      {
        DeparturX = stackX;
        DeparturY = stackY;
        RECT departRect = moveWindowAndMeasure(DeparturHan->HWindow, DeparturIcon,
                                               DeparturX, DeparturY,
                                               DeparturHan->Attr.W, DeparturHan->Attr.H,
                                               DeparturX, DeparturY);
        stackY = departRect.bottom;
      }

      if (PlatformHan != NULL)
      {
        PlatformX = stackX;
        PlatformY = stackY;
        RECT platformRect = moveWindowAndMeasure(PlatformHan->HWindow, PlatformIcon,
                                                 PlatformX, PlatformY,
                                                 PlatformHan->Attr.W, PlatformHan->Attr.H,
                                                 PlatformX, PlatformY);
        stackY = platformRect.bottom;
      }

      if (LocoyardHan != NULL)
      {
        LocoyardX = stackX;
        LocoyardY = stackY;
        moveWindowAndMeasure(LocoyardHan->HWindow, LocoyardIcon,
                             LocoyardX, LocoyardY,
                             LocoyardHan->Attr.W, LocoyardHan->Attr.H,
                             LocoyardX, LocoyardY);
      }
    }
    break;

    case 1024:
    default:
    {
      // Modern displays: arrange dynamically to avoid gaps/overlaps
      lWidth  = (Attr.W > 720) ? Attr.W : 720;
      lHeight = (Attr.H > 480) ? Attr.H : 480;

      lWinRect = moveWindowAndMeasure(HWindow, FALSE, MainWinX, MainWinY, lWidth, lHeight, MainWinX, MainWinY);

      const int mainRight = lWinRect.right;
      const int mainBottom = lWinRect.bottom;
      int stackX = mainRight;
      int stackY = lWinRect.top;

      if (PlatformHan != NULL)
      {
        PlatformX = stackX;
        PlatformY = stackY;
        RECT platformRect = moveWindowAndMeasure(PlatformHan->HWindow, PlatformIcon,
                                                 PlatformX, PlatformY,
                                                 PlatformHan->Attr.W, PlatformHan->Attr.H,
                                                 PlatformX, PlatformY);
        stackY = platformRect.bottom;
      }

      if (DeparturHan != NULL)
      {
        DeparturX = stackX;
        DeparturY = stackY;
        RECT departRect = moveWindowAndMeasure(DeparturHan->HWindow, DeparturIcon,
                                               DeparturX, DeparturY,
                                               DeparturHan->Attr.W, DeparturHan->Attr.H,
                                               DeparturX, DeparturY);
        stackY = departRect.bottom;
      }

      if (LocoyardHan != NULL)
      {
        LocoyardX = stackX;
        LocoyardY = stackY;
        moveWindowAndMeasure(LocoyardHan->HWindow, LocoyardIcon,
                             LocoyardX, LocoyardY,
                             LocoyardHan->Attr.W, LocoyardHan->Attr.H,
                             LocoyardX, LocoyardY);
      }

      if (ArrivalHan != NULL)
      {
        ArrivalX = MainWinX;
        ArrivalY = mainBottom;
        moveWindowAndMeasure(ArrivalHan->HWindow, ArrivalIcon,
                             ArrivalX, ArrivalY,
                             ArrivalHan->Attr.W, ArrivalHan->Attr.H,
                             ArrivalX, ArrivalY);
      }
    }
    break;
  }

  mOptimizedLayoutActive = TRUE;
}


void TMainWindow::HandleChildResize(TWindow* child, const RECT& oldRect, const RECT& newRect)
{
  const int oldWidth = oldRect.right - oldRect.left;
  const int oldHeight = oldRect.bottom - oldRect.top;
  const int newWidth = newRect.right - newRect.left;
  const int newHeight = newRect.bottom - newRect.top;

  if ((oldWidth == newWidth) && (oldHeight == newHeight))
  {
    return;
  }

  if (!mOptimizedLayoutActive)
  {
    return;
  }

  mResizeCoalescePending = TRUE;
  ::SetTimer(HWindow, ID_CHILD_RESIZE_COALESCE, 75, NULL);
}


void TMainWindow::QueueStageTelemetryUpdate(const StageTelemetryUpdate& update)
{
  if (DisplayHan != NULL)
  {
    DisplayHan->QueueStageTelemetryUpdate(update);
  }
}


void TMainWindow::EvTimer(owl::uint timerId)
{
  if (timerId == ID_CHILD_RESIZE_COALESCE)
  {
    ::KillTimer(HWindow, ID_CHILD_RESIZE_COALESCE);

    if (!mResizeCoalescePending)
    {
      return;
    }

    if (mLayoutInProgress)
    {
      ::SetTimer(HWindow, ID_CHILD_RESIZE_COALESCE, 75, NULL);
      return;
    }

    mResizeCoalescePending = FALSE;
    if (mOptimizedLayoutActive)
    {
      CMOptOptimi();
    }
    return;
  }

  TFrameWindow::EvTimer(timerId);
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
  {
    TConfigur dlg(this, TEXT("CONFIG"));
    dlg.Execute();
  }

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
      ArrivalHan->Create();
      // Size correctly before showing to avoid jump
      ArrivalHan->UpdateDisplay(0, TRUE);
      ArrivalHan->ShowWindow(SW_SHOWNORMAL);
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
      DeparturHan->Create();
      DeparturHan->UpdateDisplay(0, TRUE);
      DeparturHan->ShowWindow(SW_SHOWNORMAL);
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
      PlatformHan->Create();
      PlatformHan->UpdateDisplay(0, TRUE);
      PlatformHan->ShowWindow(SW_SHOWNORMAL);
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
      ClampWindowPosition(LocoyardX, LocoyardY, 200, 120);
      LocoyardHan = new TLocoyard(this, LocoyardX, LocoyardY);
      LocoyardHan->Create();
      LocoyardHan->UpdateDisplay(0, TRUE);
      LocoyardHan->ShowWindow(SW_SHOWNORMAL);
      SetFocus();
    }
    CheckMenuItem(TheMenu, CM_WINLOCOYA, MF_BYCOMMAND | MF_CHECKED);
  }
  else
  {
    ::MessageBeep(MB_ICONINFORMATION);
    ::MessageBox(HWindow,
                 "The current layout does not provide a Locoyard.",
                 APPNAME,
                 MB_OK | MB_ICONINFORMATION);
  }

  /**************************************************************************/
  /* Decide whether to enable or disable the locoyard toolbar button.       */
  /**************************************************************************/
  if (LocoyardEnabled)
  {
    if (TToolbutton* button = ToolbarHan->GetButton(10))  button->Enable(TRUE);
  }
  else
  {
    if (TToolbutton* button = ToolbarHan->GetButton(10))  button->Enable(FALSE);
  }
}


void TMainWindow::CMMnuHeCtnt()
{

  // Display help contents
  ShowHelpPlaceholder(HWindow, HELP_CONTENTS);
}


void TMainWindow::CMMnuHeHelp()
{

  // Display help contents
  ShowHelpPlaceholder(HWindow, HELP_HELPONHELP);
}


void TMainWindow::CMMnuHeAbot()
{

  // Display about box
  {
    TAbout dlg(this, TEXT("AboutBox"));
    dlg.Execute();
  }
}



DEFINE_RESPONSE_TABLE1( TMainWindow, TFrameWindow )
    EV_WM_SIZE,
    EV_WM_MOVE,
    EV_WM_TIMER,
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




#if defined(_MSC_VER)
#pragma optimize("", on)
#endif
