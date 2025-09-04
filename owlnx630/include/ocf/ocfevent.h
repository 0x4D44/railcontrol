//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.15.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Definition of OWL signatures for ObjectComponents messages
//----------------------------------------------------------------------------

#if !defined(OCF_OCFEVENT_H)
#define OCF_OCFEVENT_H

#if !defined(OWL_EVENTHAN_H)
# include <owl/eventhan.h>
#endif
#if !defined(OCF_OCAPP_H)
# include <ocf/ocapp.h>
#endif
#if !defined(OCF_OCVIEW_H)
# include <ocf/ocview.h>
#endif
#if !defined(OCF_OCCTRL_H)
# include <ocf/occtrl.h>
#endif


#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OCF_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// Signatures for ObjectComponents messages
//
DECLARE_SIGNATURE1(bool,B_OC_MD_Sig,TOcMenuDescr &)
DECLARE_SIGNATURE1(bool,B_OC_DD_Sig,TOcDragDrop &)
DECLARE_SIGNATURE1(bool,B_OC_VSCROLL_Sig,TOcScrollDir)
DECLARE_SIGNATURE1(bool,B_OC_PCI_Sig,TOcPartChangeInfo&)
DECLARE_SIGNATURE1(bool,B_OC_VP_Sig,TOcViewPaint &)
DECLARE_SIGNATURE1(bool,B_OC_VSP_Sig,TOcSaveLoad &)
DECLARE_SIGNATURE1(bool,B_OC_VCF_Sig,TOcFormatData &)
DECLARE_SIGNATURE1(bool,B_OC_VTB_Sig,TOcToolBarInfo &)
DECLARE_SIGNATURE1(bool,B_OC_VSS_Sig,TOcScaleFactor&)
DECLARE_SIGNATURE1(bool,B_OC_PS_Sig,TOcPartSize &)
DECLARE_SIGNATURE1(bool,B_OC_IT_Sig,TOcItemName&)
DECLARE_SIGNATURE1(bool,B_OC_SL_Sig,TOcLinkView&)
DECLARE_SIGNATURE1(bool,B_OC_PA_Sig,TOcPart&)
DECLARE_SIGNATURE1(bool,B_OC_PO_Sig,TOcInitInfo&)
DECLARE_SIGNATURE1(bool,B_OC_PI32_Sig,int32*)
DECLARE_SIGNATURE1(bool,B_OC_PI16_Sig,int16*)
DECLARE_SIGNATURE1(bool,B_OC_I16_Sig,int16)
DECLARE_SIGNATURE1(bool,B_OC_PB_Sig,bool*)
DECLARE_SIGNATURE1(bool,B_OC_PPTS_Sig,__OWL TString**)
DECLARE_SIGNATURE1(bool,B_OC_PPID_Sig,IDispatch **)
DECLARE_SIGNATURE1(bool,B_OC_PTS_Sig,__OWL TString*)
DECLARE_SIGNATURE1(bool,B_OC_PID_Sig,IDispatch*)
DECLARE_SIGNATURE1(bool,B_U_Sig,uint)

// Signature templates used to validate OCX-events handlers
//
DECLARE_SIGNATURE1(bool,B_OC_PE_Sig,TCtrlEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PME_Sig,TCtrlMouseEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PKE_Sig,TCtrlKeyEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PFE_Sig,TCtrlFocusEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PPE_Sig,TCtrlPropertyEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PCE_Sig,TCtrlCustomEvent*)
DECLARE_SIGNATURE1(bool,B_OC_PEE_Sig,TCtrlErrorEvent*)

__OCF_END_NAMESPACE

//----------------------------------------------------------------------------
// Event response table macros for ObjectConnections events
//

//
// Raw OC compound document family event
//
#define EV_OCEVENT(id, method)\
  {WM_OCEVENT, id, (TAnyDispatcher)::I32_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::U32_VOIDFP_Sig(&TMyClass::method)}

//
// App and app frame window events
//
#define EV_OC_APPINSMENUS\
  {WM_OCEVENT, OC_APPINSMENUS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_MD_Sig(&TMyClass::EvOcAppInsMenus)}

#define EV_OC_APPMENUS\
  {WM_OCEVENT, OC_APPMENUS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_MD_Sig(&TMyClass::EvOcAppMenus)}

