//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TScrollBar.  This defines the basic behavior of all
// scrollbar controls.
//----------------------------------------------------------------------------

#if !defined(OWL_SCROLLBAR_H)
#define OWL_SCROLLBAR_H

#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif
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
// class TScrollBar
// ~~~~~ ~~~~~~~~~~
class _OWLCLASS TScrollBar : public TControl {
  public:
    TScrollBar(TWindow*        parent,
               int             id,
               int x, int y, int w, int h,
               bool            isHScrollBar,
               TModule*        module = 0);

    TScrollBar(TWindow* parent, int resourceId, TModule* module = 0);

    int    GetLineMagnitude() const;
    void   SetLineMagnitude(int linemagnitude);

    int    GetPageMagnitude() const;
    void   SetPageMagnitude(int pagemagnitude);

    // Virtual functions to interface to scrollbars & derived
    //
    virtual void  GetRange(int& min, int& max) const;
    virtual int   GetPosition() const;
    virtual void  SetRange(int min, int max, bool redraw = true);
    virtual void  SetPosition(int thumbPos, bool redraw = true);
    virtual int   DeltaPos(int delta);
    virtual void  GetScrollInfo(SCROLLINFO* info) const;
    virtual void  SetScrollInfo(SCROLLINFO* info, bool redraw = true);

    // Override TWindow virtual member functions
    //
    uint          Transfer(void* buffer, TTransferDirection direction);

    // Called by EvHScroll and EvVScroll response table handlers
    //
    // These routines all end up calling SetPosition()
    //
    virtual void  SBLineUp();
    virtual void  SBLineDown();
    virtual void  SBPageUp();
    virtual void  SBPageDown();
    virtual void  SBThumbPosition(int thumbPos);
    virtual void  SBThumbTrack(int thumbPos);
    virtual void  SBTop();
    virtual void  SBBottom();
    virtual void  SBEndScroll();

    // Response table handlers that call above virtual functions in
    // response to messages sent by to us by TWindow::DispatchScroll()
    //
    void          EvHScroll(uint scrollCode, uint thumbPos, HWND hWndCtl);
    void          EvVScroll(uint scrollCode, uint thumbPos, HWND hWndCtl);

  protected:
    // Override TWindow virtual member functions
    //
    LPCTSTR       GetClassName();
    void          SetupWindow();

  public_data:
    int  LineMagnitude;
    int  PageMagnitude;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TScrollBar(const TScrollBar&);
    TScrollBar& operator=(const TScrollBar&);

  DECLARE_RESPONSE_TABLE(TScrollBar);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TScrollBar, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TScrollBar, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TScrollBar );

//
// scroll bar notification macros. methods are: void method()
//
// EV_SB_LINEDOWN(id, method)
// EV_SB_LINEUP(id, method)
// EV_SB_PAGEDOWN(id, method)
// EV_SB_PAGEUP(id, method)
// EV_SB_TOP(id, method)
// EV_SB_BOTTOM(id, method)
// EV_SB_THUMBPOSITION(id, method)
// EV_SB_ENDSCROLL(id, method)
// EV_SB_BEGINTRACK(id, method)

//
// struct TScrollBarData
// ~~~~~~ ~~~~~~~~~~~~~~
//  TScrollBar transfer structure
//
struct TScrollBarData {
  int  LowValue;
  int  HighValue;
  int  Position;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementation
//

//
// Return the current delta to move the thumb when line up/line down is
// received.
//
inline int TScrollBar::GetLineMagnitude() const
{
  return LineMagnitude;
}

//
// Set the delta to move the thumb when line up/line down is received.
//
inline void TScrollBar::SetLineMagnitude(int linemagnitude)
{
  LineMagnitude = linemagnitude;
}


__OWL_END_NAMESPACE


#endif  // OWL_SCROLLBAR_H
