//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 bBidus Yura, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:14 $
//
//
//------------------------------------------------------------------------------

#if !defined(OWL_TRAYICON_H)
#define OWL_TRAYICON_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>      // Supporting defines for app classes
#endif
#include <shellapi.h>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TTrayIcon 
// ~~~~~ ~~~~~~~~~
//
class _OWLCLASS TTrayIcon: protected NOTIFYICONDATA {
  public:
    TTrayIcon(TModule* module=gModule);
    TTrayIcon(TWindow* parent, uint msg, const owl_string& tip, TIcon& icon, uint id, TModule* module=gModule);
    TTrayIcon(TWindow* parent, uint msg, const owl_string& tip, TResId icon, uint id, TModule* module=gModule);
    virtual void Init(TWindow* parent, uint msg, const owl_string& tip, TIcon* icon=0, uint id=0);
    virtual ~TTrayIcon();

  // Operations
  public:
    bool Enabled() const;
    bool Visible() const;

    //Create the tray icon => must have valid hWnd => call in SetupWindow();
    bool Create();

    //Change or retrieve the Tooltip text
    bool        SetText(const owl_string& text);
    bool        SetText(TResId resId);
    owl_string  GetText() const;

    //Change or retrieve the window to send notification messages to
    bool      SetParent(TWindow* wnd);
    TWindow*   GetParent() const;

    //Change or retrieve the icon displayed
    bool    SetIcon(TIcon& icon, TAutoDelete = NoAutoDelete);
    bool    SetIcon(TResId resId);
    TIcon*  GetIcon() const;


    void  HideIcon();
    void  ShowIcon();
    void  RemoveIcon();
    void  MoveToRight();

    //Default handler for tray notification message
    TResult EvTrayNotification(TParam1 param1, TParam2 param2);

  // Implementation
  protected:
    bool       bEnabled;  // does O/S support tray icon?
    bool      bHidden;  // Has the icon been hidden?
    TWindow*   Parent;
    TModule*  Module;
    TIcon*    Icon;
    bool      ShouldDelete;

  //DECLARE_DYNAMIC(TTrayIcon)
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

inline bool TTrayIcon::Enabled() const {
  return bEnabled;
}
inline bool TTrayIcon::Visible() const {
  return !bHidden;
}
inline TWindow* TTrayIcon::GetParent() const{
  return Parent;
}
inline bool TTrayIcon::SetText(TResId resId){
  return SetText(owl_string(Module->LoadString((uint)(LPCTSTR)resId)));
}

inline bool TTrayIcon::SetIcon(TResId resId){
  return SetIcon(*new TIcon(Module? (HINSTANCE)*Module : HINSTANCE(0), resId), AutoDelete);
}

__OWL_END_NAMESPACE


#endif // #define OWL_TRAYICON_H

