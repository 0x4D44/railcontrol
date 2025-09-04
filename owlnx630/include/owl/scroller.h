//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26.4.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// Definition of class TScroller.
//----------------------------------------------------------------------------

#if !defined(OWL_SCROLLER_H)
#define OWL_SCROLLER_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(OWL_OBJSTRM_H)
# include <owl/objstrm.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

#if defined(BI_DOUBLE_SCROLLER)
# include <owl/doublescroller.h>
#else

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

#if !defined(OWL_GEOMETRY_H)
 class _OWLCLASS TRect;
#endif
class _OWLCLASS TDC;

inline long LongMulDiv(long mul1, long mul2, long div1)
                       {return (mul1*mul2) / div1;}

//
// class TScroller
// ~~~~~ ~~~~~~~~~
// Class TScroller implements the actual scroller object. All functions
// are inline or virtual to avoid pulling in code when no scrollers have
// been constructed.
//
class _OWLCLASS TScroller : public TStreamableBase {
  public:
    TScroller(TWindow* window,
              int      xUnit,  int  yUnit,
              long     xRange, long yRange);
    virtual ~TScroller();

    virtual void  SetWindow(TWindow* win);
    virtual void  SetUnits(int xUnit, int yUnit);
    virtual void  SetPageSize();
    virtual void  SetSBarRange();
    virtual void  SetRange(long xRange, long yRange);
    virtual void  SetTotalRangeOfUnits(long xTotalUnits, long yTotalUnits);

    virtual void  BeginView(TDC& dc, TRect& rect);
    virtual void  EndView();
    virtual void  VScroll(uint scrollEvent, int thumbPos);
    virtual void  HScroll(uint scrollEvent, int thumbPos);
    virtual void  ScrollTo(long x, long y);

    virtual bool   EnableScrollBar(uint flags=SB_BOTH, uint arrow=ESB_ENABLE_BOTH);
    virtual void   ShowScrollBar(int code, bool show=true);

    // Scrolls to a position calculated using the passed delta values
    //
    void          ScrollBy(long dx, long dy);

    virtual bool  IsAutoMode();
    virtual void  AutoScroll();

    // Returns a bool value indicating whether or not the passed
    // area (in units) is currently visible
    //
    bool          IsVisibleRect(long x, long y, int xExt, int yExt);

    // Converts a horizontal range value from the scrollbar to a
    // horizontal scroll value
    //
    int           XScrollValue(long rangeUnit);

    // Converts a vertical range value from the scrollbar to a
    // vertical scroll value
    //
    int           YScrollValue(long rangeUnit);

    // Converts a horizontal scroll value from the scrollbar to
    // a horizontal range value
    //
    long          XRangeValue(int scrollUnit);

    // Converts a vertical scroll value from the scrollbar to
    // a vertical range value
    //
    long          YRangeValue(int scrollUnit);

  protected:
    virtual void   SetScrollPage(int bar, int page, bool redraw = true);
    virtual int   GetScrollPage(int bar) const;
    virtual void  GetScrollRange(int bar, int& minPos, int& maxPos) const;
    virtual void  SetScrollRange(int  bar, int  minPos, int  maxPos, bool redraw = true);
    virtual int   GetScrollPos(int bar) const;
    virtual int   SetScrollPos(int bar, int pos, bool redraw = true);

  public:
    TWindow*  Window;         // Window that this scroller belongs to
    long      XPos;           // Current pos in horz/vert scroll units
    long      YPos;           //
    long      XRange;         // # of scrollable horz/vert scroll units
    long      YRange;         //
    long      XTotalUnits;    // Total number of horz/vert scroll units
    long      YTotalUnits;    //
    int       XUnit;          // Logical device units per horz/vert scroll unit
    int       YUnit;          //
    int       XLine;          // # of horz/vert scroll units per line
    int       YLine;          //
    int       XPage;          // # of horz/vert scroll units per page
    int       YPage;          //
    bool      AutoMode;       // Auto scrolling mode
    bool      TrackMode;      // Track scroll mode
    bool      AutoOrg;        // Indicates Scroller offsets origin
    bool      HasHScrollBar;
    bool      HasVScrollBar;

  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TScroller, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TScroller, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TScroller );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline void TScroller::SetWindow(TWindow* win){
  Window = win;
}

//
inline void TScroller::ScrollBy(long dx, long dy){
  ScrollTo(XPos+dx, YPos+dy);
}

//
inline bool TScroller::IsVisibleRect(long x, long y, int xExt, int yExt){
  return (x + xExt > XPos) && (y + yExt > YPos) &&
      (x <= XPos + XPage + 1) && (y <= YPos + YPage + 1);
}

//
inline int  TScroller::XScrollValue(long rangeUnit){
  return (int)LongMulDiv(rangeUnit, SHRT_MAX, XRange);
}

//
inline int  TScroller::YScrollValue(long rangeUnit){
  return (int)LongMulDiv(rangeUnit, SHRT_MAX, YRange);
}

//
inline long TScroller::XRangeValue(int scrollUnit){
  return LongMulDiv(scrollUnit, XRange, INT_MAX);
}

//
inline long TScroller::YRangeValue(int scrollUnit){
  return LongMulDiv(scrollUnit, YRange, INT_MAX);
}


//
inline void TScroller::SetScrollPage(int bar, int page, bool redraw){
  Window->SetScrollPage(bar, page, redraw);
}

//
inline int TScroller::GetScrollPage(int bar) const {
  return Window->GetScrollPage(bar);
}

//
inline void TScroller::GetScrollRange(int bar, int& minPos, int& maxPos) const{
  Window->GetScrollRange(bar,minPos,maxPos);
}

//
inline void TScroller::SetScrollRange(int  bar, int  minPos, int  maxPos, bool redraw){
  Window->SetScrollRange(bar,minPos,maxPos,redraw);
}

//
inline bool TScroller::EnableScrollBar(uint index, uint arrow){
  return Window->EnableScrollBar(index, arrow);
}

//
inline void TScroller::ShowScrollBar(int code, bool show){
  Window->ShowScrollBar(code, show);
}

//
inline int TScroller::GetScrollPos(int bar) const{
  return Window->GetScrollPos(bar);
}

//
inline int TScroller::SetScrollPos(int bar, int pos, bool redraw){
  return Window->SetScrollPos(bar, pos, redraw);
}

__OWL_END_NAMESPACE

#endif // else defined(BI_DOUBLE_SCROLLER)

#endif  // OWL_SCROLLER_H
