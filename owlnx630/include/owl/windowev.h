//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Event response table macros for windows messages
//----------------------------------------------------------------------------

#if !defined(OWL_WINDOWEV_H)
#define OWL_WINDOWEV_H

#if !defined(OWL_EVENTHAN_H)
# include <owl/eventhan.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


//
// General purpose WM_* macro
//
// wparam and lparam are passed, i.e: TParam2 method(TParam1, TParam2)
//
#define EV_MESSAGE(message, method)\
  {{message}, 0, (TAnyDispatcher)::I32_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::LRESULT_WPARAM_LPARAM_Sig<TMyClass>(&TMyClass::method)}

//
// Registered messages
//
#define EV_REGISTERED(str, method)\
  EV_MESSAGE(::RegisterWindowMessage(str), method)

//
// Handler for child ID notifications that are handled at the child's parent
//
// no arguments are passed, i.e: void method()
//
#define EV_CHILD_NOTIFY(id, notifyCode, method)\
  {{notifyCode}, id, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for child ID notifications that are handled at the child's parent
//
// The notification code is passed in as an argument; this allows multiple
// notifications to be handled by a single response function, i.e:
//   void method(uint notifyCode)
//
#define EV_CHILD_NOTIFY_AND_CODE(id, notifyCode, method)\
  {{notifyCode}, id, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::method)}

//
// Generic handler for child ID notification: rather than specify the
// specific notification codes you are interested in ALL notifications
// from the child are passed to the response function
//
// The notification code is passed in as an argument, i.e.:
//   void method(uint notifyCode)
//
#define EV_CHILD_NOTIFY_ALL_CODES(id, method)\
  {{UINT_MAX}, id, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for child ID notifications that are handled at the child
//
// No arguments are passed, i.e: void method()
//
#define EV_NOTIFY_AT_CHILD(notifyCode, method)\
  {{notifyCode}, UINT_MAX, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for menu/accelerator/push buttons
//
// No arguments are passed, i.e: void method()
//
#define EV_COMMAND(id, method)\
  {{0}, id, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for menu/accelerator/push buttons
//
// The menu id is passed in as an argument; this allows multiple
// commands to be handled by a single response function.
//  void method(uint commandId)
//
#define EV_COMMAND_AND_ID(id, method)\
  {{0}, id, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for command enabling
//  void method(uint commandId)
//
#define EV_COMMAND_ENABLE(id, method)\
  {{WM_COMMAND_ENABLE}, id, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OWL::v_COMMANDENABLER_Sig<TMyClass>(&TMyClass::method)}

//
// Handlers for document and view notifications
//
#define EV_OWLDOCUMENT(id, method)\
  {{WM_OWLDOCUMENT}, id, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OWL::v_OWLDOCUMENT_Sig<TMyClass>(&TMyClass::method)}

#define EV_OWLVIEW(id, method)\
  {{WM_OWLVIEW}, id, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OWL::v_OWLVIEW_Sig<TMyClass>(&TMyClass::method)}

#define EV_OWLNOTIFY(id, method)\
  {{WM_OWLNOTIFY}, id, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_LPARAM_Sig<TMyClass>(&TMyClass::method)}

#define EV_VIEWNOTIFY(id, method) \
  {{WM_OWLNOTIFY}, id, (TAnyDispatcher)::B_LPARAM_Dispatch, \
  (TMyPMF)NS_OWL::B_LPARAM_Sig<TMyClass>(&TMyClass::method)}

//
// Handler for the OWL frame sizing message
//
#define EV_OWLFRAMESIZE\
  {{WM_OWLFRAMESIZE}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_SIZE_Sig<TMyClass>(&TMyClass::EvOwlFrameSize)}

//
// Handler for the OWL Docking message
//
#define EV_WM_OWLWINDOWDOCKED\
  {{WM_OWLWINDOWDOCKED}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_OWLWINDOWDOCKED_Sig<TMyClass>(&TMyClass::EvOwlWindowDocked)}

#define EV_OWLWINDOWDOCKED EV_WM_OWLWINDOWDOCKED

//
// Handler for the OWL Help manager message
//
#define EV_OWLHELPHIT\
  {{WM_OWLHELPHIT}, 0, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OWL::v_OWLHELPHIT_Sig<TMyClass>(&TMyClass::EvHelpHit)}

   //
// Macros for specific WM_* messages
//
#define EV_WM_ACTIVATE\
  {{WM_ACTIVATE}, 0, (TAnyDispatcher)::v_Activate_Dispatch,\
   (TMyPMF)NS_OWL::v_U_B_W_Sig<TMyClass>(&TMyClass::EvActivate)}

#define EV_WM_ACTIVATEAPP\
  {{WM_ACTIVATEAPP}, 0, (TAnyDispatcher)::v_B_U_Dispatch,\
   (TMyPMF)NS_OWL::v_B_HTASK_Sig<TMyClass>(&TMyClass::EvActivateApp)}

#define EV_WM_ASKCBFORMATNAME\
  {{WM_ASKCBFORMATNAME}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_S_Sig<TMyClass>(&TMyClass::EvAskCBFormatName)}

#define EV_WM_CANCELMODE\
  {{WM_CANCELMODE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvCancelMode)}

#define EV_WM_CHANGECBCHAIN\
  {{WM_CHANGECBCHAIN}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_W_W_Sig<TMyClass>(&TMyClass::EvChangeCBChain)}

#define EV_WM_CHAR\
  {{WM_CHAR}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvChar)}

#define EV_WM_CHARTOITEM\
  {{WM_CHARTOITEM}, 0, (TAnyDispatcher)::i_U_W_U_Dispatch,\
   (TMyPMF)NS_OWL::i_U_W_U_Sig<TMyClass>(&TMyClass::EvCharToItem)}

#define EV_WM_CHILDACTIVATE\
  {{WM_CHILDACTIVATE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvChildActivate)}

#define EV_WM_CHILDINVALID\
  {{WM_CHILDINVALID}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvChildInvalid)}

#define EV_WM_CLOSE\
  {{WM_CLOSE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvClose)}

#define EV_WM_COMMNOTIFY\
  {{WM_COMMNOTIFY}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_Sig<TMyClass>(&TMyClass::EvCommNotify)}

#define EV_WM_COMPACTING\
  {{WM_COMPACTING}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::EvCompacting)}

#define EV_WM_COMPAREITEM\
  {{WM_COMPAREITEM}, 0, (TAnyDispatcher)::LRESULT_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::LRESULT_U_COMPAREITEM_Sig<TMyClass>(&TMyClass::EvCompareItem)}

// Win32 only
//
#define EV_WM_COPYDATA\
  {{WM_COPYDATA}, 0, (TAnyDispatcher)::v_HWND_PCOPYDATASTRUCT_Dispatch,\
   (TMyPMF)NS_OWL::v_HWND_PCOPYDATASTRUCT_Sig<TMyClass>(&TMyClass::EvCopyData)}

#define EV_WM_CREATE\
  {{WM_CREATE}, 0, (TAnyDispatcher)::i_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::i_CREATE_Sig<TMyClass>(&TMyClass::EvCreate)}

#define EV_WM_CTLCOLOR\
  {{WM_CTLCOLOR}, 0, (TAnyDispatcher)::U_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::HBRUSH_HDC_W_U_Sig<TMyClass>(&TMyClass::EvCtlColor)}

#define EV_WM_DEADCHAR\
  {{WM_DEADCHAR}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvDeadChar)}

