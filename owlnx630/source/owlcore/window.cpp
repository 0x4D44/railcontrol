//---------------------------------------------------------------------------- 
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.34.2.2 $
//$Author: jogybl $
//$Date: 2010-03-24 11:54:28 $
//
// Implementation of TWindow.  This defines the basic behavior of all Windows.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_DEFS_H)
# include <owl/defs.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_APPDICT_H)
# include <owl/appdict.h>
#endif
#if !defined(OWL_SCROLLER_H)
# include <owl/scroller.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if !defined(OWL_MENU_H)
# include <owl/menu.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#if !defined(OWL_SHELLITM_H)
# include <owl/shellitm.h>
#endif
#if !defined(OWL_TOOLTIP_H)
# include <owl/tooltip.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#if defined(BI_NEED_ZMOUSE_H)
#include <api_upd/zmouse.h>
#else
#include <zmouse.h>
#endif
#include <owl/registry.h>

#include <owl/hlpmanag.h> //THelpHitInfo

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlMsg);  // diagnostic group for message tracing
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlWin, 1, 0);  // diag. group for windows
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlCmd, 1, 0);  // diag. group for commands

__OWL_END_NAMESPACE

//
// Define to use rtti to create unique window ids for the message cache
//
#if !defined(BI_NO_RTTI)
# define OWL_RTTI_MSGCACHE
#if defined(BI_COMP_BORLANDC) 
# define TYPE_UNIQUE_UINT32(t)    reinterpret_cast<uint32>(typeid(t).tpp)
#elif defined(BI_COMP_FORTE) || defined(__EDG__) || defined(BI_COMP_ACC)
# define TYPE_UNIQUE_UINT32(t)    reinterpret_cast<uint32>(typeid(t).name())
#elif defined(BI_COMP_MSC)
# define TYPE_UNIQUE_UINT32(t)    reinterpret_cast<uint32>(typeid(t).raw_name())
#else
# undef OWL_RTTI_MSGCACHE
#endif
#endif

bool gBatchMode = false; // true if we were invoked to do batch processing
                         // which means windows should be invisible and whatnot
                         // currently this is only set if we have an output parameter

__OWL_BEGIN_NAMESPACE

//
// Externs defined in owl.cpp
//
extern _OWLFUNC(void) SetCreationWindow(NS_OWL::TWindow*);
extern _OWLFUNC(NS_OWL::TWindow*) GetCreationWindow();
extern _OWLDATA(uint) GetWindowPtrMsgId;

// register for Windows 95 or Windows NT 3.51
static uint __owlMsgMouseWheel = 
 TSystem::IsWin95() || (TSystem::IsNT()  && TSystem::GetMajorVersion() == 3) ? 
  ::RegisterWindowMessage(MSH_MOUSEWHEEL) : 0;

DEFINE_RESPONSE_TABLE(TWindow)
  EV_WM_SETCURSOR,
  EV_WM_SIZE,
  EV_WM_MOVE,
  EV_WM_COMPAREITEM,
  EV_WM_DELETEITEM,
  EV_WM_DRAWITEM,
  EV_WM_MEASUREITEM,
  EV_WM_VSCROLL,
  EV_WM_HSCROLL,
  EV_WM_CHILDINVALID,
  EV_WM_ERASEBKGND,
  EV_WM_CTLCOLOR,
  EV_WM_PAINT,
  EV_WM_LBUTTONDOWN,
  EV_WM_KILLFOCUS,
  EV_MESSAGE(WM_CTLCOLORMSGBOX, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLOREDIT, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLORLISTBOX, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLORBTN, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLORDLG, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLORSCROLLBAR, EvWin32CtlColor),
  EV_MESSAGE(WM_CTLCOLORSTATIC, EvWin32CtlColor),
  EV_WM_CREATE,
  EV_WM_CLOSE,
  EV_WM_DESTROY,
  EV_WM_NCDESTROY,
  EV_WM_QUERYENDSESSION,
  EV_WM_ENDSESSION,
  EV_WM_SYSCOLORCHANGE,
  EV_WM_INITMENUPOPUP,
  EV_WM_CONTEXTMENU,
  EV_WM_ENTERIDLE,
  EV_WM_MOUSEWHEEL,
  EV_MESSAGE(__owlMsgMouseWheel, EvRegisteredMouseWheel),
END_RESPONSE_TABLE;


#if defined(BI_NO_RTTI)
  IMPLEMENT_CASTABLE1(TCommandEnabler, TStreamableBase);
#endif

//
//
//
TCommandEnabler::TCommandEnabler(uint id, HWND hReceiver)
:
  Id(id),
  HWndReceiver(hReceiver)
{
  Flags = 0;
}

//
//
//
void
TCommandEnabler::Enable(bool)
{
  Flags |= WasHandled;
}

void TWindow::TraceWindowPlacement()
{
  WINDOWPLACEMENT wp;
  GetWindowPlacement(&wp);
  TRACEX(OwlWin,1,"PLACEMENT=" << (TRect&)wp.rcNormalPosition << " Hwnd=" << GetHandle() << "min=" << wp.ptMinPosition.x << " " << wp.ptMinPosition.y); // << " min=" << (TPoint&)wp.ptMinPosition " max=" << (TPoint&)wp.ptMaxPosition); // << " showCmd=" << wp.showCmd << " flags=" << wp.flags);
}

//
// Constructor for a TWindow.  if a parent window is passed, adds the TWindow
// to its parent's list of children
//
TWindow::TWindow(TWindow* parent, LPCTSTR title, TModule* module)
: InstanceProc(0), DefaultProc(0), Handle(0), Title(0), Module(0)
{
  Init(parent, title, module);
}

//
// Constructor for a TWindow which is being used as an alias for a
// non-OWL window.  This ctor is generally not used by derived classes
//
TWindow::TWindow(HWND handle, TModule* module)
: InstanceProc(0), DefaultProc(0), Handle(0), Title(0), Module(0)
{
  PRECONDITION(handle);
  Init(handle, module);
}

//
// Protected constructor for use by immediate virtually derived classes.
// Immediate derivitives must call an Init() before constructions are done.
//
TWindow::TWindow()
: InstanceProc(0), DefaultProc(0), Handle(0), Title(0), Module(0)
{
}

//
// Normal initialization of a default constructed TWindow. Is ignored if
// called more than once.
//
void
TWindow::Init(TWindow* parent, LPCTSTR title, TModule* module)
{
  if (!InstanceProc) {
    Title = strnewdup(title);
    PerformInit(parent, module);
    EnableAutoCreate();

    // Initialize creation attributes
    //
    Attr.Style = WS_CHILD | WS_VISIBLE;
    Attr.X = Attr.Y = Attr.W = Attr.H = 0;
    Attr.ExStyle = 0;
    Attr.Id = 0;
  }
}

//
// Wrapper initialization of a default constructed TWindow. Is ignored if
// called more than once.
//
void
TWindow::Init(HWND handle, TModule* module)
{
  PRECONDITION(handle);

  if (!InstanceProc) {
    // Perform preliminary initialization
    //
    SetHandle(handle);
    Title = 0;

    // If we've been given a module, then setup that and the app now if
    // possible so that GetWindowPtr below has some context. Otherwise at least
    // set it to 0.
    //
    Application = TYPESAFE_DOWNCAST(module,TApplication);

    // If the receiver's parent is an OWL window then pass the window to 
    // PerformInit so the receiver can be added to the parent's list of children.
    //
    HWND   hParent = GetParentH();
    TWindow*  parent = hParent ? GetWindowPtr(hParent) : 0;
    PerformInit(parent, module);

    // Install the instance window proc.
    //
    SubclassWindowFunction();

    // Copy over the title, styles, the coordinates & the id
    //
    GetWindowTextTitle();
    GetHWndState(true); //DLN now we force resysnc of style settings for non-owl windows.

    // Keep track that this is an alias object & that it is already created
    //
    SetFlag(wfAlias | wfFullyCreated);

    // Unique id may have been set inadvertantly to TWindow by the above
    // GetWindowTextTitle, et. al. Reset it just in case
    //
    SetUniqueId();
  }
}

//
// Private initializer function that does the bulk of the work for the
// protected Init()s
//
void
TWindow::PerformInit(TWindow* parent, TModule* module)
{
  // Initialize members.
  //
  ZOrder = 0;
  ChildList = 0;
  Flags = wfDeleteOnClose;
  TransferBuffer = 0;
  Parent = parent;
  Attr.Param = 0;
  Attr.Menu = 0;
  Attr.AccelTable = 0;
  HAccel = 0;
  Scroller = 0;
  ContextPopupMenu = 0;
  Tooltip = 0;
  CursorModule = 0;
  CursorResId = 0;
  HCursor = 0;
  BkgndColor = TColor::None;

  // Set the window proc for this window instance.
  //
  InstanceProc = CreateInstanceProc();

  // Link to parent.
  //
  if (Parent)
    Parent->AddChild(this);
  else
    SiblingList = 0;

  // Use passed module if one, else get parent's. If no parent, use app
  //
  if (Parent) {
    Application = Parent->GetApplication();
    Module = module ? module : Parent->GetModule();
  }
  else {
    Module = module ? module : 0;
    Application = TYPESAFE_DOWNCAST(Module,TApplication);
    if (!Application)
    {
      //Application = OWLGetAppDictionary().GetApplication(0);
      Application = GetApplicationObject();
      if (!Module)
        Module = Application;
    }
    CHECK(Application);
  }
  SetUniqueId();

  TRACEX(OwlWin, 1, _T("TWindow constructed @") << (void*)this << *this);
}

//
// Helper function for TWindow destructor.
// Shutdowns and deallocates the given window.
// 
static void shutDown(TWindow* win, void*) {
  win->Destroy();
  delete win;
}

//
// Function prototype
//
void CacheFlush(uint32 id);

//
// Destructor for a TWindow: disposes of each window in its ChildList
// and removes itself from a non-0 parent's list of children
//
// Destroys a still-associated Handle and frees the instance window proc used for
// linking the TWindow to the Handle.
//
// Disposes of its Scroller if the TScroller object was constructed
//
TWindow::~TWindow()
{
  // Clean up the instance window proc.
  // VH: Clean up the instance proc early.
  // We're in the destructor, we don't want to handle any more messages.
  //
  if (GetHandle()) {
    //
    // Restore original window proc, or set to default.
    //
    WARNX(OwlWin, GetWindowProc() != GetInstanceProc(), 0,
      _T("Restoring old WndProc after foreign subclass of:") << *this);
    SetWindowProc(DefaultProc ? DefaultProc : ::DefWindowProc);
  }
  FreeInstanceProc();
  InstanceProc = 0;

  // Flush the cache so that messages dont get dispatched to a
  // already-destructed derived class
  //
  NS_OWL::CacheFlush(UniqueId);

  // ShutDown children first, so the Owl objects get a chance to clean up
  //
  ForEach(shutDown);

  // Destroy the window if the handle is still around, and this is not an alias.
  //
  if (GetHandle() && !IsFlagSet(wfAlias)) {
    WARNX(OwlWin, GetHandle(), 0, _T("Destroying from TWindow::~TWindow: ") << *this);
    Destroy();
  }

  /*
  // If the Handle is still around, then destroy it or unlink from it as
  // appropriate.
  //
  if (GetHandle()) {
    // For aliases:
    //  - we are destructing the C++ object but not destroying the MS window
    //  - restore the original window function
    //
    if (IsFlagSet(wfAlias)) {
      // May want to check WNDPROC against InstanceProc to see if its still us
      // and not restore if it's not us...
      //
      WARNX(OwlWin, GetWindowProc() != GetInstanceProc(), 0,
            _T("Restoring old WndProc after foreign subclass of:") << *this);
      SetWindowProc(DefaultProc ? DefaultProc : ::DefWindowProc);
    }
    // For non-aliases:
    //  - destroy the windows element
    // this is not a normal condition and is a safety net only
    //
    else {
      WARNX(OwlWin, GetHandle(), 0, _T("Destroying from TWindow::~TWindow: ") << *this);
      Destroy();
    }
  }
  */

  // Remove from parent's ChildList
  //
  if (Parent)
    Parent->RemoveChild(this);

#if 0
  // Clear TApplication's member to avoid dangling pointer
  //
  //!TH Moved to frame window's destructor
  //
  if (IsFlagSet(wfMainWindow))
    GetApplication()->MainWindow = 0;
#endif

  // Remove pointer from application's CondemnList
  //
  if (Application)
    Application->Uncondemn(this);

  // Delete menu id string.
  //
  if (Attr.Menu.IsString())
    delete[] (_TCHAR *)Attr.Menu;  // Assumes strnewdup was used //???????????

  // Get rid of our scroller, if any.
  //
  SetScroller(0);

  // Clean up title member
  //
  if (HiUint16(uint32(Title)))
    delete[] Title;
  Title = 0;

  // Cleanup cursor
  //
  if (HCursor && CursorModule)
    SetCursor(0, 0);

  // Clean up context menu object
  //
  delete ContextPopupMenu;

  TRACEX(OwlWin, 1, _T("TWindow destructed @") << (void*)this << *this);
}

#if defined(BI_MULTI_THREAD_RTL)
//
// Overrides TEventHandler::Dispatch() to handle multi-thread synchronization
//
TResult TWindow::Dispatch(TEventInfo& info, TParam1 p1, TParam2 p2)
{
  TApplication::TQueueLock lock(*GetApplication());
  return TEventHandler::Dispatch(info, p1, p2);
}

#endif

