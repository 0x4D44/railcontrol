//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.16 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// TOleWindow - Class encapsulating a window which can be an OLE container
//              or server window.
//----------------------------------------------------------------------------

#if !defined(OCF_OLEWINDO_H)
#define OCF_OLEWINDO_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif
#if !defined(OWL_UIHELPER_H)
# include <owl/uihelper.h>
#endif

#if !defined(OCF_OCFEVENT_H)
# include <ocf/ocfevent.h>
#endif
#if !defined(OCF_OCREMVIE_H)
# include <ocf/ocremvie.h>
#endif
#if !defined(OCF_OCPART_H)
# include <ocf/ocpart.h>
#endif
#if !defined(OCF_OCLINK_H)
# include <ocf/oclink.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_DECLCLASS(TPopupMenu);

__OCF_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>


class _ICLASS   TOcControl;

//
// class TOleWindow
// ~~~~~ ~~~~~~~~~~
// The generic OLE2 window. Use as a client of a frame window.
//
class _OCFCLASS TOleWindow : virtual public __OWL TWindow {
  public:
    TOleWindow(__OWL TWindow* parent = 0, __OWL TModule* module = 0);
   ~TOleWindow();

    // Accessors
    //
    TOcDocument*      GetOcDoc();
    TOcView*          GetOcView();
    TOcRemView*       GetOcRemView();
    TOcApp*           GetOcApp();
    bool              HasActivePart();
    bool              SelectEmbedded();

    // Query about current state
    //
    bool              IsOpenEditing() const;
    bool              IsRemote() const;

    // Virtuals that derived class can override
    //
    virtual bool      OleShutDown();
    virtual void      SetupDC(__OWL TDC& dc, bool scale = true);
    virtual TOcView*  CreateOcView(__OWL TRegLink* link, bool isRemote, IUnknown* outer);

    // Helper routines to create new objects or controls [Container specific]
    //
    TOcPart*          InsertObject(TOcInitInfo& initInfo, __OWL TRect* pos = 0);
    TOcPart*          InsertObject(CLSID& objIID, __OWL TRect* pos = 0);
    TOcPart*          InsertObject(__OWL TString& objProgId, __OWL TRect* pos = 0);

    TOcControl*       InsertControl(TOcInitInfo& initInfo, __OWL TRect* pos = 0, int id = 0);
    TOcControl*       InsertControl(CLSID& ocxIID, __OWL TRect* pos = 0, int id = 0);
    TOcControl*       InsertControl(const __OWL TString& ocxProgID, __OWL TRect* pos = 0, int id = 0);

    // Helper routines to retrieve information about parts (embeddings/ocxes)
    //
    TOcControl*       GetOcControlOfOCX(CLSID ocxIID, uint id = 0);

  protected:
    // Overridables
    //
    virtual void      GetInsertPosition(__OWL TRect& rect);
    virtual void      InvalidatePart(TOcInvalidate invalid); // server only
    virtual bool      Deactivate();
    virtual bool      Select(uint modKeys, __OWL TPoint& point);
    virtual bool      PaintSelection(__OWL TDC& dc, bool erase, __OWL TRect& rect,
                                     void* userData = 0);
    virtual bool      PaintParts(__OWL TDC& dc, bool erase, __OWL TRect& rect, bool metafile);
    virtual bool      PaintLink(__OWL TDC& dc, bool erase, __OWL TRect& rect,
                                __OWL TString& moniker);
    virtual void      GetLogPerUnit(__OWL TSize& logPerUnit);
    virtual void      SetScale(uint16 percent);
    virtual void      SetSelection(TOcPart* part);
    virtual bool      ShowCursor(HWND wnd, uint hitTest, uint mouseMsg);
    virtual bool      StartDrag(uint modKeys, __OWL TPoint& point);
    virtual LPCTSTR		GetFileName();

  protected:
    // Override virtual functions defined by __OWL TWindow
    //
    void              SetupWindow();
    void              CleanupWindow();
    bool              CanClose();