#define EV_OC_APPPROCESSMSG\
  {WM_OCEVENT, OC_APPPROCESSMSG, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_MSG_Sig(&TMyClass::EvOcAppProcessMsg)}

#define EV_OC_APPFRAMERECT\
  {WM_OCEVENT, OC_APPFRAMERECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcAppFrameRect)}

#define EV_OC_APPBORDERSPACEREQ\
  {WM_OCEVENT, OC_APPBORDERSPACEREQ, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcAppBorderSpaceReq)}

#define EV_OC_APPBORDERSPACESET\
  {WM_OCEVENT, OC_APPBORDERSPACESET, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcAppBorderSpaceSet)}

#define EV_OC_APPSTATUSTEXT\
  {WM_OCEVENT, OC_APPSTATUSTEXT, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OWL::v_CS_Sig(&TMyClass::EvOcAppStatusText)}

#define EV_OC_APPRESTOREUI\
  {WM_OCEVENT, OC_APPRESTOREUI, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig(&TMyClass::EvOcAppRestoreUI)}

#define EV_OC_APPDIALOGHELP\
  {WM_OCEVENT, OC_APPDIALOGHELP, (TAnyDispatcher)::v_U_Dispatch,\
   (TMyPMF)NS_OCF::v_OC_AH_Sig(&TMyClass::EvOcAppDialogHelp)}

#define EV_OC_APPSHUTDOWN\
  {WM_OCEVENT, OC_APPSHUTDOWN, (TAnyDispatcher)::B_Dispatch,\
   (TMyPMF)NS_OCF::B_Sig(&TMyClass::EvOcAppShutdown)}

//
// View events
//
#define EV_OC_VIEWTITLE\
  {WM_OCEVENT, OC_VIEWTITLE, (TAnyDispatcher)::I32_Dispatch,\
   (TMyPMF)NS_OCF::CS_Sig(&TMyClass::EvOcViewTitle)}

#define EV_OC_VIEWSETTITLE\
  {WM_OCEVENT, OC_VIEWSETTITLE, (TAnyDispatcher)::v_POINTER_Dispatch,\
   (TMyPMF)NS_OCF::v_CS_Sig(&TMyClass::EvOcViewSetTitle)}

#define EV_OC_VIEWBORDERSPACEREQ\
  {WM_OCEVENT, OC_VIEWBORDERSPACEREQ, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcViewBorderSpaceReq)}

#define EV_OC_VIEWBORDERSPACESET\
  {WM_OCEVENT, OC_VIEWBORDERSPACESET, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcViewBorderSpaceSet)}

#define EV_OC_VIEWDROP\
  {WM_OCEVENT, OC_VIEWDROP, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_DD_Sig(&TMyClass::EvOcViewDrop)}

#define EV_OC_VIEWDRAG\
  {WM_OCEVENT, OC_VIEWDRAG, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_DD_Sig(&TMyClass::EvOcViewDrag)}

#define EV_OC_VIEWSCROLL\
  {WM_OCEVENT, OC_VIEWSCROLL, (TAnyDispatcher)::B_I2_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VSCROLL_Sig(&TMyClass::EvOcViewScroll)}

#define EV_OC_VIEWPARTINVALID\
  {WM_OCEVENT, OC_VIEWPARTINVALID, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PCI_Sig(&TMyClass::EvOcViewPartInvalid)}

#define EV_OC_VIEWPAINT\
  {WM_OCEVENT, OC_VIEWPAINT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VP_Sig(&TMyClass::EvOcViewPaint)}

#define EV_OC_VIEWLOADPART\
  {WM_OCEVENT, OC_VIEWLOADPART, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VSP_Sig(&TMyClass::EvOcViewLoadPart)}

#define EV_OC_VIEWSAVEPART\
  {WM_OCEVENT, OC_VIEWSAVEPART, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VSP_Sig(&TMyClass::EvOcViewSavePart)}

#define EV_OC_VIEWCLOSE\
  {WM_OCEVENT, OC_VIEWCLOSE, (TAnyDispatcher)::B_Dispatch,\
   (TMyPMF)NS_OWL::B_Sig(&TMyClass::EvOcViewClose)}

