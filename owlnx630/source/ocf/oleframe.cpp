//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.15.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:38 $
//
// Implementation of class TOleFrame.
//----------------------------------------------------------------------------
#define INC_OLE2
#include <ocf/pch.h>

#if !defined(OWL_DECMDIFR_H)
# include <owl/decmdifr.h>
#endif
#if !defined(OWL_STATUSBA_H)
# include <owl/statusba.h>
#endif
#if !defined(OCF_OCFEVENT_H)
# include <ocf/ocfevent.h>
#endif
#if !defined(OCF_OLEFRAME_H)
# include <ocf/oleframe.h>
#endif
#if !defined(OCF_OLEWINDO_H)
# include <ocf/olewindo.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif

__OCF_BEGIN_NAMESPACE

__OWL_USING_NAMESPACE

__OWL_USING_STD_NAMESPACE

OWL_DIAGINFO;
DIAG_DEFINE_GROUP_INIT(OWL_INI, OcfOleMenu, 1, 0);

//#define DEBUG_PADS
// define to make pads paint red for debugging time in MS for idle action polling
const int DllIdleTime = 200;

// Four edge locations for inplace-server space negotiation
//
const TDecoratedFrame::TLocation SpaceLoc[] = {
  TDecoratedFrame::Left,
  TDecoratedFrame::Top,
  TDecoratedFrame::Right,
  TDecoratedFrame::Bottom
};

//----------------------------------------------------------------------------
// An empty no-erase window that inserts itself into its decorated frame
// parent
//

class TPadWindow : public TWindow {
  public:
    TPadWindow(TWindow* parent, int edge, TModule* module = 0)
      : TWindow(parent, 0, module)
    {
      Attr.Id = IDW_PADDECORATION+edge;
      Attr.Style = WS_CHILD | WS_VISIBLE;
      Attr.W = Attr.H = 1;             // takeup no space until needed
                                       // need 1 since layout overlapps borders
#if defined(DEBUG_PADS)
      SetBkgndColor(TColor(255,0,0));  // to help Debug toolbar negotiations
      strstream s;
      s << _T("Pad") << (_T('0') + edge);
      Title = strnewdup(s.str());
#else
      SetBkgndColor(NoErase);          // Don't erase--to avoid flicker
#endif
    }
};

//
// class TRemViewBucket
// ~~~~~ ~~~~~~~~~~~~~~
// A small window class to act as a holder for the unattached remote server
// view windows
//
class _CALLCNVN TRemViewBucket : public TFrameWindow {
  public:
    TRemViewBucket(TWindow* parent);
    
    // Overriden virtuals of base class
    //
    bool      SetDocTitle(LPCTSTR, int);

    // Override handlers to act as pass-thru on behalf of remove server view
    //
    TResult   EvCommand(uint id, THandle hWndCtl, uint notifyCode);
    TResult   EvNotify(uint id, TNotify & notifyInfo);
    void      EvCommandEnable(TCommandEnabler& ce);
    TWindow*  FindRemoteWindow();
};

//
//
//
TRemViewBucket::TRemViewBucket(TWindow* parent) 
               :TFrameWindow(parent, _T("RemViewBucket"))
{
  Attr.Style = WS_CHILD | WS_DISABLED;
  Attr.Id = IDW_REMVIEWBUCKET;
}

//
//
//
bool
TRemViewBucket::SetDocTitle(LPCTSTR, int)
{
  return true;
}

//
//
//
TWindow*  
TRemViewBucket::FindRemoteWindow()
{
  // Check if we can find the object representing the remote window object by
  // traversing the focus window tree
  //
  TWindow* remote = 0;
  HWND focusHwnd  = GetFocus();
  HWND hwnd       = focusHwnd;
  while (hwnd) {
    remote = GetWindowPtr(hwnd);
    if (remote) {
      // The remote window has the remViewBucket as its parent
      //
      if (remote->GetParentO() == this)
        break;
      else
        remote = 0;
    }
    hwnd = ::GetParent(hwnd);
  }
  return remote;
}