//
// Called from TApplication::ProcessAppMsg() to give the window an
// opportunity to perform preprocessing of the Windows message
//
// If you return true, further processing of the message is halted
//
// If you override this method in a derived class, make sure to call this
// routine because it handles translation of accelerators...
//
bool
TWindow::PreProcessMsg(MSG& msg)
{
  PRECONDITION(GetHandle());

  // Check if this message might be worth relaying to the tooltip window
  //
  TTooltip* tooltip = GetTooltip();
  if (tooltip && tooltip->IsWindow()) {
    if (msg.hwnd == *this || IsChild(msg.hwnd)) {
      if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
        tooltip->RelayEvent(msg);
      }
    }
  }

  return HAccel ? ::TranslateAccelerator(GetHandle(), HAccel, &msg) : false;
}

//
// Propagate idle action to all children if count==0, and to any children that
// previously said they wanted more time.
//
bool
TWindow::IdleAction(long idleCount)
{
  bool wantMore = false;
  TWindow* win = GetFirstChild();
  if (win) {
    do {
      if (idleCount == 0 || win->IsFlagSet(wfPropagateIdle)) {
        if (win->IdleAction(idleCount)) {
          win->SetFlag(wfPropagateIdle);
          wantMore = true;
        }
        else {
          win->ClearFlag(wfPropagateIdle);
        }
      }
      win = win->Next();
    } while (win && win != GetFirstChild());
  }
  return wantMore;
}

//
// Respond to WM_SYSCOLORCHANGE by broadcasting it to all children
//
void
TWindow::EvSysColorChange()
{
  ChildBroadcastMessage(WM_SYSCOLORCHANGE);
  DefaultProcessing();
}

//
// Removes the passed pointer to a child window from the linked list of
// sibling windows which the TWindow's ChildList points to
//
void
TWindow::RemoveChild(TWindow* child)
{
  if (child && ChildList) {
    TWindow*  lastChild = ChildList;
    TWindow*  nextChild = lastChild;

    while (nextChild->SiblingList != lastChild &&
           nextChild->SiblingList != child) {
      nextChild = nextChild->SiblingList;
    }

    if (nextChild->SiblingList == child) {
      if (nextChild->SiblingList == nextChild)
        ChildList = 0;

      else {
        if (nextChild->SiblingList == ChildList)
          ChildList = nextChild;

        nextChild->SiblingList = nextChild->SiblingList->SiblingList;
      }
    }
  }
}

//
// Reparents this window to a new owl parent window. Also sets the windows
// parent (owner really)
//
void
TWindow::SetParent(TWindow* newParent)
{
  if (Parent != newParent) {
    if (Parent)
      Parent->RemoveChild(this);

    SiblingList = 0;

    Parent = newParent;

    if (Parent)
      Parent->AddChild(this);
  }
  // Tell Windows about the change too, if appropriate
  //
  if (GetHandle() && Parent && GetParentH() != Parent->GetHandle()) {
    if (newParent) {
      if (newParent->GetHandle())
        ::SetParent(GetHandle(), newParent->GetHandle());
    }
    else
      ::SetParent(GetHandle(), 0);
  }
}

//
// Default behavior for updating document title is to pass it to parent frame
//
bool
TWindow::SetDocTitle(LPCTSTR docname, int index)
{
  return Parent ? Parent->SetDocTitle(docname, index) : false;
}

//
// Set the scroller object for this window. This window assumes ownership of
// the scroller object, & will delete it when done & on subsequent sets.
//
void
TWindow::SetScroller(TScroller* scroller)
{
  delete Scroller;
  Scroller = scroller;
}

//
// Wildcard check used for child id notifications
//
static bool wildcardCheck(TGenericTableEntry __RTFAR& entry,
                          TEventHandler::TEventInfo& info) {
  return entry.Id == info.Id && (entry.Msg == UINT_MAX || entry.Msg == info.Msg);
}

//
// Handles default processing of events, which includes continued processing
// of menu/accelerators commands and enablers, as well as notifications
//
TResult
TWindow::DefaultProcessing()
{
  TCurrentEvent& currentEvent = GetCurrentEvent();

  if (currentEvent.Message == WM_COMMAND_ENABLE) {
    if (currentEvent.Win != this) {
      TWindow*         receiver = Parent ? Parent : currentEvent.Win;
      TCommandEnabler& commandEnabler = *(TCommandEnabler*)currentEvent.Param2;
      TEventInfo       eventInfo(WM_COMMAND_ENABLE, commandEnabler.Id);

      if (receiver->Find(eventInfo))
        return receiver->Dispatch(eventInfo, 0, currentEvent.Param2);
    }

    return 0;
  }

  // Handle normal message default processing by routing directly to the
  // virtual DefWindowProc() for the window.
  //
  if (currentEvent.Message != WM_COMMAND && currentEvent.Message != WM_NOTIFY)
    return DefWindowProc(currentEvent.Message,
                         currentEvent.Param1,
                         currentEvent.Param2);

  // currentEvent.Message == WM_COMMAND or WM_NOTIFY
  //
  uint    notifyCode;
  uint    id;
  HWND hWndCtl;

  // Get notify code, control id and control handle from packed params.
  // Unpacking based on message & platform.
  //
  if (currentEvent.Message == WM_COMMAND) {
    notifyCode = HiUint16(currentEvent.Param1);
    id = LoUint16(currentEvent.Param1);
    hWndCtl = HWND((uint)currentEvent.Param2);
  }
  else {
    TNotify& _not = *(TNotify*)currentEvent.Param2;
    notifyCode = _not.code;
    id = _not.idFrom; //currentEvent.Param1; // Y.B. In help written -> use not.idFrom.
    if (_not.code == (uint)TTN_NEEDTEXT && 
        ((TTooltipText*)&_not)->uFlags&TTF_IDISHWND){
      id = ::GetDlgCtrlID((HWND)_not.idFrom);
    }
    hWndCtl = _not.hwndFrom;
  }

  // If all special routing is done, then process the command/notify for this
  // window
  //
  if (currentEvent.Win == this) {
    // Menu command originally destined for the receiver, defer to the app.
    //
    if (hWndCtl == 0) {
      TEventInfo     eventInfo(0, id);
      TApplication*  app = GetApplication();

      if (app->Find(eventInfo)) {
        app->Dispatch(eventInfo, eventInfo.Id);
        return 0;
      }
      WARNX(OwlMsg, notifyCode<=1, 0, _T("Unprocessed WM_COMMAND(id=") << id << _T(")"));
    }
    // Originally destined for the receiver and the receiver has called us so
    // default processing can occur.
    // Unpack the original parameters and call DefWindowProc()
    //
    return DefWindowProc(currentEvent.Message, currentEvent.Param1, currentEvent.Param2);
  }

  // Perform special routing for commands and notifications
  //
  else {
    TWindow*        receiver;
    TEqualOperator  equal = 0;

    if (hWndCtl == 0) {
      // Menu/accelerator/push button
      // Either give the message to the receiver's parent or the original
      // receiver of the message
      //
      receiver = Parent ? Parent : currentEvent.Win;

      // "notifyCode" is either 0 or 1 depending on whether this is from an
      // accelerator; however, we want to explicitly look for a 0...
      //
      notifyCode = 0;
    }
    else {
      // Child id notification (either WM_COMMAND or WM_NOTIFY) sent to the
      // child and the child has called us.
      // Offer the parent an opportunity to handle the notification
      //   NOTE: use equal function that will do a wildcard search
      //
      equal = wildcardCheck;
      receiver = currentEvent.Win;

      // The child window identifier shouldn't be 0, but if it is then it will
      // look like we are searching for a WM_ message with value "notifyCode",
      // in that case just give up and call DefWindowProc() for the window.
      //
      if (receiver->IsFlagSet(wfAlias) || id == 0)
        return receiver->DefWindowProc(currentEvent.Message,
                                       currentEvent.Param1,
                                       currentEvent.Param2);
    }

    // Now dispatch the command or notification to the receiver window
    //
    TEventInfo  eventInfo(notifyCode, id);

    // NOTE: The ResponseTableEntry of handlers which expect an id (e.g.
    //       EV_COMMAND_AND_ID) have a zero in their first field. The
    //       ResponseTableEntry of handlers which expect a notification code
    //       (e.g. EV_CHILD_NOTIFY_AND_CODE and EV_NOTIFY_ALL_CODES) contain
    //       either the notifyCode or UINT_MAX in that field. Hence, we can
    //       dispatch the expected information based on the contents of that
    //       field.
    //
    //       However, this logic will fail to disambiguate cases where a
    //       notification code is 0. The only standard control with such a
    //       notification is BUTTON /w BN_CLICKED. So for button handlers
    //       expecting the id, you can use EV_COMMAND_AND_ID. For handlers
    //       expecting the notification code, you can use EV_NOTIFY_ALL_CODES,
    //       (*NOT* EV_CHILD_NOTIFY_AND_CODE(Id, BN_CLICKED, xxxx,...))
    //
    if (receiver->Find(eventInfo, equal)) {
      TParam1 param1 = eventInfo.Entry->NotifyCode ? notifyCode : id;
      return receiver->Dispatch(eventInfo, param1, currentEvent.Param2);
    }
    else
      return receiver->DefaultProcessing();
  }
}

//
// Called by WindowProc() to handle WM_COMMANDs
//
// id         - specifies the identifier of the menu item or control
//
// hWndCtl    - specifies the control sending the message if the message
//              is from a control; otherwise it is 0
//
// notifyCode - specifies the notification message if the message is from
//              a control. if the message is from an accelerator, it is 1.
//              if the message is from a menu, it is 0
//
TResult
TWindow::EvCommand(uint id, HWND hWndCtl, uint notifyCode)
{
  TRACEX(OwlCmd, 1, _T("TWindow::EvCommand - id(") << id << _T("), ctl(") <<\
                     hex << uint(hWndCtl) << _T("), code(") << notifyCode  << _T(")"));

  TWindow*        receiver = this;
  TEqualOperator  equal = 0;

  // Menu/accelerator
  //
  if (hWndCtl == 0) {
    // "notifyCode" is either 0 or 1 depending on whether this is from an
    // accelerator; however, we want to explicitly look for a 0 in the tables
    //
    notifyCode = 0;
  }

  // Child id notification
  //
  else {
    TWindow*  child = GetWindowPtr(hWndCtl);

    if (child) {
      // Give the child first crack at the event
      //
      receiver = child;
      id = UINT_MAX;  // special designator for child Id notifications that are
                      // handled at the child
    }
    else {
      // Offer the parent an opportunity to handle the notification
      //
      // NOTE: use equal function that will do a wildcard search
      //
      equal = wildcardCheck;

      // The child window identifier shouldn't be 0, but if it is then it will
      // look like we are searching for a WM_ message with value "notifyCode"
      //
      if (id == 0)
        return DefaultProcessing();
    }
  }

  TEventInfo  eventInfo(notifyCode, id);

  // NOTE: The ResponseTableEntry of handlers which expect an id
  //       (e.g. EV_COMMAND_AND_ID) have a zero in their first field.
  //       The ResponseTableEntry of handlers which expect a notification
  //       code (e.g. EV_CHILD_NOTIFY_AND_CODE and EV_NOTIFY_ALL_CODES)
  //       contain either the notifyCode or UINT_MAX in that field.
  //       Hence, we can dispatch the expected information based on the
  //       contents of that field.
  //
  //       However, this logic will fail to disambiguate cases where
  //       a notification code is 0. The only standard control with
  //       such a notification is BUTTON /w BN_CLICKED. So for button
  //       handlers expecting the id, you can use EV_COMMAND_AND_ID.
  //       For handlers expecting the notification code, you can use
  //       EV_NOTIFY_ALL_CODES.
  //
  //       Do *NOT* use "EV_CHILD_NOTIFY_AND_CODE(Id, BN_CLICKED, xxxx,...)"
  //
  if (receiver->Find(eventInfo, equal))
    return receiver->Dispatch(eventInfo,
                              eventInfo.Entry->NotifyCode ? notifyCode : id);
  else
    return receiver->DefaultProcessing();
}

//
// Handle WM_NOTIFY & subdispatch messages from child controls
//!CQ NOTE: similarity between EvCommand child notifies--merge?
//
TResult
TWindow::EvNotify(uint ctlId, TNotify& notifyInfo)
{

  // Intercept requests for tooltip texts and turn the request into
  // a 'CommandEnabler'. This mechanism allows use the route the request
  // the same way commands are routed in OWL. Therefore, the object that
  // handles a command is the object that get's first crack at providing
  // the tip text for that command.
  if (notifyInfo.code == (uint)TTN_NEEDTEXT) {
    TTooltipText& ttText = *(TTooltipText*)&notifyInfo;
    TTooltipEnabler enabler(ttText, *this);

#if defined(__TRACE) || defined(__WARN)
    _TCHAR text[50];
    _stprintf(text, (ttText.uFlags & TTF_IDISHWND) ? _T("Tip for 0x%X not found") :
                                                   _T("Text for %d not found"),
                                                    enabler.Id);
    ttText.CopyText(text);
    //enabler.SetText(text); // Do not call SetText(), it will set Handled!!!!
#endif
#if 0
    HandleMessage(WM_COMMAND_ENABLE, 0, TParam2(&enabler));
    return 0;
#else
    //RouteCommandEnable(*this, enabler); 
    RouteCommandEnable(GetParentO() ? GetParentH() : (HWND)*this, enabler); 
    if(enabler.GetHandled())//Y.B. return only if handled return !!!!!!!!!!!!!!
      return 0;
#endif
  }

  TWindow*        receiver = this;
  TEqualOperator  equal = 0;
  TWindow*         child = GetWindowPtr(notifyInfo.hwndFrom);

  if (child) {
    // Give the Owl child first crack at the event
    //
    receiver = child;
    ctlId = UINT_MAX;// special designator for child Id notifications that are
                     // handled at the child //!CQ may need a different value
  }
  else {
    // Offer the parent an opportunity to handle the notification
    //   NOTE: use equal function that will do a wildcard search
    //
    equal = wildcardCheck;

    // The child window identifier shouldn't be 0, but if it is then it will
    // look like we are searching for a WM_ message with value "notifyCode"
    //
    if (ctlId == 0)
      return DefaultProcessing();
  }

  TEventInfo  eventInfo(notifyInfo.code, ctlId);

  // Pass the "notifyCode" along in case the user wants it...
  //
  if (receiver->Find(eventInfo, equal))
    return receiver->Dispatch(eventInfo, notifyInfo.code, TParam2(&notifyInfo));
  else
    return receiver->DefaultProcessing();
}