    bool              InClient(__OWL TDC& dc, __OWL TPoint& point);

    __OWL TResult           EvCommand(uint id, HWND hWndCtl, uint notifyCode);
    void              EvCommandEnable(__OWL TCommandEnabler& commandEnabler);
    void              EvPaint();
    void              EvSize(uint sizeType, __OWL TSize& size);
    void              EvMDIActivate(HWND hWndActivated, HWND hWndDeactivated);
    uint              EvMouseActivate(HWND topParent, uint hitCode, uint msg);
    void              EvSetFocus(HWND hWndLostFocus);

    virtual bool      EvOcViewPartInvalid(TOcPartChangeInfo& changeInfo);

    void              CeFileClose(__OWL TCommandEnabler& ce);
    void              CmEditDelete();
    void              CmEditCut();
    void              CmEditCopy();
    void              CmEditPaste();
    void              CmEditPasteSpecial();
    void              CmEditPasteLink();
    void              CmEditLinks();
    void              CmEditConvert();
    void              CmEditInsertObject();
    void              CmEditInsertControl();
    void              CmEditShowObjects();

    void              CeEditDelete(__OWL TCommandEnabler& ce);
    void              CeEditCut(__OWL TCommandEnabler& ce);
    void              CeEditCopy(__OWL TCommandEnabler& ce);
    void              CeEditPaste(__OWL TCommandEnabler& ce);
    void              CeEditPasteSpecial(__OWL TCommandEnabler& ce);
    void              CeEditPasteLink(__OWL TCommandEnabler& ce);
    void              CeEditLinks(__OWL TCommandEnabler& ce);
    void              CeEditObject(__OWL TCommandEnabler& ce);
    void              CeEditConvert(__OWL TCommandEnabler& ce);
    void              CeEditVerbs(__OWL TCommandEnabler& ce);
    void              CeEditInsertObject(__OWL TCommandEnabler& ce);
    void              CeEditInsertControl(__OWL TCommandEnabler& ce);
    void              CeEditShowObjects(__OWL TCommandEnabler& ce);

    void              EvMouseMove(uint modKeys, __OWL TPoint& point);
    void              EvRButtonDown(uint modKeys, __OWL TPoint& point);
    void              EvLButtonUp(uint modKeys, __OWL TPoint& point);
    void              EvLButtonDown(uint modKeys, __OWL TPoint& point);
    void              EvLButtonDblClk(uint modKeys, __OWL TPoint& point);
    bool              EvSetCursor(HWND hWndCursor, uint hitTest, uint mouseMsg);
    void              EvDropFiles(__OWL TDropInfo dropInfo);
    void              EvHScroll(uint scrollCode, uint thumbPos, HWND hWndCtl);
    void              EvVScroll(uint scrollCode, uint thumbPos, HWND hWndCtl);
    void              EvMenuSelect(uint menuItemId, uint flags, HMENU hMenu);

    __OWL TResult           EvOcEvent(__OWL TParam1 param1, __OWL TParam2 param2);
//! uint32  EvOcViewDiscardUndo(void * undo);

    // Container specific messages
    //
    LPCTSTR						EvOcViewTitle();
    void              EvOcViewSetTitle(LPCTSTR title);

    bool              EvOcViewBorderSpaceReq(__OWL TRect * rect);
    bool              EvOcViewBorderSpaceSet(__OWL TRect * rect);
    bool              EvOcViewDrop(TOcDragDrop & ddInfo);
    bool              EvOcViewDrag(TOcDragDrop & ddInfo);
    bool              EvOcViewScroll(TOcScrollDir scrollDir);
    bool              EvOcViewGetScale(TOcScaleFactor& scaleFactor);
    bool              EvOcViewGetSiteRect(__OWL TRect * rect);
    bool              EvOcViewSetSiteRect(__OWL TRect * rect);
    bool              EvOcViewPartActivate(TOcPart& ocPart);
    bool              EvOcViewPasteObject(TOcInitInfo& init);

