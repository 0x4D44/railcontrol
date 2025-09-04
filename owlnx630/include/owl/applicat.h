//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TApplication. This defines the basic behavior for OWL
// applications. Also definitions for TBwccDll, TCtl3dDll, TXInvalidMainWindow
//----------------------------------------------------------------------------

#if !defined(OWL_APPLICAT_H)
#define OWL_APPLICAT_H

#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_MODULE_H)
# include <owl/module.h>
#endif
#if !defined(OWL_EVENTHAN_H)
# include <owl/eventhan.h>
#endif
#if !defined(OWL_MSGTHRED_H)
# include <owl/msgthred.h>
#endif
#if !defined(OWL_GDIBASE_H)
# include <owl/gdibase.h>
#endif
#if !defined(OWL_LCLSTRNG_H)
#include <owl/lclstrng.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE


//
// Language defines for TApplication::EnableBWCC()
//
#define BWCC_LANG_GERMAN 0x07
#define BWCC_LANG_US     0x09
#define BWCC_LANG_FRENCH 0x0c

class _OWLCLASS TWindow;
class _OWLCLASS TFrameWindow;
class _OWLCLASS TDocManager;
class _OWLCLASS TAppDictionary;
class _OWLCLASS TXInvalidMainWindow;
class _OWLCLASS TBwccDll;
class _OWLCLASS TCtl3dDll;
class _OWLCLASS TCursor;
class _OWLCLASS TTooltip;
class TWaitHook;

// Additional Diagnostic support
class TTraceWindow;
#define  SCM_TRACE  0xFEC0

typedef int (*TMessageBox)(HWND wnd, LPCTSTR text, LPCTSTR caption, uint type);

_OWLFUNC(TMessageBox) SetDefMessageBox(TMessageBox);

_OWLFUNC(int) OWLMessageBox(HWND wnd, LPCTSTR text, LPCTSTR caption, uint type);
_OWLFUNC(int) OWLMessageBox(TWindow* wnd, TResId resId, LPCTSTR caption, uint type, TModule* module = 0);

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>
//
// class TWaitCursor
// ~~~~~ ~~~~~~~~~~~
//
class _OWLCLASS TWaitCursor {
  public:
    TWaitCursor();
    TWaitCursor(LPCTSTR msg);
    TWaitCursor(TCursor* cursor, TAutoDelete = AutoDelete);
    ~TWaitCursor();
    
    void Restore();
    void SetCursor(TCursor* cursor, TAutoDelete = AutoDelete);
    void Message(LPCTSTR text);
  
  private:
    void Init();

    TWaitCursor*  Next;

    friend class TWaitHook;
};
//
// struct TCurrentEvent
// ~~~~~~ ~~~~~~~~~~~~~
// Current event structure for windows events
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
struct TCurrentEvent {
  TWindow*  Win;      // Window that message was sent/dispatched to
  uint      Message;  // Message ID
  union {
#if defined(OWL2_COMPAT)
    TParam1 WParam;   // Old name
#endif
    TParam1 Param1;   // New name of first parameter
  };
  union {
#if defined(OWL2_COMPAT)
    TParam2 LParam;   // Old name
#endif
    TParam2 Param2;   // New name of secont parameter
  };

