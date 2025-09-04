//----------------------------------------------------------------------------
//  ObjectWindows
//  OWL NExt
//  Copyright © 1999 by Yura Bidus . All Rights Reserved.
//
//$Revision: 1.25 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TTreeWindowView (TTreeWindow).
//  Class definition for TListWindowView (TListWindow).
//
//------------------------------------------------------------------------------

#if !defined(OWL_COMMVIEW_H) // Sentry, use file only if it's not already included.
#define OWL_COMMVIEW_H

#include <owl/docview.h>
#include <owl/treewind.h>
#include <owl/listwind.h>
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
// class TTreeWindowView
// ~~~~~ ~~~~~~~~~~~~~~~
//
//
class _OWLCLASS TTreeWindowView : public TTreeWindow,  public TView {
  public:
    TTreeWindowView(TDocument& doc, TWindow* parent = 0, int id = -1);
    virtual ~TTreeWindowView();

    static LPCTSTR StaticName();  // put in resource

    // Override virtuals from TWindow
    //
    bool     CanClose();

    // Override virtuals from TView
    //
    LPCTSTR  GetViewName();
    TWindow* GetWindow();
    bool     SetDocTitle(LPCTSTR docname, int index);

  private:
    // Event handlers
    //
    bool     VnIsWindow(HWND hWnd);

  DECLARE_RESPONSE_TABLE(TTreeWindowView);
//  DECLARE_STREAMABLE_OWL(NS_OWL::TTreeWindowView, 1);
};

// define streameable inlines (VC)
//DECLARE_STREAMABLE_INLINES(TTreeWindowView);

//
// class TListWindowView
// ~~~~~ ~~~~~~~~~~~~~~~
//
//

const int ListWindId  = -1;

class _OWLCLASS TListWindowView : public TListWindow,  public TView {
  public:
    TListWindowView(TDocument& doc, TWindow* parent = 0, int id = -1);
    virtual ~TListWindowView();

    static LPCTSTR StaticName();  // put in resource

    // Override virtuals from TWindow
    //
    bool     CanClose();

    // Override virtuals from TView
    //
    LPCTSTR  GetViewName();
    TWindow* GetWindow();
    bool     SetDocTitle(LPCTSTR docname, int index);

  private:
    // Event handlers
    //
    bool     VnIsWindow(HWND hWnd);

    DECLARE_RESPONSE_TABLE(TListWindowView);
//  DECLARE_STREAMABLE_OWL(NS_OWL::TListWindowView, 1);
};

// define streameable inlines (VC)
//DECLARE_STREAMABLE_INLINES(TListWindowView);


// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//
// Inlines 
//

//
// class TTreeWindowView
// ~~~~~ ~~~~~~~~~~~~~~~
inline TTreeWindowView::~TTreeWindowView() 
{
}
//
inline LPCTSTR TTreeWindowView::StaticName()
{
  return _T("TreeWindow View");
}
//
// Only query document if this is the last view open to it.
//
inline bool TTreeWindowView::CanClose() 
{
  return TTreeWindow::CanClose() && 
         (Doc->NextView(this) || 
          Doc->NextView(0) != this ||
          Doc->CanClose());
}
//
inline LPCTSTR TTreeWindowView::GetViewName() 
{
  return StaticName();
}
//
inline TWindow* TTreeWindowView::GetWindow() 
{
  return (TWindow*)this;
}
//
// Does a given HWND belong to this view? Yes if it is us, or a child of us
//
inline bool TTreeWindowView::VnIsWindow(HWND hWnd)
{
  return hWnd == GetHandle() || IsChild(hWnd);
}
//
inline bool TTreeWindowView::SetDocTitle(LPCTSTR docname, int index) 
{
  return TTreeWindow::SetDocTitle(docname, index);
}

//
//
// class TListWindowView
// ~~~~~ ~~~~~~~~~~~~~~~
inline TListWindowView::~TListWindowView() 
{
}
//
inline LPCTSTR TListWindowView::StaticName()
{
  return _T("ListWindow View");
}
//
// Only query document if this is the last view open to it.
//
inline bool TListWindowView::CanClose() 
{
  return TListWindow::CanClose() && 
         (Doc->NextView(this) || 
          Doc->NextView(0) != this ||
          Doc->CanClose());
}
//
inline LPCTSTR TListWindowView::GetViewName() 
{
  return StaticName();
}
//
inline TWindow* TListWindowView::GetWindow() 
{
  return (TWindow*)this;
}
//
// Does a given HWND belong to this view? Yes if it is us, or a child of us
//
inline bool TListWindowView::VnIsWindow(HWND hWnd)
{
  return hWnd == GetHandle() || IsChild(hWnd);
}
//
inline bool TListWindowView::SetDocTitle(LPCTSTR docname, int index) 
{
  return TListWindow::SetDocTitle(docname, index);
}
//================================================================

__OWL_END_NAMESPACE

#endif  // OWL_COMMVIEW_H sentry.
