//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Bidus Yura, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of the TPageScroller class
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_PAGER_H)
# include <owl/pager.h>
#endif


__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlCommCtrl);        // CommonCtrl Diagnostic group

//
//
//
TPageScroller::TPageScroller(TWindow* parent, int id, TWindow* child, int x, int y,
                       int w, int h, TModule* module)
:
  TControl(parent,id,_T(""),x,y,w,h,module),
  ChildWindow(child)
{
  // Does this apply to extended common controls?
  if (!TCommCtrl::IsAvailable() &&
       TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionIE4)
    TXCommCtrl::Raise();

  TCommCtrl::Dll()->CheckCommonControl(ICC_PAGESCROLLER_CLASS);
}


//
//
//
TPageScroller::TPageScroller(TWindow* parent, int resourceId, TWindow* child,
                       TModule* module)
:
  TControl(parent, resourceId, module),
  ChildWindow(child)
{
  // Does this apply to extended common controls?
  if (!TCommCtrl::IsAvailable() &&
       TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionIE4)
    TXCommCtrl::Raise();

  TCommCtrl::Dll()->CheckCommonControl(ICC_PAGESCROLLER_CLASS);

  DisableTransfer();
}

//
//
//
TPageScroller::~TPageScroller()
{
}


// Return the proper class name.
// Windows class: WC_PAGESCROLLER is defined in commctrl.h
LPCTSTR
TPageScroller::GetClassName()
{
  return WC_PAGESCROLLER;
}

//
//
//
void
TPageScroller::SetChild(TWindow* child)
{
  ChildWindow = child;
  if(child && GetHandle()){
    if(!child->GetHandle())
      child->Create();
     SendMessage(PGM_SETCHILD,0,TParam2(child->GetHandle()));
  }
}

void
TPageScroller::SetupWindow()
{
  TControl::SetupWindow();
  if(ChildWindow)
    SetChild(ChildWindow);
}
__OWL_END_NAMESPACE
/* ========================================================================== */