//
// Handle an ObjectWindow's defined WM_COMMANDENABLE message by
// sub-dispatching it based on the command id
//
void
TWindow::EvCommandEnable(TCommandEnabler& commandEnabler)
{

// code copied from old unix owl (JAM 04-16-01)
//DLN test replace of DispatchMsg for CC 5.1
  TEventInfo eventInfo(WM_COMMAND_ENABLE,commandEnabler.Id);
  if (Find(eventInfo))
    Dispatch(eventInfo,0,TParam2(&commandEnabler));

//  DispatchMsg(WM_COMMAND_ENABLE, commandEnabler.Id, 0, TParam2(&commandEnabler));
}

//
// Don't process for windows out of our window tree (esp. other apps)
//
void
TWindow::RouteCommandEnable(HWND hInitCmdTarget, TCommandEnabler& commandEnabler)
{
  // Extra processing for commands & commandEnablers: send the command down the
  // command chain if we are the original receiver
  //
  if (commandEnabler.IsReceiver(*this)) {
    HWND  hCmdTarget = hInitCmdTarget;
    while (hCmdTarget && hCmdTarget != *this) {
      TWindow*  cmdTarget = GetWindowPtr(hCmdTarget);

      if (cmdTarget) {
        cmdTarget->EvCommandEnable(commandEnabler);

        if (commandEnabler.GetHandled())
          return;
      }
      hCmdTarget = ::GetParent(hCmdTarget);
    }
  }

  // Always call base handler
  //
  TWindow::EvCommandEnable(commandEnabler);

  // No one explicitly enabled/disabled the command via the enabler, so run up
  // the command chain checking for any one who is going to handle the command
  // itself; if not then disable it...
  // Don't do this for command senders that don't actually generate commands,
  // like popup menu items.
  //
  if (commandEnabler.IsReceiver(*this) && !commandEnabler.GetHandled()
      && commandEnabler.SendsCommand()) {
    bool        enable = false;
    TEventInfo  eventInfo(0, commandEnabler.Id);

    HWND  hCmdTarget = hInitCmdTarget;
    while (true) {
      TWindow*  cmdTarget = GetWindowPtr(hCmdTarget);

      if (cmdTarget && cmdTarget->Find(eventInfo)) {
        enable = true;  // command will be handled, leave sender alone
        break;
      }
      if (!hCmdTarget || hCmdTarget == *this)
        break;

      hCmdTarget = ::GetParent(hCmdTarget);
    }

    if (!enable) {
      // Check if the app wants to handle it
      //
      TEventInfo    enableInfo(WM_COMMAND_ENABLE, commandEnabler.Id);
      TApplication* app = GetApplication();
      if (app->Find(enableInfo)) {
        app->Dispatch(enableInfo, 0, TParam2(&commandEnabler));
        if (commandEnabler.GetHandled())
          return;
      }
      enable = app->Find(eventInfo);
    }

    commandEnabler.Enable(enable);
  }
}

//
// Virtual function provides final default processing for an incoming message
// Calls original window proc that was subclassed, using ::CallWindowProc to
// make sure that registers get setup correctly.
//
TResult
TWindow::DefWindowProc(uint message, TParam1 param1, TParam2 param2)
{
  PRECONDITION(DefaultProc);

  TResult result = ::CallWindowProc(DefaultProc, GetHandle(), message, param1, param2);
  return result;
}

//__OWL_BEGIN_NAMESPACE

//
// Message cache
//
static const int     msgCacheSize = 31;
struct TCacheEntry{
  uint32                       UniqueId;
  TGenericTableEntry  __RTFAR* Entry;
  uint                         Msg;
  int                          Delta;  // adjustment to "this" pointer

	//30.05.2007 - Submitted by Frank Rast: Initialization of TCacheEntry data members was missing.
	TCacheEntry() : UniqueId(0), Entry(0), Msg(0), Delta(0) {}

  void Set(uint32 uniqueId, uint msg, TGenericTableEntry __RTFAR* entry, int delta = 0) {
    UniqueId = uniqueId;
    Entry = entry;
    Msg = msg;
    Delta = delta;
  }
  bool  Hit(uint msg, uint32 uniqueId) {return msg == Msg && uniqueId == UniqueId;}

  static uint  Key(uint32 id, uint msg) {return (uint(id) ^ msg) % msgCacheSize;}
};


struct TCacheEntryStr 
#if defined(BI_MULTI_THREAD_RTL)
: public TLocalObject
#endif
{ 
  TCacheEntryStr():Enabled(true)
    {
    }
  ~TCacheEntryStr()
    {
    }

  TCacheEntry  Cache[msgCacheSize];
  bool         Enabled;

  void CacheFlush(uint32 id);
  void Set(int index, uint32 uniqueId, uint msg, TGenericTableEntry __RTFAR* entry, int delta = 0); 
  bool Check(int index, uint msg, uint32 id); 

#if defined(BI_MULTI_THREAD_RTL)
//  TMRSWSection  Lock;
#endif
};

uint32 TWindow::LastUniqueId = 0;

static TCacheEntryStr& GetCache()
{
#if defined(BI_MULTI_THREAD_RTL)
  static TTlsContainer<TCacheEntryStr> __CacheEntry;
#else
  static TCacheEntryStr __CacheEntry;
#endif
  return __CacheEntry;
};
//
#if defined(BI_MULTI_THREAD_RTL)
#define LOCKCACHE //TMRSWSection::TLock Lock(GetCache().Lock);
#else
#define LOCKCACHE
#endif
//
void TCacheEntryStr::CacheFlush(uint32 id)
{
  LOCKCACHE
  for (int i = 0; i < msgCacheSize; i++)
    if (Cache[i].UniqueId == id)
      Cache[i].Msg = 0;
}
//
void TCacheEntryStr::Set(int index, uint32 uniqueId, uint msg, TGenericTableEntry __RTFAR* entry, int delta) 
{
  LOCKCACHE
  Cache[index].Set(uniqueId, msg, entry, delta);
}
//
bool TCacheEntryStr::Check(int index, uint msg, uint32 id)
{
  LOCKCACHE
  return Enabled && Cache[index].Hit(msg, id);
}
//
void CacheFlush(uint32 id) 
{
  GetCache().CacheFlush(id);
}


//
// If rtti is available, then use it get an id for this window that is unique
// on a per-class basis.
//
// Without rtti, use a per-instance unique id. Less efficient, but safe.
//
void
TWindow::SetUniqueId()
{
#if defined(OWL_RTTI_MSGCACHE)
  UniqueId = 0;
#else
  if (++LastUniqueId == 0) {
    //
    // numbers wrapped around. disable the cache to be safe...
    //
    LastUniqueId = 1;
    GetCache().Enabled = false;
  }
  UniqueId = LastUniqueId;
#endif
}

//
// First virtual function called to handling incoming messages to a TWindow
//
// Hand calls handler for WM_COMMAND, WM_COMMAND_ENABLE and WM_NOTIFY.
// otherwise looks for a handler in the message response table
//
TResult
TWindow::WindowProc(uint msg, TParam1 param1, TParam2 param2)
{
  PRECONDITION(GetHandle());  // Should never get here without a handle

  // Handle WM_COMMAND_ENABLE command enable msgs by directly calling the
  // virtual EvCommandEnable
  //
  if (msg == WM_COMMAND_ENABLE) {
    TRACEX(OwlMsg, 1, _T("WM_COMMAND_ENABLE") << _T("! => ") << *this);
    EvCommandEnable(*(TCommandEnabler*)param2);
    return 0;
  }
  // Handle WM_COMMAND command msgs by directly calling the
  // virtual EvCommand
  //
  else if (msg == WM_COMMAND) {
    TRACEX(OwlMsg, 1, _T("WM_COMMAND, ") << LoUint16(param1) << _T(" ! => ") << *this);
    return EvCommand(LoUint16(param1), HWND(uint(param2)), HiUint16(param1));
  }
  // Handle WM_NOTIFY msgs by directly calling the virtual EvNotify
  //!CQ why not use response table dispatch? For above too?
  //
  else if (msg == WM_NOTIFY) {

    // We've received reports of some controls (out there) sending
    // WM_NOTIFY with a NULL LPARAM !!
    //
    static TNotify dumbNMHDR;
    //DLN SET ID to 0 for these NULL LPARAM to avoid bad resource access
    dumbNMHDR.idFrom = static_cast<UINT>(-1);
    TNotify* notify = param2 ? (TNotify*)param2 : &dumbNMHDR;

    TRACEX(OwlMsg, 1, _T("WM_NOTIFY, ") << notify->idFrom   << _T(", ")\
                                    << notify->code     << _T(", ")\
                                    << hex << uint(notify->hwndFrom) << _T(", ")\
                                    << _T(" ! => ") << *this);
    return EvNotify(param1, *notify);
  }

  // Handle all other messages by looking up and dispatching using the
  // response tables
  //
  else {
#if defined(OWL_RTTI_MSGCACHE)
    if (!UniqueId)
      UniqueId = TYPE_UNIQUE_UINT32(*this);
#endif
    uint        key = TCacheEntry::Key(UniqueId, msg);
    TEventInfo  eventInfo(msg);

    // Check the cache. A cache hit may be an RT handler, or an RT miss.
    //
    TCacheEntryStr& cache = GetCache();
    if(cache.Check(key, msg, UniqueId)) {
      eventInfo.Entry = cache.Cache[key].Entry;
      if (eventInfo.Entry) {
        TRACEX(OwlMsg, 1, MsgName(msg) << _T("* => ") << *this);
        eventInfo.Object = (GENERIC*)(((char*)this) + cache.Cache[key].Delta);
        return Dispatch(eventInfo, param1, param2);

      } // else fall out & do default below
    }
    // Perform the lookup on this window.
    //
    else if (this->Find(eventInfo)) {
      TRACEX(OwlMsg, 1, MsgName(msg) << _T("  => ") << *this);
      cache.Set(key,UniqueId, msg, eventInfo.Entry, int(eventInfo.Object) - int(this));
      return Dispatch(eventInfo, param1, param2);
    }
    else  // not found
      cache.Set(key,UniqueId, msg, 0);  // Cache no-handler entries too

    // Behavior for messages that have no handler. If this is the main window,
    // then give the app a chance to handle the message. If not the main
    // window, or if the app had no handler, just call DefWindowProc() to
    // pass the message back to whomever we subclassed
    //
    if (IsFlagSet(wfMainWindow)) {
      TEventInfo cmdEventInfo(msg, param1);
      if (GetApplication()->Find(cmdEventInfo))
        return GetApplication()->Dispatch(cmdEventInfo, param1, param2);
      if (GetApplication()->Find(eventInfo))
        return GetApplication()->Dispatch(eventInfo, param1, param2);
    }
    return DefWindowProc(msg, param1, param2);
  }
}

//
// Save current event and call this window's WindowProc. Handles exceptions,
// error checking, & GetWindowPtr messages.
//
TResult
TWindow::HandleMessage(uint msg, TParam1 param1, TParam2 param2)
{
  // Save event away in "CurrentEvent"
  //
  TCurrentEvent& currentEvent = GetCurrentEvent();
  TCurrentEvent  saveEvent = currentEvent;  // for nested calls
  currentEvent.Win = this;
  currentEvent.Message = msg;
  currentEvent.Param1 = param1;
  currentEvent.Param2 = param2;

  // Call window object's WindowProc virtual function
  //
  TResult  result;
  result = WindowProc(msg, param1, param2);
  currentEvent = saveEvent;  // restore previous event to current event
  return result;
}


//
// First member function that receives messages from windows thru the
// instance window proc. Deals with exception suspension for non-NT
// environments.
//
TResult
TWindow::ReceiveMessage(uint msg, TParam1 param1, TParam2 param2)
{
  // If it's a "GetWindowPtr" message, then return pointer to this window
  //
  if (msg == GetWindowPtrMsgId && (!param2 || param2 == TParam2(Application)))
    return TResult(this);

  if (TSystem::SupportsExceptions())
    return HandleMessage(msg, param1, param2);

  // When exceptions are disabled CATCH is defined as empty, so the only code
  // is the TRY block, making this return unreachable
  //
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
# pragma warn -rch
#endif
  return msg == WM_CREATE ? -1 : 0;  // default value returned when exception caught
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
# pragma warn .rch
#endif
}

//
// Determine the object pointer by sending a GetWindowPtrMsgId message to the
// window.  When the first StdWndProc() in the subclass chain receives this
// message it returns a pointer to the object.
// If app is non-zero, then the process makes sure that the corresponding
// TWindow is returned.
//
_OWLFUNC(TWindow*) GetWindowPtr(HWND hWnd, const TApplication* app)
{
  if (!hWnd /* && ::IsWindow(hWnd) */)  // Could also check handle validity
    return 0;

  TParam2 param2 = TParam2(app);

  // Avoid SendMessage to cutdown the overhead & message spy tool flooding
  //
  // Under Win16, need to fallback to SendMessage when the given hWnd is owned
  // by another task. Using CallWindowProc on it would result in a bad SS
  // setup when it received the message.
  //
  // Under Win32 don't even try if it is not our process. Some Win32's will
  // return a wndProc that crashes.
  //
  DWORD processId;
  ::GetWindowThreadProcessId(hWnd, &processId);
  if (processId != ::GetCurrentProcessId())
    return 0;

  WNDPROC wndProc = (WNDPROC)::GetWindowLong(hWnd, GWL_WNDPROC);
  if (!wndProc)
    return 0;
  return (TWindow*)::CallWindowProc(wndProc, hWnd, GetWindowPtrMsgId, 0, param2);
}

