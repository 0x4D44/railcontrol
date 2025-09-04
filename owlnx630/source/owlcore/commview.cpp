//----------------------------------------------------------------------------
//  ObjectWindows
//  OWL NExt
//  Copyright © 1999 by Yura Bidus . All Rights Reserved.
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TTreeWindowView (TTreeWindow).
//  Source file for implementation of TListWindowView (TListWindow).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include <owl/commview.h>

__OWL_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// TTreeWindowView Implementation
//

//
// Build a response table for all messages/commands handled
// by TTreeWindowView derived from TTreeWindow.
//
DEFINE_RESPONSE_TABLE1(TTreeWindowView, TTreeWindow)
  EV_VN_ISWINDOW,
END_RESPONSE_TABLE;


TTreeWindowView::TTreeWindowView(TDocument& doc, TWindow* parent, int id)
:
  TTreeWindow(parent, id, 0, 0, 0, 0),
  TView(doc)
{
}
//DLN
IMPLEMENT_CASTABLE2(TTreeWindowView,TTreeWindow,TView);
//IMPLEMENT_STREAMABLE2(TTreeWindowView, TTreeWindow, TView);
#if !defined(BI_NO_OBJ_STREAMING)
/*
//
//
void*
TTreeWindowView::Streamer::Read(ipstream& is, uint32 version) const
{
  ReadBaseObject((TTreeWindow*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TWindowView::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TTreeWindow*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
}
*/
#endif  // if !defined(BI_NO_OBJ_STREAMING)

//----------------------------------------------------------------------------
// TListWindowView Implementation
//

//
// Build a response table for all messages/commands handled
// by TListWindowView derived from TListWindow.
//
DEFINE_RESPONSE_TABLE1(TListWindowView, TListWindow)
  EV_VN_ISWINDOW,
END_RESPONSE_TABLE;


TListWindowView::TListWindowView(TDocument& doc, TWindow* parent, int id)
:
  TListWindow(parent, id, 0, 0, 0, 0),
  TView(doc)
{
}
//DLN
IMPLEMENT_CASTABLE2(TListWindowView, TListWindow, TView);
//IMPLEMENT_STREAMABLE2(TListWindowView, TListWindow, TView);
#if !defined(BI_NO_OBJ_STREAMING)
/*
//
//
void*
TListWindowView::Streamer::Read(ipstream& is, uint32 version) const
{
  ReadBaseObject((TListWindow*)GetObject(), is);
  ReadBaseObject((TView*)GetObject(), is);
  return GetObject();
}

//
//
//
void
TListWindowView::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TListWindow*)GetObject(), os);
  WriteBaseObject((TView*)GetObject(), os);
}
*/
#endif  // if !defined(BI_NO_OBJ_STREAMING)


__OWL_END_NAMESPACE

  
//==============================================================================