#define EV_OC_VIEWINSMENUS\
  {WM_OCEVENT, OC_VIEWINSMENUS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_MD_Sig(&TMyClass::EvOcViewInsMenus)}

#define EV_OC_VIEWSHOWTOOLS\
  {WM_OCEVENT, OC_VIEWSHOWTOOLS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VTB_Sig(&TMyClass::EvOcViewShowTools)}

#define EV_OC_VIEWGETPALETTE\
  {WM_OCEVENT, OC_VIEWGETPALETTE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_PPALETTE_Sig(&TMyClass::EvOcViewGetPalette)}

#define EV_OC_VIEWCLIPDATA\
  {WM_OCEVENT, OC_VIEWCLIPDATA, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VCF_Sig(&TMyClass::EvOcViewClipData)}

#define EV_OC_VIEWSETDATA\
  {WM_OCEVENT, OC_VIEWSETDATA, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VCF_Sig(&TMyClass::EvOcViewSetData)}

#define EV_OC_VIEWPARTSIZE\
  {WM_OCEVENT, OC_VIEWPARTSIZE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PS_Sig(&TMyClass::EvOcViewPartSize)}

#define EV_OC_VIEWOPENDOC\
  {WM_OCEVENT, OC_VIEWOPENDOC, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_CS_Sig(&TMyClass::EvOcViewOpenDoc)}

#define EV_OC_VIEWATTACHWINDOW\
  {WM_OCEVENT, OC_VIEWATTACHWINDOW, (TAnyDispatcher)::B_B_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcViewAttachWindow)}

#define EV_OC_VIEWSETSCALE\
  {WM_OCEVENT, OC_VIEWSETSCALE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VSS_Sig(&TMyClass::EvOcViewSetScale)}

#define EV_OC_VIEWGETSCALE\
  {WM_OCEVENT, OC_VIEWGETSCALE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_VSS_Sig(&TMyClass::EvOcViewGetScale)}

#define EV_OC_VIEWGETSITERECT\
  {WM_OCEVENT, OC_VIEWGETSITERECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcViewGetSiteRect)}

#define EV_OC_VIEWSETSITERECT\
  {WM_OCEVENT, OC_VIEWSETSITERECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_RECT_Sig(&TMyClass::EvOcViewSetSiteRect)}

#define EV_OC_VIEWGETITEMNAME\
  {WM_OCEVENT, OC_VIEWGETITEMNAME, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_IT_Sig(&TMyClass::EvOcViewGetItemName)}

#define EV_OC_VIEWSETLINK\
  {WM_OCEVENT, OC_VIEWSETLINK, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_SL_Sig(&TMyClass::EvOcViewSetLink)}

#define EV_OC_VIEWBREAKLINK\
  {WM_OCEVENT, OC_VIEWBREAKLINK, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_SL_Sig(&TMyClass::EvOcViewBreakLink)}

#define EV_OC_VIEWPARTACTIVATE\
  {WM_OCEVENT, OC_VIEWPARTACTIVATE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PA_Sig(&TMyClass::EvOcViewPartActivate)}

#define EV_OC_VIEWPASTEOBJECT\
  {WM_OCEVENT, OC_VIEWPASTEOBJECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PO_Sig(&TMyClass::EvOcViewPasteObject)}

#define EV_OC_VIEWDOVERB\
  {WM_OCEVENT, OC_VIEWDOVERB, (TAnyDispatcher)::B_WPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_U_Sig(&TMyClass::EvOcViewDoVerb)}


// OCX Container event/dispatch macros
//

// Standard Ambient properties - Get support
//
#define EV_OC_AMBIENT_GETBACKCOLOR\
  {WM_OCEVENT, OC_AMBIENT_GETBACKCOLOR, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PI32_Sig(&TMyClass::EvOcAmbientGetBackColor)}

#define EV_OC_AMBIENT_GETFORECOLOR\
  {WM_OCEVENT, OC_AMBIENT_GETFORECOLOR, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PI32_Sig(&TMyClass::EvOcAmbientGetForeColor)}

#define EV_OC_AMBIENT_GETLOCALEID\
  {WM_OCEVENT, OC_AMBIENT_GETLOCALEID, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PI32_Sig(&TMyClass::EvOcAmbientGetLocaleID)}