//
//
//
TResult
TRemViewBucket::EvCommand(uint id, THandle hWndCtl, uint notifyCode)
{
//#if !defined(_OWLDLL) && !defined(BI_NAMESPACE)   //Jogy - having problems with using the OwlCmd which resides in OWL namespace
//  TRACEX(OwlCmd, 1, _T("TRemViewBucket::EvCommand - id(") << id << _T("), ctl(") <<\
//                     hex << uint(hWndCtl) << _T("), code(") << notifyCode  << _T(")"));
//#endif

  // Find remote view
  //
  TWindow* handler = FindRemoteWindow();

  // Default to parent if none was found
  //
  if (!handler)
    handler = GetParentO();

//#if !defined(_OWLDLL) && !defined(BI_NAMESPACE)   //Jogy - having problems with using the OwlCmd which resides in OWL namespace
//  TRACEX(OwlCmd, 1, _T("TRemViewBucket::EvCommand - forwarding to ") << *handler);
//#endif

  // Forward event
  //
  CHECK(handler);
  return handler->EvCommand(id, hWndCtl, notifyCode);
}

//
//
//
TResult
TRemViewBucket::EvNotify(uint id, TNotify & notifyInfo)
{
//#if !defined(_OWLDLL) && !defined(BI_NAMESPACE)   //Jogy - having problems with using the OwlCmd which resides in OWL namespace
//  TRACEX(OwlCmd, 1, _T("TRemViewBucket::EvNotify - id(") << id << _T("), ctl(")\
//                    << hex << uint(notifyInfo.hwndFrom) << _T("), code(") \
//                    << notifyInfo.code  << _T(")"));
//#endif

  // Find remote view
  //
  TWindow* handler = FindRemoteWindow();

  // Default to parent if none was found
  //
  if (!handler)
    handler = GetParentO();

//#if !defined(_OWLDLL) && !defined(BI_NAMESPACE)   //Jogy - having problems with using the OwlCmd which resides in OWL namespace
//  TRACEX(OwlCmd, 1, _T("TRemViewBucket::EvNotify - forwarding to ") << *handler);
//#endif

  // Forward event
  //
  CHECK(handler);
  return handler->EvNotify(id, notifyInfo);
}

//
//
//
void
TRemViewBucket::EvCommandEnable(TCommandEnabler& ce)
{
  // Find remote view
  //
  TWindow* handler = FindRemoteWindow();

  // Default to parent if none was found
  //
  if (!handler)
    handler = GetParentO();

//#if !defined(_OWLDLL) && !defined(BI_NAMESPACE)   //Jogy - having problems with using the OwlCmd which resides in OWL namespace
//  TRACEX(OwlCmd, 1, _T("TRemViewBucket::EvCommandEnable - forwarding to ") << *handler);
//#endif

  // Forward event
  //
  CHECK(handler);
  handler->EvCommandEnable(ce);
}

DEFINE_RESPONSE_TABLE1(TOleFrame, TDecoratedFrame)
  EV_WM_SIZE,
  EV_WM_TIMER,
  EV_WM_ACTIVATEAPP,

  EV_MESSAGE(WM_OCEVENT, EvOcEvent),
  EV_OC_APPINSMENUS,
  EV_OC_APPMENUS,
  EV_OC_APPPROCESSMSG,
  EV_OC_APPFRAMERECT,
  EV_OC_APPBORDERSPACEREQ,
  EV_OC_APPBORDERSPACESET,
  EV_OC_APPSTATUSTEXT,
  EV_OC_APPRESTOREUI,
  EV_OC_APPSHUTDOWN,
END_RESPONSE_TABLE;

//
//
//
TOleFrame::TOleFrame(LPCTSTR 	title,
                     TWindow* clientWnd,
                     bool     trackMenuSelection,
                     TModule* module)
:
  TDecoratedFrame(0, title, clientWnd, trackMenuSelection, module),
  TFrameWindow(0, title, clientWnd, false, module),
  TWindow(0, title, module),
  HOldMenu(0),
  OcApp(0),
  StashCount(0),
  OcShutDown(DontCare)
{
  new TRemViewBucket(this);  // Construct bucket to hold hidden server windows

  // Retrieve the OcApp ptr from our owning application if it is a TOcAppHost
  //
  TOcAppHost* ocm = TYPESAFE_DOWNCAST(GetApplication(), TOcAppHost);
  if (ocm)
    SetOcApp(ocm->OcApp);
}

//
// Let the OC app go. It will delete itself when it can
//
TOleFrame::~TOleFrame()
{
}

//
// Initial set of OcApp being passed to us to use.
//
void
TOleFrame::SetOcApp(TOcApp* ocApp)
{
  PRECONDITION(ocApp);
  OcApp = ocApp;

  // Initialize OLE 2 clipboard format names
  //
  TCHAR f[] = _T("%s");
  AddUserFormatName(f, f, ocrEmbedSource);
  AddUserFormatName(f, f, ocrEmbeddedObject);
  AddUserFormatName(f, f, ocrLinkSource);
}