//__OWL_END_NAMESPACE
//__OWL_BEGIN_NAMESPACE
//
// Response method for an incoming WM_CREATE message
//
// Call virtual SetupWindow to give derived classes a chance to set things up
// now that we are created & have a Handle
//
int
TWindow::EvCreate(CREATESTRUCT &)
{
  SetupWindow();
  SetFlag(wfFullyCreated);
  return (int)DefaultProcessing();
}

//
// Regular windows never hold focus child handles--just say no.
//
bool
TWindow::HoldFocusHWnd(HWND /*hWndLose*/, HWND /*hWndGain*/)
{
  return false;
}

//
// Handle WM_KILLFOCUS so that we can have a parent window hold onto our
// Handle and possibly restore focus later.
//
void
TWindow::EvKillFocus(HWND getFocus)
{
  // Follow the parent chain back until a window volunteers to hold our handle
  //
  if (IsFlagSet(wfFullyCreated)) {
    TWindow* holdWin = Parent;
    while (holdWin && !holdWin->HoldFocusHWnd(GetHandle(), getFocus))
      holdWin = holdWin->Parent;
  }

  DefaultProcessing();
}

//
// Response method for an incoming WM_SIZE message
//
// Saves the normal size of the window in Attr.
// Also calls the SetPageSize() and SetBarRange() methods of the TWindow's
// scroller, if it has been constructed.
//
void
TWindow::EvSize(uint sizeType, TSize&)
{
  TraceWindowPlacement();
  static bool inScroller = false;
  if (!inScroller && Scroller && sizeType != SIZE_MINIMIZED) {
    inScroller = true;
    Scroller->SetPageSize();
    Scroller->SetSBarRange();
    inScroller = false;
  }

  if (sizeType == SIZE_RESTORED) {
    TRect  wndRect;
    GetWindowRect(wndRect);

    Attr.W = wndRect.Width();
    Attr.H = wndRect.Height();
  }

  // Added Owl functionality: notify parent of a resize in case it wants to
  // adjust accordingly
  //
  if (Parent && !(GetExStyle() & WS_EX_NOPARENTNOTIFY))
    Parent->SendMessage(WM_PARENTNOTIFY, WM_SIZE, TParam2(GetHandle()));

  DefaultProcessing();
}

//
// Save the normal position of the window.
// If IsIconic() or IsZoomed() ignore the position since it does not reflect
// the normal state
//
void
TWindow::EvMove(TPoint&)
{
  if (!IsIconic() && !IsZoomed()) {
    TRect wndRect;

    GetWindowRect(wndRect);

    if ((GetWindowLong(GWL_STYLE) & WS_CHILD) == WS_CHILD && Parent &&
         Parent->GetHandle())
      Parent->ScreenToClient(wndRect.TopLeft());

    Attr.X = wndRect.left;
    Attr.Y = wndRect.top;
  }

  DefaultProcessing();
}

//
// Handles WM_COMPAREITEM message (for owner draw controls) by forwarding
// message to control itself
//
TResult
TWindow::EvCompareItem(uint /*ctrlId*/, COMPAREITEMSTRUCT & compareInfo)
{
  TWindow* win = GetWindowPtr(compareInfo.hwndItem);
  if (win)
    return win->ForwardMessage();
  return DefaultProcessing();
}

//
// Handles WM_DELETEITEM message (for owner draw controls) by forwarding
// message to control itself
//
void
TWindow::EvDeleteItem(uint /*ctrlId*/, DELETEITEMSTRUCT & deleteInfo)
{
  TWindow* win = GetWindowPtr(deleteInfo.hwndItem);
  if (deleteInfo.hwndItem != GetHandle() && win)
    win->ForwardMessage();
  else
    DefaultProcessing();
}
//
// Handles WM_DRAWITEM message (for owner draw controls & menus) by forwarding
// message to control itself
//
TDrawItem* ItemData2DrawItem(uint32 data);
void
TWindow::EvDrawItem(uint /*ctrlId*/, DRAWITEMSTRUCT & drawInfo)
{
  if (drawInfo.CtlType == ODT_MENU) {
    TDrawItem* item = ItemData2DrawItem(drawInfo.itemData);
    if(item){
      item->Draw(drawInfo);
      return;
    }
  }
  else {
    TWindow* win = GetWindowPtr(drawInfo.hwndItem);
    if (drawInfo.hwndItem != GetHandle() && win) {
      win->ForwardMessage();
      return;
    }
  }
  DefaultProcessing();
}

//
// Handles WM_MEASUREITEM message (for owner draw controls & menus) by
// forwarding message to control itself
//
void
TWindow::EvMeasureItem(uint ctrlId, MEASUREITEMSTRUCT & measureInfo)
{
  if (measureInfo.CtlType == ODT_MENU) {
    TDrawItem* item = ItemData2DrawItem(measureInfo.itemData);
    if(item){
      item->Measure(measureInfo);
      return;
    }
  }
  else {
    HWND  hCtl = GetDlgItem(measureInfo.CtlID);  // hWnd not in struct, get
    TWindow* win = GetWindowPtr(hCtl);

    // If the GetWindowPtr failed, & CreationWindow is non-zero, then this
    // WM_MEASUREITEM is probably for the ctrl which is not yet subclassed.
    // Route the message directly to creation window.
    // NOTE: Msg. may be sent before OWL has had a chance to subclass the ctrl.
    //
    if (!win) {
      TWindow* creationWindow = GetCreationWindow();
      if (creationWindow) {
        if (creationWindow != this)   // Don't cause a recursion loop
          win = creationWindow;
      }
      else
        win = ChildWithId(ctrlId);
    }
    if(win){
      // 970921 MILI 11 The first WM_MEASUREITEM message for a control may
      // be is sent before OWL has had any chance to grab the handle for that
      // control. In that case we use Find+Dispatch instead of SendMessage to
      // forward the message to the control.
      if(win->GetHandle()){
//!CQ Handle causes bad behavior on DefWindowProc of control in some cases
//!   win->HandleMessage(WM_MEASUREITEM, ctrlId, TParam2(&measureInfo));
//!   win->ForewardMessage();
        win->SendMessage(WM_MEASUREITEM, ctrlId, TParam2(&measureInfo));
        return;
      }
      else{
        TEventInfo eventInfo(WM_MEASUREITEM);
        if (win->Find(eventInfo)){
          win->Dispatch(eventInfo, ctrlId, TParam2(&measureInfo));
           return;
        }
      }
    }
  }
  DefaultProcessing();
}

//
// Process Win32 Control Color messages by redispatching to Win16-like member
// function.
//
TResult
TWindow::EvWin32CtlColor(TParam1 param1, TParam2 param2)
{
  int  ctlType = GetCurrentEvent().Message - WM_CTLCOLORMSGBOX;

  CHECK(ctlType >= CTLCOLOR_MSGBOX && ctlType <= CTLCOLOR_STATIC);

  TEventInfo  eventInfo(WM_CTLCOLOR);

  if (Find(eventInfo)) {
    typedef HBRUSH(GENERIC::*CTL_COLOR_PMF)(HDC, HWND, uint);

    CTL_COLOR_PMF&  pmf = (CTL_COLOR_PMF&)(eventInfo.Entry->Pmf);

    return (TResult)(eventInfo.Object->*pmf)((HDC)param1, (HWND)param2, ctlType);
  }
  return DefWindowProc(GetCurrentEvent().Message, param1, param2);
}

//
// Dispatches scroll messages as if they were Command messages.
//
void
TWindow::DispatchScroll(uint scrollCode, uint /*thumbPos*/, HWND hWndCtrl)
{
  if (hWndCtrl) {
    TWindow* win = GetWindowPtr(hWndCtrl);
    if (win)
      win->ForwardMessage();

    // Adjust "CurrentEvent" to allow DefaultProcessing to work
    //
    uint16 id = (uint16)::GetDlgCtrlID(hWndCtrl);
    TCurrentEvent& currentEvent = GetCurrentEvent();
    currentEvent.Message = WM_COMMAND;
      currentEvent.Param1 = MkParam1(id, scrollCode);
      currentEvent.Param2 = TParam2(hWndCtrl);

    EvCommand(id, hWndCtrl, scrollCode);
    return;
  }
  DefaultProcessing();
}


//
// Response method for an incoming WM_HSCROLL message
//
TResult 
TWindow::EvRegisteredMouseWheel(TParam1 param1, TParam2 param2)
{
  // convert from MSH_MOUSEWHEEL to WM_MOUSEWHEEL

  uint keyState = 0;
  keyState |= (::GetKeyState(VK_CONTROL) < 0) ? MK_CONTROL : 0;
  keyState |= (::GetKeyState(VK_SHIFT) < 0) ? MK_SHIFT : 0;

  class _TDesktopWindow {
    public:
    _TDesktopWindow() : hwnd(::GetDesktopWindow()){}
    operator  HWND() const { return hwnd; }
    HWND      hwnd;
  };
  static const _TDesktopWindow deskTopHwnd;

  TResult lResult;
  HWND hwFocus = GetFocus();

  if(hwFocus == NULL)
    lResult = SendMessage(WM_MOUSEWHEEL, (param1 << 16) | (uint16)keyState, param2);
  else{
    do{
      lResult = ::SendMessage(hwFocus, WM_MOUSEWHEEL,(param1<<16)|(uint16)keyState, param2);
      hwFocus = ::GetParent(hwFocus);
    }
    while (lResult == 0 && hwFocus != NULL && hwFocus != deskTopHwnd);
  }
  return lResult;
}

//
static bool _owlGotScrollLines = false;
// not defined for BC 5.01
#if !defined(SPI_GETWHEELSCROLLLINES)
#define SPI_GETWHEELSCROLLLINES   104
#endif
//
static uint 
_OwlGetMouseScrollLines()
{
  static uint uCachedScrollLines = 0;

  // if we've already got it and we're not refreshing,
  // return what we've already got
  if(_owlGotScrollLines)
    return uCachedScrollLines;

  // see if we can find the mouse window
  _owlGotScrollLines = true;

  static uint    msgGetScrollLines  = 0;
  static uint16 nRegisteredMessage = 0;

  if (nRegisteredMessage == 0){
    msgGetScrollLines = ::RegisterWindowMessage(MSH_SCROLL_LINES);
    if (msgGetScrollLines == 0)
      nRegisteredMessage = 1;     // couldn't register!  never try again
    else
      nRegisteredMessage = 2;     // it worked: use it
  }

  if (nRegisteredMessage == 2){
    HWND hwMouseWheel = FindWindow(MSH_WHEELMODULE_CLASS, MSH_WHEELMODULE_TITLE);
    if (hwMouseWheel && msgGetScrollLines){
      uCachedScrollLines = (uint)
        ::SendMessage(hwMouseWheel, msgGetScrollLines, 0, 0);
      return uCachedScrollLines;
    }
  }

  // couldn't use the window -- try system settings
  uCachedScrollLines = 3; // reasonable default
  if(!TSystem::IsWin95()){
    ::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uCachedScrollLines, 0);
  }
  return uCachedScrollLines;
}

//
// Respond to MouseWheel event if Scroller exist
//
bool 
TWindow::EvMouseWheel(uint modKeys, int zDelta, TPoint& /*point*/)
{
  // we don't handle anything but scrolling just now
  if (modKeys & (MK_SHIFT | MK_CONTROL))
    return DefaultProcessing();

  if (Scroller)
  {
    uint  uWheelScrollLines = _OwlGetMouseScrollLines();

    int scrollCnt = 0;
    if (uWheelScrollLines == WHEEL_PAGESCROLL)
    {
      if (zDelta > 0)
        scrollCnt  = -((int)Scroller->YPage);
      else
        scrollCnt  = ((int)Scroller->YPage);
    }
    else
    {
      scrollCnt = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA) * (int)Scroller->YLine;
    }
		

//    int    scrollCnt = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
//    scrollCnt = scrollCnt*(int)Scroller->YLine;
//    if (scrollCnt == -1 || uWheelScrollLines == WHEEL_PAGESCROLL){
//      scrollCnt  = (int)Scroller->YPage;
//      if (zDelta > 0)
//        scrollCnt  = -((int)Scroller->YPage);
//    } 

    Scroller->ScrollBy(0, scrollCnt);
    return true;
  }
  return DefaultProcessing();
}

//
// Response method for an incoming WM_HSCROLL message
//
// If the message is from a scrollbar control, calls DispatchScroll()
// otherwise passes the message to the TWindow's scroller if it has been
// constructed, else calls DefaultProcessing()
//
// Assumes, because of a Windows bug, that if the window has the scrollbar
// style, it will not have scrollbar controls
//
void
TWindow::EvHScroll(uint scrollCode, uint thumbPos, HWND hWndCtl)
{
  if (!(GetWindowLong(GWL_STYLE) & WS_HSCROLL) && !Scroller)
  {
    DispatchScroll(scrollCode, thumbPos, hWndCtl);  // from scrollbar control
  }
  else if (Scroller)
  {
    Scroller->HScroll(scrollCode, thumbPos);
  }
  else
  {
    DefaultProcessing();
  }
}

//
// Response method for an incoming WM_VSCROLL message
//
// If the message is from a scrollbar control, calls DispatchScroll()
// otherwise passes the message to the TWindow's scroller if it has been
// constructed, else calls DefaultProcessing()
//
// Assumes, because of a Windows bug, that if the window has the scrollbar
// style, it will not have scrollbar controls
//
void
TWindow::EvVScroll(uint scrollCode, uint thumbPos, HWND hWndCtl)
{
  if (!(GetWindowLong(GWL_STYLE) & WS_VSCROLL) && !Scroller)
    DispatchScroll(scrollCode, thumbPos, hWndCtl);

  else if (Scroller)
    Scroller->VScroll(scrollCode, thumbPos);

  else
    DefaultProcessing();
}