#define EV_WM_DELETEITEM\
  {{WM_DELETEITEM}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_DELETEITEM_Sig<TMyClass>(&TMyClass::EvDeleteItem)}

#define EV_WM_DESTROY\
  {{WM_DESTROY}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvDestroy)}

#define EV_WM_DESTROYCLIPBOARD\
  {{WM_DESTROYCLIPBOARD}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvDestroyClipboard)}

#define EV_WM_DEVMODECHANGE\
  {{WM_DEVMODECHANGE}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_S_Sig<TMyClass>(&TMyClass::EvDevModeChange)}

#define EV_WM_DRAWCLIPBOARD\
  {{WM_DRAWCLIPBOARD}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvDrawClipboard)}

#define EV_WM_DRAWITEM\
  {{WM_DRAWITEM}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_DRAWITEM_Sig<TMyClass>(&TMyClass::EvDrawItem)}

#define EV_WM_DROPFILES\
  {{WM_DROPFILES}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_DROP_Sig<TMyClass>(&TMyClass::EvDropFiles)}

#define EV_WM_ENABLE\
  {{WM_ENABLE}, 0, (TAnyDispatcher)::v_B_Dispatch,\
   (TMyPMF)NS_OWL::v_B_Sig<TMyClass>(&TMyClass::EvEnable)}

#define EV_WM_ENDSESSION\
  {{WM_ENDSESSION}, 0, (TAnyDispatcher)::v_B_B_Dispatch,\
   (TMyPMF)NS_OWL::v_B_B_Sig<TMyClass>(&TMyClass::EvEndSession)}

#define EV_WM_ENTERIDLE\
  {{WM_ENTERIDLE}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_W_Sig<TMyClass>(&TMyClass::EvEnterIdle)}

#define EV_WM_ERASEBKGND\
  {{WM_ERASEBKGND}, 0, (TAnyDispatcher)::B_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_HDC_Sig<TMyClass>(&TMyClass::EvEraseBkgnd)}

#define EV_WM_FONTCHANGE\
  {{WM_FONTCHANGE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvFontChange)}

