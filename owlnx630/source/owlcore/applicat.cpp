//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.31.2.1 $
//$Author: jogybl $
//$Date: 2010-05-17 07:12:33 $
//
// Implementation of class TApplication. This defines the basic behavior
// for ObjectWindows applications.
//----------------------------------------------------------------------------

#if defined(__HPUX_SOURCE)
#include <owl/private/memory.h>
#include <owl/exbase.h>
#endif

#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif
#if !defined(OWL_DOCMANAG_H)
# include <owl/docmanag.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_MESSAGEB_H)
# include <owl/messageb.h>
#endif
#if !defined(OWL_WINDOW_RH)
# include <owl/window.rh>
#endif
#if !defined(OWL_TOOLTIP_H)
# include <owl/tooltip.h>
#endif
#if defined(BI_COMP_BORLANDC) && !defined(__BWCC_H)
#  if (__BORLANDC__ < 0x530)
#   include <bwcc.h>
#  else
#   include <owl/private/bwcc.h>
#  endif
#endif
#if !defined(__OWL_TRACEWND_H) 
#include <owl/tracewnd.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#  include <owl/thread.h>
#endif

#if defined(BI_STD_EXCEPTIONS)
__OWL_USING_STD_NAMESPACE
#endif

__OWL_BEGIN_NAMESPACE
OWL_DIAGINFO;
__OWL_END_NAMESPACE

////////////////////////////////////////////////////////////////////////////////////
//
// Here globally visible declarations
//
#if defined(SECTION) && SECTION != 1
__OWL_BEGIN_NAMESPACE
DIAG_DECLARE_GROUP(OwlApp);        // General Application diagnostic group
__OWL_END_NAMESPACE
#endif

__OWL_BEGIN_NAMESPACE

/////////////////////////////////////////////////////
// global definitions for both modules if sectioning
struct TEnumInfo {
  HWND      ModalWnd;    // The window being made modal if needed
  short     Count;      // Count of windows in Wnds below
  HWND*     Wnds;        // list of windows that were disabled
  WNDPROC*  PrevWndProc;//
};

//////////////////////////////////////////////////////////////////////////////////
// Multithread support 
//
class TWaitHook{
  public:
    TWaitHook();
    ~TWaitHook();
  
    void SetCursor(TCursor* cursor, TAutoDelete = AutoDelete);

  private:
    void FreeCursors();
    void SetWaitCursor();

  private:
    TEnumInfo    Subclass;
    TEnumInfo*  LastStackTop;
    uint        Count;
    uint        HideCaretCount;
    TCursor*    Cursor;
    bool        DeleteOnClose;

    TResult                  WaitWndMethod(HWND, uint, TParam1, TParam2);
    TResult                  DefWndProc (HWND, uint, TParam1, TParam2);
  public:
    static TResult CALLBACK WaitWndProc (HWND, uint, TParam1, TParam2);
};

//
struct TEnumInfoStr 
#if defined(BI_MULTI_THREAD_RTL)
: public TLocalObject
#endif
{
  
  TEnumInfoStr():Info(0),Hook(0),Top(0)
  {
  }
  ~TEnumInfoStr()
    {
    }

  TEnumInfo* GetInfo()                { return Info; }
  void       SetInfo(TEnumInfo* info) { Info = info; }

  TEnumInfo*    Info;
  TWaitHook*    Hook;
  TWaitCursor*  Top;

#if defined(BI_MULTI_THREAD_RTL)
  TMRSWSection  Lock;  
#endif
};
//
//
#if !defined(SECTION)
static TEnumInfoStr& GetEnumInfo();
#else 
TEnumInfoStr& GetEnumInfo();
#endif

//
//
//
#if !defined(BI_MULTI_THREAD_RTL)
struct TApplicatData {
#else
struct TApplicatData : public TLocalObject {
#endif
  TApplicatData();
  ~TApplicatData();
  
  TMessageBox OwlMsgBox;

#if defined(BI_MULTI_THREAD_RTL)
  TMRSWSection  Lock;
#endif
};

//
#  if !defined(SECTION)
static TApplicatData& GetApplicatData();
#else 
TApplicatData& GetApplicatData();
#  endif

//
//
//
#if defined(BI_MULTI_THREAD_RTL) //TMRSWSection::TLock lock(GetEnumInfo().Lock);
#define LOCKENUMINFO(l,s) TMRSWSection::TLock __lock(l,s) 
#else
#define LOCKENUMINFO(l,s)
#endif

//
//
//
#if defined(BI_MULTI_THREAD_RTL) //GetApplicatData().Lock,true
#define LOCKAPPLDATA(l,s) TMRSWSection::TLock __lock(l,s)
#else
#define LOCKAPPLDATA(l,s)
#endif


#if !defined(SECTION) || SECTION == 1

DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlApp, 1, 0);

// -----------------------------------------------------------------------------
static int 
OWLDefaultMsgBox(HWND wnd, LPCTSTR text, LPCTSTR caption, uint type)
{
  TApplication* app = OWLGetAppDictionary().GetApplication(0);
  return ::MessageBoxEx(wnd, text, caption, type, app ? app->GetLangId() : LangNeutral);
}
// -----------------------------------------------------------------------------
TApplicatData::TApplicatData()
: OwlMsgBox(OWLDefaultMsgBox)
{
}
// -----------------------------------------------------------------------------
TApplicatData::~TApplicatData()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// multithread support
//
#  if !defined(SECTION)
static 
#  endif
TApplicatData& GetApplicatData()
{
//#if defined(BI_MULTI_THREAD_RTL)
//  static TProcessContainer<TApplicatData> __ApplData;
//#else
  static TApplicatData __ApplData;
//#endif
  return __ApplData;
}
//
#  if !defined(SECTION)
static 
#  endif
TEnumInfoStr& GetEnumInfo()
{
//#if defined(BI_MULTI_THREAD_RTL)
//  static TProcessContainer<TEnumInfoStr> __EnumInfo;
//#else
  static TEnumInfoStr  __EnumInfo;
//#endif
  return __EnumInfo;
}
// -----------------------------------------------------------------------------
//
// Static members for initialization of app prior to initial construction
//
HINSTANCE  TApplication::InitHInstance;
HINSTANCE  TApplication::InitHPrevInstance;
int        TApplication::InitCmdShow;

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 2