//
// Response method for an incoming WM_ERASEBKGND message
//
bool
TWindow::EvEraseBkgnd(HDC hDC)
{
  // If color is set, we'll handle erasing (or doing nothing) here
  //
  if (BkgndColor != TColor::None) {

    // If a color is set, blt out a rectangle of it, else don't erase & let
    // paint handle background
    //
    if (BkgndColor != TColor::Transparent) {
      TDC dc(hDC);
      TBrush bkBrush(BkgndColor);
      dc.SelectObject(bkBrush);
      dc.SetBkColor(BkgndColor);
      dc.PatBlt(GetClientRect());
      dc.RestoreBrush();
    }
    return true;
  }

  // No color set, use default class brush
  //
  return (bool)DefaultProcessing();
}

//
// Respond to WM_CTLCOLOR if we have a bkgnd color set & pass that to the
// control
//
HBRUSH
TWindow::EvCtlColor(HDC hDC, HWND /*hWndChild*/, uint ctlType)
{
  // If a bkgnd color is set, then setup the hdc's bkcolor and return a
  // brush for the child to use
  //
  if (BkgndColor != TColor::None && BkgndColor != TColor::Transparent
      && ctlType != CTLCOLOR_EDIT && ctlType != CTLCOLOR_LISTBOX)
  {
    ::SetBkColor(hDC, BkgndColor);
    return TBrush(BkgndColor);  // HBRUSH will stay in cache
  }

  // No color set, use default windows behavior
  //
  return (HBRUSH)DefaultProcessing();
}

//
// Response method for an incoming WM_PAINT message
//
// Calls Paint(), performing Windows-required paint setup and cleanup before
// and after. if the TWindow has a TScroller, also calls its BeginView() and
// EndView() methods before and after call to Paint()
//
void
TWindow::EvPaint()
{
  if (IsFlagSet(wfAlias))
    DefaultProcessing();  // use application-defined wndproc

  else {
    TPaintDC dc(*this);
    TRect&   rect = *(TRect*)&dc.Ps.rcPaint;

    if (Scroller)
      Scroller->BeginView(dc, rect);

    Paint(dc, dc.Ps.fErase, rect);

    if (Scroller)
      Scroller->EndView();
  }
}

//
// Redraws the contents of the TWindow after a WM_PAINT message
// is received
//
// Placeholder for descendant object types to redefine
//
void
TWindow::Paint(TDC&, bool /*erase*/, TRect&)
{
}

//
// Response method for an incoming WM_SETCURSOR message
//
// If a cursor has been set for this window, & the mouse is over the
// client area, set the cursor
//
bool
TWindow::EvSetCursor(HWND hWndCursor, uint hitTest, uint /*mouseMsg*/)
{
  if (hWndCursor == GetHandle() && hitTest == HTCLIENT && HCursor) {
    ::SetCursor(HCursor);
    return true;
  }
  return (bool)DefaultProcessing();
}

//
// Response method for an incoming WM_LBUTTONDOWN message
//
// If the TWindow's Scroller has been constructed and if auto-scrolling
// has been requested, captures mouse input, loops until a WM_LBUTTONUP
// message comes in calling the Scroller's AutoScroll method, and then
// releases capture on mouse input.
// Will also break if a WM_MOUSEMOVE comes in without the left button down
// indicating a lost WM_LBUTTONUP
//
void
TWindow::EvLButtonDown(uint /*modKeys*/, TPoint& /*pt*/)
{
  if (Scroller && Scroller->IsAutoMode()) {
    MSG  loopMsg;

    loopMsg.message = 0;
    SetCapture();

    while (loopMsg.message != WM_LBUTTONUP && Scroller->IsAutoMode() && 
          (loopMsg.message != WM_MOUSEMOVE || (loopMsg.wParam&MK_LBUTTON))) {
      if (::PeekMessage(&loopMsg, 0, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&loopMsg);
        ::DispatchMessage(&loopMsg);
      }

      Scroller->AutoScroll();
    }

    ReleaseCapture();
  }

  DefaultProcessing();
}

//__OWL_BEGIN_NAMESPACE
//
//
//
void
DoEnableAutoCreate(TWindow* win, void* /*retVal*/)
{
  if (win->GetHandle())
    win->EnableAutoCreate();
}

//__OWL_END_NAMESPACE

//
// Destroys an MS-Windows element associated with the TWindow.
// First, sets the wfAutoCreate flag to ON for each of the windows in the
// TWindow's ChildList to allow later re-creation.
//
void
TWindow::Destroy(int ret)
{
  if (GetHandle()) {
    ForEach(DoEnableAutoCreate, 0);

    if (IsFlagSet(wfModalWindow)) {
      GetApplication()->EndModal(ret);
      ClearFlag(wfModalWindow);
      if (Parent && Parent->GetHandle())
        Parent->SetFocus();
    }

    // For aliases:
    //  - we are destructing the C++ object but not destroying the MS window
    //  - restore the original window function
    //
    if (!IsFlagSet(wfAlias)) {
      if (::DestroyWindow(GetHandle()))
        SetHandle(0);
      }

    WARNX(OwlWin, GetHandle(), 0, _T("::DestroyWindow(") << 
          (uint)GetHandle() << _T(") failed"));
  }
}

//
// Specifies registration attributes for the MS-Windows window class of the
// TWindow, allowing instances of TWindow to be registered
//
// Sets the fields of the passed WNDCLASS parameter to the default attributes
// appropriate for a TWindow
//
// Register unique classes for windows that want system background colors so
// that full-drag erasing uses the right color (NT fails to let window erase
// itself)
//
void
TWindow::GetWindowClass(WNDCLASS& wndClass)
{
  wndClass.cbClsExtra = 0;
  wndClass.cbWndExtra = 0;
  wndClass.hInstance = *GetModule();
  wndClass.hIcon = 0;
  wndClass.hCursor = ::LoadCursor(0, IDC_ARROW);

  if (BkgndColor.IsSysColor() &&
      BkgndColor != TColor::None && BkgndColor != TColor::Transparent)
    wndClass.hbrBackground = HBRUSH(BkgndColor.Index()+1);
  else
    wndClass.hbrBackground = HBRUSH(COLOR_WINDOW+1);

  wndClass.lpszMenuName = 0;
  wndClass.lpszClassName = GetClassName();
  wndClass.style = CS_DBLCLKS;
  wndClass.lpfnWndProc = InitWndProc;
}

//
// Return the classname for a generic owl window. Assume instance private class
// registration so that no instance mangling is needed.
//
// Register unique classnames for windows that want system background colors
//
LPCTSTR
TWindow::GetClassName()
{
  static const _TCHAR baseClassName[] = _T("OWL_Window");

  if (BkgndColor.IsSysColor() &&
      BkgndColor != TColor::None && BkgndColor != TColor::Transparent) {
    static _TCHAR namebuff[COUNTOF(baseClassName) + 1 + 10 + 1 + 4];
    _stprintf(namebuff, _T("%s:%X"), baseClassName, BkgndColor.Index());
    //!CQ could hash in classStyle too.
    return namebuff;
  }
  return baseClassName;  // cast for old non-const type
}

//
// Set this window's accelerator table, performing the load also if this window
// is already created
//
void
TWindow::SetAcceleratorTable(TResId resId)
{
  Attr.AccelTable = resId;
  if (GetHandle())
    LoadAcceleratorTable();
  else
    HAccel = 0;
}

//
// Load the accelerator table indicated in Attr from this window's module's
// resource.
//
void
TWindow::LoadAcceleratorTable()
{
  if (Attr.AccelTable) {
    HAccel = GetModule()->LoadAccelerators(Attr.AccelTable);
    WARNX(OwlWin, !HAccel, 0,
          _T("Unable to load accelerators ") << Attr.AccelTable
          << _T(" from ") << *GetModule());
  }
}

//
// Perform MS Windows HWND creation and keep hwnd in Handle member
//
void
TWindow::PerformCreate(int menuOrId)
{
  PRECONDITION(GetModule());
  HWND hwnd = ::CreateWindowEx(Attr.ExStyle,
                               GetClassName(),
                               Title,
                               Attr.Style,
                               Attr.X, Attr.Y, Attr.W, Attr.H,
                               Parent ? Parent->GetHandle() : 0,
                               REINTERPRET_CAST(HMENU,menuOrId),
                               *GetModule(),
                               Attr.Param);
  SetHandle(hwnd);
//  if(!hwnd){
//    TSystemMessage().MessageBox(Parent);
//  }
#if defined(__WARN)
  _TCHAR modName[_MAX_PATH];
  GetModule()->GetModuleFileName(modName, _MAX_PATH);
  uint parent = Parent ? uint(Parent->GetHandle()) : 0;
  WARNX(OwlWin, !hwnd, 0, _T("CreateWindowEx failed: ") <<\
                _T("Class: \"") << owl_string(GetClassName()).c_str() << _T("\", ") <<\
                _T("Title: \"") << owl_string(Title).c_str() << _T("\", ") <<\
                _T("Style: ") << Attr.Style << _T(", ") <<\
                _T("Parent: ") << parent << _T(", ")        <<\
                _T("Module: \"") << modName);
#endif
}

//
// Associates an MS Windows interface element with the TWindow object,
// after creating the interface element if not already created
//
// When creating an element, uses the creation attributes previously set in
// the Attr data field (simply associates the TWindow with an already
// created interface element if the "FromResource" flag is set)
//
// Since this member function now throws an exception on error, it always
// returns true.
//
bool
TWindow::Create()
{
  if (GetHandle())
    return true;

  int  menuOrId = 0;

  DisableAutoCreate();

  // If this is the main window, make sure it is treated as one by the shell
  //
  if (IsFlagSet(wfMainWindow))
    ModifyExStyle(0, WS_EX_APPWINDOW);

  if (IsFlagSet(wfFromResource))
    SetHandle(Parent ? Parent->GetDlgItem(Attr.Id) : 0);  // Windows already created it

  else if (Register()) {
    SetCreationWindow(this);

    LoadAcceleratorTable();

    if (Attr.Menu) {
      menuOrId = (int)GetModule()->LoadMenu(Attr.Menu);
      WARNX(OwlWin, !menuOrId, 0, _T("Unable to load menu: ") << Attr.Menu <<
            _T(" from ") << *GetModule());
    }
    else
      menuOrId = Attr.Id;

    PerformCreate(menuOrId);
  }
  else
    TXWindow::Raise(this, IDS_CLASSREGISTERFAIL);

  if (!GetHandle()) {
    if (Attr.Menu)  // && !IsFlagSet(wfFromResource) ?
      DestroyMenu(HMENU(menuOrId));

    TXWindow::Raise(this, IDS_WINDOWCREATEFAIL);
  }

  // Here we have a non-subclassed handle. This may be caused by two scenarios:
  // 1. Predefined-class (non-owl) windows.
  // 2. OWL controls which were created from resource [Although these
  //    are registered with InitWndProc, they did not get subclassed since
  //    'CreationWindow' was not set when they received their first messages].
  //
  if (!GetWindowPtr(GetHandle())) {

    // Grab the state info.
    //
    GetWindowTextTitle();
    GetHWndState();

    // If it's a 'predefinedClass' window, subclass it
    //
    if (GetWindowProc() != InitWndProc) {
      SubclassWindowFunction();

      // Reset the 'CreationWindow' pointer [if necessary]
      //
      if (GetCreationWindow() == this)
        SetCreationWindow(0);

      // Set status flag [since we missed EvCreate]
      //
      SetFlag(wfPredefinedClass|wfFullyCreated);
    }
    else {
      // This window's WNDPROC is 'InitWndProc' - However, it has not
      // been subclassed since 'CreationWindow' was not set when it received
      // its first messages [it was probably created from a resource but
      // registered by OWL]. We'll set 'CreationWindow' and send a dummy
      // message to allow subclassing to take place
      //
      if (!GetCreationWindow())
        SetCreationWindow(this);
      SendMessage(WM_USER+0x4000, 0, 0);

      // Update flags
      //
      SetFlag(wfFullyCreated);
    }

    // Invoke SetupWindow since 'EvCreate' was missed..
    //
    SetupWindow();
  }
  return true;
}

//
// Creates the underlying HWND and makes modal with the help
// of TApplication's BeginModal support.
//
int
TWindow::Execute()
{
  return DoExecute();
}

//
// Do actual modal execution using the Begin/End Modal
// support of TApplication.
// NOTE: Defaults to 'TASKMODAL'
//
int
TWindow::DoExecute()
{
  // Attempting to go modal when one's a child is indicative of
  // suicidal tendencies!!
  //
  PRECONDITION((GetStyle() & WS_CHILD) == 0);

  if (GetApplication()) {
    if (Create()) {
      SetFlag(wfModalWindow);
      return GetApplication()->BeginModal(this, MB_TASKMODAL);
    }
  }
  return -1;
}

