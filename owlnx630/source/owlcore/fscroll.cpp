//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $ 
//$Date: 2007-07-20 10:47:42 $
//
// Implementation of class TFlatScroller.
//------------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_FSCROLL_H)
# include <owl/fscroll.h>
#endif
#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif

__OWL_BEGIN_NAMESPACE


OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1
//
// constructs a TScroller object
//
TFlatScroller::TFlatScroller(TWindow* window,
                             int      xUnit,
                             int      yUnit,
                             long     xRange,
                             long     yRange)
:
  TScroller(window,xUnit,yUnit,xRange,yRange)
{
  // Update flag based on availability of Common Control Library
  //
  NativeUse = nuNever;

  if(TCommCtrl::IsAvailable() &&
     TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionIE3)
    NativeUse = nuAlways;
}

//
// destructs a TFlatScroller object
//
TFlatScroller::~TFlatScroller()
{
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 3

void
TFlatScroller::SetScrollPage(int bar, int page, bool redraw)
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways){
    TScroller::SetScrollPage(bar, page, redraw);
    return;
  }
  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_PAGE;
  info.nPage = page;
  TCommCtrl::Dll()->FlatSB_SetScrollInfo(*Window, bar, &info, redraw);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 4

#if !defined(__GNUC__) //JJH removal of following pragma warn for gcc
#pragma warn -par
#endif
//
//
//
int
TFlatScroller::GetScrollPage(int bar) const
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways){
    return TScroller::GetScrollPage(bar);
  }
  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_PAGE;
  TCommCtrl::Dll()->FlatSB_GetScrollInfo(*Window, bar, &info);
  return info.nPage;
}
#if !defined(__GNUC__) //JJH removal of following pragma warn for gcc
#pragma warn .par
#endif //__GNUC__

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 5

//
void
TFlatScroller::GetScrollRange(int bar, int& minPos, int& maxPos) const
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways)
    TScroller::GetScrollRange(bar, minPos, maxPos);
  else{
    SCROLLINFO info;
    ZeroMemory(&info, sizeof info);
    info.cbSize = sizeof info;
    info.fMask = SIF_RANGE;
    TCommCtrl::Dll()->FlatSB_GetScrollInfo(*Window, bar, &info);
    minPos = info.nMin;
    maxPos = info.nMax;
    //TCommCtrl::Dll()->FlatSB_GetScrollRange(*Window,bar, &minPos, &maxPos);
  }
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 6

//
void
TFlatScroller::SetScrollRange(int bar, int minPos, int maxPos, bool redraw)
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways)
    TScroller::SetScrollRange(bar, minPos, maxPos, redraw);
  else{
    SCROLLINFO info;
    ZeroMemory(&info, sizeof info);
    info.cbSize = sizeof info;
    info.fMask = SIF_RANGE;
    info.nMin = minPos;
    info.nMax = maxPos;
    TCommCtrl::Dll()->FlatSB_SetScrollInfo(*Window, bar, &info, redraw);
    //TCommCtrl::Dll()->FlatSB_SetScrollRange(*Window,bar, minPos, maxPos, redraw);
  }
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 7

//
int
TFlatScroller::GetScrollPos(int bar) const
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways)
    return TScroller::GetScrollPos(bar);

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
//  info.fMask = SIF_DISABLENOSCROLL | SIF_POS;
  info.fMask = SIF_POS;
  TCommCtrl::Dll()->FlatSB_GetScrollInfo(*Window, bar, &info);
  return info.nPos;
  //return TCommCtrl::Dll()->FlatSB_GetScrollPos(*Window, bar);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 8

//
int
TFlatScroller::SetScrollPos(int bar, int pos, bool redraw)
{
  PRECONDITION(Window->GetHandle());
   if(NativeUse != nuAlways)
    return TScroller::SetScrollPos(bar, pos, redraw);

  int oldPos = GetScrollPos(bar);

  SCROLLINFO info;
  ZeroMemory(&info, sizeof info);
  info.cbSize = sizeof info;
  info.fMask = SIF_POS;
  info.nPos = pos;
  TCommCtrl::Dll()->FlatSB_SetScrollInfo(*Window, bar, &info, redraw);
  return oldPos;

  //return TCommCtrl::Dll()->FlatSB_SetScrollPos(*Window, bar,pos, redraw);
}

#endif
//--------------------------------------------------------------------------
#if !defined(SECTION) || SECTION == 9

IMPLEMENT_STREAMABLE1(TFlatScroller, TScroller);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TFlatScroller from the passed ipstream
//
void*
TFlatScroller::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TScroller*)GetObject(), is);

  GetObject()->NativeUse = nuNever;
  TDllVersionInfo info;
  if(TCommCtrl::IsAvailable() && TCommCtrl::Dll()->GetCtrlVersion(info) &&
     info.dwMinorVersion > ComCtlVersionIE3)
    GetObject()->NativeUse = nuAlways;

  return GetObject();
}

//
// writes the TFlatScroller to the passed opstream
//
void
TFlatScroller::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TScroller*)GetObject(), os);
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