// -----------------------------------------------------------------------------
_OWLFUNC(TMessageBox) 
SetDefMessageBox(TMessageBox newMsgBox)
{
  TApplicatData& data = GetApplicatData();
  LOCKAPPLDATA(data.Lock,true);
  TMessageBox msgBox = data.OwlMsgBox;
  data.OwlMsgBox = newMsgBox;
  return msgBox;
}
// -----------------------------------------------------------------------------
_OWLFUNC(int) 
OWLMessageBox(HWND wnd, LPCTSTR text, LPCTSTR caption, uint type)
{
  TApplication* app = OWLGetAppDictionary().GetApplication(0);
  if(!caption && app)
    caption =app->GetName();

  // If no parent is supplied need to use task modal to disable all toplevel
  // windows in this task.
  //
  if (!wnd && !(type & MB_SYSTEMMODAL))
    type |= MB_TASKMODAL;
  
  return (GetApplicatData().OwlMsgBox)(wnd, text, caption, type);
}
// -----------------------------------------------------------------------------
_OWLFUNC(int)
OWLMessageBox(TWindow* wnd, TResId resId, LPCTSTR caption, uint type, TModule* module)
{
  if(!module){
    if(wnd)
      module = wnd->GetModule();
  }
  if(!module)
    module = gModule;
  if(!caption && wnd){
    if(wnd)
      caption = wnd->GetApplication()->GetName();
    else if(OWLGetAppDictionary().GetApplication(0))
      caption = OWLGetAppDictionary().GetApplication(0)->GetName();
  }

  owl_string text = module->LoadString((uint)(LPTSTR)resId);

  return OWLMessageBox((HWND)*wnd, text.c_str(), caption, type);
}
#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 3
// -----------------------------------------------------------------------------
//
//
owl_string&
TApplication::GetInitCmdLine()
{
  static owl_string InitCmdLine;
  return InitCmdLine;
};

//
// Constructor for use in OwlMain(). Gets members from statics set earlier by
// a call to InitWinMainParams() in Owl's WinMain.
//
TApplication::TApplication(LPCTSTR name, TModule*& module,
                           TAppDictionary* appDict)
:
  TModule(name, InitHInstance, GetInitCmdLine().c_str()),
  TMsgThread(TMsgThread::Current),
  // Copy over values that were stashed in static members before this instance
  // was constructed.
  //
  hPrevInstance(InitHPrevInstance), nCmdShow(InitCmdShow),
  DocManager(0), MainWindow(0),
  LangId(LangUserDefault),
  Tooltip(0),
#ifndef NO_MSGWAIT_MOD
  WaitCount(0),
  WaitHandles(NULL),
#endif
#if defined(OWL2_COMPAT)
  HAccTable(0),
#endif
  CmdLine(GetInitCmdLine()),
  BWCCOn(false),BWCCModule(0),
  Ctl3dOn(false),Ctl3dModule(0),
  CondemnedWindows(0),
  Dictionary(appDict ? appDict : &(OWLGetAppDictionary()))
{
  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication constructing @") << (void*)this);

  Dictionary->Add(this);
  module = this;

  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication constructed @") << (void*)this);
}
#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 4

//
// Constructor for use in user defined WinMain() when all the args are
// available
//
TApplication::TApplication(LPCTSTR         name,
                           HINSTANCE       instance,
                           HINSTANCE       prevInstance,
                           LPCTSTR         cmdLine,
                           int             cmdShow,
                           TModule*&       module,
                           TAppDictionary* appDict)
:
  TModule(name, instance, cmdLine),
  TMsgThread(TMsgThread::Current),
  hPrevInstance(prevInstance), nCmdShow(cmdShow),
  DocManager(0), MainWindow(0),
  LangId(LangUserDefault),
  Tooltip(0),
#ifndef NO_MSGWAIT_MOD
  WaitCount(0),
  WaitHandles(NULL),
#endif
#if defined(OWL2_COMPAT)
  HAccTable(0),