//
// Performs setup following creation of an associated MS-Windows window.
// Iterates through the TWindow's ChildList, attempting to create
// an associated MS-Windows interface element for each child window
// object in the list (a child's Create method is not called if its
// wfAutoCreate flag is not set)
//
// Calls TransferData to transfer data for its children for whom data transfer
// is enabled.
//
// If the receiver has a TScroller object, calls the scroller's SetBarRange()
// method.
//
// Can be redefined in derived classes to perform additional special
// initialization that requires an HWND.
//
void
TWindow::SetupWindow()
{
  TRACEX(OwlWin, 1, _T("TWindow::SetupWindow() @") << (void*)this << *this);

  // Update scrollbar - if a scroller was attached to the window
  //
  if (Scroller){
    Scroller->SetWindow(this);
    Scroller->SetSBarRange();
  }

  // If this is main Window and GetAplication()->GetTooltip() != 0; create it.
  if (IsFlagSet(wfMainWindow)){
    TTooltip* tooltip = GetApplication()->GetTooltip();
    if(tooltip){
      if(!tooltip->GetParentO())
        tooltip->SetParent(this);
      if(!tooltip->GetHandle()){
        // Make sure tooltip is disabled so it does not take input focus
        tooltip->ModifyStyle(0,WS_DISABLED);
        tooltip->Create();
      }
    }
  }

  // NOTE: CreateChildren will throw a TXWindow exception if one of the
  //       child objects could not be created.
  //
  CreateChildren();

  // Initialize children specified data
  //
  TransferData(tdSetData);
}

//
// Always called just before Handle goes away to give derived classes a
// chance to cleanup native Handle related resources.
//
void
TWindow::CleanupWindow()
{
  TRACEX(OwlWin, 1, _T("TWindow::CleanupWindow() @") << (void*)this << *this);
}

//
// Transfer window 'data' to/from the passed data buffer.  Used to initialize
// windows and get data in or out of them.
//
// The direction passed specifies whether data is to be read from or written
// to the passed buffer, or whether the data element size is simply to be
// returned
//
// The return value is the size (in bytes) of the transfer data.  this method
// recursively calls transfer on all its children that have wfTransfer set.
//
#include <pshpack1.h>

struct TTransferIterInfo {
  void*               Data;
  TTransferDirection  Direction;
};

#include <poppack.h>

static void transferDatchild(TWindow* child, TTransferIterInfo* info) {
  if (child->IsFlagSet(wfTransfer))
    info->Data = (char*)info->Data + child->Transfer(info->Data, info->Direction);
}

//
//
//
uint
TWindow::Transfer(void* buffer, TTransferDirection direction)
{
  if (buffer) {
    TTransferIterInfo info = { buffer, direction };
    ForEach((TActionFunc)transferDatchild, &info);
    return (char*)info.Data - (char*)buffer;
  }
  return 0;
}

//
// Transfers data between the TWindow's data buffer and the child
// windows in its ChildList (data is not transfered between any child
// windows whose wfTransfer flag is not set)
//
void
TWindow::TransferData(TTransferDirection direction)
{
  if (TransferBuffer)
    Transfer(TransferBuffer, direction);
}

//
// Checks whether the given HWND belongs to this process.
// Internal function.
//
inline static BOOL BelongsToCurrentProcess (HWND h)
{
  DWORD processId = 0;
  ::GetWindowThreadProcessId(h, &processId);
  return processId == ::GetCurrentProcessId();
}

//
// Installs the instance window proc as the window proc and 
// saves the old window proc in the DefaultProc member.
//
void
TWindow::SubclassWindowFunction()
{
  PRECONDITION(GetHandle());
  PRECONDITION(GetInstanceProc());
  
  if (!BelongsToCurrentProcess(GetHandle())) return;

  // If the window already has the window proc we want,
  // then just ensure that it has a default proc and return.
  //
  if (GetInstanceProc() == GetWindowProc())
  {
    if (!DefaultProc) DefaultProc = ::DefWindowProc;
    return;
  }

  // Initialize the instance proc and install it.
  //
  InitInstanceProc();
  DefaultProc = SetWindowProc(GetInstanceProc());
}

//
// Registers the TWindow's MS-Windows, if not already registered
//
bool
TWindow::Register()
{
  // Only check for globally registered classes if not under NT or WoW box,
  // since NT treats a 0 hInstance completely differently than Windows.
  //
  static bool checkGlobal = !TSystem::IsNT();

  WNDCLASS  windowClass;
  bool gc;
  if (checkGlobal)
    gc = TUser::GetClassInfo(0, GetClassName(), &windowClass) != 0;
  else
    gc = false;

  // If the wndclass is not yet registered, call GetWindowClass() to let our
  // derived class fill in the appropriate info. If the class is still then not
  // registered, then make it so.
  //
  if (!gc && !GetModule()->GetClassInfo(GetClassName(), &windowClass)) {
    GetWindowClass(windowClass);
    WNDCLASS dummy;
    if (!GetModule()->GetClassInfo(GetClassName(), &dummy))
      return ::RegisterClass(&windowClass);
  }

  return true;
}

//
// Returns a bool indicating whether or not it is Ok to close the TWindow
//
// Iterates through the TWindow's ChildList, calling the CanClose()
// method of each
//
// Returns false if any of the child windows returns false
//

static bool cannotClose(TWindow* win, void*) {
  return win->GetHandle() && !win->CanClose();
}

//
//
//
bool
TWindow::CanClose()
{
  return !FirstThat(cannotClose);
}

//
// Destroys the associated MS-Windows interface element after determining that
// it is Ok to do so
//
// If the TWindow is the main window of the application, calls the CanClose()
// method of the application, else calls this->CanClose(), before calling
// Destroy()
//
void
TWindow::CloseWindow(int retVal)
{
  bool  willClose;

  if (IsFlagSet(wfMainWindow))
    willClose = GetApplication()->CanClose();

  else
    willClose = CanClose();

  if (willClose)
    Destroy(retVal);
}

//
// The default response to a WM_CLOSE message is to call CloseWindow()
// and then have the window deleted if the Handle was really destroyed.
//
void
TWindow::EvClose()
{
  if (IsFlagSet(wfAlias))
    DefaultProcessing();

  else {
    CloseWindow();
    if (!GetHandle() && IsFlagSet(wfDeleteOnClose))
      GetApplication()->Condemn(this);  // Assumes delete
  }
}

//
// Responds to an incoming WM_DESTROY message
//
// Calls CleanupWindow() to let derived classes cleanup
// Clears the wfFullyCreated flag since this window is no longer fully created
//
// If the TWindow is the application's main window posts a 'quit' message to
// end the application, unless already in ~TApplication() (MainWindow == 0)
//
void
TWindow::EvDestroy()
{
  ClearFlag(wfFullyCreated);
  CleanupWindow();

  if (!IsFlagSet(wfAlias)) {
    if (IsFlagSet(wfMainWindow) && GetApplication()->IsRunning())
      ::PostQuitMessage(0);
  }

  DefaultProcessing();
}

//
// Responds to an incoming WM_NCDESTROY message, the last message
// sent to an MS-Windows interface element
//
// Sets the Handle data member of the TWindow to zero to indicate that an
// interface element is no longer associated with the object
//
void
TWindow::EvNCDestroy()
{
  DefaultProcessing();
  SetHandle(0);
}

//
// Respond to Windows attempt to close down. Determines if this app or window
// is ready to close.
//
bool
TWindow::EvQueryEndSession()
{
  if (IsFlagSet(wfAlias))
    return (bool)DefaultProcessing();

  else if (IsFlagSet(wfMainWindow))
    return GetApplication()->CanClose();

  else
    return CanClose();
}

//
// Windows is shutting down.
//!TH Throw an exception that is to be caught only by Owl's WinMain,
//!TH which shuts down the entire application.
//
void
TWindow::EvEndSession(bool endSession, bool)
{
  if (endSession)
    throw TXEndSession();
  else
    DefaultProcessing();
}


//
// Handle message posted to us by a control needing assistance in dealing with
// invalid inputs
//
void
TWindow::EvChildInvalid(HWND handle)
{
  PRECONDITION(handle);

  ::SendMessage(handle, WM_CHILDINVALID, 0, 0);
}

//----------------------------------------------------------------------------
// Non-virtuals
//

void 
TWindow::AttachHandle(HWND handle)
{
  ClearFlag(wfDetached);
  FreeInstanceProc();
  InstanceProc = 0;
  Init(handle, GetModule());
}


void 
TWindow::DetachHandle()
{
  // NOTE: This is by no means a complete way of detaching the window...
  //       The following is logic allows Delphi/OWL interaction..
  //
  ClearFlag(wfFullyCreated);
  SetHandle(0);
  SetFlag(wfDetached);
}


//
//
//
unsigned
TWindow::NumChildren() const
{
  return IndexOf(ChildList) + 1;
}

//
// Walks over children and assigns a z-order index to the ZOrder member
//
void
TWindow::AssignZOrder()
{
  TWindow*  wnd;
  HWND   curWindow = GetHandle();

  if (curWindow) {
    curWindow = ::GetWindow(curWindow, GW_CHILD);

    if (curWindow) {
      int  i = 1;

      for (curWindow = ::GetWindow(curWindow, GW_HWNDLAST);
           curWindow;
           curWindow = ::GetWindow(curWindow, GW_HWNDPREV))
      {
        wnd = GetWindowPtr(curWindow);

        if (wnd)
          wnd->ZOrder = (uint16)i++;
      }
    }
  }
}

//
// The private field ZOrder is used to ensure the Z-order is
// consistent through read and write of the object.
//
// When the object is written, parent->AssignZOrder will fill in this value
//
// ZOrder ranges from 1 to N where N is the number of objects and the top one.
// A ZOrder value of 0 means that the Z-ordering has not be recoreded.
//
bool
TWindow::OrderIsI(TWindow* win, void* position)
{
  return win->ZOrder == *(int*)position;
}

//
// Returns true if the child was supposed to be created but couldn't be
//
static bool
cantCreate(TWindow* win, void*)
{
  // If child is already created, then no need to re-create it.
  //
  if (win->GetHandle()) {
//!CQ make sure that child is parented to us? Pass this hwnd in arg
//!CQ if (win->GetParentH() != *(HWND*)arg)
//!CQ   ::SetParent(win->GetHandle(), *(HWND*)arg);
    return false;
  }

  bool autoCreate = win->IsFlagSet(wfAutoCreate);

  WARNX(OwlWin, !autoCreate, 0,
        _T("Child window(Id=") << win->GetId() << _T(") not autocreated"));
  if (!autoCreate)
    return false;

  // This call will only fail if a user-defined Create() returns false. Owl's
  // Creates always throw exceptions.
  //
  if (!win->Create())
    return true;

  // Get & set the window text for the child if it is iconic.
  //!CQ why?
  //
  if (win->IsIconic()) {
    int    textLen = win->GetWindowTextLength();
    _TCHAR*  text = new _TCHAR[textLen + 1];

    win->GetWindowText(text, textLen + 1);
    win->SetWindowText(text);
    delete[] text;
  }
  return false;
}

