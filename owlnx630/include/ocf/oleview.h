//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.14 $
//$Author: jogybl $
//$Date: 2007-09-15 11:43:47 $
//
//----------------------------------------------------------------------------

#if !defined(OCF_OLEVIEW_H)
#define OCF_OLEVIEW_H

#if !defined(OWL_DOCVIEW_H)
# include <owl/docview.h>
#endif
#if !defined(OWL_DOCMANAG_H)
# include <owl/docmanag.h>
#endif

#if !defined(OCF_OLEWINDO_H)
# include <ocf/olewindo.h>
#endif

#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OCF_BEGIN_NAMESPACE

//
// Additional notification messages
//
const int vnInvalidate  = 9;   // a rect in view needs repaint
const int vnLinkView    = 10;  // find view with matched TOcLinkView
const int vnLinkMoniker = 11;  // find view with matched moniker

NOTIFY_SIG(vnInvalidate, __OWL TRect&)
NOTIFY_SIG(vnLinkView, TOcLinkView&)
NOTIFY_SIG(vnLinkMoniker, __OWL TString&)

#define EV_VN_INVALIDATE  VN_DEFINE(vnInvalidate,  VnInvalidate,  long)
#define EV_VN_LINKVIEW    VN_DEFINE(vnLinkView,    VnLinkView,    long)
#define EV_VN_LINKMONIKER VN_DEFINE(vnLinkMoniker, VnLinkMoniker, long)

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TOleView
// ~~~~~ ~~~~~~~~
// The ole 2 window view class. Used as a view in doc/view model
//
class _OCFCLASS TOleView : public TOleWindow, public __OWL TView {
  public:
    TOleView(__OWL TDocument& doc, __OWL TWindow* parent = 0);
   ~TOleView();

    static LPCTSTR StaticName();

    // Inherited virtuals from TView
    //
    LPCTSTR  GetViewName();
    __OWL TWindow* GetWindow();
    bool     SetDocTitle(LPCTSTR docname, int index);
    bool     OleShutDown();

    // Inherited virtuals from TWindow
    //
    bool 		 CanClose();

  protected:
    TOcView* CreateOcView(__OWL TRegLink* link, bool isEmbedded, IUnknown* outer);
    void CleanupWindow();
    bool OtherViewExists();

    // View notifications
    //
    bool VnInvalidate(__OWL TRect& rect);
    bool VnDocOpened(int omode);
    bool VnDocClosed(int omode);

    __OWL TMenuDescr* GetViewMenu();

    LPCTSTR GetFileName();

    bool EvOcViewPartInvalid(TOcPartChangeInfo& changeInfo);
//    uint32 EvOcViewDiscardUndo(void * undo);

    // Container specific messages
    //
//    bool   EvOcAppInsMenus(TOcMenuDescr &);

    // Server specific messages
    //
    bool   EvOcViewClose();
    bool   EvOcViewSavePart(TOcSaveLoad & ocSave);
    bool   EvOcViewLoadPart(TOcSaveLoad & ocLoad);
    bool   EvOcViewOpenDoc(LPCTSTR path);
    bool   EvOcViewInsMenus(TOcMenuDescr & sharedMenu);
    bool   EvOcViewAttachWindow(bool attach);
    bool   EvOcViewSetLink(TOcLinkView& view);
    bool   EvOcViewBreakLink(TOcLinkView& view);
    bool   EvOcViewGetItemName(TOcItemName& item);

  private:
    bool   Destroying;

    // event handlers
    //
    bool     VnIsWindow(HWND hWnd);

  DECLARE_RESPONSE_TABLE(TOleView);
  DECLARE_STREAMABLE_OCF(NS_OCF::TOleView,1);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OCF::TOleView );

//
// class TOleLinkView
// ~~~~~ ~~~~~~~~~~~~
class _OCFCLASS TOleLinkView : public __OWL TView {
  public:
    TOleLinkView(__OWL TDocument& doc, TOcLinkView& view);
   ~TOleLinkView();

    virtual bool VnLinkView(TOcLinkView& view);
    virtual bool VnLinkMoniker(__OWL TString& moniker);
    virtual bool UpdateLinks();
				__OWL TString& GetMoniker();

    static LPCTSTR StaticName();
    LPCTSTR GetViewName();

  protected:
    TOcLinkView&  OcLinkView;     // TOcLinkView partner for this view

  DECLARE_RESPONSE_TABLE(TOleLinkView);
  //DECLARE_STREAMABLE_OCF(NS_OCF::TOleLinkView,1);
};

// define streameable inlines (VC)
//DECLARE_STREAMABLE_INLINES( NS_OCF::TOleLinkView );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// --------------------------------------------------------------------------
// Inline implementation
//

//
inline LPCTSTR TOleView::StaticName() {
  return _T("Ole View");
}

//
inline LPCTSTR TOleView::GetViewName() {
  return StaticName();
}

//
inline __OWL TWindow* TOleView::GetWindow() {
  return (__OWL TWindow*)this;
}

//
inline bool TOleView::SetDocTitle(LPCTSTR docname, int index) {
#  if defined(BI_COMP_BORLANDC)
  return __OWL TWindow::SetDocTitle(docname, index);
#else
  return TWindow::SetDocTitle(docname, index);
#endif
}

//
inline LPCTSTR TOleView::GetFileName() {
  return GetDocument().GetTitle();
}

//
inline LPCTSTR TOleLinkView::StaticName() {
  return _T("Link View");
}

//
inline LPCTSTR TOleLinkView::GetViewName() {
  return StaticName();
}

__OCF_END_NAMESPACE

#endif  // OWL_OLEVIEW_H