//
// Let the OcApp know our THandle so that it can talk to us
//
void
TOleFrame::SetupWindow()
{
  PRECONDITION(OcApp);
  TDecoratedFrame::SetupWindow();
  OcApp->SetupWindow(GetHandle());

  // Insert the four pad windows for in-place server toolbars. Inserting last
  // will place them as the inner-most decorations, which is needed with the
  // status bar
  //
  for (int edge = 0; edge < 4; edge++)
    Insert(*new TPadWindow(this, edge), ::__OCF SpaceLoc[edge]);

  // Create a timer to allow us to poll for idle time when we are a dll server
  //
  if (!OcApp->IsOptionSet(amExeMode))
    SetTimer(IDT_DLLIDLE, DllIdleTime);
}

//
//
//
void
TOleFrame::CleanupWindow()
{
  if (!OcApp->IsOptionSet(amExeMode))
    KillTimer(IDT_DLLIDLE);
}

//
// Add user defined format and result names to the list
//
void
TOleFrame::AddUserFormatName(LPCTSTR name, LPCTSTR resultName,
                             LPCTSTR id)
{
  PRECONDITION(OcApp);
  OcApp->AddUserFormatName(name, resultName, id);
}

//
//
//
void
TOleFrame::OleViewClosing(bool close)
{
  if (close && OcShutDown == DontCare) {
    OcShutDown = ViewInitiated;
  }
  else if (!close && OcShutDown == ViewInitiated) {
    OcShutDown = DontCare;
  }
}

// Disconnect document servers with their clients.
// Document servers can be documents with objects copied on the clipboard or
// documents brought up though linking.
//
void
sDisconnectDocServer(TWindow* win, void* /*retVal*/)
{
  if (win) {
    win->ForEach(sDisconnectDocServer);

    TOleWindow* oleWin = TYPESAFE_DOWNCAST(win, TOleWindow);
    if (oleWin)
      oleWin->OleShutDown();
  }
}

//
// Make sure that any embeded servers also get a chance to OK the close
// sequence. If the user closes the app, but there are still embeddings, hide
// the frame instead of destroying it
//
void
TOleFrame::Destroy(int retVal)
{
  if (!GetHandle() || OcShutDown == UserInitiated)
    return;

  // Disconnect document servers with their clients if user shuts down the app
  //
  if (OcShutDown == DontCare) {
    OcShutDown = UserInitiated;
    ForEach(sDisconnectDocServer, 0);
  }

  if (!OcApp->CanClose()) {
    OcApp->SetOption(amEmbedding, true);
    OcShutDown = DontCare; // reset the shutdown flag
    ShowWindow(SW_HIDE);
  }
  else {
    bool dllServer = !OcApp->IsOptionSet(amExeMode);

    TDecoratedFrame::Destroy(retVal);

    // If user shuts down the DLL server (as in the case of open-editing,
    // we need to set the mainwindow flag to 0 so that only the application
    // will be destroyed.  All windows are destroyed in previous calls.
    //
    if (dllServer && OcShutDown != ServerInitiated) {
//      GetApplication()->SetMainWindow(0);
      GetApplication()->MainWindow = 0;
      delete GetApplication();
    }
  }
}

//
// Forward size message to OcApp to allow it to notify any embedded servers
// about size changes
//
void
TOleFrame::EvSize(uint sizeType, TSize& size)
{
  TDecoratedFrame::EvSize(sizeType, size);
  if (OcApp)
    OcApp->EvResize();
}

//
// Forward Activate messages to OcApp to allow it to notify any embedded servers
// about being activated
//
void
TOleFrame::EvActivateApp(bool active, HTASK hTask)
{
  OcApp->EvActivate(active);
  TDecoratedFrame::EvActivateApp(active, hTask);
}

//
//
//
void
TOleFrame::EvTimer(uint timerId)
{
  if (timerId == IDT_DLLIDLE)
    GetApplication()->PostDispatchAction();
  TWindow::EvTimer(timerId);
}

//
// Handle & sub-dispatch the OC event message.
//
TResult
TOleFrame::EvOcEvent(TParam1 param1, TParam2 param2)
{
  TEventHandler::TEventInfo eventInfo(WM_OCEVENT, param1);

  // Give the client window the first chance at it
  //
  TWindow* receiver = GetClientWindow();
  if (receiver->Find(eventInfo))
    return receiver->Dispatch(eventInfo, param1, param2);

  // Then try this frame
  //
  if (Find(eventInfo))
    return Dispatch(eventInfo, param1, param2);

  // Last, try the application in case it wants to override events
  //
  if (GetApplication()->Find(eventInfo))
    return GetApplication()->Dispatch(eventInfo, param1, param2);
  return 0;
}