//
// Create the children of the object.  Returns true if all the windows
// were successfully created. 
// NOTE: Throws an exception (TXWindow) if a child object could not be
//       created.
//
bool
TWindow::CreateChildren()
{
  // Create children first to restore creation order
  //
  TWindow* childFailed = FirstThat(cantCreate);
  if (childFailed) {
    TXWindow::Raise(childFailed, IDS_CHILDCREATEFAIL); 
  }

  // Restore Z-ordering for children that have Z-ordering recorded
  //
  HWND above = HWND_TOP;
  for (int top = NumChildren(); top; top--) {
    TWindow* wnd = FirstThat(&TWindow::OrderIsI, &top);
    if (wnd) {
      wnd->SetWindowPos(above, 0,0,0,0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
      above = wnd->GetHandle();
    }
  }
  return true;
}

//
// adds the passed pointer to a child window to the linked list
// of sibling windows which ChildList points to
//
void
TWindow::AddChild(TWindow* child)
{
  if (child)
    if (ChildList) {
      child->SiblingList = ChildList->SiblingList;
      ChildList->SiblingList = child;
      ChildList = child;
    }
    else {
      ChildList = child;
      child->SiblingList = child;
    }
}

//
// Returns a pointer to the TWindow's previous sibling (the window previous to
// the TWindow in its parent's child window list)
//
// Returns the sibling which points to the TWindow
//
// If the TWindow was the first child added to the list, returns a pointer to
// the last child added
//
TWindow*
TWindow::Previous()
{
  if (!SiblingList) {
    return 0;
  }
  else {
    TWindow*  CurrentIndex = this;

    while (CurrentIndex->Next() != this)
      CurrentIndex = CurrentIndex->Next();

    return CurrentIndex;
  }
}

//
// Returns a pointer to the first TWindow in the ChildList that meets some
// specified criteria
//
// If no child in the list meets the criteria, 0 is returned
//
// The Test parameter which defines the criteria, is a pointer to a
// function that takes a TWindow pointer & a pointer to void
//
// The TWindow* will be used as the pointer to the child window and
// the void* as a pointer to the Test function's additional parameters
//
// The Test function must return a Boolean value indicating whether the
// child passed meets the criteria
//
TWindow*
TWindow::FirstThat(TCondFunc test, void* paramList) const
{
  if (ChildList) {
    TWindow*  nextChild = ChildList->Next();
    TWindow*  curChild;

    do {
      curChild = nextChild;
      nextChild = nextChild->Next();

      //
      // Test curChild for okay
      //
      if (test(curChild, paramList))
        return curChild;
    } while (curChild != ChildList && ChildList);
  }
  return 0;
}

//
// Iterates over each child window in the TWindow's ChildList,
// calling the procedure whose pointer is passed as the Action to be
// performed for each child
//
// A pointer to a child is passed as the first parameter to the iteration
// procedure
//
void
TWindow::ForEach(TActionFunc action, void* paramList)
{
  if (ChildList) {
    TWindow*  curChild;
    TWindow*  nextChild = ChildList->Next();  // Allows ForEach to delete children

    do {
      curChild = nextChild;
      nextChild = nextChild->Next();
      action(curChild, paramList);
    } while (curChild != ChildList && ChildList);
  }
}

//
// Returns a pointer to the first TWindow in the ChildList that
// meets some specified criteria
//
// If no child in the list meets the criteria, 0 is returned
//
// The Test parameter which defines the criteria, is a pointer to a member
// function (this is how it's different from FirstThat above) that takes a
// pointer to a TWindow & a pointer to void
//
// The TWindow pointer will be used as the pointer to the child window and the
// void pointer as a pointer to the Test function's additional parameters
//
// The Test function must return a Boolean value indicating whether the child
// passed meets the criteria
//
TWindow*
TWindow::FirstThat(TCondMemFunc test, void* paramList)
{
  if (ChildList) {
    TWindow*  nextChild = ChildList->Next();
    TWindow*  curChild;

    do {
      curChild = nextChild;
      nextChild = nextChild->Next();

      if ((this->*test)(curChild, paramList))
        return curChild;
    } while (curChild != ChildList && ChildList);
  }
  return 0;
}

//
// Iterates over each child window in the TWindow's ChildList,
// calling the member function (unlike ForEach above which takes pointer
// to non-member function) whose pointer is passed as the Action to
// be performed for each child
//
// A pointer to a child is passed as the first parameter to the iteration
// procedure
//
void
TWindow::ForEach(TActionMemFunc action, void* paramList)
{
  if (ChildList) {
    TWindow*  nextChild = ChildList->Next();
    TWindow*  curChild;

    do {
      curChild = nextChild;
      nextChild = nextChild->Next();
      (this->*action)(curChild, paramList);
    } while (curChild != ChildList && ChildList);
  }
}

//
// Returns the position at which the passed child window appears
// in the TWindow's ChildList
//
// If the child does not appear in the list, -1 is returned
//
// Zero'th position is ChildList->Next
//
static int position;
static bool isItThisChild1(TWindow* win, void* child)
{
  ++position;
  return win == (TWindow*)child;
}

//
//
//
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
#pragma warn -ncf
#endif
int
TWindow::IndexOf(TWindow* child) const
{
  position = -1;
  return FirstThat(isItThisChild1, child) ? position : -1;
}
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
#pragma warn .ncf
#endif

//
// Returns the child at the passed position in the TWindow's
// ChildList
//
// The ChildList is circularly-referent so that passing a position
// larger than the number of children will cause the traversal of the
// list to wrap
//
// Zero'th position is ChildList->Next
//
TWindow*
TWindow::At(int position)
{
  if (position == -1)
    return 0;

  else {
    TWindow*  currentChild = ChildList;

    if (currentChild) {
      currentChild = ChildList->Next();

      while (position > 0) {
        currentChild = currentChild->Next();
        position--;
      }
    }
    return currentChild;
  }
}

//
// Returns a pointer to the window in the ChildList with the passed Id
//
// If no child in the list has the passed Id, 0 is returned
//
static bool isItThisChild2(TWindow* win, void* id) {
  return win->GetId() == *(int*)id;
}

//
//
//
TWindow*
TWindow::ChildWithId(int id) const
{
  return ((TWindow*)this)->FirstThat(isItThisChild2, &id);
}

//
// Send a message directly to this window using system API. Handles exception
// resumption for systems that need it.
//
TResult
TWindow::SendMessage(uint msg, TParam1 param1, TParam2 param2) const
{
  PRECONDITION(GetHandle());

  TResult result = ::SendMessage(GetHandle(), msg, param1, param2);
  return result;
}

//
// Forward the current event to a given window. Can either send or post, based
// on the send argument.
//
TResult
TWindow::ForwardMessage(HWND handle, bool send)
{
  if (!handle)
    return 0;

  TCurrentEvent& currentEvent = GetCurrentEvent();
  if (send) {
    TResult result = ::SendMessage(handle, currentEvent.Message,
                                   currentEvent.Param1,
                                   currentEvent.Param2);
    return result;
  }
  else
    return ::PostMessage(handle, currentEvent.Message,
                         currentEvent.Param1,
                         currentEvent.Param2);
}

//
// Forward a message to an Owl window. If send, then bypass windows directly
// and call the owl window's window proc.
//
TResult
TWindow::ForwardMessage(bool send)
{
  TCurrentEvent& currentEvent = GetCurrentEvent();
  if (send)
    return HandleMessage(currentEvent.Message, currentEvent.Param1,
                         currentEvent.Param2);
  return ForwardMessage(GetHandle(), send);
}

//
// Broadcast a given message to all the children of this window, including
// non-object windows
//
void
TWindow::ChildBroadcastMessage(uint msg, TParam1 param1, TParam2 param2)
{
  for (HWND hWndChild = GetWindow(GW_CHILD); hWndChild; ) {
    HWND hWndNext = ::GetWindow(hWndChild, GW_HWNDNEXT);
    ::SendMessage(hWndChild, msg, param1, param2);
    hWndChild = hWndNext;
  }
}

//
// Destroys the associated MS-Windows interface element and deletes the C++
// object unconditionally (without calling CanClose())
//
// This function is static to avoid side effects of deleting 'this'.
//
void
TWindow::ShutDownWindow(TWindow* win, int retVal)
{
  win->Destroy(retVal);
  delete win;
}

//
// Sets the Title and caption of this TWindow
//
void
TWindow::SetCaption(LPCTSTR title)
{
  if (Title != title) {
    if (HiUint16(uint32(Title)))
      delete[] Title;

    Title = strnewdup(title);
  }

  if (GetHandle())
    ::SetWindowText(GetHandle(), Title);
}

//
// Gets the Title member var from the current window caption or text
//
void
TWindow::GetWindowTextTitle()
{
  if (LoUint16(uint32(Title)) == 0xFFFF)  // ignore "don't-change" titles
    return;

  if (HiUint16(uint32(Title)))
    delete[] Title;

  int titleLength = GetWindowTextLength();
  if (titleLength < 0) {
    Title = strnewdup(_T(""));
  }
  else {
    Title = new  _TCHAR[titleLength + 1];
    Title[0] = 0;
    Title[titleLength] = 0;
    GetWindowText(Title, titleLength + 1);
  }
}

//
// Copy over the styles, the coordinates & the id from the existing HWnd into
// the Owl TWindow members.
//   NOTE: the title is not copied here, but in GetWindowTextTitle()
//
void
TWindow::GetHWndState(bool forceStyleSync)
{
  // Retrieve Attr.Style and Attr.ExStyle
  //
  // NOTE: some windows controls (e.g. EDIT) change the style bits
  // (e.g. WS_BORDER) from their original values.  if we always reset
  // Attr.Style and Attr.ExStyle by extracting their values from
  // Windows, we will lose some of the style bits we supplied
  // in the CreateWindowEx call.  in the case of the ResourceId
  // constructors, of course, we must retrieve these values.
  //
  if (IsFlagSet(wfFromResource) || forceStyleSync) {
    Attr.Style = GetWindowLong(GWL_STYLE);
    Attr.ExStyle = GetWindowLong(GWL_EXSTYLE);
  }

  // Retrieve Attr.X, Attr.Y, Attr.W and Attr.H
  //
  TRect  wndRect;

  GetWindowRect(wndRect);
  Attr.H = wndRect.Height();
  Attr.W = wndRect.Width();

  HWND  hParent = GetParentH();
  if ((Attr.Style & WS_CHILD) && hParent)
    ::ScreenToClient(hParent, &wndRect.TopLeft());

  Attr.X = wndRect.left;
  Attr.Y = wndRect.top;

  Attr.Id = GetWindowLong(GWL_ID);
}

//
//
//
uint
TWindow::GetDlgItemInt(int childId, bool* translated, bool isSigned) const
{
  PRECONDITION(GetHandle());

#if defined(BI_COMP_BORLANDC) 
#  if (sizeof(bool) == sizeof(int))
    uint retVal = ::GetDlgItemInt(GetHandle(), childId, (int*)translated, isSigned);
#  else
    int tempTranslated;
    uint retVal = ::GetDlgItemInt(GetHandle(), childId, &tempTranslated, isSigned);
    if (translated)
      *translated = tempTranslated;
#  endif
#else
  int tempTranslated;
  uint retVal = ::GetDlgItemInt(GetHandle(), childId, &tempTranslated, isSigned);
  if (translated)
    *translated = tempTranslated;
#endif
  return retVal;
}

//
// Gets/Sets the style bits of the underlying window or the 'Style'
// member of the attribute structure associated with this TWindow object.
//
uint32
TWindow::GetStyle() const
{
  return GetHandle() ? GetWindowLong(GWL_STYLE) : Attr.Style;
}

//
// Gets/Sets the style bits of the underlying window or the 'Style'
// member of the attribute structure associated with this TWindow object.
//
uint32
TWindow::SetStyle(uint32 style)
{
  if (!GetHandle()) {
    uint32 oldstyle = Attr.Style;
    Attr.Style = style;
    return oldstyle;
  }
  return SetWindowLong(GWL_STYLE, style);
}

//
// Gets the extra style bits of the window.
//
uint32
TWindow::GetExStyle() const
{
  return GetHandle() ? GetWindowLong(GWL_EXSTYLE) : Attr.ExStyle;
}

//
// Sets the extra style bits of the window.
//
uint32
TWindow::SetExStyle(uint32 style)
{
  if (!GetHandle()) {
    uint32 oldstyle = Attr.ExStyle;
    Attr.ExStyle = style;
    return oldstyle;
  }
  return SetWindowLong(GWL_EXSTYLE, style);
}

//
// Modify the style bits of the window.
//
bool
TWindow::ModifyStyle(uint32 offBits, uint32 onBits, uint swpFlags)
{
  uint32 style = GetStyle();
  uint32 newStyle = (style & ~offBits) | onBits;
  if (style == newStyle)
    return false;

  SetStyle(newStyle);

  if (swpFlags && GetHandle())
    SetWindowPos(0, 0, 0, 0, 0,
                 SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|swpFlags);
  return true;
}

//
// Modify the style bits of the window.
//
bool
TWindow::ModifyExStyle(uint32 offBits, uint32 onBits, uint swpFlags)
{
  uint32 style = GetExStyle();
  uint32 newStyle = (style & ~offBits) | onBits;
  if (style == newStyle)
    return false;

  SetExStyle(newStyle);
  if (swpFlags)
    SetWindowPos(0, 0, 0, 0, 0,
                 SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|swpFlags);
  return true;
}

//
// Get the window rectangle whether it has been created or not
//
void
TWindow::GetWindowRect(TRect& rect) const {
  if (GetHandle()) {
    ::GetWindowRect(GetHandle(), &rect);
  }
  else {
    rect.SetWH(Attr.X, Attr.Y, Attr.W, Attr.H);
  }
}

//
// Get the window's client rectangle whether it has been created or not
//
void
TWindow::GetClientRect(TRect& rect) const {
  if (GetHandle()) {
    ::GetClientRect(GetHandle(), &rect);
  }
  else {
    rect.Set(0, 0, Attr.W, Attr.H);
  }
}

//
// Set the new window position.
//
bool
TWindow::SetWindowPos(HWND hWndInsertAfter,
                      int x, int y, int w, int h,
                      uint flags)
{
  if (GetHandle())
    return ::SetWindowPos(GetHandle(), hWndInsertAfter, x, y, w, h, flags);

  if (!(flags & SWP_NOMOVE)) {
    Attr.X = x;
    Attr.Y = y;
  }
  if (!(flags & SWP_NOSIZE)) {
    Attr.W = w;
    Attr.H = h;
  }

  //!CQ Can't do much with Z-Order or owner Z-Order

  if (flags & SWP_SHOWWINDOW)
    Attr.Style |= WS_VISIBLE;
  else if (flags & SWP_HIDEWINDOW)
    Attr.Style &= ~WS_VISIBLE;

  return true;
}

//
// Displays this TWindow in a given state. Can be called either before or after
// the Window is created. If before, the show state is placed into Attr for use
// at creation
//
bool
TWindow::ShowWindow(int cmdShow)
{
  // If the window is being minimzed send a WM_SYSCOMMAND; this way the
  // frame window focus saving works properly
  //!CQ do we still need this with final owl2 focus saving?
  //
  if (gBatchMode)
    return true;
  if (GetHandle()) {
    if (cmdShow == SW_MINIMIZE)
      return HandleMessage(WM_SYSCOMMAND, SC_MINIMIZE);

    else
      return ::ShowWindow(GetHandle(), cmdShow);
  }

  switch (cmdShow) {
    case SW_HIDE:
      Attr.Style &= ~WS_VISIBLE;
      break;
    case SW_SHOWNORMAL:
    case SW_RESTORE:
      Attr.Style |= WS_VISIBLE;
      Attr.Style &= ~(WS_MINIMIZE | WS_MAXIMIZE);
      break;
    case SW_SHOWMINIMIZED:
    case SW_MINIMIZE:
    case SW_SHOWMINNOACTIVE:
      Attr.Style |= WS_VISIBLE;
      Attr.Style |= WS_MINIMIZE;
      break;
    case SW_SHOWMAXIMIZED:
      Attr.Style |= WS_VISIBLE;
      Attr.Style |= WS_MAXIMIZE;
      break;
    case SW_SHOWNOACTIVATE:
    case SW_SHOW:
    case SW_SHOWNA:
      Attr.Style |= WS_VISIBLE;
      break;
  }
  return true;
}

//
// Set the cursor for this window given a TModule and a resId
// Updates the current cursor if it is over this window.
//
bool
TWindow::SetCursor(TModule* module, TResId resId)
{
  if (module == CursorModule && resId == CursorResId)
    return false;

  HCURSOR hOldCursor = (HCursor && CursorModule) ? HCursor : 0;

  CursorModule = module;
  CursorResId = resId;
  if (CursorResId)
    if (CursorModule)
      HCursor = CursorModule->LoadCursor(CursorResId);
    else
      HCursor = ::LoadCursor(0, CursorResId);
  else
    HCursor = 0;

  // If the cursor is in our client window then set it now
  //
  if (GetHandle()) {
    TPoint p;
    GetCursorPos(p);
    ScreenToClient(p);
    if (GetClientRect().Contains(p))
      ::SetCursor(HCursor);
  }

  // Destroy old cursor if there was one & it was not loaded from USER
  //
  if (hOldCursor)
    ::DestroyCursor(hOldCursor);
  return true;
}

//
// Handle WM_INITMENUPOPUP while embeded to generate command enable messages
// for our server menu items. Very similar to TFrameWindow::EvInitMenuPopup;
// could rearrange code to share better.
//
void
TWindow::EvInitMenuPopup(HMENU hPopupMenu, uint /*index*/, bool sysMenu)
{
  if (IsFlagSet(wfAlias))
    DefaultProcessing();

  else if (!sysMenu && hPopupMenu) {
    const int count = ::GetMenuItemCount(hPopupMenu);

    for (int pos = 0; pos < count; pos++) {
      uint  id;

      if (hPopupMenu == GetMenu()) // top level menu
        id = ::GetMenuItemID(hPopupMenu, pos);

      else {
        // For second level and below menus, return the implied id for popup
        // sub-menus. The implied id for a sub-menu is the id of the first item
        // in the popup sub-menu less 1. If there are more than one level of
        // popup menus, it will recursively look into those sub-menus as well.
        //
        TMenu popupMenu(hPopupMenu);
        id = popupMenu.GetMenuItemID(pos);
      }

      // Ignore separators
      //
      if (id == 0 || uint16(id) == uint16(-1))
        continue;

      // Skip the rest if it is the mdi child list, or system commands
      //
      if (id == (uint)IDW_FIRSTMDICHILD || id > 0xF000)
        break;

      TMenuItemEnabler mie(hPopupMenu, id, GetHandle(), pos);
      EvCommandEnable(mie);
    }
  }
}

//
//!CQ Is this unusual/confusing to take the pointer & own it, & not a ref
//!CQ & copy it???
// Associate a popup menu with the window so that it can automatically
// handle a WM_CONTEXTMENU message.
//
void
TWindow::AssignContextMenu(TPopupMenu* popupMenu)
{
  delete ContextPopupMenu;
  ContextPopupMenu = popupMenu;
}

//
/// The default message handler for WM_CONTEXTMENU.
/// Respond to a right button click or VK_APPS key press in the window.
/// If a context menu is set, display it.
//
void
TWindow::EvContextMenu(HWND child, int x, int y)
{
  TPopupMenu* m = GetContextMenu();
  if (!m) 
  {
    DefaultProcessing();
    return;
  }

  TPoint p(x, y);
  bool invalidPos = (x < 0 || y < 0);
  if (invalidPos)
  {
    // The message was probably generated by the VK_APPS key (sets x = y = -1).
    // See the Windows API documentation for WM_CONTEXTMENU.
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms647592.aspx
    // In this case, we open the menu in the upper left corner of the given window.
    //
    p = TPoint(0, 0);
    ::ClientToScreen(child, &p);
  }
  else
  {
    // Provide additional help support by reporting the click position to the main window.
    //
    THelpHitInfo hit(p, this);
    GetApplication()->GetMainWindow()->HandleMessage(WM_OWLHELPHIT, 0, reinterpret_cast<TParam2>(&hit));
  }
  m->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p, 0, GetHandle());
}

//
//
//
void            
TWindow::EvEnterIdle(uint source, HWND hWndDlg)
{
  // If we're the main window, let it rip from the receiver
  //
  if (source == MSGF_DIALOGBOX) {
    if (IsFlagSet(wfMainWindow))
      IdleAction(0);  
    else {
      TWindow* win = GetWindowPtr(hWndDlg);
      if (win)
        win->IdleAction(0);
    }
  } 

  // Let original proc. have a crack at msg.
  //
  DefaultProcessing();
}

//
// Wrapper for Windows API.
//
int
TWindow::GetScrollPos(int bar) const
{
  PRECONDITION(GetHandle());
  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
//  info.fMask = SIF_DISABLENOSCROLL | SIF_POS;
  info.fMask = SIF_POS;
  GetScrollInfo(bar, &info);
  return info.nPos;
}

//
// Wrapper for Windows API.
//
int
TWindow::SetScrollPos(int bar, int pos, bool redraw)
{
  PRECONDITION(GetHandle());

  int oldPos = GetScrollPos(bar);

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_POS;
  info.nPos = pos;
  SetScrollInfo(bar, &info, redraw);
  return oldPos;
}

//
// Wrapper for Windows API.
//
void
TWindow::GetScrollRange(int bar, int& minPos, int& maxPos) const
{
  PRECONDITION(GetHandle());

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_RANGE;
  GetScrollInfo(bar, &info);
  minPos = info.nMin;
  maxPos = info.nMax;
}

//
// Wrapper for Windows API.
//
void
TWindow::SetScrollRange(int bar, int minPos, int maxPos, bool redraw)
{
  PRECONDITION(GetHandle());

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_RANGE;
  info.nMin = minPos;
  info.nMax = maxPos;
  SetScrollInfo(bar, &info, redraw);
}

#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
#pragma warn -par
#endif

//
//
//
void
TWindow::SetScrollPage(int bar, int page, bool redraw)
{
  PRECONDITION(GetHandle());

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_PAGE;
  info.nPage = page;
  SetScrollInfo(bar, &info, redraw);
}
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
#pragma warn .par

#pragma warn -par
#endif
//
//
//
int
TWindow::GetScrollPage(int bar) const
{
  PRECONDITION(GetHandle());

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_PAGE;
  GetScrollInfo(bar, &info);
  return info.nPage;
}
#if !defined(__GNUC__) //JJH removal of pragma warn for gcc
#pragma warn .par
#endif

//
//
//
void
TWindow::EnableTooltip(bool enable)
{
  if (!Tooltip) {

    // To circumvent this scenario, we'll look for a window which is fairly
    // stable/rooted as owner of the tooltip. Ideally, we'll get the
    // application's main window.
    //
    TWindow* tipParent = this;

/*  // check it what if window -> is closed but tooltip live?????????
    // it is for gastget only ????????????????????
    while (tipParent->GetParentO()){
      tipParent = tipParent->GetParentO();
      if (tipParent->IsFlagSet(wfMainWindow))
        break;
    }
*/
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
// Set a specific tooltip for this window. Assume we now own the ToolTip
//
void
TWindow::SetTooltip(TTooltip* tooltip)
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
  //
  Tooltip = tooltip;
  if (Tooltip) {
    if (!Tooltip->GetHandle()) {

      // Make sure tooltip is disabled so it does not take input focus
      Tooltip->Attr.Style |= WS_DISABLED;
      Tooltip->Create();
    }
  }
}

//
//
//
void
TWindow::SetScrollInfo(int bar, SCROLLINFO* scrollInfo, bool redraw)
{
  PRECONDITION(GetHandle());
  ::SetScrollInfo(GetHandle(), bar, scrollInfo, redraw);
}

//
//
//
void
TWindow::GetScrollInfo(int bar, SCROLLINFO* scrollInfo) const
{
  PRECONDITION(GetHandle());
  ::GetScrollInfo(GetHandle(), bar, scrollInfo);
}


//
// Not inline to avoid requiring gdiobjec.h by window.h just to get TRegion's
// conversion operator
//
bool
TWindow::GetUpdateRgn(TRegion& region, bool erase) const
{
  PRECONDITION(GetHandle());

  return ::GetUpdateRgn(GetHandle(), region, erase);
}


//
// Wrapper for Windows API.
//
void
TWindow::DragAcceptFiles(bool accept)
{
  PRECONDITION(GetHandle());
  TShell::DragAcceptFiles(GetHandle(), accept);
}

//
//
//
int 
TWindow::MessageBox(uint resId, LPCTSTR caption, uint type, ...)
{
  PRECONDITION(GetApplication());
  PRECONDITION(GetHandle());
  PRECONDITION(resId);

  TTmpBuffer<_TCHAR> buffer(MAX_PATH);
  owl_string text = GetModule()->LoadString(resId);
  
//FMM  
//  wvsprintf(buffer, text.c_str(), (char *)(&type+1));
  wvsprintf(buffer, text.c_str(), (va_list)(&type+1));

  return MessageBox(buffer, caption, type);
}

//
//
//
void TWindow::SethAccel(HACCEL _hAccel)
{
  HAccel = _hAccel;
}

//
//
//
#if defined(__TRACE) || defined(__WARN)
//__OWL_BEGIN_NAMESPACE
  ostream& operator <<(ostream& os, const TWindow& w)
  {
    os << '(';
#if !defined(BI_NO_RTTI)
      os << typeid(w).name() << ',';
#else
      os << _OBJ_FULLTYPENAME(&w) << ',';
#endif
    os << "0x" << hex << uint(w.GetHandle()) << ',';
    if (w.Title)
      os << '\'' << TResId(w.Title) << '\'' << ',';
    if (w.Parent)
      os << "id=" << w.GetId();
    os << ')';
    return os;
  }
//__OWL_END_NAMESPACE
#endif


//
// Returns a string which kind-of identifies the window (used during autopsy
// and vivisection of dead/dying window)
//
static const _TCHAR*
winInfo(TWindow* win)
{
  const _TCHAR noTitle[] = _T("Untitled");
  static _TCHAR winStr[MAX_PATH];
  if (win) {

    _stprintf(winStr, _T("%s, %u, %s.\nError: %s"),
                    HiUint16(uint32(win->Title)) ? win->Title : noTitle,
                    win->Attr.Id, _OBJ_FULLTYPENAME(win),
                    TSystemMessage().SysMessage().c_str());
    return winStr;
  }
  return _T("");
}
///////////////////////////////////////////////////////////////////////////////////////
#if 0
// this code cope with BC 5.02 assert in __DynamicCast(...)
#if defined(BI_COMP_BORLANDC) && (__BORLANDC__ == 0x520)
//
// Temporary defines for using type_info with dynamic cast
//
void* __cdecl __DynamicCast(void* object, void* vtable,
                            void* srctyp, void* dsttyp,
                            int reference = 0);
struct tpid {int s; short m; short n; int VptrOffs; int Flags;};  // partial
#define CF_HAS_FARVPTR  0x1000
//
//
static const void*
DynamicCast(const void* obj, const type_info& src, const type_info& dst)
{
  int vtblOff;
  if (!obj)
    return obj;
  else if ((vtblOff = src.tpp->VptrOffs) == -1)
    return src==dst ? obj : 0;
  else if (src.tpp->Flags & CF_HAS_FARVPTR)
    return __DynamicCast(const_cast<void *>(obj),
                         *(void **)((char*)obj+vtblOff), src.tpp, dst.tpp);
  else{
    char* _vptr = *(char**)obj;
    if(IsBadReadPtr(*(void**)_vptr, sizeof(void*)))
      return 0;     // quick escape
    if(IsBadReadPtr(((char**)_vptr)[-1], sizeof(void*)))
      return 0;     // quick escape
    return __DynamicCast(const_cast<void *>(obj),
                         *(void**)((char*)obj+vtblOff), src.tpp,dst.tpp);
  }
}
static TDrawItem* localTryCatchBlock(uint32 data)
{
  return (TDrawItem*)DynamicCast((void*)data, typeid(TDrawItemBase), typeid(TDrawItem));
}
#elif defined(BI_COMP_GNUC) && !defined(WINELIB)
TDrawItem* lTCB(uint32);
static TDrawItem* localTryCatchBlock(uint32 data)
{
  return lTCB(data);
}
#endif // #if defined(BI_COMP_BORLANDC) && (__BORLANDC__ == 0x520)

#endif // #if 0
//
TDrawItem* ItemData2DrawItem(uint32 data)
{
  // 0. If 0
  if(data==0)
    return 0;
  // 1. check to make sure the pointer is valid
  if(IsBadReadPtr((void*)data, sizeof(TDrawItem)))  
    return 0;     // quick escape

  // 2. check signature -> starting from 5's byte
  uint32* itemId = (uint32*)((uint8*)data+4); 
  if(*itemId != TDrawItemBase::drawId)
    return 0;

  // 3. check to make sure the VTable pointer is valid
  if(IsBadReadPtr(*(void**)data, sizeof(void*)))  
    return 0;     // quick escape

#if 0 //?? maby simple casting enauph after checking itemId??
  TDrawItem* item;
  TRY{
    item = localTryCatchBlock(data);
  }
  CATCH((...){
    return 0;
  })
  return item;
#else
  return (TDrawItem*)(void*)data;
#endif
}
//

#if defined(OWL2_COMPAT)
//
// For backward compatibility
//
owl_string
TXWindow::Msg(TWindow* win, uint resId)
{
  return MakeMessage(resId, winInfo(win));
}
#endif

//
// InvalidWindow exception constructor
//
TXWindow::TXWindow(TWindow* win, uint resId)
:
  TXOwl(MakeMessage(resId, winInfo(win)), resId),
  Window(win)
{
}

//
// Copy the exception object.
//
TXWindow::TXWindow(const TXWindow& src)
:
  TXOwl(src),
  Window(src.Window)
{
}

//
// Unhandled exception.
//
int
TXWindow::Unhandled(TModule* app, uint promptResId)
{
  Window = 0;
  return TXOwl::Unhandled(app, promptResId);
}

//
// Clone the exception object for safe-throwing in Win16.
//
#if defined(BI_NO_COVAR_RET)
TXBase*
#else
TXWindow*
#endif
TXWindow::Clone()
{
  return new TXWindow(*this);
}

//
// Throw the exception.
//
void
TXWindow::Throw()
{
  THROW( *this );
}

//
// Creates the TXWindow exception and throws it.
//
void
TXWindow::Raise(TWindow* win, uint resourceId)
{
  TXWindow(win, resourceId).Throw();
}

//
// Returns the window causing the exception.
//
TWindow*
TXWindow::GetWindow() const
{
  return Window;
}
__OWL_END_NAMESPACE

/* ========================================================================== */
