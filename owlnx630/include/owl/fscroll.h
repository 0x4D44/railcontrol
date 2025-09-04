//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Yura Bidus, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of class TFlatScroller.
//----------------------------------------------------------------------------

#if !defined(OWL_FSCROLL_H)
#define OWL_FSCROLL_H

#if !defined(OWL_PRIVATE_DEFS_H)
# include <owl/private/defs.h>
#endif
#if !defined(OWL_SCROLLER_H)
# include <owl/scroller.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
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
// class TFlatScroller
// ~~~~~ ~~~~~~~~~~~~~
// Class TFlatScroller implements the actual scroller object. All functions
// are inline or virtual to avoid pulling in code when no scrollers have
// been constructed.
//
class _OWLCLASS TFlatScroller : public TScroller {
  public:
    TFlatScroller(TWindow* window,
              int      xUnit,  int  yUnit,
              long     xRange, long yRange);
    virtual ~TFlatScroller();

    virtual void SetWindow(TWindow* win);
    virtual bool EnableScrollBar(uint flags=SB_BOTH, uint arrow=ESB_ENABLE_BOTH);
    virtual void ShowScrollBar(int code, bool show=true);

    void  EnableFlatStyle(bool enable=true);
    bool   GetScrollProp(uint index, int& value);
    bool   SetScrollProp(uint index, int value, bool redraw = true);

  protected:
    virtual void   SetScrollPage(int bar, int page, bool redraw = true);
    virtual int   GetScrollPage(int bar) const;
    virtual void  GetScrollRange(int bar, int& minPos, int& maxPos) const;
    virtual void  SetScrollRange(int  bar, int  minPos, int  maxPos, bool redraw = true);
    virtual int   GetScrollPos(int bar) const;
    virtual int   SetScrollPos(int bar, int pos, bool redraw = true);


  protected:
    TNativeUse   NativeUse;  // Using a native control implementation

  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TFlatScroller, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TFlatScroller, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TFlatScroller );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline void TFlatScroller::SetWindow(TWindow* win)
{
  TScroller::SetWindow(win);
   EnableFlatStyle(true);
}

//
inline void TFlatScroller::EnableFlatStyle(bool enable)
{
  if(NativeUse == nuAlways && Window->GetHandle()){
    if(enable)
      TCommCtrl::Dll()->InitializeFlatSB(*Window);
    else
      TCommCtrl::Dll()->UninitializeFlatSB(*Window);
  }
}

//
inline bool TFlatScroller::GetScrollProp(uint index, int& value)
{
  PRECONDITION(Window->GetHandle());
  if(NativeUse == nuAlways)
    return TCommCtrl::Dll()->FlatSB_GetScrollProp(*Window, index, &value);
  return false;
}

//
inline bool TFlatScroller::SetScrollProp(uint index, int value, bool redraw)
{
  PRECONDITION(Window->GetHandle());
  if(NativeUse == nuAlways)
    return TCommCtrl::Dll()->FlatSB_SetScrollProp(*Window, index, value, redraw);
  return false;
}

//
inline bool TFlatScroller::EnableScrollBar(uint index, uint arrow)
{
  PRECONDITION(Window->GetHandle());
  if(NativeUse == nuAlways)
    return TCommCtrl::Dll()->FlatSB_EnableScrollBar(*Window, index, arrow);
  return TScroller::EnableScrollBar(index, arrow);
}

//
inline void TFlatScroller::ShowScrollBar(int code, bool show)
{
  PRECONDITION(Window->GetHandle());
  if(NativeUse == nuAlways)
    TCommCtrl::Dll()->FlatSB_ShowScrollBar(*Window, code, show);
  else
    TScroller::ShowScrollBar(code, show);
}

////////////////////////////////////////////////////////////////////////////////

__OWL_END_NAMESPACE


#endif  // OWL_SCROLLER_H
