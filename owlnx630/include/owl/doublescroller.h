//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.1 $
//
// Definition of class TScroller.
//----------------------------------------------------------------------------
#if !defined(OWL_DOUBLESCROLLER_H_)

__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

#if !defined(OWL_GEOMETRY_H)
 class _OWLCLASS TRect;
#endif
class _OWLCLASS TDC;

//PRS inline long LongMulDiv(long mul1, long mul2, long div1)
//PRS                       {return (mul1*mul2) / div1;}

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
              double     xRange, double yRange);
    virtual ~TScroller();

    virtual void  SetWindow(TWindow* win);
    virtual void  SetUnits(int xUnit, int yUnit);
    virtual void  SetPageSize();
    virtual void  SetSBarRange();
    virtual void  SetRange(double xRange, double yRange);
    virtual void  SetTotalRangeOfUnits(double xTotalUnits, double yTotalUnits);

    virtual void  BeginView(TDC& dc, TRect& rect);
    virtual void  EndView();
    virtual void  VScroll(uint scrollEvent, int thumbPos);
    virtual void  HScroll(uint scrollEvent, int thumbPos);
    virtual void  ScrollTo(double x, double y);

		virtual bool 	EnableScrollBar(uint flags=SB_BOTH, uint arrow=ESB_ENABLE_BOTH);
    virtual void 	ShowScrollBar(int code, bool show=true);

    // Scrolls to a position calculated using the passed delta values
    //
    void          ScrollBy(double dx, double dy);

    virtual bool  IsAutoMode();
    virtual void  AutoScroll();

    // Returns a bool value indicating whether or not the passed
    // area (in units) is currently visible
    //
    bool          IsVisibleRect(double x, double y, int xExt, int yExt);

    // Converts a horizontal range value from the scrollbar to a
    // horizontal scroll value
    //
    int           XScrollValue(double rangeUnit);

    // Converts a vertical range value from the scrollbar to a
    // vertical scroll value
    //
    int           YScrollValue(double rangeUnit);

    // Converts a horizontal scroll value from the scrollbar to
    // a horizontal range value
    //
    double          XRangeValue(int scrollUnit);

    // Converts a vertical scroll value from the scrollbar to
    // a vertical range value
    //
    double          YRangeValue(int scrollUnit);

	protected:
    virtual void 	SetScrollPage(int bar, int page, bool redraw = true);
    virtual int   GetScrollPage(int bar) const;
    virtual void  GetScrollRange(int bar, int& minPos, int& maxPos) const;
    virtual void  SetScrollRange(int  bar, int  minPos, int  maxPos, bool redraw = true);
    virtual int   GetScrollPos(int bar) const;
    virtual int   SetScrollPos(int bar, int pos, bool redraw = true);

  public:
    TWindow*  Window;         // Window that this scroller belongs to
    double      XPos;           // Current pos in horz/vert scroll units
    double      YPos;           //
    double      XRange;         // # of scrollable horz/vert scroll units
    double      YRange;         //
    double      XTotalUnits;    // Total number of horz/vert scroll units
    double      YTotalUnits;    //
    int       XUnit;          // Logical device units per horz/vert scroll unit
    int       YUnit;          //
    double       XLine;          // # of horz/vert scroll units per line
    double       YLine;          //
    double       XPage;          // # of horz/vert scroll units per page
    double       YPage;          //
    bool      AutoMode;       // Auto scrolling mode
    bool      TrackMode;      // Track scroll mode
    bool      AutoOrg;        // Indicates Scroller offsets origin
    bool      HasHScrollBar;
    bool      HasVScrollBar;

  DECLARE_STREAMABLE_OWL(__OWL TScroller, 1);
};
DECLARE_STREAMABLE_INLINES( __OWL TScroller );
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
inline void TScroller::ScrollBy(double dx, double dy)
{
  ScrollTo(XPos+dx, YPos+dy);
}

//
inline bool TScroller::IsVisibleRect(double x, double y, int xExt, int yExt)
{
  return (x + xExt > XPos) && (y + yExt > YPos) &&
      (x <= XPos + XPage + 1) && (y <= YPos + YPage + 1);
}

//
inline int  TScroller::XScrollValue(double rangeUnit)
{
  //KEV - Changed for doubles
  //return (int)MulDiv(rangeUnit, SHRT_MAX, XRange);

  return (int)(rangeUnit * SHRT_MAX / XRange);
}

//
inline int  TScroller::YScrollValue(double rangeUnit)
{
  //KEV - Changed for doubles
  //return (int)MulDiv(rangeUnit, SHRT_MAX, YRange);

  return (int)(rangeUnit * SHRT_MAX / YRange);
}

//
inline double TScroller::XRangeValue(int scrollUnit)
{
  //KEV - Changed for doubles
  //return MulDiv(scrollUnit, XRange, SHRT_MAX);

  return (scrollUnit * XRange / SHRT_MAX);
}

//
inline double TScroller::YRangeValue(int scrollUnit)
{
  //KEV - Changed for doubles
  //return MulDiv(scrollUnit, YRange, SHRT_MAX);

  return (scrollUnit * YRange / SHRT_MAX);
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


#define OWL_DOUBLESCROLLER_H_
#endif