#endif
  CmdLine(cmdLine),
  BWCCOn(false),BWCCModule(0),
  Ctl3dOn(false),Ctl3dModule(0),
  CondemnedWindows(0),
  Dictionary(appDict ? appDict : &(OWLGetAppDictionary()))
{
  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication constructing @") << (void*)this);

  Dictionary->Add(this);
  module = this;

  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication constructed @") << (void*)this);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 5

//
//
//
TApplication::~TApplication()
{
  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication destructing @") << (void*)this);

  DeleteCondemned();

  // Unregister ourselves from the Ctl3d DLL and/or the BWCC DLL if they are
  // loaded.
  //
  if (Ctl3dModule) {
    Ctl3dModule->Unregister(*this);
    delete Ctl3dModule;
  }
  if (BWCCModule) {
    BWCCModule->IntlTerm();
    delete BWCCModule;
  }

  // Delete the main window if still present, may be destroyed but not deleted
  // Set MainWindow to 0 first to prevent it from calling ::PostQuitMessage
  //
  TWindow* mainWindow = SetMainWindow(0);
  if (mainWindow) {
    mainWindow->Destroy();
    delete mainWindow;
    delete Tooltip; // if SetMainWindow(0) need manually delete Tooltip
  }

  delete DocManager;

  // Remove this app from the application dictionary that it is in
  //
  Dictionary->Remove(this);

  TRACEX(OwlApp, OWL_CDLEVEL, _T("TApplication destructed @") << (void*)this);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 6

//
// Popup a mesasge box. Automatically deals with BWCC and Ctl3d if enabled for
// this app.
//
int
TApplication::MessageBox(HWND      wnd,
                         LPCTSTR   text,
                         LPCTSTR   caption,
                         uint     type)
{
  // Default caption to this application's name
  //
  if (!caption)
    caption = GetName();

  // If no parent is supplied need to use task modal to disable all toplevel
  // windows in this task.
  //
  if (!wnd && !(type & MB_SYSTEMMODAL))
    type |= MB_TASKMODAL;

  // Use the BWCC message box if BWCC is enabled
  //
#if defined(BI_COMP_BORLANDC)
  if (BWCCEnabled() && GetBWCCModule()) {
    return GetBWCCModule()->MessageBox( wnd, text, caption, type);
  }
  else
#endif
  // Otherwise, 3d-ize the message box if ctl3d is enabled
  //
#if !defined(BI_COMP_MSC)
  {
    EnableCtl3dAutosubclass(true);
    int retValue = (GetApplicatData().OwlMsgBox)(wnd, text, caption, type);
    EnableCtl3dAutosubclass(false);
    return retValue;
  }
#else
  return (GetApplicatData().OwlMsgBox)(wnd, text, caption, type);
#endif
}

//
//
//
int 
TApplication::MessageBox(HWND  wnd, uint resId, LPCTSTR caption, uint type, ...)
{
  PRECONDITION(resId);

  TTmpBuffer<_TCHAR> buffer(MAX_PATH);
  owl_string text = LoadString(resId);
  uint* nextArgument = &type;
        ++nextArgument;
  wvsprintf(buffer, text.c_str(), (va_list)(nextArgument));

  return MessageBox(wnd, buffer, caption, type);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 7

//
// Handle initialization for the first executing instance of the OWL
// application. Under Win32, every app instance is treated as the first.
//
// Derived classes can override this to perform app initialization, or they
// can use the derived class constructor.
//
void
TApplication::InitApplication()
{
  TRACEX(OwlApp, 1, _T("TApplication::InitApplication() called @") << (void*)this);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 8

//
// Handle initialization for each executing instance of the OWL application.
// Derived classes can override this to perform initialization for each
// instance.
//
// This default implementation calls InitMainWindow() to initialize the
// main window, and then if successful, creates and shows it. A
// TXInvalidMainWIndow exception is thrown if no main window is set during
// InitMainWindow()
//
void
TApplication::InitInstance()
{
  TMsgThread::InitInstance();

  TRACEX(OwlApp, 1, _T("TApplication::InitInstance() called @") << (void*)this);

  InitMainWindow();

  if (MainWindow) {
    MainWindow->SetFlag(wfMainWindow);
    MainWindow->Create();

#if defined(__TRACE) || defined(__WARN)
    TSystemMenu sysMenu (MainWindow->Handle);
    if(sysMenu.IsOK()){
      sysMenu.AppendMenu (MF_SEPARATOR);
      sysMenu.AppendMenu (MF_STRING, SCM_TRACE, _T("Diagnostic Window"));
    }
#endif

    MainWindow->ShowWindow(nCmdShow);
  }
  else
    TXInvalidMainWindow::Raise();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 9

//
// Initialize the application's main window. Derived classes should override
// this to construct, initialize and set the main window using SetMainWindow().
//
// Default main window is a plain TFrameWindow with a title that is the same
// as this application's name
//
void
TApplication::InitMainWindow()
{
  SetMainWindow(new TFrameWindow(0, GetName()));
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 10

//
// Handle termination for each executing instance of the application. Called
// at the end of a Run() with the final return status.
//
int
TApplication::TermInstance(int status)
{
  TRACEX(OwlApp, 1, _T("TApplication::TermInstance() called @") << (void*)this);

#if defined(__TRACE) || defined(__WARN)
  // the main window is being destroyed, so destroy the trace window...
  if (TTraceWindow::TraceWindow())
    delete TTraceWindow::TraceWindow();
#endif

  return TMsgThread::TermInstance(status);
}

//
// Set (or reset) the main window. Return, but do not destroy the previous
// main window.
//
TFrameWindow*
TApplication::SetMainWindow(TFrameWindow* window)
{
  if (MainWindow) {
    MainWindow->ClearFlag(wfMainWindow);
    uint32 style = MainWindow->GetExStyle();
    if (style & WS_EX_APPWINDOW)
      MainWindow->SetExStyle(style & ~WS_EX_APPWINDOW);
    if(Tooltip && Tooltip->GetParentO()==MainWindow)
      Tooltip->SetParent(0);
  }

  TFrameWindow* oldMainWindow = MainWindow;
  MainWindow = window;

  if (MainWindow) {
    MainWindow->SetFlag(wfMainWindow);
    uint32 style = MainWindow->GetExStyle();
    if (!(style & WS_EX_APPWINDOW))
      MainWindow->SetExStyle(style | WS_EX_APPWINDOW);
    // set new parent only if MainWindow created
    if(Tooltip && Tooltip->GetParentO()==0 && MainWindow->GetHandle())
      Tooltip->SetParent(MainWindow);
  }
  return oldMainWindow;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 11

//
// Set (or resets) the document manager, return the previous one if present
//
TDocManager*
TApplication::SetDocManager(TDocManager* docManager)
{
  TDocManager* oldDocManager = DocManager;
  DocManager = docManager;
  return oldDocManager;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 12

#if defined(BI_MULTI_THREAD_RTL)
//
// Override TEventHandler::Dispatch() to handle multi-thread synchonization
//
TResult TApplication::Dispatch(TEventInfo& info, TParam1 p1, TParam2 p2)
{
  TApplication::TQueueLock lock(*this);
  return TEventHandler::Dispatch(info, p1, p2);
}
#endif

//Jogy  to prevent error C3204: '_alloca' cannot be called from within a catch block under VC.NET
owl_string TApplication::ConvertA2W(LPCSTR string)
{
   _USES_CONVERSION;
   return (owl_string(_A2W(string)));
}


//
// Run this application, return when application terminates
//
// Initialize instances, create and display their main window (calls
// InitApplication for the first executing instance and calls InitInstance for
// all instances). Run the application's message loop. Each of the virtual
// functions called are expected to throw an exception if there is an error.
//
// Exceptions that are not handled, i.e. status remains non-zero, are
// propagated out of this function. Msg queue is still flushed & TermInstance
// called.
//
int
TApplication::Run()
{
  int status = 0;
  try {
    try {
      if (!MainWindow) {
        if (!hPrevInstance)
          InitApplication();
        InitInstance();
      }
      LoopRunning = true;
      status = MessageLoop();
    }
    catch (TXEndSession&) {
      throw;
    }
    catch (TXOwl& x) {
      status = x.Unhandled(this, 0);
      if (status)
        throw;
    }
    catch (TXBase& x) {
      status = Error(x, 0);
      if (status)
        throw;
    }
//  BC bad_cast and bad_typeid base classes independed classes
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
    catch (XBASE& x) {  // XBASE 
      status = Error(x, 0);
      if (status)
        throw;
    }
#endif
    catch (bad_cast& x) {
//      _USES_CONVERSION;      //Jogy is this needed?
			TXBase msg(ConvertA2W(typeid(x).name()));
      status = Error(msg, 0);
      if (status)
        throw;
    }
    catch (bad_typeid& x) {
//      _USES_CONVERSION;      //Jogy is this needed?
			TXBase msg(ConvertA2W(typeid(x).name()));
      status = Error(msg, 0);
      if (status)
        throw;
    }
// new STL defined base class exception and derived  bad_cast and bad_typeid 
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
    catch (XBASE& x) {
      status = Error(x, 0);
      if (status)
        throw;
    }
#endif
  }
  catch (...) {
    FlushQueue();
    LoopRunning = false;
    TermInstance(status);
    throw;
  }

  FlushQueue();

  LoopRunning = false;
  return TermInstance(status);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 13

//
// Start this application and return immediately. Used for component DLLs
//
// Initializes instances, creating and displaying their main window (calls
// InitApplication for the first executing instance and calls InitInstance for
// all instances). Each of the virtual functions called are expected to throw
// an exception if there is an error. Does not run message loop.
//
int
TApplication::Start()
{
  int status = 0;
  TRY {
    if (!hPrevInstance)
      InitApplication();
    InitInstance();
  }
  CATCH( (TXOwl& x) {status = x.Unhandled(this, 0);})
  CATCH( (TXBase& x) {status = Error(x, 0);})
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
  CATCH( (XBASE& x) {status = Error(x, 0);}) 
#endif
  CATCH( (bad_cast& x) {
    _USES_CONVERSION;
     TXBase msg(owl_string(ConvertA2W(typeid(x).name())));
    status = Error(msg, 0);
  })
  CATCH( (bad_typeid& x) {
    _USES_CONVERSION;
     TXBase msg(owl_string(ConvertA2W(typeid(x).name())));
    status = Error(msg, 0);
  })
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
  CATCH( (XBASE& x) {status = Error(x, 0);})
#endif
  return status;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 14

//
// General message loop: retrieves and processes messages from the OWL
// application's message queue using PumpWaitingMessages() until
// BreakMessageLoop becomes true. Catches and defers to handlers a number of
// exceptions.
//
// Call IdleAction() when there are no messages
//
int
TApplication::MessageLoop()
{
  long idleCount = 0;

  MessageLoopResult = 0;
  while (!BreakMessageLoop) { //!CQ Use ::MsgWaitMultipleObjects() ?
    try {
      if (!IdleAction(idleCount++))
#ifdef NO_MSGWAIT_MOD
        ::WaitMessage();             // allow system to go idle
#else
        {
        DWORD result = MsgWaitForMultipleObjects(WaitCount, WaitHandles, FALSE,
                                                 INFINITE, QS_ALLINPUT);
          if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + WaitCount)
          ObjectSignaled(WaitHandles[result - WAIT_OBJECT_0], false);
          else if (result >= WAIT_ABANDONED_0 && result < WAIT_ABANDONED_0 + WaitCount)
          ObjectSignaled(WaitHandles[result - WAIT_ABANDONED_0], true);
        }
#endif
      if (PumpWaitingMessages())     // pumps any waiting messages
        idleCount = 0;
    }
    catch (TXOwl& x) {
      MessageLoopResult = x.Unhandled(this, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
    catch (TXBase& x) {
      MessageLoopResult = Error(x, 0, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
#if defined(BI_COMP_BORLANDC) && !defined(BI_STD_EXCEPTIONS)
    catch (XBASE& x) {    // XBASE = exception for BI_STD_EXCEPTIONS
      MessageLoopResult = Error(x, 0, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
#endif
    catch (bad_cast& x) {
      _USES_CONVERSION;
       TXBase msg(owl_string(ConvertA2W(typeid(x).name())));
      MessageLoopResult = Error(msg, 0, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
    catch (bad_typeid& x) {
      _USES_CONVERSION;
       TXBase msg(owl_string(ConvertA2W(typeid(x).name())));
      MessageLoopResult = Error(msg, 0, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
#if !defined(BI_COMP_BORLANDC) || defined(BI_STD_EXCEPTIONS)
    catch (XBASE& x) {
      MessageLoopResult = Error(x, 0, IDS_OKTORESUME);
      if (MessageLoopResult != 0) {
        ::PostQuitMessage(MessageLoopResult);
        break;
      }
    }
#endif

  }
  BreakMessageLoop = false;
  return MessageLoopResult;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 15

//
// Called each time there are no messages in the queue. Idle count is
// incremented each time, & zeroed when messages are pumped. Return
// whether or not more processing needs to be done.
//
// Default behavior is to give the main window an opportunity to do idle
// processing by invoking its IdleAction() member function when
// "idleCount" is 0
//
bool
TApplication::IdleAction(long idleCount)
{
  TRACEX(OwlApp, 1, _T("TApplication::IdleAction() called @") << (void*)this <<
                    _T(" idleCount ") << idleCount);
  bool more = false;
  if (MainWindow)
    more = MainWindow->IdleAction(idleCount);

#if defined(__TRACE) || defined(__WARN)
  if (DiagIdleAction(idleCount))
    more = true;
#endif
  return more;
}

//
// Called for each message that is pulled from the queue, to perform all
// translation & dispatching.
// dispatched.
// Return true to drop out of pump
//
bool
TApplication::ProcessMsg(MSG& msg)
{
  // Let the app preprocess the message. If it is not eaten, then translate
  // it, & dispatch it. If no HWND, then first find/dispatch it directly
  // to the app (for PostAppMessage() functionality)
  //
  if (!ProcessAppMsg(msg)) {
    ::TranslateMessage(&msg);
    if (!msg.hwnd) {
      TEventInfo cmdEventInfo(msg.message, msg.wParam);
      if (Find(cmdEventInfo)) {
        Dispatch(cmdEventInfo, msg.wParam, msg.lParam);
        return true;
      }
      else {
        TEventInfo eventInfo(msg.message);
        if (Find(eventInfo)) {
          Dispatch(eventInfo, msg.wParam, msg.lParam);
          return true;
        }
      }
    }
    ::DispatchMessage(&msg);
    DeleteCondemned();
  }
  return false;
}

//
// Called after each message is pulled from the queue, and before it is
// dispatched. Return true if the message was handled completely here.
//
bool
TApplication::ProcessAppMsg(MSG& msg)
{
#if defined(__TRACE) || defined(__WARN)
  if (msg.message == WM_SYSCOMMAND && (msg.wParam & 0xFFF0) == SCM_TRACE){
    SystemCommandTrace();
    return true;
  }
#endif

  // Check if this message might be worth relaying to the tooltip window
  //
  if (Tooltip && Tooltip->IsWindow()) {
    if (msg.hwnd == *MainWindow || MainWindow->IsChild(msg.hwnd)) {
      if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
        Tooltip->RelayEvent(msg);
      }
    }
  }

  // Start with the window that the event was destined for and allow it
  // and its parent (and its parent...) an opportunity to preprocess the
  // event
  //
  for (HWND hWnd = msg.hwnd; hWnd; hWnd = ::GetParent(hWnd)) {
    // NT seems very picky about calls to ::GetWindowThreadProcessId
    // with the HWND of the desktop. Hence we'll abort this loop
    // when encountering the desktop. Specially useful when dealing with
    // DropDown[List] ComboBoxes thunked by OWL when hit the ComboLBox
    // window which is parented to the Desktop.
    //
    static HWND deskTopHwnd = ::GetDesktopWindow();    
    if (hWnd == deskTopHwnd)
      break;

    TWindow*  win = GetWindowPtr(hWnd);

    if (win && win->PreProcessMsg(msg))
      return true;
  }

#if defined(OWL2_COMPAT)
  // For compatability with OWL 2.0, check the application's accelerator
  // table if it is being used.
  //
  if (HAccTable && MainWindow)
    return ::TranslateAccelerator(MainWindow->GetHandle(), HAccTable, &msg);
#endif

  return false;
}

#ifndef NO_MSGWAIT_MOD

void TApplication::WaitOnObject(HANDLE handle, bool wait)
{
  bool exists = false;
  int index;

  if (WaitHandles)
    {
    for (int i = 0; i < static_cast<int>(WaitCount); i++) //JJH added static_cast
      if (WaitHandles[i] == handle)
        {
        exists = true;
        index = i;
        }
    }

  if (wait)
    {
    if (!exists)
      {
      LPHANDLE newHandles = new HANDLE[WaitCount + 1];
      for (int i = 0; i < static_cast<int>(WaitCount); i++) //JJH added static_cast
        newHandles[i] = WaitHandles[i];
      delete[] WaitHandles;
      WaitHandles = newHandles;
      WaitHandles[WaitCount] = handle;
      WaitCount++;
      }
    }
  else
    {
    if (exists)
      {
      if (WaitCount == 1)
        {
        delete[] WaitHandles;
        WaitHandles = NULL;
        WaitCount = 0;
        }
      else
        {
        LPHANDLE newHandles = new HANDLE[WaitCount - 1];

        int i;
        for (i = 0; i < index; i++)
          newHandles[i] = WaitHandles[i];
        for (i = index + 1; i < static_cast<int>(WaitCount); i++) //JJH added static_cast
          newHandles[i-1] = WaitHandles[i];
        delete[] WaitHandles;
        WaitHandles = newHandles;
        WaitCount--;
        }
      }
    } 
}

#endif

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 16

//
// Should call this after each message dispatch when TApplication's message
// loop is not being used.
//
void
TApplication::PostDispatchAction()
{
  DeleteCondemned();

  MSG msg;
  if (!::PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
    IdleAction(0);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 17

// Only 16-bit code was here

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 18
//
//
//
void
TApplication::EnableTooltip(bool enable)
{
  if (!Tooltip) {
    // To circumvent this scenario, we'll look for a window which is fairly
    // stable/rooted as owner of the tooltip. Ideally, we'll get the
    // application's main window.
    //
    TWindow* tipParent = MainWindow;
    if(!MainWindow->GetHandle())
      tipParent = 0;
    // Create and initialize tooltip
    //
    SetTooltip(new TTooltip(tipParent));
  }
  else {
    if (Tooltip->GetHandle())
      Tooltip->Activate(enable);
  }
}

//
// Set tooltip . Assume we now own the ToolTip
//
void
TApplication::SetTooltip(TTooltip* tooltip)
{
  // Cleanup; via Condemned list if tooltip was created
  //
  if (Tooltip) {
    if (Tooltip->GetHandle())
      Tooltip->SendMessage(WM_CLOSE);
    else
      delete Tooltip;
  }

  // Store new tooltip and create if necessary
  // Create only if MainWindows already created
  Tooltip = tooltip;
  if (Tooltip && !Tooltip->GetHandle() && 
      MainWindow && MainWindow->GetHandle()) {
    if(!Tooltip->GetParentO())
      Tooltip->SetParent(MainWindow);
    // Make sure tooltip is disabled so it does not take input focus
    Tooltip->ModifyStyle(0,WS_DISABLED);
    Tooltip->Create();
  }
}

#endif // !defined(SECTION) || SECTION == 18
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 19

DEFINE_RESPONSE_TABLE_ENTRIES(TApplication)
  EV_WM_ACTIVATEAPP,
  EV_COMMAND(CM_EXIT, CmExit),
END_RESPONSE_TABLE;

//
//Catch the EV_WM_ACTIVATEAPP message, and activate main window
//
void 
TApplication::EvActivateApp(bool active, HTASK /*hTask*/)
{
  if(active && GetMainWindow())
    GetMainWindow()->SetWindowPos(NULL,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
}

//
// Close down main window if application receives a CM_EXIT command.
//
void
TApplication::CmExit()
{
  TFrameWindow* frame = GetMainWindow();
  if (frame) {
    frame->SendMessage(WM_CLOSE);
    DeleteCondemned();
  }
}

//
// TApplication defers event handling to DocManager first if one has been
// installed.
//
bool
TApplication::Find(TEventInfo &eventInfo, TEqualOperator equal)
{
  // Doc manager gets a chance to handle the event first
  //
  bool retVal = DocManager ? DocManager->Find(eventInfo, equal) : false;
  if (retVal == false) {
    eventInfo.Object = (GENERIC*)this;
    retVal = SearchEntries((TGenericTableEntry __RTFAR*)__entries, eventInfo, equal) ||
             TEventHandler::Find(eventInfo, equal);
  }
  return retVal;
}

//
// Determine whether the application can be closed.
// Makes sure the MainWindow can close & doc manager can close.
//
bool
TApplication::CanClose()
{
  TEventInfo eventInfo(WM_OWLCANCLOSE);
  return (!MainWindow || MainWindow->CanClose())
      && (!DocManager ||!DocManager->Find(eventInfo)
                      || DocManager->Dispatch(eventInfo, 0, 0));
}

//
// Called by the main window to provide an oportunity to preprocess the main
// window's menubar before it is installed.
// Normally delegated to the doc manager to install a file menu as needed
//
void
TApplication::PreProcessMenu(HMENU hMenubar)
{
  TEventInfo eventInfo(WM_OWLPREPROCMENU);
  if (DocManager && DocManager->Find(eventInfo)) {
    DocManager->Dispatch(eventInfo, TParam1(hMenubar), 0);
    MainWindow->DrawMenuBar();
  }
}

//
// Condemn a window to be deleted the at the next available safe time.
// Adds the window to a normal single linked list
//
// Condemned windows should be removed if they are destructed in the mean 
// time thru some other mechanism (i.e. stack, aggregate, etc)
//
void
TApplication::Condemn(TWindow* win)
{
  TRACEX(OwlApp, 1, _T("Condemning window @") << (void*)win << *win);
  win->SetParent(0);

#if 0

  // The following logic is from previous versions of ObjectWindows.
  // It results in LIFO destructions which is somewhat unfair.
  // However, we'll keep this code around in case previous applications 
  // relied on this destruction order
  //
  win->SetNext(CondemnedWindows);
  CondemnedWindows = win;

#else

  // Insert the new window to be deleted at the end of the list
  //
  win->SetNext(0);
  if (CondemnedWindows) {
    TWindow* eol = CondemnedWindows;
    while (eol->Next())
      eol = eol->Next();
    eol->SetNext(win);
  } 
  else{
    CondemnedWindows = win;
  }

#endif
}

//
// Remove a condemned window from the list.
//
void
TApplication::Uncondemn(TWindow* win)
{
  if (win && CondemnedWindows) {
    TWindow* w = 0;
    if (CondemnedWindows != win)
      for (w = CondemnedWindows; w->Next() != win; w = w->Next())
        if (!w->Next())
          return;

    TRACEX(OwlApp, 1, _T("Uncondemning window @") << (void*)win << *win);
    if (w)
      w->SetNext(win->Next());
    else
      CondemnedWindows = win->Next();
  }
}

//
// Walk the condemned window list & delete each window. Assumes that the
// windows were constructed using 'new'
//
void
TApplication::DeleteCondemned()
{
  while (CondemnedWindows) {
    TRACEX(OwlApp, 1, _T("Deleting condemned window @") << CondemnedWindows << *CondemnedWindows);
    TWindow* next = CondemnedWindows->Next();
    delete CondemnedWindows;
    CondemnedWindows = next;
  }
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 20


//
//
//
TXInvalidMainWindow::TXInvalidMainWindow()
:
  TXOwl(IDS_INVALIDMAINWINDOW)
{
}

#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXInvalidMainWindow*
#endif
TXInvalidMainWindow::Clone()
{
  return new TXInvalidMainWindow(*this);
}

//
//
//
void
TXInvalidMainWindow::Throw()
{
  THROW( *this );
}

//
// Throws a TXInvalidMainWindow exception.
//
void
TXInvalidMainWindow::Raise()
{
  TXInvalidMainWindow().Throw();
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 21

//
// Enum[Thread/Task]Windows callback. Counts or disables given window based on
// whether or not window list has been allocated yet.
//
static bool CALLBACK OWL_EXPORT16
disableWnds(HWND hWnd, TEnumInfo * ei)
{
  if (!(::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD)) {
    if (hWnd != ei->ModalWnd && ::IsWindowEnabled(hWnd)) {
      if (!ei->Wnds) {
        ei->Count++;  // no buffer yet, we are just counting
      }
      else {
        *(ei->Wnds++) = hWnd;
        ::EnableWindow(hWnd, false);
      }
    }
  }
  return true;
}

//
// Terminate the modal state initiated by BeginModal. Needs topmost ei block,
// and cleans the block up as needed inorder to be safe to be called twice.
//
static void termModal(TEnumInfo& ei)
{
  // Re-enable window(s) that are disabled in BeginModal()
  //
  if (ei.Wnds) {
    for (HWND* hWnd = ei.Wnds; *hWnd; hWnd++)
      ::EnableWindow(*hWnd, true);
    delete[] ei.Wnds;
    ei.Wnds = 0;
  }
  else {
    if (ei.ModalWnd && IsWindow(ei.ModalWnd)) {
      ::EnableWindow(ei.ModalWnd, true);
      ei.ModalWnd = 0;
    }
  }
}

//
// Go modal and enter a message loop until a quit message goes by.
// the flag values determine how window is used:
//
//   MB_APPLMODAL -   window is the owner to disable (if 0, none are disabled)
//   MB_TASKMODAL -   window is the window to make modal,
//                    all other top level windows are disabled
//   MB_SYSTEMMODAL - window is the window to make system modal (16-bit only)
//
// return -1 on errors
//
int
TApplication::BeginModal(TWindow* window, int flags)
{
  // lock enuminfo if multithreading
  TEnumInfo  ei = { 0, 0, 0, 0};
  TEnumInfo* lastStackTop;
  {
    TEnumInfoStr& data = GetEnumInfo();
    LOCKENUMINFO(data.Lock, false); // Y.B. do we have do lock here ?????????
    lastStackTop = data.GetInfo();  // Keep last stackTop to restore later
    data.SetInfo(&ei);              // Point stackTop to topmost ei
  }

  // The concept of SYSTEM MODAL applies only to the 16-bit environment of
  // Windows. The semantics of TASKMODAL is the closest to SYSMODAL in
  // 32-bit - specially in relation to the meaning of the 'window' parameter.
  // So we'll coerce SYSTEM MODAL to TASK MODAL
  //
  if (flags & MB_SYSTEMMODAL) {
    flags &= ~MB_SYSTEMMODAL;
    flags |=  MB_TASKMODAL;
  }

  // Check modal state
  //
  if (flags & MB_TASKMODAL) {
    LOCKENUMINFO(GetEnumInfo().Lock, false); // lock data
    // Save the window to make modal
    //
    if (window)
      ei.ModalWnd = window->GetHandle();

    // Count windows to disable
    //
    if (!EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)disableWnds,
                           TParam2((TEnumInfo *)&ei)))
      return -1;

    // Allocate list of windows to disable, disable windows that are
    // enabled and then stuff them into the list.
    //
    HWND* hWnds = ei.Wnds = new HWND[ei.Count + 1];
    memset(ei.Wnds, 0, sizeof(TModule::THandle)*(ei.Count + 1));

    EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)disableWnds,
                      TParam2((TEnumInfo *)&ei));

    ei.Wnds = hWnds;  // Restore alloc'd pointer since enumerator bumps it
  }

  else if (window) {

    // In MB_APPMODAL mode, we simply disable the window specified
    //
    ei.ModalWnd = window->GetHandle();  // Remember who to re-enable later
    CHECK(ei.ModalWnd);
    window->EnableWindow(false);
  }

  // Enter message loop, saving & restoring the current status & getting the
  // returned result.
  //
  int result;
  TRY {
    result = MessageLoop();
  }
  CATCH( (...) {
    TEnumInfoStr& data = GetEnumInfo();
    LOCKENUMINFO(data.Lock, false);
    termModal(ei);
    data.SetInfo(lastStackTop);
    RETHROW;
  })

  {
    TEnumInfoStr& data = GetEnumInfo();
    LOCKENUMINFO(data.Lock, false);
    data.SetInfo(lastStackTop);
    termModal(ei);            // Just in case termModal was missed in EndModal
  }

  // Return the result from the modal window's EndModal call
  //
  return result;
}

//
// Cause the current modal message loop to break and have it return a result
// Re-enable the disabled windows here, if the EnumInfo is available.
//
void
TApplication::EndModal(int result)
{
  MessageLoopResult = result;
  BreakMessageLoop = true;
  TEnumInfoStr& data = GetEnumInfo();
  if (data.GetInfo()){
    LOCKENUMINFO(data.Lock, false);
    termModal(*data.GetInfo());
  }
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 22

//
// Load the BWCC DLL if needed & set the BWCC on flag according to 'enable'
// Library is not free'd on disable to reduce re-loads if enabling on the fly
//
#if !defined(BWCCVERSION)  // solely foe WARNX
#define BWCCVERSION 0x0200   // version 2.00
#endif

void
TApplication::EnableBWCC(bool enable, uint language)
{
  if (enable) {
    if (!BWCCModule) {
      try {
        BWCCModule = new TBwccDll();
        BWCCModule->IntlInit(language);
        BWCCModule->Register(GetHandle());

        WARNX(OwlApp, BWCCModule->GetVersion() < BWCCVERSION, 0, \
              _T("Old version of BWCC DLL found"));
      }
      catch (...) {
        TRACEX(OwlApp, 0, _T("Unable to create instance of TBwccDll"));
        return;
      }
    }
  }
  BWCCOn = enable;
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 23

//
// Enable or disable the use of the CTL3D DLL. Loads it on demand if needed.
//
void
TApplication::EnableCtl3d(bool enable)
{
  // As per Article Q125684 of Microsoft Development Library:
  // "If major version is 4 or greater, the application should not
  //  implement CTL3D".
  //      'How to Use CTL3D Under the Windows 95 Operating System'
  //
  if (TSystem::GetMajorVersion() >= 4)
    return;

  // Load the Ctl3d DLL if needed & register our instance
  //
  if (enable) {
    if (!Ctl3dModule) {
      try {
        Ctl3dModule = new TCtl3dDll();
        Ctl3dModule->Register(*this);
      }
      catch (...) {
        TRACEX(OwlApp, 0, _T("Unable to create instance of TCtl3dDll"));
        return;
      }
    }
  }
  Ctl3dOn = enable;
}

//
// Enable or disable Ctl3d's use of auto-subclassing.
//
// Caller should turn on autosubclassing before creating a non-owl dialog to
// make it 3d, & turn it off immediatly after.
//
void
TApplication::EnableCtl3dAutosubclass(bool enable)
{
  if (Ctl3dEnabled()) {
    if (enable) {
      Ctl3dModule->Register(*this);
      Ctl3dModule->AutoSubclass(*this);
    }
    else {
      Ctl3dModule->Unregister(*this);
    }
  }
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 24

//
// Predefined DLLs that TApplication knows how to find.
//
  const TCHAR BwccDllName[]  = _T("BWCC32.DLL");

//
// Load the BWCC DLL dynamically.
// Bind the member functions to the exported functions.
//
TBwccDll::TBwccDll()
:
  TModule(BwccDllName, true, true, false), // shouldLoad, mustLoad and !addToList

  IntlInit(*this, "BWCCIntlInit"),
  Register(*this, "BWCCRegister"),
  IntlTerm(*this, "BWCCIntlTerm"),

  SpecialLoadDialog(*this, "SpecialLoadDialog"),
  MangleDialog(*this, "MangleDialog"),
  DefDlgProc(*this, "BWCCDefDlgProc"),
  DefGrayDlgProc(*this, "BWCCDefGrayDlgProc"),
  DefWindowProc(*this, "BWCCDefWindowProc"),
  DefMDIChildProc(*this, "BWCCDefMDIChildProc"),
  MessageBox(*this, "BWCCMessageBox"),
  GetPattern(*this, "BWCCGetPattern"),
  GetVersion(*this, "BWCCGetVersion")
{
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 25

//
// Predefined DLLs that TApplication knows how to find.
//
  const TCHAR Ctl3dDllName[] = _T("CTL3D32.DLL");

//
// Load the Ctl3d DLL dynamically.
// Bind the member functions to the exported functions.
//
TCtl3dDll::TCtl3dDll()
:
  TModule(Ctl3dDllName, true, true, false), // shouldLoad, mustLoad and !addToList

  Register(*this, "Ctl3dRegister"),
  Unregister(*this, "Ctl3dUnregister"),
  AutoSubclass(*this, "Ctl3dAutoSubclass"),
  CtlColorEx(*this, "Ctl3dCtlColorEx"),
  SubclassDlg(*this, "Ctl3dSubclassDlg"),
  SubclassDlgEx(*this, "Ctl3dSubclassDlgEx"),
  GetVer(*this, "Ctl3dGetVer"),
  Enabled(*this, "Ctl3dEnabled"),
  ColorChange(*this, "Ctl3dColorChange"),
  SubclassCtl(*this, "Ctl3dSubclassCtl"),
  DlgFramePaint(*this, "Ctl3dDlgFramePaint"),
  WinIniChange(*this, "Ctl3dWinIniChange")
{
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 26

#if !defined(BI_NO_OBJ_STREAMING)

// Broken apart: IMPLEMENT_STREAMABLE1(TApplication, TModule);
// to replace the ctor
//
IMPLEMENT_STREAMABLE_CLASS(TApplication);
IMPLEMENT_CASTABLE1(TApplication, TModule);
IMPLEMENT_STREAMER(TApplication);
IMPLEMENT_STREAMABLE_POINTER(TApplication)

// IMPLEMENT_STREAMABLE_CTOR1(TApplication, TModule), plus TMsgThread init
//
TApplication::TApplication(StreamableInit)
:
  TModule(streamableInit),
  TMsgThread(TMsgThread::Current)
{
}

//
//
//
void*
TApplication::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TApplication* o = GetObject();
  if (o != gModule)
    is >> *gModule;   // set reference to OWL module
  return o;
}

//
//
//
void
TApplication::Streamer::Write(opstream& os) const
{
  TApplication* o = GetObject();
  if (o != gModule)
    os << *gModule;    // write out reference to OWL module, no data written
}

#else

IMPLEMENT_STREAMABLE1(TApplication, TModule);

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 27

//
bool
TApplication::DiagIdleAction(long idleCount)
{
  LOCKAPPLDATA(GetApplicatData().Lock,true);
  bool more = false;
  if (TTraceWindow::TraceWindow() &&
    TTraceWindow::TraceWindow()->IdleAction (idleCount)){
      more = true;
  }
  return more;
}

//
void
TApplication::SystemCommandTrace()
{
  LOCKAPPLDATA(GetApplicatData().Lock,false);
  if (TTraceWindow::TraceWindow())
    delete TTraceWindow::TraceWindow();
  else{
    new TTraceWindow;
    TTraceWindow::TraceWindow()->Create ();
    TTraceWindow::TraceWindow()->ShowWindow (SW_SHOWNORMAL);
  }
}

#endif  // SECTION

// TWaitCursor data
#if !defined(SECTION) || SECTION == 28

inline bool FilterWindow (HWND hWnd){
  return !::IsWindowEnabled (hWnd) || !::IsWindowVisible (hWnd);
}
//
// Enum[Thread/Task]Windows callback. Counts or subclasses given window based on
// whether or not window list has been allocated yet.
//
static bool CALLBACK OWL_EXPORT16
subclassWnds(HWND hWnd, TEnumInfo * ei)
{
  if (!FilterWindow (hWnd)) {
    if (!ei->Wnds) {
      ei->Count++;  // no buffer yet, we are just counting
    }
    else {
      *(ei->Wnds++) = hWnd;
      *(ei->PrevWndProc++) = (WNDPROC)::SetWindowLong(hWnd, GWL_WNDPROC, (LPARAM)TWaitHook::WaitWndProc);
    }
  }
  return true;
}
//
enum{
    WM_NCMOUSEFIRST = WM_NCMOUSEMOVE,
    WM_NCMOUSELAST  = WM_NCMBUTTONDBLCLK
};
//
TWaitHook::TWaitHook()
:
  LastStackTop(0),
  HideCaretCount(0),
  Cursor(0),
  DeleteOnClose(false)
{
  memset(&Subclass,0,sizeof(Subclass));

  TEnumInfoStr& data = GetEnumInfo();
  LastStackTop = data.GetInfo(); // Keep last stackTop to restore later
  data.SetInfo(&Subclass);       // Point stackTop to topmost ei

  // Count windows to subclass
  //
  if (!::EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)subclassWnds,
                         TParam2((TEnumInfo *)&Subclass)))
      return;

   // Allocate list of windows to disable, disable windows that are
   // enabled and then stuff them into the list.
   //
   HWND* hWnds = Subclass.Wnds = new HWND[Subclass.Count + 1];
   WNDPROC* fProc = Subclass.PrevWndProc = new WNDPROC[Subclass.Count + 1];
   memset(Subclass.Wnds, 0, sizeof(HWND)*(Subclass.Count + 1));
   memset(Subclass.PrevWndProc, 0, sizeof(WNDPROC)*(Subclass.Count + 1));

   EnumThreadWindows(GetCurrentThreadId(), (WNDENUMPROC)subclassWnds,
                     TParam2((TEnumInfo *)&Subclass));

  Subclass.Wnds          = hWnds;  // Restore alloc'd pointer since enumerator bumps it
  Subclass.PrevWndProc  = fProc;  // Restore alloc'd pointer 

  HideCaret(0);
  HideCaretCount = 1;
  Cursor = new TCursor(::LoadCursor (0, IDC_WAIT));
  DeleteOnClose = true;
  SetWaitCursor();
}
//
TWaitHook::~TWaitHook()
{
  for (int i = 0; i < Subclass.Count; i++)
    ::SetWindowLong(Subclass.Wnds[i], GWL_WNDPROC, (LPARAM)Subclass.PrevWndProc[i]);

  delete [] Subclass.Wnds;
  delete [] Subclass.PrevWndProc;

  while(HideCaretCount--)
    ShowCaret(0);

  TPoint  pt;
  ::GetCursorPos(&pt);
  ::SetCursorPos(pt.x, pt.y);

  GetEnumInfo().SetInfo(LastStackTop); // Restore stackttop
  
  FreeCursors();
}
//
void TWaitHook::FreeCursors()
{
  if(DeleteOnClose)
    delete Cursor;
  Cursor    = 0;
}
//
void TWaitHook::SetCursor(TCursor* cursor, TAutoDelete del)
{
  FreeCursors();
  Cursor        = cursor;
  DeleteOnClose  = del == AutoDelete;
}
//
void TWaitHook::SetWaitCursor()
{
  ::SetCursor(*Cursor);
}
//
TResult CALLBACK
#if defined(BI_COMP_BORLANDC) && __BORLANDC__ < 0x0630
__export
#endif
TWaitHook::WaitWndProc(HWND wnd, UINT msg, TParam1 param1, TParam2 param2)
{
  return GetEnumInfo().Hook->WaitWndMethod(wnd, msg, param1, param2);
}
//
TResult TWaitHook::WaitWndMethod(HWND wnd, UINT msg, TParam1 param1, TParam2 param2)
{
  HWND    hTopMostWindow;
  switch (msg){
    case WM_SETCURSOR:
      SetWaitCursor();
      return TRUE;

    case WM_MOUSEACTIVATE:
      hTopMostWindow = (HWND)param1;

      if (hTopMostWindow == wnd)
        return MA_ACTIVATEANDEAT;

       return ::DefWindowProc(wnd, msg, param1, param2);

    case WM_SETFOCUS:
       DefWndProc (wnd, msg, param1, param2);
       HideCaret(0);
       ++HideCaretCount;
       return 0;
  }

  if ((msg >= WM_KEYFIRST && msg <= WM_KEYLAST) ||
     (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) ||
     (msg >= WM_NCMOUSEFIRST && msg <= WM_NCMOUSELAST))
  {
    return 0; // ::DefWindowProc (wnd, msg, param1, param2);
  }
  return DefWndProc (wnd, msg, param1, param2);
}
//
TResult TWaitHook::DefWndProc(HWND wnd, UINT msg, TParam1 param1, TParam2 param2)
{
  for(int i = 0; i < (int)Subclass.Count; i++){
    if (Subclass.Wnds[i] == wnd)
      return CallWindowProc(Subclass.PrevWndProc[i], wnd, msg, param1, param2);
  }
  PRECONDITION(0);
  return 0;
}
#endif

//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 29
void TWaitCursor::SetCursor(TCursor* cursor, TAutoDelete del)
{
  TEnumInfoStr& data = GetEnumInfo();
  LOCKENUMINFO(data.Lock, false); 
  if(data.Hook)
    data.Hook->SetCursor(cursor,del);
}

#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 30

//
void TWaitCursor::Init()
{
  TEnumInfoStr& data = GetEnumInfo();
  LOCKENUMINFO(data.Lock, false); 
  Next      = data.Top;
  data.Top  = this;
  if(!Next){
    PRECONDITION(data.Hook == 0);
    data.Hook = new TWaitHook;
  }
}
//
TWaitCursor::~TWaitCursor()
{
  TEnumInfoStr& data = GetEnumInfo();
  LOCKENUMINFO(data.Lock, false); 
  
  PRECONDITION(data.Top == this);  // must be destructed in reverse order...

  if(data.Top){
    data.Top  = Next;
    if(!Next){
      delete data.Hook;
      data.Hook = 0;
    }
  }
}
//
void TWaitCursor::Restore()
{
  TEnumInfoStr& data = GetEnumInfo();
  LOCKENUMINFO(data.Lock, false); 

  delete data.Hook;
  data.Hook = 0;
  data.Top  = 0;
}
#endif
//----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 31
//
void TWaitCursor::Message(LPCTSTR msg)
{
  TEnumInfoStr& data = GetEnumInfo();
  LOCKENUMINFO(data.Lock, true); 
  if(data.Top == this && data.Hook){
    TApplication* app = OWLGetAppDictionary().GetApplication(0);
    if(!app)
      return;
    
    TFrameWindow* frame= app->GetMainWindow();
    if (frame){
      TMessageBar* messageBar = 
        TYPESAFE_DOWNCAST(frame->ChildWithId(IDW_STATUSBAR), TMessageBar);
      if(messageBar){
        messageBar->SetHintText(msg);       
        messageBar->UpdateWindow();
      }
    }
  }
}
#endif
//-----------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 32
//
TLangModule::TLangModule(LPCTSTR prefix, const TApplication& appl)
:
  Prefix(prefix),
  Module(0),
  Application(&(TApplication&)appl),
  LangId(static_cast<unsigned short>(-1)) //JJH added static_cast
{
  SetLanguage(appl.GetLangId());
}
//-----------------------------------------------------------------------------
TLangModule::~TLangModule()
{
  if(Module && Module != Application)
    delete Module;
}
//-----------------------------------------------------------------------------
void TLangModule::SetLanguage(const TLangId& langId)
{
  owl_string mname = Prefix;
  
  _TCHAR szLocName[10];
  int retval = ::GetLocaleInfo(langId,LOCALE_SABBREVLANGNAME, szLocName, 10);
  if(retval)
    mname += szLocName;
  else
    mname += _T("ENU");
  mname += _T(".DLL");
  TModule* module = new TModule(mname.c_str(),true, false, true); // shouldLoad, !mustLoad and addToList
  if(module->GetHandle() <= HINSTANCE(HINSTANCE_ERROR)){
    delete module;
    owl_string locname = Prefix;
    locname += _T("ENU.DLL");
    module = new TModule(locname.c_str(),true, false, true); // shouldLoad, !mustLoad and addToList
    if(module->GetHandle() > HINSTANCE(HINSTANCE_ERROR))
      mname = locname;
    else{
      delete module;
      module = Application;
    }
  }
  if(Module && Module != Application)
    delete Module;
  Module = module;

  // init LangId
  InitLanguage pProc = (InitLanguage)Module->GetProcAddress("InitLanguage");
  if(pProc)
    pProc();
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------

__OWL_END_NAMESPACE
//===============================================================================