#define EV_WM_GETDLGCODE\
  {{WM_GETDLGCODE}, 0, (TAnyDispatcher)::U_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::U_MSG_Sig<TMyClass>(&TMyClass::EvGetDlgCode)}

#define EV_WM_GETFONT\
  {{WM_GETFONT}, 0, (TAnyDispatcher)::U_Dispatch,\
   (TMyPMF)NS_OWL::HFONT_Sig<TMyClass>(&TMyClass::EvGetFont)}

#define EV_WM_GETMINMAXINFO\
  {{WM_GETMINMAXINFO}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_MINMAXINFO_Sig<TMyClass>(&TMyClass::EvGetMinMaxInfo)}

#define EV_WM_GETTEXT\
  {{WM_GETTEXT}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_S_Sig<TMyClass>(&TMyClass::EvGetText)}

#define EV_WM_GETTEXTLENGTH\
  {{WM_GETTEXTLENGTH}, 0, (TAnyDispatcher)::U_Dispatch,\
   (TMyPMF)NS_OWL::U_Sig<TMyClass>(&TMyClass::EvGetTextLength)}

// Win32 only
//
#define EV_WM_HOTKEY\
  {{WM_HOTKEY}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_i_Sig<TMyClass>(&TMyClass::EvHotKey)}

#define EV_WM_HSCROLL\
  {{WM_HSCROLL}, 0, (TAnyDispatcher)::v_U_U_W_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_W_Sig<TMyClass>(&TMyClass::EvHScroll)}

#define EV_WM_HSCROLLCLIPBOARD\
  {{WM_HSCROLLCLIPBOARD}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_W_U_U_Sig<TMyClass>(&TMyClass::EvHScrollClipboard)}

#define EV_WM_ICONERASEBKGND\
  {{WM_ICONERASEBKGND}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_HDC_Sig<TMyClass>(&TMyClass::EvIconEraseBkgnd)}

#define EV_WM_INITMENU\
  {{WM_INITMENU}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_HMENU_Sig<TMyClass>(&TMyClass::EvInitMenu)}

#define EV_WM_INITMENUPOPUP\
  {{WM_INITMENUPOPUP}, 0, (TAnyDispatcher)::v_U_U_B_Dispatch,\
   (TMyPMF)NS_OWL::v_HMENU_U_B_Sig<TMyClass>(&TMyClass::EvInitMenuPopup)}

#define EV_WM_KEYDOWN\
  {{WM_KEYDOWN}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvKeyDown)}

#define EV_WM_KEYUP\
  {{WM_KEYUP}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvKeyUp)}

#define EV_WM_KILLFOCUS\
  {{WM_KILLFOCUS}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvKillFocus)}

#define EV_WM_LBUTTONDBLCLK\
  {{WM_LBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvLButtonDblClk)}

#define EV_WM_LBUTTONDOWN\
  {{WM_LBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvLButtonDown)}

#define EV_WM_LBUTTONUP\
  {{WM_LBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvLButtonUp)}

#define EV_WM_MBUTTONDBLCLK\
  {{WM_MBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvMButtonDblClk)}

#define EV_WM_MBUTTONDOWN\
  {{WM_MBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvMButtonDown)}

#define EV_WM_MBUTTONUP\
  {{WM_MBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvMButtonUp)}

#define EV_WM_MDIACTIVATE\
  {{WM_MDIACTIVATE}, 0, (TAnyDispatcher)::v_MdiActivate_Dispatch,\
   (TMyPMF)NS_OWL::v_W_W_Sig<TMyClass>(&TMyClass::EvMDIActivate)}

#define EV_WM_MDICREATE\
  {{WM_MDICREATE}, 0, (TAnyDispatcher)::i_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::LRESULT_MDICREATESTRUCT_Sig<TMyClass>(&TMyClass::EvMDICreate)}

#define EV_WM_MDIDESTROY\
  {{WM_MDIDESTROY}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvMDIDestroy)}

#define EV_WM_MENUCHAR\
  {{WM_MENUCHAR}, 0, (TAnyDispatcher)::I32_MenuChar_Dispatch,\
   (TMyPMF)NS_OWL::LRESULT_U_U_HMENU_Sig<TMyClass>(&TMyClass::EvMenuChar)}

#define EV_WM_MENUSELECT\
  {{WM_MENUSELECT}, 0, (TAnyDispatcher)::v_U_U_W_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_HMENU_Sig<TMyClass>(&TMyClass::EvMenuSelect)}

#define EV_WM_MEASUREITEM\
  {{WM_MEASUREITEM}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_MEASUREITEM_Sig<TMyClass>(&TMyClass::EvMeasureItem)}

#define EV_WM_MOUSEACTIVATE\
  {{WM_MOUSEACTIVATE}, 0, (TAnyDispatcher)::U_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::U_W_U_U_Sig<TMyClass>(&TMyClass::EvMouseActivate)}

