//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:38 $
//
// Implementation of class TControl.  This defines the basic behavior of all
// controls.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
#if defined(SECTION) && SECTION != 1
DIAG_DECLARE_GROUP(OwlControl);
#endif

// from window.cpp
//
TDrawItem* ItemData2DrawItem(uint32 data);

#if !defined(SECTION) || SECTION == 1

//
// General Controls diagnostic group
//
DIAG_DEFINE_GROUP_INIT(OWL_INI, OwlControl, 1, 0);


DEFINE_RESPONSE_TABLE1(TControl, TWindow)
  EV_WM_PAINT,
  EV_WM_COMPAREITEM,
  EV_WM_DELETEITEM,
  EV_WM_DRAWITEM,
  EV_WM_MEASUREITEM,
END_RESPONSE_TABLE;

#endif
#if !defined(SECTION) || SECTION == 2

//
// Constructor for a TControl
//
TControl::TControl(TWindow*        parent,
                   int             id,
                   LPCTSTR         title,
                   int x, int y, int w, int h,
                   TModule*        module)
{
  // Initialize virtual base, in case the derived-most used default ctor
  //
  TWindow::Init(parent, title, module);

  Attr.Id = id;
  Attr.X = x;
  Attr.Y = y;
  Attr.W = w;
  Attr.H = h;
  Attr.Style = WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP;
  NativeUse = nuDontCare;

  TRACEX(OwlControl, OWL_CDLEVEL, "TControl constructed @" << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 3

//
// Constructor for a TControl to be associated with a MS-Windows
// interface element created by MS-Windows from a resource definition
//
// Data transfer is enabled for the TControl by default
//
TControl::TControl(TWindow*   parent,
                   int        resourceId,
                   TModule*   module)
{
  // Initialize virtual base, in case the derived-most used default ctor
  //
  TWindow::Init(parent, 0, module);

  if (HiUint16(uint32(Title)))
    delete[] Title;     // Free memory allocated by TWindow's constructor
  Title = 0;

  SetFlag(wfFromResource);
  memset(&Attr, 0x0, sizeof(Attr));
  Attr.Id = resourceId;
  EnableTransfer();
  NativeUse = nuDontCare;

  TRACEX(OwlControl, OWL_CDLEVEL, "TControl constructed from resource @" << (void*)this);
}

#endif
#if !defined(SECTION) || SECTION == 4

//
// Constructor to alias a non-OWL control
//
TControl::TControl(THandle handle, TModule* module)
{
  // Initialize virtual base, in case the derived-most used default ctor
  //
  TWindow::Init(handle, module);
  NativeUse = nuDontCare;

  TRACEX(OwlControl, OWL_CDLEVEL, "TControl aliased @" << (void*)this <<
    " to handle" << uint(handle));
}

#endif
#if !defined(SECTION) || SECTION == 5

//
//
//
TControl::~TControl()
{
  TRACEX(OwlControl, OWL_CDLEVEL, "TControl destructed @" << (void*)this);
}

//
// Intercept WM_PAINT to redirect from TWindow to the underlying control iff
// this Owl object is just a wrapper for a predefined class.
//
void
TControl::EvPaint()
{
  if (IsFlagSet(wfPredefinedClass))
    DefaultProcessing();  // don't call TWindow::EvPaint()

  else
    TWindow::EvPaint();
}

//
// Handles WM_COMPAREITEM message (for owner draw controls) by calling the
// corresponding virtual function.
//
TResult
TControl::EvCompareItem(uint /*ctrlId*/, COMPAREITEMSTRUCT & compareInfo)
{
  return CompareItem(compareInfo);
}
//

//
// Function called when a WM_COMPAREITEM is sent to parent on our behalf
//
int
TControl::CompareItem(COMPAREITEMSTRUCT & compareInfo)
{
  TRACEX(OwlControl, 1, "TControl::CompareItem() @" << (void*)this);
  if (!GetHandle() || compareInfo.itemData1 == compareInfo.itemData2) 
    return 0;
  if(compareInfo.itemData1){
    TDrawItem* item = ItemData2DrawItem(compareInfo.itemData1);
    if(item)
      return item->Compare(compareInfo);
  }
  return 0;
}

//
// Handles WM_DELETEITEM message (for owner draw controls) by calling the
// corresponding virtual function.
//
void
TControl::EvDeleteItem(uint /*ctrlId*/, DELETEITEMSTRUCT & deleteInfo)
{
  DeleteItem(deleteInfo);
}

//
// Function called when a WM_DELETEITEM is sent to parent on our behalf
//
void
TControl::DeleteItem(DELETEITEMSTRUCT &)
{
  TRACEX(OwlControl, 1, "TControl::DeleteItem() @" << (void*)this);
}

//
// Handles WM_MEASUREITEM message (for owner draw controls) by calling the
// corresponding virtual function.
//
void
TControl::EvMeasureItem(uint /*ctrlId*/, MEASUREITEMSTRUCT & measureInfo)
{
  MeasureItem(measureInfo);
}

//
// Function called when a WM_MEASUREITEM is sent to parent on our behalf
//
void
TControl::MeasureItem(MEASUREITEMSTRUCT & measureInfo)
{
  TRACEX(OwlControl, 1, "TControl::MeasureItem() @" << (void*)this);
  if(measureInfo.itemData){
    TDrawItem* item = ItemData2DrawItem(measureInfo.itemData);
    if(item)
      item->Measure(measureInfo);
  }
}

//
// Handles WM_DRAWITEM message (for owner draw controls) by calling the
// corresponding virtual function.
//
void
TControl::EvDrawItem(uint /*ctrlId*/, DRAWITEMSTRUCT & drawInfo)
{
  DrawItem(drawInfo);
}

//
// Function called when a WM_DRAWITEM is sent to parent on our behalf. This
// is in turn broken up into one of three draw events.
//
void
TControl::DrawItem(DRAWITEMSTRUCT & drawInfo)
{
  switch (drawInfo.itemAction) {
    case ODA_DRAWENTIRE:
      ODADrawEntire(drawInfo);
      break;

    case ODA_FOCUS:
      ODAFocus(drawInfo);
      break;

    case ODA_SELECT:
      ODASelect(drawInfo);
      break;
  }
}

//
// Function called when the entire owner-draw control needs to be redrawn
//
// Will usually be redefined by descendants of TControl which use owner draw
// style
//
void
TControl::ODADrawEntire(DRAWITEMSTRUCT & drawInfo)
{
  TRACEX(OwlControl, 1, "TControl::ODADrawEntire() @" << (void*)this);
  if(drawInfo.itemData && drawInfo.itemID != (uint16)-1){
    TDrawItem* item = ItemData2DrawItem(drawInfo.itemData);
    if(item)
      item->Draw(drawInfo);
  }
}

//
// Function called when an owner-draw control gains or loses focus
//
// Will usually be redefined by descendants of TControl which use owner draw
// style
//
void
TControl::ODAFocus(DRAWITEMSTRUCT & drawInfo)
{
  TRACEX(OwlControl, 1, "TControl::ODAFocus() @" << (void*)this);
  if(drawInfo.itemData){
    TDrawItem* item = ItemData2DrawItem(drawInfo.itemData);
    if(item)
      item->Draw(drawInfo);
  }
}

//
// Function called when an owner-draw control's selection status changes
//
// Will usually be redefined by descendants of TControl which use owner draw
// style
//
void
TControl::ODASelect(DRAWITEMSTRUCT & drawInfo)
{
  TRACEX(OwlControl, 1, "TControl::ODASelect() @" << (void*)this);
  if(drawInfo.itemData){
    TDrawItem* item = ItemData2DrawItem(drawInfo.itemData);
    if(item)
      item->Draw(drawInfo);
  }
}

#endif
#if !defined(SECTION) || SECTION == 6

IMPLEMENT_STREAMABLE1(TControl, TWindow);

#if !defined(BI_NO_OBJ_STREAMING)

//
//
//
void*
TControl::Streamer::Read(ipstream& is, uint32 version) const
{
  TControl* o = GetObject();
  ReadBaseObject((TWindow*)o, is);

  if (version > 1) {
    int nu;
    is >> nu;
    o->NativeUse = TNativeUse(nu);  //!CQ maybe recalc dynamically & not save?
  }

  return o;
}

//
//
//
void
TControl::Streamer::Write(opstream& os) const
{
  TControl* o = GetObject();
  WriteBaseObject((TWindow*)o, os);
  os << o->NativeUse;  // version 2
}
#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE

