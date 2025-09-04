//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28.2.1 $
//$Author: jogybl $
//$Date: 2009-11-23 08:45:18 $
//
// Implementation of class TScroller.
// Including bug fixes by Rich Goldstein
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_SCROLLER_H)
# include <owl/scroller.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif
#include <stdlib.h>

#if defined(BI_DOUBLE_SCROLLER)

__OWL_BEGIN_NAMESPACE

#define max(a,b)   (((a) > (b)) ? (a) : (b))
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#define abs(a)     ((a) >= 0 ? (a) : -(a))


OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1
//
// constructs a TScroller object
//
TScroller::TScroller(TWindow* window,
                     int      xUnit,
                     int      yUnit,
                     double     xRange,
                     double     yRange)
{
  Window = window;
  XPos = YPos = 0;
  XUnit = xUnit;
  YUnit = yUnit;
  XRange = xRange;
  YRange = yRange;
  XTotalUnits = 0;
  YTotalUnits = 0;
  XLine = 1;  YLine = 1;
  XPage = 1;  YPage = 1;
  AutoMode = true;
  TrackMode = true;
  AutoOrg = true;
  HasHScrollBar = ToBool(Window && (Window->Attr.Style&WS_HSCROLL));
  HasVScrollBar = ToBool(Window && (Window->Attr.Style&WS_VSCROLL));
}

//
// destructs a TScroller object
//
// sets owning Window's Scroller member to 0
//
TScroller::~TScroller()
{
  if (Window)
    Window->Scroller = 0;
}

//
// sets the number of units per page (amount by which to scroll on a page
// scroll request) according to the current size of the Window's client area
//
void
TScroller::SetPageSize()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    TRect  clientRect;
    Window->GetClientRect(clientRect);
    int width = clientRect.Width();
    int height = clientRect.Height();

    if (width && XUnit > 0) {
      XPage = max(1, (width+1) / XUnit - 1);
      if (XTotalUnits && XTotalUnits > width) {
        SetScrollPage(SB_HORZ, int(width*XRange/XTotalUnits), true);
      }
//-----------------------------------------------------------  
//Commented out by DLN to maintain non-proportional thumb when
//  user doesn't specify totalUnit value
//-----------------------------------------------------------  
//      else
//        Window->SetScrollPage(SB_HORZ, XPage, false);
    }

    if (height && YUnit > 0) {
      YPage = max(1, (height+1) / YUnit - 1);
      if (YTotalUnits && YTotalUnits > height) {
        SetScrollPage(SB_VERT, (height * YRange) / YTotalUnits, true);
      }
//-----------------------------------------------------------  
//Commented out by DLN to maintain non-proportional thumb when
//  user doesn't specify totalUnit value
//-----------------------------------------------------------  
//      else
//        Window->SetScrollPage(SB_VERT, YPage, false);
    }
  }
}

//
// sets the range of the TScroller and also sets the range of its
// Window's scrollbars
//
void
TScroller::SetRange(double xRange, double yRange)
{
  XRange = xRange;
  YRange = yRange;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// Sets the total units of the window
//
void
TScroller::SetTotalRangeOfUnits(double xTotalUnits, double yTotalUnits)
{
  XTotalUnits = xTotalUnits;
  YTotalUnits = yTotalUnits;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// resets the X and Y scroll unit size (in device units) to the passed
// parameters
//
// calls SetPageSize to update the X and Y page size, which are specified
// in scroll units
//
void
TScroller::SetUnits(int xUnit, int yUnit)
{
  XUnit = xUnit;
  YUnit = yUnit;
  SetPageSize();
}

//
// sets the range of the Window's scrollbars & repaints as necessary
//
void
TScroller::SetSBarRange()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    if (HasHScrollBar) {
      int curMin, curMax;
      GetScrollRange(SB_HORZ, curMin, curMax);
      int newMax = max(0, (int)min(XRange, INT_MAX));
      if (newMax != curMax || curMin != 0)
        SetScrollRange(SB_HORZ, 0, newMax, true);
    }

    if (HasVScrollBar) {
      int curMin, curMax;
      GetScrollRange(SB_VERT, curMin, curMax);
      int newMax = max(0, (int)min(YRange, INT_MAX));
      if (newMax != curMax || curMin != 0)
        SetScrollRange(SB_VERT, 0, newMax, true);
    }
  }
}

//
// sets the origin for the paint display context according to XPos, YPos
//
void
TScroller::BeginView(TDC& dc, TRect& rect)
{
  double  xOrg = XPos * XUnit;
  double  yOrg = YPos * YUnit;

  if (AutoOrg && xOrg <= INT_MAX && yOrg <= INT_MAX) {
    TPoint offset(int(-xOrg), int(-yOrg));
    dc.SetViewportOrg(offset);
    rect -= offset;
  }
}

//
// updates the position of the Window's scrollbar(s)
//
void
TScroller::EndView()
{
  if (Window) {
    if (HasHScrollBar) {
      int  newPos = XRange > INT_MAX ? XScrollValue(XPos) : (int)XPos;
      if (newPos != GetScrollPos(SB_HORZ))
        SetScrollPos(SB_HORZ, newPos, true);
    }

    if (HasVScrollBar) {
      int  newPos = YRange > INT_MAX ? YScrollValue(YPos) : (int)YPos;
      if (newPos != GetScrollPos(SB_VERT))
        SetScrollPos(SB_VERT, newPos, true);
    }
  }
}

//
// scrolls vertically according to scroll action and thumb position
//
void
TScroller::VScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINEDOWN:
      ScrollBy(0, YLine);
      break;

    case SB_LINEUP:
      ScrollBy(0, -YLine);
      break;

    case SB_PAGEDOWN:
      ScrollBy(0, YPage);
      break;

    case SB_PAGEUP:
      ScrollBy(0, -YPage);
      break;

    case SB_TOP:
      ScrollTo(XPos, 0);
      break;

    case SB_BOTTOM:
      ScrollTo(XPos, YRange);
      break;

    case SB_THUMBPOSITION:
      if (YRange > INT_MAX)
        ScrollTo(XPos, YRangeValue(thumbPos));

      else
        ScrollTo(XPos, thumbPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (YRange > INT_MAX)
         ScrollTo(XPos, YRangeValue(thumbPos));

        else
         ScrollTo(XPos, thumbPos);

      if (Window && HasVScrollBar)
        SetScrollPos(SB_VERT, thumbPos, true);
      break;
  }
}

//
// scrolls horizontally according to scroll action and thumb position
//
void
TScroller::HScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINERIGHT:
      ScrollBy(XLine, 0);
      break;

    case SB_LINELEFT:
      ScrollBy(-XLine, 0);
      break;

    case SB_PAGERIGHT:
      ScrollBy(XPage, 0);
      break;

    case SB_PAGELEFT:
      ScrollBy(-XPage, 0);
      break;

    case SB_LEFT:
      ScrollTo(0, YPos);
      break;

    case SB_RIGHT:
      ScrollTo(XRange, YPos);
      break;

    case SB_THUMBPOSITION:
      if (XRange > INT_MAX)
        ScrollTo(XRangeValue(thumbPos), YPos);

      else
        ScrollTo(thumbPos, YPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (XRange > INT_MAX)
          ScrollTo(XRangeValue(thumbPos), YPos);

        else
          ScrollTo(thumbPos, YPos);

      if (Window && HasHScrollBar)
        SetScrollPos(SB_HORZ, thumbPos, true);
      break;
  }
}

//
// scrolls to an (x,y) position, after checking boundary conditions
// causes a WM_PAINT message to be sent
//
// first scrolls the contents of the client area, if a portion of the client
// area will remain visible
//
void
TScroller::ScrollTo(double x, double y)
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    double  newXPos = max(0, min(x, XRange));
    double  newYPos = max(0, min(y, YRange));

    if (newXPos != XPos || newYPos != YPos) {
      //
      // scaling isn't needed here. if condition is met, ScrollWindow()
      // will succeed since XPage and YPage are ints
      //
      // if condition isn't met, ScrollWindow() is called in EndScroll()
      // as a result of calling UpdateWindow()
      //
      // EndScroll() performs the necessary scaling
      //
      if (AutoOrg || abs(YPos-newYPos) < YPage && abs(XPos-newXPos) < XPage)
        Window->ScrollWindow((int)(XPos - newXPos) * XUnit,
                (int)(YPos - newYPos) * YUnit, 0, 0);

      else
        Window->Invalidate();

      XPos = newXPos;
      YPos = newYPos;
      Window->UpdateWindow();
    }
  }
}

//
//
//
bool
TScroller::IsAutoMode()
{
  return AutoMode;
}

//
// performs "auto-scrolling" (dragging the mouse from within the client
// client area of the Window to without results in auto-scrolling when
// the AutoMode data member of the Scroller is true)
//
void
TScroller::AutoScroll()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (AutoMode && Window) {
    TRect  clientRect;
    TPoint cursorPos;
    double  dx = 0, dy = 0;

    GetCursorPos(&cursorPos);
    Window->ScreenToClient(cursorPos);
    Window->GetClientRect(clientRect);

    if (cursorPos.y < 0)
      dy = min(-YLine, max(-YPage, cursorPos.y / 10 * YLine));

    else if (cursorPos.y > clientRect.bottom)
      dy = max(YLine, min(YPage, (cursorPos.y-clientRect.bottom)/10 * YLine));

    if (cursorPos.x < 0)
      dx = min(-XLine, max(-XPage, cursorPos.x / 10.0L * XLine));

    else if (cursorPos.x > clientRect.right)
      dx = max(XLine, min(XPage, (cursorPos.x-clientRect.right)/10.0L * XLine));

    ScrollBy(dx, dy);
  }
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE(TScroller);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TScroller from the passed ipstream
//
void*
TScroller::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TScroller* o = GetObject();
  is >> o->XPos >> o->YPos >>
      o->XUnit >> o->YUnit >>
      o->XRange >> o->YRange >>
      o->XLine >> o->YLine >>
      o->XPage >> o->YPage >>
      o->AutoMode >> o->TrackMode >>
      o->AutoOrg >>
      o->HasHScrollBar >> o->HasVScrollBar;

  o->Window = 0;
  return o;
}

//
// writes the TScroller to the passed opstream
//
void
TScroller::Streamer::Write(opstream& os) const
{
  TScroller* o = GetObject();
  os << o->XPos << o->YPos <<
      o->XUnit << o->YUnit <<
      o->XRange << o->YRange <<
      o->XLine << o->YLine <<
      o->XPage << o->YPage <<
      o->AutoMode << o->TrackMode <<
      o->AutoOrg <<
      o->HasHScrollBar << o->HasVScrollBar;
}


#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif


__OWL_END_NAMESPACE



#else //defined(BI_DOUBLE_SCROLLER)

__OWL_BEGIN_NAMESPACE

#define abs(a)     ((a) >= 0 ? (a) : -(a))


OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1
//
// constructs a TScroller object
//
TScroller::TScroller(TWindow* window,
                     int      xUnit,
                     int      yUnit,
                     long     xRange,
                     long     yRange)
{
  Window = window;
  XPos = YPos = 0;
  XUnit = xUnit;
  YUnit = yUnit;
  XRange = xRange;
  YRange = yRange;
  XTotalUnits = 0;
  YTotalUnits = 0;
  XLine = 1;  YLine = 1;
  XPage = 1;  YPage = 1;
  AutoMode = true;
  TrackMode = true;
  AutoOrg = true;
  HasHScrollBar = ToBool(Window && (Window->Attr.Style&WS_HSCROLL));
  HasVScrollBar = ToBool(Window && (Window->Attr.Style&WS_VSCROLL));
}

//
// destructs a TScroller object
//
// sets owning Window's Scroller member to 0
//
TScroller::~TScroller()
{
  if (Window)
    Window->Scroller = 0;
}

//
// sets the number of units per page (amount by which to scroll on a page
// scroll request) according to the current size of the Window's client area
//
void
TScroller::SetPageSize()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    TRect  clientRect;
    Window->GetClientRect(clientRect);
    int width = clientRect.Width();
    int height = clientRect.Height();

    if (width && XUnit > 0) {
      XPage = __max(1, (width+1) / XUnit - 1);
      if (XTotalUnits && XTotalUnits > width) {
        SetScrollPage(SB_HORZ, (width * XRange) / XTotalUnits, true);
      }
      else
        SetScrollPage(SB_HORZ, XPage, false); 
    }

    if (height && YUnit > 0) {
      YPage = __max(1, (height+1) / YUnit - 1);
      if (YTotalUnits && YTotalUnits > height) {
        SetScrollPage(SB_VERT, (height * YRange) / YTotalUnits, true);
      }
      else
        SetScrollPage(SB_VERT, YPage, false); 
    }
  }
}

//
// sets the range of the TScroller and also sets the range of its
// Window's scrollbars
//
void
TScroller::SetRange(long xRange, long yRange)
{
  XRange = xRange;
  YRange = yRange;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// Sets the total units of the window
//
void
TScroller::SetTotalRangeOfUnits(long xTotalUnits, long yTotalUnits)
{
  XTotalUnits = xTotalUnits;
  YTotalUnits = yTotalUnits;
  SetSBarRange();
  ScrollTo(XPos, YPos);
}

//
// resets the X and Y scroll unit size (in device units) to the passed
// parameters
//
// calls SetPageSize to update the X and Y page size, which are specified
// in scroll units
//
void
TScroller::SetUnits(int xUnit, int yUnit)
{
  XUnit = xUnit;
  YUnit = yUnit;
  SetPageSize();
}

//
// sets the range of the Window's scrollbars & repaints as necessary
//
void
TScroller::SetSBarRange()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    if (HasHScrollBar) {
      int curMin, curMax;
      GetScrollRange(SB_HORZ, curMin, curMax);
      int newMax = __max(0, __min(XRange - 1, INT_MAX));
      if (newMax != curMax || curMin != 0)
        SetScrollRange(SB_HORZ, 0, newMax, true);
    }

    if (HasVScrollBar) {
      int curMin, curMax;
      GetScrollRange(SB_VERT, curMin, curMax);
      int newMax = __max(0, __min(YRange - 1, INT_MAX));
      if (newMax != curMax || curMin != 0)
        SetScrollRange(SB_VERT, 0, newMax, true);
    }
  }
}

//
// sets the origin for the paint display context according to XPos, YPos
//
void
TScroller::BeginView(TDC& dc, TRect& rect)
{
  long  xOrg = XPos * XUnit;
  long  yOrg = YPos * YUnit;

  if (AutoOrg && xOrg <= INT_MAX && yOrg <= INT_MAX) {
    TPoint offset(int(-xOrg), int(-yOrg));
    dc.SetViewportOrg(offset);
    rect -= offset;
  }
}

//
// updates the position of the Window's scrollbar(s)
//
void
TScroller::EndView()
{
  if (Window) {
    if (HasHScrollBar) {
      int  newPos = XRange > INT_MAX ? XScrollValue(XPos) : (int)XPos;
      if (newPos != GetScrollPos(SB_HORZ))
        SetScrollPos(SB_HORZ, newPos, true);
    }

    if (HasVScrollBar) {
      int  newPos = YRange > INT_MAX ? YScrollValue(YPos) : (int)YPos;
      if (newPos != GetScrollPos(SB_VERT))
        SetScrollPos(SB_VERT, newPos, true);
    }
  }
}

//
// scrolls vertically according to scroll action and thumb position
//
void
TScroller::VScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINEDOWN:
      ScrollBy(0, YLine);
      break;

    case SB_LINEUP:
      ScrollBy(0, -YLine);
      break;

    case SB_PAGEDOWN:
      ScrollBy(0, YPage);
      break;

    case SB_PAGEUP:
      ScrollBy(0, -YPage);
      break;

    case SB_TOP:
      ScrollTo(XPos, 0);
      break;

    case SB_BOTTOM:
      ScrollTo(XPos, YRange);
      break;

    case SB_THUMBPOSITION:
      if (YRange > INT_MAX)
        ScrollTo(XPos, YRangeValue(thumbPos));

      else
        ScrollTo(XPos, thumbPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (YRange > INT_MAX)
         ScrollTo(XPos, YRangeValue(thumbPos));

        else
         ScrollTo(XPos, thumbPos);

      if (Window && HasVScrollBar)
        SetScrollPos(SB_VERT, thumbPos, true);
      break;
  }
}