#define EV_WM_MOUSEMOVE\
  {{WM_MOUSEMOVE}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvMouseMove)}

#define EV_WM_MOUSEWHEEL\
  {{WM_MOUSEWHEEL}, 0, (TAnyDispatcher)::B_U_i_POINT_Dispatch,\
   (TMyPMF)NS_OWL::B_U_i_POINT_Sig<TMyClass>(&TMyClass::EvMouseWheel)}

#define EV_WM_MOVE\
  {{WM_MOVE}, 0, (TAnyDispatcher)::v_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_POINT_Sig<TMyClass>(&TMyClass::EvMove)}

#define EV_WM_NCACTIVATE\
  {{WM_NCACTIVATE}, 0, (TAnyDispatcher)::B_B_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig<TMyClass>(&TMyClass::EvNCActivate)}

#define EV_WM_NCCALCSIZE\
  {{WM_NCCALCSIZE}, 0, (TAnyDispatcher)::U_B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::U_B_CALCSIZE_Sig<TMyClass>(&TMyClass::EvNCCalcSize)}

#define EV_WM_NCCREATE\
  {{WM_NCCREATE}, 0, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_CREATE_Sig<TMyClass>(&TMyClass::EvNCCreate)}

#define EV_WM_NCDESTROY\
  {{WM_NCDESTROY}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvNCDestroy)}

#define EV_WM_NCHITTEST\
  {{WM_NCHITTEST}, 0, (TAnyDispatcher)::U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::U_POINT_Sig<TMyClass>(&TMyClass::EvNCHitTest)}

#define EV_WM_NCLBUTTONDBLCLK\
  {{WM_NCLBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCLButtonDblClk)}

#define EV_WM_NCLBUTTONDOWN\
  {{WM_NCLBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCLButtonDown)}

#define EV_WM_NCLBUTTONUP\
  {{WM_NCLBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCLButtonUp)}

#define EV_WM_NCMBUTTONDBLCLK\
  {{WM_NCMBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCMButtonDblClk)}

#define EV_WM_NCMBUTTONDOWN\
  {{WM_NCMBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCMButtonDown)}

#define EV_WM_NCMBUTTONUP\
  {{WM_NCMBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCMButtonUp)}

#define EV_WM_NCMOUSEMOVE\
  {{WM_NCMOUSEMOVE}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCMouseMove)}

// WM_NCPAINT has different parameters for 16 and 32bit
//
#define EV_WM_NCPAINT\
  {{WM_NCPAINT}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_HRGN_Sig<TMyClass>(&TMyClass::EvNCPaint)}

#define EV_WM_NCRBUTTONDBLCLK\
  {{WM_NCRBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCRButtonDblClk)}

#define EV_WM_NCRBUTTONDOWN\
  {{WM_NCRBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCRButtonDown)}

#define EV_WM_NCRBUTTONUP\
  {{WM_NCRBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvNCRButtonUp)}

#define EV_WM_NEXTDLGCTL\
  {{WM_NEXTDLGCTL}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_Sig<TMyClass>(&TMyClass::EvNextDlgCtl)}

#define EV_WM_PAINT\
  {{WM_PAINT}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvPaint)}

#define EV_WM_PAINTCLIPBOARD\
  {{WM_PAINTCLIPBOARD}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_W_H_Sig<TMyClass>(&TMyClass::EvPaintClipboard)}

// Win32 only
//
#define EV_WM_PAINTICON\
  {{WM_PAINTICON}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvPaintIcon)}

#define EV_WM_PALETTECHANGED\
  {{WM_PALETTECHANGED}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvPaletteChanged)}

#define EV_WM_PALETTEISCHANGING\
  {{WM_PALETTEISCHANGING}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvPaletteIsChanging)}

#define EV_WM_PARENTNOTIFY\
  {{WM_PARENTNOTIFY}, 0, (TAnyDispatcher)::v_ParentNotify_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvParentNotify)}

#define EV_WM_POWER\
  {{WM_POWER}, 0, (TAnyDispatcher)::i_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::i_U_Sig<TMyClass>(&TMyClass::EvPower)}

#define EV_WM_QUERYDRAGICON\
  {{WM_QUERYDRAGICON}, 0, (TAnyDispatcher)::U_Dispatch,\
   (TMyPMF)NS_OWL::H_Sig<TMyClass>(&TMyClass::EvQueryDragIcon)}

#define EV_WM_QUERYENDSESSION\
  {{WM_QUERYENDSESSION}, 0, (TAnyDispatcher)::B_Dispatch,\
   (TMyPMF)NS_OWL::B_Sig<TMyClass>(&TMyClass::EvQueryEndSession)}

#define EV_WM_QUERYNEWPALETTE\
  {{WM_QUERYNEWPALETTE}, 0, (TAnyDispatcher)::B_Dispatch,\
   (TMyPMF)NS_OWL::B_Sig<TMyClass>(&TMyClass::EvQueryNewPalette)}