            TCurrentEvent() : Win(0), Message(0), Param1(0), Param2(0)
              {
              }

};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// class TApplication
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TApplication : virtual public TEventHandler,
                                       public TModule,
                                       public TMsgThread
{
  public:
    // Constructors for TApplication. Default args for the ctor allow
    // TApplication to access global pointers in the user exe/dll.
    // Default OwlAppDictionary can be overridden by passing non-0 appDict arg
    //
    TApplication(LPCTSTR          name = 0,
                 TModule*&        module = gModule,
                 TAppDictionary*  appDict = 0);
    TApplication(LPCTSTR          name,
                 HINSTANCE        hInstance,
                 HINSTANCE        hPrevInstance,
                 LPCTSTR          cmdLine,
                 int              cmdShow,
                 TModule*&        module = gModule,
                 TAppDictionary*  appDict = 0);

   ~TApplication();

    TFrameWindow*    GetMainWindow();
    TDocManager*     GetDocManager();
    TLangId          GetLangId() const;
    void             SetLangId(TLangId landid);

    static void      SetWinMainParams(HINSTANCE hInstance,
                                      HINSTANCE hPrevInstance,
                                      LPCTSTR   cmdLine,
                                      int       cmdShow);

    void             GetWinMainParams();

    HINSTANCE        GetPrevInstance() const;
    void             SetPrevInstance(HINSTANCE pi);

    int              GetCmdShow() const;
    void             SetCmdShow(int cmdshow);

    static owl_string& GetCmdLine();
    TCurrentEvent&   GetCurrentEvent();

    virtual bool     CanClose();
    virtual int      Run();
    virtual int      Start();

    // Message queue thread synchronization mechanism
    //
#if defined(BI_MULTI_THREAD_RTL)
    typedef TMsgThread::TQueueLock TAppLock;

    // Override TEventHandler::Dispatch() to handle multi-thread
    // synchronization
    //
    virtual TResult  Dispatch(TEventInfo& info, TParam1 wp, TParam2 lp = 0);
#endif

    // Message queue loop & response functions
    //
    virtual int      MessageLoop();
    virtual bool     IdleAction(long idleCount);
    virtual bool     ProcessMsg(MSG& msg);
    virtual bool     ProcessAppMsg(MSG& msg);

#ifndef NO_MSGWAIT_MOD
    virtual void     WaitOnObject(HANDLE handle, bool wait);
    virtual void     ObjectSignaled(HANDLE /*handle*/, bool /*abandoned*/) {}
#endif

		owl_string ConvertA2W(LPCSTR string);

    // Get the TWindow pointer belonging to this app given an hWnd
    //
    TWindow*         GetWindowPtr(HWND hWnd) const;

    // Begin and end of a modal window's modal message loop
    //
    int              BeginModal(TWindow* window, int flags=MB_APPLMODAL);
    void             EndModal(int result);
    virtual void     PreProcessMenu(HMENU hMenubar);

    // Dead TWindow garbage collection
    //
    void             Condemn(TWindow* win);
    void             Uncondemn(TWindow* win);

    // Call this function after each msg dispatch if TApplication's message
    // loop is not used.
    //
    void             PostDispatchAction();

    // TApplication defers event handling to DocManager if one has been
    // installed.
    //
    bool             Find(TEventInfo&, TEqualOperator = 0);

    // Control of UI enhancing libraries
    void             EnableBWCC(bool enable = true, uint language = 0);
    bool             BWCCEnabled() const;
    TBwccDll*        GetBWCCModule() const;

    void             EnableCtl3d(bool enable = true);
    void             EnableCtl3dAutosubclass(bool enable);
    bool             Ctl3dEnabled() const;
    TCtl3dDll*       GetCtl3dModule() const;

    // Open a modal message box, with appropriate BWCC or Ctl3d handling
    virtual int      MessageBox(HWND    hParentWnd,
                                LPCTSTR text,
                                LPCTSTR caption = 0,
                                uint    type = MB_OK);
    int              MessageBox(HWND    wnd, 
                                uint    resId, 
                                LPCTSTR caption = 0, 
                                uint    type = MB_OK, 
                                ...);

    // Retrieves/enables tooltip 
    //
    virtual TTooltip* GetTooltip() const;
    virtual void      EnableTooltip(bool enable=true);

#if defined(OWL2_COMPAT)
    typedef __OWL_GLOBAL::TXInvalidMainWindow TXInvalidMainWindow;  // Exceptions moved to global scope
#endif

  protected:
    virtual void     InitApplication();
    virtual void     InitInstance();
    virtual void     InitMainWindow();
    virtual int      TermInstance(int status);
    
    // Diagnostic support automatic on for __TRACE or __WARN
    void SystemCommandTrace();          // System Command Message reponse
    bool DiagIdleAction(long idleCount);//

    // Assigns tooltip 
    void SetTooltip(TTooltip* tooltip);

    // (Re)set a new main-window and DocManager either at construction or
    // sometime later
    //
    TFrameWindow*    SetMainWindow(TFrameWindow* window);
    TDocManager*     SetDocManager(TDocManager* docManager);

    // Member data -- use accessors to get at these
    //
  public_data:
    HINSTANCE     hPrevInstance;
    int           nCmdShow;
    TDocManager*  DocManager;
    TFrameWindow* MainWindow;
    TLangId       LangId;
    TTooltip*     Tooltip;
#if defined(OWL2_COMPAT)
    HACCEL        HAccTable;    // Obsolete, use each TWindow's
#endif

  protected_data:
    owl_string    CmdLine;      // string object copy of cmd line

#ifndef NO_MSGWAIT_MOD
    DWORD    WaitCount;
    LPHANDLE WaitHandles;
#endif

  private:
    bool          BWCCOn;
    TBwccDll*     BWCCModule;

    bool          Ctl3dOn;
    TCtl3dDll*    Ctl3dModule;

    TCurrentEvent CurrentEvent;

    // Condemned TWindow garbage collection
    //
    void          DeleteCondemned();
    TWindow*      CondemnedWindows;

    // The dictionary that this app is in
    //
    TAppDictionary*     Dictionary;

    // Static application initialization parameters cached here before app
    // is actually constructed
    //
    static HINSTANCE    InitHInstance;          // WinMain's 1st param
    static HINSTANCE    InitHPrevInstance;      // WinMain's 2nd param
    static owl_string&  GetInitCmdLine();       // WinMain's 3rd param
    static int          InitCmdShow;            // WinMain's 4th param

    // Response tables
    //
    typedef TResponseTableEntry<TApplication>::PMF TMyPMF;
    typedef TApplication                           TMyClass;

    static TResponseTableEntry<TApplication> __RTFAR __entries[];
    void EvActivateApp(bool active, HTASK hTask); // activate main window
    void CmExit();            // Exit from file menu

    // Hidden to prevent accidental copying or assignment
    //
    TApplication(const TApplication&);
    TApplication& operator =(const TApplication&);

//  DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TApplication, 1);
  DECLARE_STREAMABLE_OWL(TApplication, 1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( TApplication );

//
// class TBwccDll
// ~~~~~ ~~~~~~~~
// Wrapper for the BWCC Dll
//
class _OWLCLASS TBwccDll : public TModule {
  public:
    TBwccDll();

    // Used by TApplication
    //
    TModuleProc1<bool,uint>      IntlInit;
    TModuleProc1<bool,HINSTANCE> Register;
    TModuleProc0<bool>           IntlTerm;

    // Not used by OWL
    TModuleProc3<HGLOBAL,HINSTANCE,LPCTSTR,DLGPROC> SpecialLoadDialog;
    TModuleProc3<HGLOBAL,HGLOBAL,HINSTANCE,DLGPROC> MangleDialog;
    TModuleProc4<LRESULT,HWND,UINT,WPARAM,LPARAM>   DefDlgProc;
    TModuleProc4<LRESULT,HWND,UINT,WPARAM,LPARAM>   DefGrayDlgProc;
    TModuleProc4<LRESULT,HWND,UINT,WPARAM,LPARAM>   DefWindowProc;
    TModuleProc4<LRESULT,HWND,UINT,WPARAM,LPARAM>   DefMDIChildProc;
    TModuleProc4<int,HWND,LPCTSTR,LPCTSTR,UINT>     MessageBox;
    TModuleProc0<HBRUSH> GetPattern;
    TModuleProc0<DWORD>  GetVersion;
};

//
// class TCtl3dDll
// ~~~~~ ~~~~~~~~~
// Wrapper for the Control 3D Dll
//
class _OWLCLASS TCtl3dDll : public TModule {
  public:
    TCtl3dDll();

    // Used by TApplication
    TModuleProc1<BOOL,HANDLE> Register;
    TModuleProc1<BOOL,HANDLE> Unregister;
    TModuleProc1<BOOL,HANDLE> AutoSubclass;

    // Used by TDialog
    TModuleProc3<HBRUSH,uint,WPARAM,LPARAM> CtlColorEx;
    TModuleProc2<BOOL,HWND,uint16>          SubclassDlg;

    // Not used by OWL
    TModuleProc2<BOOL,HWND,DWORD> SubclassDlgEx;
    TModuleProc0<WORD> GetVer;
    TModuleProc0<BOOL> Enabled;
    TModuleProc0<BOOL> ColorChange;
    TModuleProc1<BOOL,HWND> SubclassCtl;
    TModuleProc4<LONG,HWND,UINT,WPARAM,LPARAM> DlgFramePaint;

    TModuleProc0<int> WinIniChange;
};

//
// class TXInvalidMainWindow
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
class _OWLCLASS TXInvalidMainWindow : public TXOwl {
  public:
    TXInvalidMainWindow();

#if defined(BI_NO_COVAR_RET)
    TXBase* Clone();
#else
    TXInvalidMainWindow* Clone();
#endif
    void Throw();

    static void Raise();
};


//
// class TLangModule
// ~~~~~ ~~~~~~~~~~~
// International resource support:  [APP]ENG.DLL - default application module
// Module == TApplication if not found any resource module
//
// Usage:
// 
// static TLangModule* langModulePtr = 0;
// TLangModule* GetResModule()
// {
//    return langModulePtr;
// }
// 
// int OwlMain(int,LPCTSTR*)
// {
//   TMyAppl appl;
//   TLangModule langModule("MRES_",appl); // will be MRES_###.dll
//   langModulePtr = &langModule;
//   return appl.Run();
// }
class _OWLCLASS TLangModule {
  public:
    // export function: void __stdcall InitLanguage();
    typedef void (*InitLanguage)(); 
    // Constructors & destructor
    //
    TLangModule(LPCTSTR prefix, const TApplication& appl);
    virtual ~TLangModule();

    operator TModule*()         { return Module; }
    TModule*  GetModule()       { return Module; }

    void SetLanguage(const TLangId& langId);
    TLangId GetLanguage() const { return LangId;}

  protected:
    owl_string    Prefix;
    TModule*      Module;
    TApplication* Application;
    TLangId       LangId;
};


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
inline TWaitCursor::TWaitCursor()
{
  Init();
}
//
inline TWaitCursor::TWaitCursor(LPCTSTR msg)
{
  Init();
  Message(msg);
}
//
inline TWaitCursor::TWaitCursor(TCursor* cursor, TAutoDelete del)
{
  Init();
  SetCursor(cursor, del);
}
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

////////////////////
//
// Return the current main window.
//
inline TFrameWindow* TApplication::GetMainWindow()
{
  return MainWindow;
}

//
// Return the current document manager.
//
inline TDocManager* TApplication::GetDocManager()
{
  return DocManager;
}

//
// Return the HINSTANCE of the previous running instance.
//
inline HINSTANCE TApplication::GetPrevInstance() const
{
  return hPrevInstance;
}

//
// Set the previous instance.
// This should not be called by normal programs.
//
inline void TApplication::SetPrevInstance(HINSTANCE pi)
{
  hPrevInstance = pi;
}

//
// Retrieve the initial state of the main window.
//
inline int TApplication::GetCmdShow() const
{
  return nCmdShow;
}

//
// Sets the initial state of the main window.
// Typically passed by the operating system.
//
inline void TApplication::SetCmdShow(int cmdshow)
{
  nCmdShow = cmdshow;
}

//
// Return the command line of the application.
// Most programs do not need to call this because OwlMain has the
// parameters already parsed.
//
inline owl_string& TApplication::GetCmdLine()
{
  return GetInitCmdLine();
}

//
// Return the current event from the message queue.
//
inline TCurrentEvent& TApplication::GetCurrentEvent()
{
  return CurrentEvent;
}

extern _OWLFUNC(TWindow*) GetWindowPtr(HWND, const TApplication*);

//
// Return the window pointer given a window's handle.
//
inline TWindow* TApplication::GetWindowPtr(HWND hWnd) const
{
#if !defined(BI_NO_NAMESPACE)
    return ::NS_OWL::GetWindowPtr(hWnd, this);
#else
    return ::GetWindowPtr(hWnd, this);
#endif
}

//
//
inline TLangId TApplication::GetLangId() const{
  return LangId;
}
//
//
//
inline void TApplication::SetLangId(TLangId landid){
  LangId = landid;
}

//
// Set the data members with data from WinMain.
//
inline void TApplication::SetWinMainParams(HINSTANCE       hInstance,
                                           HINSTANCE       hPrevInstance,
                                           LPCTSTR         cmdLine,
                                           int             cmdShow)
{
  InitHInstance = hInstance;
  InitHPrevInstance = hPrevInstance;
  if (NULL!=cmdLine) // Ma, 23.11.00 avoid crash !!!!
    GetInitCmdLine() = cmdLine;
    
  InitCmdShow = cmdShow;
}

//
// Retrieve the WinMain parameters.
//
inline void TApplication::GetWinMainParams()
{
  InitModule(InitHInstance, GetInitCmdLine().c_str());
  hPrevInstance = InitHPrevInstance;
  nCmdShow = InitCmdShow;
}

//
// Returns true if the BWCC is enabled for the application.
//
inline bool TApplication::BWCCEnabled() const
{
  return BWCCOn;
}

//
// If BWCC is enabled, return the module associated with it.
//
inline TBwccDll* TApplication::GetBWCCModule() const
{
  return BWCCModule;
}

//
// Returns true if Ctl3d is enabled.
//
inline bool TApplication::Ctl3dEnabled() const
{
  return Ctl3dOn;
}

//
// If Ctl3D is enabled, return the module associated with it.
//
inline TCtl3dDll* TApplication::GetCtl3dModule() const
{
  return Ctl3dModule;
}

//
// Get Tooltip
//
inline TTooltip* TApplication::GetTooltip() const 
{
  return Tooltip;
}


__OWL_END_NAMESPACE


#endif  // OWL_APPLICAT_H