#define EV_OC_AMBIENT_GETTEXTALIGN\
  {WM_OCEVENT, OC_AMBIENT_GETTEXTALIGN, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PI16_Sig(&TMyClass::EvOcAmbientGetTextAlign)}

#define EV_OC_AMBIENT_GETMESSAGEREFLECT\
  {WM_OCEVENT, OC_AMBIENT_GETMESSAGEREFLECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetMessageReflect)}

#define EV_OC_AMBIENT_GETUSERMODE\
  {WM_OCEVENT, OC_AMBIENT_GETUSERMODE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetUserMode)}

#define EV_OC_AMBIENT_GETUIDEAD\
  {WM_OCEVENT, OC_AMBIENT_GETUIDEAD, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetUIDead)}

#define EV_OC_AMBIENT_GETSHOWGRABHANDLES\
  {WM_OCEVENT, OC_AMBIENT_GETSHOWGRABHANDLES, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetShowGrabHandles)}

#define EV_OC_AMBIENT_GETSHOWHATCHING\
  {WM_OCEVENT, OC_AMBIENT_GETSHOWHATCHING, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetShowHatching)}

#define EV_OC_AMBIENT_GETSUPPORTSMNEMONICS\
  {WM_OCEVENT, OC_AMBIENT_GETSUPPORTSMNEMONICS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetSupportsMnemonics)}

#define EV_OC_AMBIENT_GETDISPLAYASDEFAULT\
  {WM_OCEVENT, OC_AMBIENT_GETDISPLAYASDEFAULT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PB_Sig(&TMyClass::EvOcAmbientGetDisplayAsDefault)}

#define EV_OC_AMBIENT_GETDISPLAYNAME\
  {WM_OCEVENT, OC_AMBIENT_GETDISPLAYNAME, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PPTS_Sig(&TMyClass::EvOcAmbientGetDisplayName)}

#define EV_OC_AMBIENT_GETSCALEUNITS\
  {WM_OCEVENT, OC_AMBIENT_GETSCALEUNITS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PPTS_Sig(&TMyClass::EvOcAmbientGetScaleUnits)}

#define EV_OC_AMBIENT_GETFONT\
  {WM_OCEVENT, OC_AMBIENT_GETFONT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PPID_Sig(&TMyClass::EvOcAmbientGetFont)}


// Standard Ambient properties - Set support
//
#define EV_OC_AMBIENT_SETBACKCOLOR\
  {WM_OCEVENT, OC_AMBIENT_SETBACKCOLOR, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_I32_Sig(&TMyClass::EvOcAmbientSetBackColor)}

#define EV_OC_AMBIENT_SETFORECOLOR\
  {WM_OCEVENT, OC_AMBIENT_SETFORECOLOR, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_I32_Sig(&TMyClass::EvOcAmbientSetForeColor)}

#define EV_OC_AMBIENT_SETLOCALEID\
  {WM_OCEVENT, OC_AMBIENT_SETLOCALEID, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_I32_Sig(&TMyClass::EvOcAmbientSetLocaleID)}

#define EV_OC_AMBIENT_SETTEXTALIGN\
  {WM_OCEVENT, OC_AMBIENT_SETTEXTALIGN, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_I16_Sig(&TMyClass::EvOcAmbientSetTextAlign)}

#define EV_OC_AMBIENT_SETMESSAGEREFLECT\
  {WM_OCEVENT, OC_AMBIENT_SETMESSAGEREFLECT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetMessageReflect)}

#define EV_OC_AMBIENT_SETUSERMODE\
  {WM_OCEVENT, OC_AMBIENT_SETUSERMODE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetUserMode)}

#define EV_OC_AMBIENT_SETUIDEAD\
  {WM_OCEVENT, OC_AMBIENT_SETUIDEAD, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetUIDead)}

#define EV_OC_AMBIENT_SETSHOWGRABHANDLES\
  {WM_OCEVENT, OC_AMBIENT_SETSHOWGRABHANDLES, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetShowGrabHandles)}

#define EV_OC_AMBIENT_SETSHOWHATCHING\
  {WM_OCEVENT, OC_AMBIENT_SETSHOWHATCHING, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetShowHatching)}