    // Server specific messages
    //
    bool              EvOcPartInvalid(TOcPart & /*part*/);
    bool              EvOcViewSavePart(TOcSaveLoad & ocSave);
    bool              EvOcViewLoadPart(TOcSaveLoad & ocLoad);
    bool              EvOcViewPaint(TOcViewPaint & vp);
    bool              EvOcViewInsMenus(TOcMenuDescr & sharedMenu);
    bool              EvOcViewShowTools(TOcToolBarInfo & tbi);
    bool              EvOcViewGetPalette(LOGPALETTE * * palette);
    bool              EvOcViewClipData(TOcFormatData & format);
    bool              EvOcViewSetData(TOcFormatData & format);
    bool              EvOcViewClose();
    bool              EvOcViewPartSize(TOcPartSize & size);
    bool              EvOcViewOpenDoc(LPCTSTR path);
    bool              EvOcViewAttachWindow(bool attach);
    bool              EvOcViewSetScale(TOcScaleFactor& scaleFactor);
    bool              EvOcViewGetItemName(TOcItemName& item);
    bool              EvOcViewSetLink(TOcLinkView& view);
    bool              EvOcViewBreakLink(TOcLinkView& view);
    bool              EvOcViewDoVerb(uint verb);

    virtual bool      EvOcViewTransformCoords(uint verb);

    // Ambient property specific messages
    //
    virtual bool      EvOcAmbientGetBackColor(long* rgb);
    virtual bool      EvOcAmbientGetForeColor(long* rgb);
    virtual bool      EvOcAmbientGetLocaleID(long* locale);
    virtual bool      EvOcAmbientGetTextAlign(short* align);
    virtual bool      EvOcAmbientGetMessageReflect(bool* msgReflect);
    virtual bool      EvOcAmbientGetUserMode(bool* mode);
    virtual bool      EvOcAmbientGetUIDead(bool* dead);
    virtual bool      EvOcAmbientGetShowGrabHandles(bool* show);
    virtual bool      EvOcAmbientGetShowHatching(bool* show);
    virtual bool      EvOcAmbientGetDisplayAsDefault(bool* disp);
    virtual bool      EvOcAmbientGetSupportsMnemonics(bool* support);
    virtual bool      EvOcAmbientGetDisplayName(__OWL TString** name);
    virtual bool      EvOcAmbientGetScaleUnits(__OWL TString** units);
    virtual bool      EvOcAmbientGetFont(IDispatch** font);

    virtual bool      EvOcAmbientSetBackColor(long rgb);
    virtual bool      EvOcAmbientSetForeColor(long rgb);
    virtual bool      EvOcAmbientSetLocaleID(long locale);
    virtual bool      EvOcAmbientSetTextAlign(short align);
    virtual bool      EvOcAmbientSetMessageReflect(bool msgReflect);
    virtual bool      EvOcAmbientSetUserMode(bool mode);
    virtual bool      EvOcAmbientSetUIDead(bool dead);
    virtual bool      EvOcAmbientSetShowGrabHandles(bool show);
    virtual bool      EvOcAmbientSetShowHatching(bool show);
    virtual bool      EvOcAmbientSetDisplayAsDefault(bool disp);
    virtual bool      EvOcAmbientSetSupportsMnemonics(bool support);
    virtual bool      EvOcAmbientSetDisplayName(__OWL TString* name);
    virtual bool      EvOcAmbientSetScaleUnits(__OWL TString* units);
    virtual bool      EvOcAmbientSetFont(IDispatch* font);