//
// Insert our menus into a provided menu bar, possibly merging them with a
// servers.
//
bool
TOleFrame::EvOcAppInsMenus(TOcMenuDescr & sharedMenu)
{
  if (HOldMenu) {
    TRACEX(OcfOleMenu, 0, _T("EvOcAppInsMenus called while HOldMenu is ") << hex <<
          (uint)HOldMenu);
    return true;
  }

  // Recreate a temporary composite menu for frame
  //
  TMenuDescr compMenuDesc; // empty menudescr
  if (GetMenuDescr()) {
    compMenuDesc.Merge(*GetMenuDescr());

    // Mask off the server menus
    //
    compMenuDesc.Merge(TMenuDescr(0,  0, -1, 0, -1, 0, -1));
  }
  else
    return false; // stop menu negotiation if we don't have menu

  // Merge into the OLE shared menubar
  //
  TMenuDescr shMenuDescr(sharedMenu.HMenu,
                         sharedMenu.Width[0],
                         sharedMenu.Width[1],
                         sharedMenu.Width[2],
                         sharedMenu.Width[3],
                         sharedMenu.Width[4],
                         sharedMenu.Width[5]);
  shMenuDescr.Merge(compMenuDesc);

  // Copy the shared menu widths back to the OC struct
  //
  for (int i = 0; i < 6; i++)
    sharedMenu.Width[i] = shMenuDescr.GetGroupCount(i);

  // Save the container popups so they can be destroyed later
  //
  StashContainerPopups(shMenuDescr);

  TRACEX(OcfOleMenu, 0, _T("Merged menu ") << hex << (uint)sharedMenu.HMenu);
  return true;
}

//
// Now, actually set the merged menubar into our frame
//
bool
TOleFrame::EvOcAppMenus(TOcMenuDescr & appMenus)
{
  if (!appMenus.HMenu) {
    if (HOldMenu) {
      TRACEX(OcfOleMenu, 0, _T("EvOcAppMenus(0) resetting Old ") << hex <<
             (uint)HOldMenu);
      TMenu oleMenu(GetHandle());
      SetMenu(HOldMenu);     // assumes we are just restoring the old owl menu
      HOldMenu = 0;
    }
    DestroyStashedPopups();  // destroy the popup copies we made
    return true;
  }

  // Don't set the menu again if we are holding a merged one already
  //
  if (HOldMenu) {
    TRACEX(OcfOleMenu, 0, _T("EvOcAppMenus called while HOldMenu is ") << hex <<
          (uint)HOldMenu);
    return true;
  }

  HOldMenu = GetMenu();
  TRACEX(OcfOleMenu, 0, _T("Saved Old ") << hex << (uint)HOldMenu);

  SetMenu(appMenus.HMenu);
  TRACEX(OcfOleMenu, 0, _T("Set merged ") << hex << (uint)appMenus.HMenu);

  return true;
}

//
// Process accelerators and other queued messages from the server's queue in
// our app
//
bool
TOleFrame::EvOcAppProcessMsg(MSG * msg)
{
  return GetApplication()->ProcessAppMsg(*msg);
}

//
// Let the server know our frame's client rect, minus the status bar height,
// if there is one
//
bool
TOleFrame::EvOcAppFrameRect(TRect * rect)
{
  PRECONDITION(rect);
  *rect = GetClientRect();
  TWindow*  sb = ChildWithId(IDW_STATUSBAR);
  if (sb) {
    TRect sbr = sb->GetClientRect();
    rect->bottom -= sbr.bottom+1;
  }
  return true;
}

//
// Let the server know that we can handle border adornments on all four edges
//
bool
TOleFrame::EvOcAppBorderSpaceReq(TRect * /*space*/)
{
  return true;
}