#define EV_OC_AMBIENT_SETDISPLAYASDEFAULT\
  {WM_OCEVENT, OC_AMBIENT_SETDISPLAYASDEFAULT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetDisplayAsDefault)}

#define EV_OC_AMBIENT_SETSUPPORTSMNEMONICS\
  {WM_OCEVENT, OC_AMBIENT_SETSUPPORTSMNEMONICS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OWL::B_B_Sig(&TMyClass::EvOcAmbientSetSupportsMnemonics)}

#define EV_OC_AMBIENT_SETDISPLAYNAME\
  {WM_OCEVENT, OC_AMBIENT_SETDISPLAYNAME, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PTS_Sig(&TMyClass::EvOcAmbientSetDisplayName)}

#define EV_OC_AMBIENT_SETSCALEUNITS\
  {WM_OCEVENT, OC_AMBIENT_SETSCALEUNITS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PTS_Sig(&TMyClass::EvOcAmbientSetScaleUnits)}

#define EV_OC_AMBIENT_SETFONT\
  {WM_OCEVENT, OC_AMBIENT_SETFONT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PID_Sig(&TMyClass::EvOcAmbientSetFont)}


// Standard Ctrl Events
//
#define EV_OC_CTRLEVENT_CLICK\
  {WM_OCEVENT, OC_CTRLEVENT_CLICK, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PE_Sig(&TMyClass::EvOcCtrlClick)}

#define EV_OC_CTRLEVENT_DBLCLICK\
  {WM_OCEVENT, OC_CTRLEVENT_DBLCLICK, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PE_Sig(&TMyClass::EvOcCtrlDblClick)}

#define EV_OC_CTRLEVENT_MOUSEDOWN\
  {WM_OCEVENT, OC_CTRLEVENT_MOUSEDOWN, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PME_Sig(&TMyClass::EvOcCtrlMouseDown)}

#define EV_OC_CTRLEVENT_MOUSEUP\
  {WM_OCEVENT, OC_CTRLEVENT_MOUSEUP, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PME_Sig(&TMyClass::EvOcCtrlMouseUp)}

#define EV_OC_CTRLEVENT_MOUSEMOVE\
  {WM_OCEVENT, OC_CTRLEVENT_MOUSEMOVE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PME_Sig(&TMyClass::EvOcCtrlMouseMove)}

#define EV_OC_CTRLEVENT_KEYDOWN\
  {WM_OCEVENT, OC_CTRLEVENT_KEYDOWN, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PKE_Sig(&TMyClass::EvOcCtrlKeyDown)}

#define EV_OC_CTRLEVENT_KEYUP\
  {WM_OCEVENT, OC_CTRLEVENT_KEYUP, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PKE_Sig(&TMyClass::EvOcCtrlKeyUp)}

#define EV_OC_CTRLEVENT_ERROREVENT\
  {WM_OCEVENT, OC_CTRLEVENT_ERROREVENT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PEE_Sig(&TMyClass::EvOcCtrlErrorEvent)}

#define EV_OC_CTRLEVENT_FOCUS\
  {WM_OCEVENT, OC_CTRLEVENT_FOCUS, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PFE_Sig(&TMyClass::EvOcCtrlFocus)}

#define EV_OC_CTRLEVENT_PROPERTYCHANGE\
  {WM_OCEVENT, OC_CTRLEVENT_PROPERTYCHANGE, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PPE_Sig(&TMyClass::EvOcCtrlPropertyChange)}

#define EV_OC_CTRLEVENT_PROPERTYREQUESTEDIT\
  {WM_OCEVENT, OC_CTRLEVENT_PROPERTYREQUESTEDIT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PPE_Sig(&TMyClass::EvOcCtrlPropertyRequestEdit)}

#define EV_OC_CTRLEVENT_CUSTOMEVENT\
  {WM_OCEVENT, OC_CTRLEVENT_CUSTOMEVENT, (TAnyDispatcher)::B_LPARAM_Dispatch,\
   (TMyPMF)NS_OCF::B_OC_PCE_Sig(&TMyClass::EvOcCtrlCustomEvent)}


#endif    //  OCF_OCFEVENT_H