#define EV_WM_QUERYOPEN\
  {{WM_QUERYOPEN}, 0, (TAnyDispatcher)::B_Dispatch,\
   (TMyPMF)NS_OWL::B_Sig<TMyClass>(&TMyClass::EvQueryOpen)}

#define EV_WM_QUEUESYNC\
  {{WM_QUEUESYNC}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvQueueSync)}

#define EV_WM_RBUTTONDBLCLK\
  {{WM_RBUTTONDBLCLK}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvRButtonDblClk)}

#define EV_WM_RBUTTONDOWN\
  {{WM_RBUTTONDOWN}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvRButtonDown)}

#define EV_WM_RBUTTONUP\
  {{WM_RBUTTONUP}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvRButtonUp)}

#define EV_WM_RENDERALLFORMATS\
  {{WM_RENDERALLFORMATS}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvRenderAllFormats)}

#define EV_WM_RENDERFORMAT\
  {{WM_RENDERFORMAT}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::EvRenderFormat)}

#define EV_WM_SETCURSOR\
  {{WM_SETCURSOR}, 0, (TAnyDispatcher)::B_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::B_W_U_U_Sig<TMyClass>(&TMyClass::EvSetCursor)}

#define EV_WM_SETFOCUS\
  {{WM_SETFOCUS}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvSetFocus)}

#define EV_WM_SETFONT\
  {{WM_SETFONT}, 0, (TAnyDispatcher)::v_U_B_Dispatch,\
   (TMyPMF)NS_OWL::v_HFONT_B_Sig<TMyClass>(&TMyClass::EvSetFont)}

#define EV_WM_SETREDRAW\
  {{WM_SETREDRAW}, 0, (TAnyDispatcher)::v_B_Dispatch,\
   (TMyPMF)NS_OWL::v_B_Sig<TMyClass>(&TMyClass::EvSetRedraw)}

#define EV_WM_SETTEXT\
  {{WM_SETTEXT}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_CS_Sig<TMyClass>(&TMyClass::EvSetText)}

#define EV_WM_SHOWWINDOW\
  {{WM_SHOWWINDOW}, 0, (TAnyDispatcher)::v_B_U_Dispatch,\
   (TMyPMF)NS_OWL::v_B_U_Sig<TMyClass>(&TMyClass::EvShowWindow)}

#define EV_WM_SIZE\
  {{WM_SIZE}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_SIZE_Sig<TMyClass>(&TMyClass::EvSize)}

#define EV_WM_SIZECLIPBOARD\
  {{WM_SIZECLIPBOARD}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_W_H_Sig<TMyClass>(&TMyClass::EvSizeClipboard)}

#define EV_WM_SPOOLERSTATUS\
  {{WM_SPOOLERSTATUS}, 0, (TAnyDispatcher)::v_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_Sig<TMyClass>(&TMyClass::EvSpoolerStatus)}

#define EV_WM_SYSCHAR\
  {{WM_SYSCHAR}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvSysChar)}

#define EV_WM_SYSCOLORCHANGE\
  {{WM_SYSCOLORCHANGE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvSysColorChange)}

#define EV_WM_SYSCOMMAND\
  {{WM_SYSCOMMAND}, 0, (TAnyDispatcher)::v_U_POINT_Dispatch,\
   (TMyPMF)NS_OWL::v_U_POINT_Sig<TMyClass>(&TMyClass::EvSysCommand)}

#define EV_WM_SYSDEADCHAR\
  {{WM_SYSDEADCHAR}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvSysDeadChar)}

#define EV_WM_SYSKEYDOWN\
  {{WM_SYSKEYDOWN}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvSysKeyDown)}

#define EV_WM_SYSKEYUP\
  {{WM_SYSKEYUP}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvSysKeyUp)}

#define EV_WM_SYSTEMERROR\
  {{WM_SYSTEMERROR}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::EvSystemError)}

#define EV_WM_TIMECHANGE\
  {{WM_TIMECHANGE}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvTimeChange)}

#define EV_WM_TIMER\
  {{WM_TIMER}, 0, (TAnyDispatcher)::v_WPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_U_Sig<TMyClass>(&TMyClass::EvTimer)}

#define EV_WM_VKEYTOITEM\
  {{WM_VKEYTOITEM}, 0, (TAnyDispatcher)::i_U_W_U_Dispatch,\
   (TMyPMF)NS_OWL::i_U_W_U_Sig<TMyClass>(&TMyClass::EvVKeyToItem)}

#define EV_WM_VSCROLL\
  {{WM_VSCROLL}, 0, (TAnyDispatcher)::v_U_U_W_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_W_Sig<TMyClass>(&TMyClass::EvVScroll)}