    // Ctrl event specific messages
    //
    virtual bool      EvOcCtrlClick(TCtrlEvent* pev);
    virtual bool      EvOcCtrlDblClick(TCtrlEvent* pev);
    virtual bool      EvOcCtrlMouseDown(TCtrlMouseEvent* pev);
    virtual bool      EvOcCtrlMouseMove(TCtrlMouseEvent* pev);
    virtual bool      EvOcCtrlMouseUp(TCtrlMouseEvent* pev);
    virtual bool      EvOcCtrlKeyDown(TCtrlKeyEvent* pev);
    virtual bool      EvOcCtrlKeyUp(TCtrlKeyEvent* pev);
    virtual bool      EvOcCtrlErrorEvent(TCtrlErrorEvent* pev);
    virtual bool      EvOcCtrlFocus(TCtrlFocusEvent* pev);
    virtual bool      EvOcCtrlPropertyChange(TCtrlPropertyEvent* pev);
    virtual bool      EvOcCtrlPropertyRequestEdit(TCtrlPropertyEvent* pev);
    virtual bool      EvOcCtrlCustomEvent(TCtrlCustomEvent* pev);

    __OWL TPopupMenu*				CreateVerbPopup(const TOcVerb& ocVerb);
    void              EvDoVerb(uint whichVerb);
    void              Init();

  protected:
    TOcPart*          DragPart;
    __OWL TUIHandle::TWhere DragHit;
    __OWL TDC*							DragDC;
    __OWL TPoint						DragPt;
    __OWL TRect							DragRect;
    __OWL TPoint						DragStart;
    TOcScaleFactor    Scale;              // current scaling factor
    __OWL TRect							Pos;                // current position

    TOcDocument*      OcDoc;              // associated OCF document partner
    TOcView*          OcView;             // associated OCF view partner
    TOcApp*           OcApp;              // associated OCF app partner
		owl_string        ContainerName;      // Name for this container

    bool              Remote;             // Using a remote OC view?
    bool              ShowObjects;        // Show embedded part frames?

    int               MinWidth;           // minimum width of part
    int               MinHeight;          // minimum height of part

  DECLARE_RESPONSE_TABLE(TOleWindow);
  DECLARE_STREAMABLE_OCF(NS_OCF::TOleWindow,1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OCF::TOleWindow );

//
// class TOleClientDC
// ~~~~~ ~~~~~~~~~~~~
// The TOleClientDC is a client dc with window origin, ext, and viewport
// origin, ext, setup to reflect the current scrolling and scaling information.
//
class _OCFCLASS TOleClientDC : public __OWL TClientDC {
  public:
    TOleClientDC(TOleWindow& win, bool scale = true);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// --------------------------------------------------------------------------
// Inline implementation
//

//
// Return pointer to TOcDocument helper object associated with this window
//
inline TOcDocument* TOleWindow::GetOcDoc() {
  return OcDoc;
}

//
// Return pointer to the 'TOcView' helper object associated with this window
//
inline TOcView* TOleWindow::GetOcView() {
  return OcView;
}

//
// Return pointer to 'TOcRemView' object associated with this window.
// NOTE: Returns 0 in case of failure. For example, a TOleWindow of a
//       container application does not have an associated 'TOcRemView'
//       and will return 0.
//
inline TOcRemView* TOleWindow::GetOcRemView() {
  return TYPESAFE_DOWNCAST(OcView, TOcRemView);
}

//
inline TOcApp* TOleWindow::GetOcApp() {
  return OcApp;
}

//
inline bool TOleWindow::SelectEmbedded() {
  return DragPart != 0;
}

//
inline bool TOleWindow::IsRemote() const {
  return Remote;
}

//
inline bool TOleWindow::PaintSelection(__OWL TDC& /*dc*/, bool /*erase*/,
                                       __OWL TRect& /*rect*/, void* /*userData*/) {
  return false;
}

//
inline bool TOleWindow::PaintLink(__OWL TDC& /*dc*/, bool /*erase*/,
                                  __OWL TRect& /*rect*/, __OWL TString& /*moniker*/) {
  return false;
}

//
inline bool TOleWindow::EvOcPartInvalid(TOcPart & /*part*/) {
  return false;
}

// on linking must be implemented and return the file name
inline LPCTSTR TOleWindow::GetFileName() {
  return NULL;
}

__OCF_END_NAMESPACE

#endif
