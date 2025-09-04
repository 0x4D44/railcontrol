//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:47 $
//
//----------------------------------------------------------------------------

#if !defined(OCF_OLEFRAME_H)
#define OCF_OLEFRAME_H

#if !defined(OWL_DECFRAME_H)
# include <owl/decframe.h>
#endif
#if !defined(OCF_OCFEVENT_H)
# include <ocf/ocfevent.h>
#endif
#if !defined(OCF_OCREG)
# include <ocf/ocreg.h>
#endif
#if !defined(OCF_OLEFACTO_H)
# include <ocf/olefacto.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OCF_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// pad decorations IDs, 4 total: 32042, 32043, 32044, 32045
//
const int IDW_PADDECORATION = 32042;
const int IDW_REMVIEWBUCKET = 32046;  // Window id for remote view holder
const int IDT_DLLIDLE       = 32000;  // Idle timer ID for DLL servers

struct TOcMenuDescr;
class _ICLASS TOcApp;

//
// class TOleFrame
// ~~~~~ ~~~~~~~~~
// Decorated frame that supports OLE 2 using OCF
//
class _OCFCLASS TOleFrame : public __OWL TDecoratedFrame {
  public:
    TOleFrame(LPCTSTR		title,
              __OWL TWindow*  clientWnd,
              bool      trackMenuSelection = false,
              __OWL TModule*  module = 0);
   ~TOleFrame();
    TOcApp*   GetOcApp();
    void      SetOcApp(TOcApp* app);
    void      AddUserFormatName(LPCTSTR name, LPCTSTR resultName, LPCTSTR id);

    __OWL TWindow*	GetRemViewBucket();
    void       OleViewClosing(bool close);

  protected:
    void      SetupWindow();
    void      CleanupWindow();
    void      Destroy(int retVal);

    void      EvSize(uint sizeType,  __OWL TSize& size);
    void      EvActivateApp(bool active, HTASK hTask);
    void      EvTimer(uint timerId);

    __OWL TResult   EvOcEvent(__OWL TParam1 param1, __OWL TParam2 param2);
    bool      EvOcAppInsMenus(TOcMenuDescr & sharedMenu);
    bool      EvOcAppMenus(TOcMenuDescr & md);
    bool      EvOcAppProcessMsg(MSG * msg);
    bool      EvOcAppFrameRect(__OWL TRect * rect);
    bool      EvOcAppBorderSpaceReq(__OWL TRect * rect);
    bool      EvOcAppBorderSpaceSet(__OWL  TRect * rect);
    void      EvOcAppStatusText(LPCTSTR rect);
    void      EvOcAppRestoreUI();
    void      EvOcAppDialogHelp(TOcDialogHelp & dh);
    bool      EvOcAppShutdown();

  protected:
    void			DestroyStashedPopups();
    void			StashContainerPopups(const  __OWL TMenuDescr& shMenuDescr);

    TOcApp*		OcApp;
    __OWL TMenu			StashedContainerPopups;
    int				StashCount;
    HMENU			HOldMenu;

  private:
    enum {
      DontCare, UserInitiated, ViewInitiated, ServerInitiated
    } OcShutDown;

  DECLARE_RESPONSE_TABLE(TOleFrame);
};

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>

// --------------------------------------------------------------------------
// Inline implementations
//

//
inline TOcApp*
TOleFrame::GetOcApp()
{
  return OcApp;
}

//
inline __OWL TWindow*
TOleFrame::GetRemViewBucket()
{
  return ChildWithId(IDW_REMVIEWBUCKET);
}

//
inline void
TOleFrame::EvOcAppDialogHelp(TOcDialogHelp & dh)
{
}

__OCF_END_NAMESPACE


#endif  // OWL_OLEFRAME_H