#define EV_WM_VSCROLLCLIPBOARD\
  {{WM_VSCROLLCLIPBOARD}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_W_U_U_Sig<TMyClass>(&TMyClass::EvVScrollClipboard)}

#define EV_WM_WINDOWPOSCHANGED\
  {{WM_WINDOWPOSCHANGED}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_WINDOWPOS_Sig<TMyClass>(&TMyClass::EvWindowPosChanged)}

#define EV_WM_WINDOWPOSCHANGING\
  {{WM_WINDOWPOSCHANGING}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_WINDOWPOS_Sig<TMyClass>(&TMyClass::EvWindowPosChanging)}

#define EV_WM_WININICHANGE\
  {{WM_WININICHANGE}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::v_S_Sig<TMyClass>(&TMyClass::EvWinIniChange)}

//
//!CQ Win4.0 messages here for now
//
//
#define EV_WM_SETTINGCHANGE\
  {{WM_SETTINGCHANGE}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_U_CS_Sig<TMyClass>(&TMyClass::EvSettingChange)}

#define EV_WM_DISPLAYCHANGE\
  {{WM_DISPLAYCHANGE}, 0, (TAnyDispatcher)::v_U_U_U_Dispatch,\
   (TMyPMF)NS_OWL::v_U_U_U_Sig<TMyClass>(&TMyClass::EvDisplayChange)}

//WM_NOTIFY  // Not dispatched, use virtual

#define EV_WM_INPUTLANGCHANGEREQUEST\
  {{WM_INPUTLANGCHANGEREQUEST}, 0, (TAnyDispatcher)::B_B_U_Dispatch,\
   (TMyPMF)NS_OWL::B_B_HKL_Sig<TMyClass>(&TMyClass::EvInputLangChangeRequest)}

#define EV_WM_INPUTLANGCHANGE\
  {{WM_INPUTLANGCHANGE}, 0, (TAnyDispatcher)::B_U_U_Dispatch,\
  (TMyPMF)NS_OWL::B_I_HKL_Sig<TMyClass>(&TMyClass::EvInputLangChange)}

#define EV_WM_TCARD\
  {{WM_TCARD}, 0, (TAnyDispatcher)::v_U_U32_Dispatch,\
  (TMyPMF)NS_OWL::v_I_I32_Sig<TMyClass>(&TMyClass::EvTCard)}

#define EV_WM_HELP\
  {{WM_HELP}, 0, (TAnyDispatcher)::v_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_HELPINFO_Sig<TMyClass>(&TMyClass::EvHelp)}

#define EV_WM_USERCHANGED\
  {{WM_USERCHANGED}, 0, (TAnyDispatcher)::B_Dispatch,\
  (TMyPMF)NS_OWL::B_Sig<TMyClass>(&TMyClass::EvUserChanged)}

#define EV_WM_CONTEXTMENU\
  {{WM_CONTEXTMENU}, 0, (TAnyDispatcher)::v_U_i_i_Dispatch,\
  (TMyPMF)NS_OWL::v_W_I_I_Sig<TMyClass>(&TMyClass::EvContextMenu)}

#define EV_WM_STYLECHANGING\
  {{WM_STYLECHANGING}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_U_STYLE_Sig<TMyClass>(&TMyClass::EvStyleChanging)}

#define EV_WM_STYLECHANGED\
  {{WM_STYLECHANGED}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_U_STYLE_Sig<TMyClass>(&TMyClass::EvStyleChanged)}

#define EV_WM_GETICON\
  {{WM_GETICON}, 0, (TAnyDispatcher)::U_B_Dispatch,\
  (TMyPMF)NS_OWL::HICON_B_Sig<TMyClass>(&TMyClass::EvGetIcon)}

#define EV_WM_SETICON\
  {{WM_SETICON}, 0, (TAnyDispatcher)::U_B_U_Dispatch,\
  (TMyPMF)NS_OWL::HICON_B_HICON_Sig<TMyClass>(&TMyClass::EvSetIcon)}

#define EV_WM_SIZING\
  {{WM_SIZING}, 0, (TAnyDispatcher)::B_U_RECT_Dispatch,\
  (TMyPMF)NS_OWL::B_U_RECT_Sig<TMyClass>(&TMyClass::EvSizing)}

#define EV_WM_CAPTURECHANGED\
  {{WM_CAPTURECHANGED}, 0, (TAnyDispatcher)::v_U_Dispatch,\
  (TMyPMF)NS_OWL::v_W_Sig<TMyClass>(&TMyClass::EvCaptureChanged)}

#define EV_WM_MOVING\
  {{WM_MOVING}, 0, (TAnyDispatcher)::B_U_RECT_Dispatch,\
  (TMyPMF)NS_OWL::B_U_RECT_Sig<TMyClass>(&TMyClass::EvMoving)}