//
// scrolls horizontally according to scroll action and thumb position
//
void
TScroller::HScroll(uint scrollEvent, int thumbPos)
{
  switch (scrollEvent) {
    case SB_LINEDOWN:
      ScrollBy(XLine, 0);
      break;

    case SB_LINEUP:
      ScrollBy(-XLine, 0);
      break;

    case SB_PAGEDOWN:
      ScrollBy(XPage, 0);
      break;

    case SB_PAGEUP:
      ScrollBy(-XPage, 0);
      break;

    case SB_TOP:
      ScrollTo(0, YPos);
      break;

    case SB_BOTTOM:
      ScrollTo(XRange, YPos);
      break;

    case SB_THUMBPOSITION:
      if (XRange > INT_MAX)
        ScrollTo(XRangeValue(thumbPos), YPos);

      else
        ScrollTo(thumbPos, YPos);
      break;

    case SB_THUMBTRACK:
      if (TrackMode)
        if (XRange > INT_MAX)
          ScrollTo(XRangeValue(thumbPos), YPos);

        else
          ScrollTo(thumbPos, YPos);

      if (Window && HasHScrollBar)
        SetScrollPos(SB_HORZ, thumbPos, true);
      break;
  }
}

//
// scrolls to an (x,y) position, after checking boundary conditions
// causes a WM_PAINT message to be sent
//
// first scrolls the contents of the client area, if a portion of the client
// area will remain visible
//
void
TScroller::ScrollTo(long x, long y)
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (Window && Window->GetHandle()) {
    long  newXPos = __max(0, __min(x, XRange - XPage ));
    long  newYPos = __max(0, __min(y, YRange - YPage));

    if (newXPos != XPos || newYPos != YPos) {
      //
      // scaling isn't needed here. if condition is met, ScrollWindow()
      // will succeed since XPage and YPage are ints
      //
      // if condition isn't met, ScrollWindow() is called in EndScroll()
      // as a result of calling UpdateWindow()
      //
      // EndScroll() performs the necessary scaling
      //
      if (AutoOrg || abs(YPos-newYPos) < YPage && abs(XPos-newXPos) < XPage)
        Window->ScrollWindow((int)(XPos - newXPos) * XUnit,
                (int)(YPos - newYPos) * YUnit, 0, 0);

      else
        Window->Invalidate();

      XPos = newXPos;
      YPos = newYPos;
      Window->UpdateWindow();
    }
  }
}

//
//
//
bool
TScroller::IsAutoMode()
{
  return AutoMode;
}

//
// performs "auto-scrolling" (dragging the mouse from within the client
// client area of the Window to without results in auto-scrolling when
// the AutoMode data member of the Scroller is true)
//
void
TScroller::AutoScroll()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  if (AutoMode && Window) {
    TRect  clientRect;
    TPoint cursorPos;
    long  dx = 0, dy = 0;

    GetCursorPos(&cursorPos);
    Window->ScreenToClient(cursorPos);
    Window->GetClientRect(clientRect);

    if (cursorPos.y < 0)
      dy = __min(-YLine, __max(-YPage, cursorPos.y / 10 * YLine));

    else if (cursorPos.y > clientRect.bottom)
      dy = __max(YLine, __min(YPage, (cursorPos.y-clientRect.bottom)/10 * YLine));

    if (cursorPos.x < 0)
      dx = __min(-XLine, __max(-XPage, cursorPos.x / 10 * XLine));

    else if (cursorPos.x > clientRect.right)
      dx = __max(XLine, __min(XPage, (cursorPos.x-clientRect.right)/10 * XLine));

    ScrollBy(dx, dy);
  }
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE(TScroller);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TScroller from the passed ipstream
//
void*
TScroller::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TScroller* o = GetObject();
  is >> o->XPos >> o->YPos >>
      o->XUnit >> o->YUnit >>
      o->XRange >> o->YRange >>
      o->XLine >> o->YLine >>
      o->XPage >> o->YPage >>
      o->AutoMode >> o->TrackMode >>
      o->AutoOrg >>
      o->HasHScrollBar >> o->HasVScrollBar;

  o->Window = 0;
  return o;
}

//
// writes the TScroller to the passed opstream
//
void
TScroller::Streamer::Write(opstream& os) const
{
  TScroller* o = GetObject();
  os << o->XPos << o->YPos <<
      o->XUnit << o->YUnit <<
      o->XRange << o->YRange <<
      o->XLine << o->YLine <<
      o->XPage << o->YPage <<
      o->AutoMode << o->TrackMode <<
      o->AutoOrg <<
      o->HasHScrollBar << o->HasVScrollBar;
}


#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

#endif //else defined(BI_DOUBLE_SCROLLER)

__OWL_END_NAMESPACE
/* ========================================================================== */