//
// Actually create space on the requested edges for the server's adornments
//
bool
TOleFrame::EvOcAppBorderSpaceSet(TRect * space)
{
  // Resize pad decorations based on edges requested
  //
  int* edges = (int*)space;   // treat space as array of 4 int edges
  bool needLayout = false;
  for (int i = 0; i < 4; i++) {
    TWindow*  pad = ChildWithId(IDW_PADDECORATION+i);
    if (pad) {
      int edge = edges && edges[i] ? edges[i]+1 : 1;
      if (i%1 == 0 && pad->Attr.W != edge ||   // left & right edge
          i%1 == 1 && pad->Attr.H != edge) {   // top & bottom edge
        TLayoutMetrics m;
        GetChildLayoutMetrics(*pad, m);
        pad->Attr.H = pad->Attr.W = edge;  // set both axis, one will be stretched
        SetChildLayoutMetrics(*pad, m);
        needLayout = true;
      }
    }
  }

  // Turn on/off control bar as needed
  //
  TWindow*  tb = ChildWithId(IDW_TOOLBAR);
  if (tb)
    if (space && tb->IsWindowVisible() || !space && !tb->IsWindowVisible()) {
      SendMessage(WM_COMMAND, IDW_TOOLBAR);  // toggle tool bar on/off
      needLayout = false;  // layout already done now by decorated frame.
    }

  // Now do layout once at the end to reduce repaint
  //
  if (needLayout)
    Layout();

  return true;
}

//
// Display the servers status messages on our status bar
//
void
TOleFrame::EvOcAppStatusText(LPCTSTR text)
{
  TMessageBar* mb = TYPESAFE_DOWNCAST(ChildWithId(IDW_STATUSBAR), TMessageBar);
  if (mb) {
    //mb->SetHintText(text);   // is the text a hint, or general status??
    mb->SetText(text);
  }
}

//
// Save off the popup menu handles that are copies so that we can destroy
// them later. Keep track of active object menus, to allow cleanup when
// the last one goes away.
//
void TOleFrame::StashContainerPopups(const TMenuDescr& shMenuDescr)
{
  StashCount++;

  int m = 0;
  for (int i = 0; i < 6; i++) {
    if (i%2 == 0)
      for (int j = 0; j < shMenuDescr.GetGroupCount(i); j++) {
        uint  state = shMenuDescr.GetMenuState(m+j, MF_BYPOSITION);
        if (state == uint(-1))
          continue;
        TRACEX(OcfOleMenu, 1, _T("Stashing ") << hex << (uint)shMenuDescr.GetSubMenu(m+j));
        StashedContainerPopups.AppendMenu(state, TMenu(shMenuDescr.GetSubMenu(m+j)), _T(""));
      }
    m += shMenuDescr.GetGroupCount(i);
  }
}

//
// Throw away our popup copies that are no longer needed, when the last active
// object is deactivated.
//
void TOleFrame::DestroyStashedPopups()
{
  if (--StashCount)
    return;

  while (StashedContainerPopups.GetMenuItemCount()) {
    TRACEX(OcfOleMenu, 1, _T("Destroying ") << hex << (uint)StashedContainerPopups.GetSubMenu(0));
    StashedContainerPopups.DeleteMenu(0, MF_BYPOSITION);
  }
}

//
// Restore the UI back to pre-inplace server state. Remove the border space pads
// and restore the menu.
//
void
TOleFrame::EvOcAppRestoreUI()
{
  // Only restore the old menu if we are holding a merged one
  //
  if (HOldMenu) {
    TRACEX(OcfOleMenu, 0, _T("EvOcAppRestoreUI resetting Old ") << hex <<
           (uint)HOldMenu);
    TMenu oleMenu(GetHandle());
    SetMenu(HOldMenu);    // assumes we are just restoring the old owl menu
    HOldMenu = 0;
  }

  // Remove pad decorations & restore our toobar if we have one
  //
  EvOcAppBorderSpaceSet(0);
}

//
// Called by TOcRemView::Close when last embedding is closed
// If that's the only reason the app is up we need to shut ourselves down
//
bool
TOleFrame::EvOcAppShutdown()
{
  // If TOleFrame was created purely for embedded server, then
  // we want to shut down the app when nobody is using the server.
  // The amEmbedding flag will be set to false if user created normal
  // document in this frame.
  //
  if (OcShutDown == DontCare && OcApp->IsOptionSet(amEmbedding)) {
    // The shut down is initiated by OCF
    //
    OcShutDown = ServerInitiated;

    // Post frame close to kill the app later
    //
    PostMessage(WM_CLOSE);

    return true;
  }
  else {

    // If the last view closing caused the ocapp to shutdown, then close this
    // frame.
    //
    if (OcApp->IsOptionSet(amEmbedding) && OcShutDown == ViewInitiated)
      PostMessage(WM_CLOSE);

    return false; // Shut down initiated by user
  }
}

__OCF_END_NAMESPACE

//==============================================================================