#define EV_WM_ENTERSIZEMOVE\
  {{WM_ENTERSIZEMOVE}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvEnterSizeMove)}

#define EV_WM_EXITSIZEMOVE\
  {{WM_EXITSIZEMOVE}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvExitSizeMove)}

#define EV_WM_POWERBROADCAST\
  {{WM_POWERBROADCAST}, 0, (TAnyDispatcher)::B_U_U_Dispatch,\
  (TMyPMF)NS_OWL::B_U_U_Sig<TMyClass>(&TMyClass::EvPowerBroadCast)}

#define EV_WM_DEVICECHANGE\
  {{WM_DEVICECHANGE}, 0, (TAnyDispatcher)::B_U_U_Dispatch,\
  (TMyPMF)NS_OWL::B_U_U_Sig<TMyClass>(&TMyClass::EvDeviceChange)}

#if 0
//WM_IME_SETCONTEXT
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_NOTIFY
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_CONTROL
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_COMPOSITIONFULL
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_SELECT
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_CHAR
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_KEYDOWN
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_KEYUP
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//
//WM_IME_STARTCOMPOSITION
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_ENDCOMPOSITION
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_COMPOSITION
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}

//WM_IME_KEYLAST
#define EV_\
  {{xxx}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::Ev)}
#endif

#define EV_WM_ENTERMENULOOP\
  {{WM_ENTERMENULOOP}, 0, (TAnyDispatcher)::v_B_Dispatch,\
  (TMyPMF)NS_OWL::v_B_Sig<TMyClass>(&TMyClass::EvEnterMenuLoop)}

#define EV_WM_EXITMENULOOP\
  {{WM_EXITMENULOOP}, 0, (TAnyDispatcher)::v_B_Dispatch,\
  (TMyPMF)NS_OWL::v_B_Sig<TMyClass>(&TMyClass::EvExitMenuLoop)}

#define EV_WM_NEXTMENU\
  {{WM_NEXTMENU}, 0, (TAnyDispatcher)::v_Dispatch,\
  (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvNextMenu)}

#define EV_WM_PRINT\
  {{WM_PRINT}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_HDC_U_Sig<TMyClass>(&TMyClass::EvPrint)}

#define EV_WM_PRINTCLIENT\
  {{WM_PRINTCLIENT}, 0, (TAnyDispatcher)::v_WPARAM_LPARAM_Dispatch,\
  (TMyPMF)NS_OWL::v_HDC_U_Sig<TMyClass>(&TMyClass::EvPrintClient)}

//WM_HANDHELDFIRST    //!CQ No Doc
//WM_HANDHELDLAST     //!CQ No Doc

//WM_AFXFIRST         //!CQ No Doc
//WM_AFXLAST          //!CQ No Doc

//WM_APP              //!CQ No Doc
//WM_USER             //!CQ make generic wrappers?

//
// Dialog messages
//
#define EV_DM_GETDEFID\
  {{DM_GETDEFID}, 0, (TAnyDispatcher)::U32_Dispatch,\
    (TMyPMF)NS_OWL::U32_Sig<TMyClass>(&TMyClass::EvGetDefId)}

#define EV_DM_SETDEFID\
  {{DM_SETDEFID}, 0, (TAnyDispatcher)::B_WPARAM_Dispatch,\
    (TMyPMF)NS_OWL::B_I_Sig<TMyClass>(&TMyClass::EvSetDefId)}

# define EV_DM_REPOSITION\
   {{DM_REPOSITION}, 0, (TAnyDispatcher)::v_Dispatch,\
    (TMyPMF)NS_OWL::v_Sig<TMyClass>(&TMyClass::EvReposition)}

//
// Control notification macros
//

#define EV_BN_CLICKED(id, method)   EV_CHILD_NOTIFY(id, BN_CLICKED, method)
#define EV_BN_DISABLE(id, method)   EV_CHILD_NOTIFY(id, BN_DISABLE, method)
#define EV_BN_DOUBLECLICKED(id, method)  EV_CHILD_NOTIFY(id, BN_DOUBLECLICKED, method)
#define EV_BN_HILITE(id, method)    EV_CHILD_NOTIFY(id, BN_HILITE, method)
#define EV_BN_PAINT(id, method)     EV_CHILD_NOTIFY(id, BN_PAINT, method)
#define EV_BN_UNHILITE(id, method)  EV_CHILD_NOTIFY(id, BN_UNHILITE, method)
#define EV_BN_KILLFOCUS(id, method) EV_CHILD_NOTIFY(id, BN_KILLFOCUS, method)
#define EV_BN_SETFOCUS(id, method)  EV_CHILD_NOTIFY(id, BN_SETFOCUS, method)

#define EV_CBN_CLOSEUP(id, method)    EV_CHILD_NOTIFY(id, CBN_CLOSEUP, method)
#define EV_CBN_DBLCLK(id, method)     EV_CHILD_NOTIFY(id, CBN_DBLCLK, method)
#define EV_CBN_DROPDOWN(id, method)   EV_CHILD_NOTIFY(id, CBN_DROPDOWN, method)
#define EV_CBN_EDITCHANGE(id, method) EV_CHILD_NOTIFY(id, CBN_EDITCHANGE, method)
#define EV_CBN_EDITUPDATE(id, method) EV_CHILD_NOTIFY(id, CBN_EDITUPDATE, method)
#define EV_CBN_ERRSPACE(id, method)   EV_CHILD_NOTIFY(id, CBN_ERRSPACE, method)
#define EV_CBN_KILLFOCUS(id, method)  EV_CHILD_NOTIFY(id, CBN_KILLFOCUS, method)
#define EV_CBN_SELCHANGE(id, method)  EV_CHILD_NOTIFY(id, CBN_SELCHANGE, method)
#define EV_CBN_SELENDCANCEL(id, method) EV_CHILD_NOTIFY(id, CBN_SELENDCANCEL, method)
#define EV_CBN_SELENDOK(id, method)   EV_CHILD_NOTIFY(id, CBN_SELENDOK, method)
#define EV_CBN_SETFOCUS(id, method)   EV_CHILD_NOTIFY(id, CBN_SETFOCUS, method)

#define EV_EN_CHANGE(id, method)    EV_CHILD_NOTIFY(id, EN_CHANGE, method)
#define EV_EN_ERRSPACE(id, method)  EV_CHILD_NOTIFY(id, EN_ERRSPACE, method)
#define EV_EN_HSCROLL(id, method)   EV_CHILD_NOTIFY(id, EN_HSCROLL, method)
#define EV_EN_KILLFOCUS(id, method) EV_CHILD_NOTIFY(id, EN_KILLFOCUS, method)
#define EV_EN_MAXTEXT(id, method)   EV_CHILD_NOTIFY(id, EN_MAXTEXT, method)
#define EV_EN_SETFOCUS(id, method)  EV_CHILD_NOTIFY(id, EN_SETFOCUS, method)
#define EV_EN_UPDATE(id, method)    EV_CHILD_NOTIFY(id, EN_UPDATE, method)
#define EV_EN_VSCROLL(id, method)   EV_CHILD_NOTIFY(id, EN_VSCROLL, method)

#define EV_LBN_DBLCLK(id, method)    EV_CHILD_NOTIFY(id, LBN_DBLCLK, method)
#define EV_LBN_ERRSPACE(id, method)  EV_CHILD_NOTIFY(id, LBN_ERRSPACE, method)
#define EV_LBN_KILLFOCUS(id, method) EV_CHILD_NOTIFY(id, LBN_KILLFOCUS, method)
#define EV_LBN_SELCANCEL(id, method) EV_CHILD_NOTIFY(id, LBN_SELCANCEL, method)
#define EV_LBN_SELCHANGE(id, method) EV_CHILD_NOTIFY(id, LBN_SELCHANGE, method)
#define EV_LBN_SETFOCUS(id, method)  EV_CHILD_NOTIFY(id, LBN_SETFOCUS, method)

#define EV_SB_LINEDOWN(id, method)   EV_CHILD_NOTIFY(id, SB_LINEDOWN, method)
#define EV_SB_LINEUP(id, method)     EV_CHILD_NOTIFY(id, SB_LINEUP, method)
#define EV_SB_PAGEDOWN(id, method)   EV_CHILD_NOTIFY(id, SB_PAGEDOWN, method)
#define EV_SB_PAGEUP(id, method)     EV_CHILD_NOTIFY(id, SB_PAGEUP, method)
#define EV_SB_TOP(id, method)        EV_CHILD_NOTIFY(id, SB_TOP, method)
#define EV_SB_BOTTOM(id, method)     EV_CHILD_NOTIFY(id, SB_BOTTOM, method)
#define EV_SB_THUMBPOSITION(id, method) EV_CHILD_NOTIFY(id, SB_THUMBPOSITION, method)
#define EV_SB_ENDSCROLL(id, method)  EV_CHILD_NOTIFY(id, SB_ENDSCROLL, method)
#define EV_SB_BEGINTRACK(id, method) EV_CHILD_NOTIFY(id, 9, method)

#define EV_STN_DBLCLK(id, method)    EV_CHILD_NOTIFY(id, STN_DBLCLK, method)
#define EV_STN_CLICKED(id, method)   EV_CHILD_NOTIFY(id, STN_CLICKED, method)
#define EV_STN_DISABLE(id, method)   EV_CHILD_NOTIFY(id, STN_DISABLE, method)
#define EV_STN_ENABLE(id, method)    EV_CHILD_NOTIFY(id, STN_ENABLE, method)


#endif  // OWL_WINDOWEV_H
