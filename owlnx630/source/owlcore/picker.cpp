//----------------------------------------------------------------------------//
// ObjectWindows, Copyright (c) 1998 by Yura Bidus.                           //
//                                                                            //
//$Revision: 1.29 $                                                             //
//$Author: jogybl $                                                             //
//$Date: 2009-08-05 10:32:03 $                                                     //
//                                                                            //
// THIS CLASS AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF         //
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO        //
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A             //
// PARTICULAR PURPOSE.                                                        //
//                                                                            //
// Implementation of class TPickerCell, TColorCell, TBitmapCell, TTextCell,   //
//                         TGridPicker, TColorPicker and TColorPickerGadget   //
//------------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>

#include <owl/uihelper.h>
#include <owl/celarray.h>
#include <owl/tooltip.h>
#include <owl/gadgetwi.h>
#include <owl/chooseco.h>
#include <owl/uimetric.h>
#include <owl/picker.h>
#if !defined(OWL_TEMPLATE_H)
#include <owl/template.h>
#endif
#if defined(BI_MULTI_THREAD_RTL)
#include <owl/thread.h>
#endif

#define ID_STATITEM    0
#define ID_CUSTITEM    1
#define ID_DEFITEM    2
#define ID_DELTA      3
#define ID_MINITEM    2

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//------------------------------------------------------------------------------
//
// class TPickerCellArray
// ~~~~~ ~~~~~~~~~~~~~~~~
//
class TPickerCellArray: public TIPtrArray<TPickerCell*>{
  public:
    TPickerCellArray(){}
};

//------------------------------------------------------------------------------
//
// class TDefTextCell
// ~~~~~ ~~~~~~~~~~~~
//
class TDefTextCell: public TTextCell {
  public:
    TDefTextCell(int id=0, LPCTSTR text=_T(""),TPickerCell* cell=0);
    virtual  ~TDefTextCell();

    virtual void   PaintCell(TDC& dc, TRect& rect);
    void            SetCell(TPickerCell* cell) {  delete Cell; Cell = cell; }
    TPickerCell*   GetCell()                  { return Cell;               }

  protected:
    TPickerCell*   Cell;
};
//------------------------------------------------------------------------------
// class TLocalTipCell
// ~~~~~ ~~~~~~~~~~~~~
//
class TLocalTipCell: public TTextCell {
  public:
    TLocalTipCell(int id=0, LPCTSTR text=_T(""));

    virtual void   PaintCell(TDC& dc, TRect& rect);
};
//------------------------------------------------------------------------------


#if !defined(SECTION) || SECTION == 1
//------------------------------------------------------------------------------
//
// class TPickerCell
// ~~~~~~~~~~~~~~~~~
//
//
// Set the border style used by this gadget. Internal Border members are
// updated and owning Window is notified of a size change.
//
void
TPickerCell::SetBorderStyle(TBorderStyle borderStyle)
{
  BorderStyle = borderStyle;
  if (Parent)
    Parent->Layout();
}

//
void
TPickerCell::Paint(TDC& dc, TRect* r, bool drawbrd)
{
  TRect rect(r ? *r : TRect(TPoint(0,0),Bounds.Size()));

  if(!Parent->IsFlatStyle())
    rect.Inflate(-1,-1);

  if(drawbrd)
    PaintBorder(dc, rect);

  if(!Parent->IsFlatStyle())
    PaintCell(dc, rect.Inflate(-2,-2));
  else
    PaintCell(dc, rect.Inflate(-4,-4));
}

//
void
TPickerCell::PaintBorder(TDC& dc, TRect& rect)
{
  if (BorderStyle == None)
    return;

  if(BorderStyle == Flat){
    if(!Parent->IsFlatStyle()){
      uint flag  = TUIBorder::Soft|TUIBorder::Rect;
      uint style = TUIBorder::WndRecessed;

      if(IsSet(csMouseIn))
        style  = TUIBorder::Embossed;
      if(IsSet(csSelected))
        flag |= TUIBorder::Mono;

      TUIBorder(rect, TUIBorder::TStyle(style),TUIBorder::TFlag(flag)).Paint(dc);
    }
    else{
      if(IsSet(csSelected)){
        TUIBorder(rect, TUIBorder::Recessed).Paint(dc);
        if(!IsSet(csMouseIn)){
#if 1   //  Y.B. Excell Style
          dc.FillRect(rect.InflatedBy(-1, -1), TUIFace::GetDitherBrush());
#else   // Y.B. Dieter Windau style. What better ??????
          dc.FillRect(rect.InflatedBy(-2, -2), TUIFace::GetDitherBrush());
#endif
        }
      }
      else if(IsSet(csMouseIn))
        TUIBorder(rect, TUIBorder::RaisedInner, TUIBorder::Adjust|TUIBorder::Rect).Paint(dc);
      else{
        TBrush br(TColor::Sys3dFace);
        dc.FrameRect(rect, br);
      }
      TBrush shbr(TColor::Sys3dShadow);
      dc.FrameRect(rect.InflatedBy(-3, -3), shbr);
    }
  }
  else{
    if (BorderStyle == Plain) {
      TBrush winBru(TColor::SysWindowFrame);
      dc.OWLFastWindowFrame(winBru, rect,TUIMetric::CxBorder, TUIMetric::CyBorder);
      dc.RestoreBrush();
    }
    else
      TUIBorder(rect, TUIBorder::TStyle(BorderStyle)).Paint(dc);
  }
}

//
void
TPickerCell::QueryBounds(TRect& rect)
{
   TSize size = TSize(13,13);
  if(rect.Width() < size.cx)
    rect.right = rect.left + size.cx;
  if(rect.Height() < size.cy)
    rect.bottom = rect.top + size.cy;
}

//
void
TPickerCell::EnterMouse()
{
  Set(csMouseIn);
}

//
void
TPickerCell::LeaveMouse()
{
  Clear(csMouseIn);
}

//
void
TPickerCell::Select(bool sel)
{
  if(sel)
    Set(csSelected);
  else
    Clear(csSelected);
}
#endif
#if !defined(SECTION) || SECTION == 2

//------------------------------------------------------------------------------
// class TColorCell
// ~~~~~ ~~~~~~~~~~
//
void
TColorCell::PaintCell(TDC& dc, TRect& rect)
{
  TColor clr2 = dc.SetBkColor(Color);
  dc.ExtTextOut(0,0, ETO_OPAQUE, &rect, 0, 0, 0);
  dc.SetBkColor(clr2);
}

//------------------------------------------------------------------------------
// class TBitmapCell
// ~~~~~ ~~~~~~~~~~~
TBitmapCell::TBitmapCell(int id, TCelArray* array, int index,TBorderStyle style)
:
  TPickerCell(id,style),
  CelArray(array),
  Index(index),
  ShouldDelete(false)
{
}

//
TBitmapCell::~TBitmapCell()
{
  if(ShouldDelete)
    delete CelArray;
}

//
void
TBitmapCell::SetCelArray(TCelArray* array, TAutoDelete del)
{
  if(ShouldDelete)
    delete CelArray;
  CelArray = array;
  ShouldDelete = del==AutoDelete;
}

//
void
TBitmapCell::PaintCell(TDC& dc, TRect& rect)
{
  if(CelArray){
    if(rect.Size() == CelArray->CelSize())
      CelArray->BitBlt(Index, dc, rect.left, rect.top);
    else
      CelArray->StretchBlt(Index, dc, rect);
  }
}

//
void
TBitmapCell::QueryBounds(TRect& rect)
{
  TSize size;
  if(CelArray){
    if(Parent->IsFlatStyle())
     size = CelArray->CelSize()+TSize(8,8);
    else
     size = CelArray->CelSize()+TSize(6,6);
  }
  else{
    if(Parent->IsFlatStyle())
      size = TSize(13,13);
    else
      size = TSize(9,9);
  }
  if(rect.Width() < size.cx)
    rect.right = rect.left + size.cx;
  if(rect.Height() < size.cy)
    rect.bottom = rect.top + size.cy;
}
#endif
#if !defined(SECTION) || SECTION == 3
//------------------------------------------------------------------------------
//
// class TTextCell
// ~~~~~ ~~~~~~~~~
void
TTextCell::PaintCell(TDC& dc, TRect& rect)
{
  if(!Parent->IsFlatStyle())
    rect.Inflate(-1, -1);

  TFont* font = Parent->GetFont();
  if(font){
    CHECK(font->IsGDIObject());
    dc.SelectObject(*font);
  }

  int mode = dc.SetBkMode(TRANSPARENT);
  dc.DrawTextEx((LPTSTR)Text.c_str(), -1, &rect, Format);
  dc.SetBkMode(mode);

  if(font)
    dc.RestoreFont();
}
//------------------------------------------------------------------------------
//
// class TDefTextCell
// ~~~~~ ~~~~~~~~~~~~
//
//
TDefTextCell::TDefTextCell(int id, LPCTSTR text,TPickerCell* cell)
:
  TTextCell(id,text),
  Cell(cell)
{
}

//
TDefTextCell::~TDefTextCell()
{
  delete Cell;
}

//
void
TDefTextCell::PaintCell(TDC& dc, TRect& rect)
{
  if(!Parent->IsFlatStyle())
    rect.Inflate(-1, -1);

  if(Cell){
    int width = Bounds.Height()-6;
    TRect rc(rect.left-2, rect.top-2, rect.left+width, rect.top+width);
    if(!Parent->IsFlatStyle())
      rc.Inflate(1, 1);
    Cell->Paint(dc, &rc, false);
    if(Parent->IsFlatStyle())
      dc.FrameRect(rc.InflatedBy(-3,-3), TBrush(TColor::Sys3dShadow));
    rect.left += rc.Width()-5;
  }
   rect.Inflate(1,1);
  TTextCell::PaintCell(dc, rect);
}

//------------------------------------------------------------------------------
// class TLocalTipCell
// ~~~~~ ~~~~~~~~~~~~~
//
TLocalTipCell::TLocalTipCell(int id, LPCTSTR text)
:
  TTextCell(id,text, None)
{
  Format = DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOCLIP;
}

//
void
TLocalTipCell::PaintCell(TDC& dc, TRect&)
{
  TRect rect(TPoint(0,0),Bounds.Size());
  rect.Inflate(-2,-2);

  TColor clr;
  if(!Parent->IsFlatStyle())
    clr = dc.SetBkColor(TColor::White);

  dc.ExtTextOut(rect.TopLeft(), ETO_OPAQUE, &rect,0,0);

  if(!Parent->IsFlatStyle())
    dc.SetBkColor(clr);

  TTextCell::PaintCell(dc, rect);

  TUIBorder(rect.InflatedBy(2,2), Parent->IsFlatStyle()?TUIBorder::Recessed:
            TUIBorder::WndRecessed).Paint(dc);
}
//------------------------------------------------------------------------------
//
// Response Table
//
DEFINE_RESPONSE_TABLE1(TGridPicker, TControl)
  //EV_WM_ACTIVATEAPP,
  EV_WM_SETFOCUS,
  EV_WM_KILLFOCUS,
  //EV_WM_GETDLGCODE,
  EV_WM_LBUTTONDOWN,
  EV_WM_LBUTTONDBLCLK,
  EV_WM_LBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_WM_KEYDOWN,
  EV_WM_KEYUP,
  EV_WM_CANCELMODE,
END_RESPONSE_TABLE;

TGridPicker::TGridPicker(TWindow* parent, int id, int x, int y, int w, int h,
                           TModule* module)
:
  TControl(parent, id, 0, x, y, w, h, module)
{
  InitPicker();
}

//
TGridPicker::TGridPicker(TWindow* parent, int resourceId, TModule* module)
:
  TControl(parent, resourceId, module)
{
  InitPicker();
}

//
void
TGridPicker::InitPicker()
{
  Selected     = -1;
  Focused     = -1;
  NRows        = 0;
  NColumns    = 4;
//  TextHeight  = 0;
  Margins     = TMargins(TMargins::Pixels, 4, 4, 4, 4);
  Font        = 0;
  Target      = 0;
  Tooltip      = 0;
  Cells       = new  TPickerCellArray;


  ModifyExStyle(0,WS_EX_WINDOWEDGE);

  SetBkgndColor(TColor::Sys3dFace);

  // Create the font
  NONCLIENTMETRICS ncm;
  ncm.cbSize = sizeof(NONCLIENTMETRICS);
  SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
  Font = new TFont(&ncm.lfMessageFont);

  TEXTMETRIC  metrics;
  Font->GetTextMetrics(metrics);
  TextHeight = __max(metrics.tmHeight + metrics.tmExternalLeading, 20l);


  Add(new TLocalTipCell(ID_STATITEM));  // == 0
  Add(new TDefTextCell(ID_CUSTITEM));   // == 1
  Add(new TDefTextCell(ID_DEFITEM));    // == 2
}

TGridPicker::~TGridPicker()
{
  delete Font;
  delete Cells;
}

//
void
TGridPicker::SetSelection(int loc)
{
  loc += ID_DELTA;
  if(loc >= (int)Cells->Size() || loc < ID_MINITEM){
    if(loc == ID_CUSTITEM){
      Set(ppButtonSel);
      (*Cells)[ID_CUSTITEM]->Select(true);
      if(GetHandle())
        InvalidateRect((*Cells)[ID_CUSTITEM]->Bounds,true);
    }
    return;
  }
  if(IsSet(ppButtonSel)){
    Clear(ppButtonSel);
    (*Cells)[ID_CUSTITEM]->Select(false);
    if(GetHandle())
      InvalidateRect((*Cells)[ID_CUSTITEM]->Bounds,true);
  }

  if(Selected >= 0){
    (*Cells)[Selected]->Select(false);
    if(GetHandle())
      InvalidateRect((*Cells)[Selected]->Bounds,true);
  }
  Selected   = loc;
  (*Cells)[Selected]->Select(true);
  if(GetHandle()){
    InvalidateRect((*Cells)[Selected]->Bounds,true);
  }
}
//
void
TGridPicker::ApplyChanges()
{
  if(GetHandle()){
    Layout();
    Invalidate();
  }
}

//
void
TGridPicker::SetColumns(int colmn)
{
  NColumns  = colmn;
  NRows  = (Cells->Size()-ID_DELTA)/NColumns;
  if((Cells->Size()-ID_DELTA)%NColumns)
    NRows++;

  ApplyChanges();
}

//
void
TGridPicker::SetMargins(TMargins& margins)
{
  Margins = margins;
  ApplyChanges();
}

//
void
TGridPicker::SetFont(const TFont& font)
{
  delete Font;
  Font  = new TFont(font);
}

//
void
TGridPicker::EnableDefault(LPCTSTR text, int resId, TPickerCell* cl)
{
  TDefTextCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_DEFITEM], TDefTextCell);
  if(text){
    Set(ppShowDefItem);
    if(cell){
      cell->SetText(text);
      cell->SetId(resId);
      if(cl)
        cl->Parent  = this;
      cell->SetCell(cl);
    }
  }
  else{
    Clear(ppShowDefItem);
    if(cell)
      cell->Bounds.SetEmpty();
  }

  ApplyChanges();
}

//
void
TGridPicker::EnableButton(LPCTSTR text, int resId, TPickerCell* cl)
{
  TDefTextCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_CUSTITEM], TDefTextCell);
  if(text){
    Set(ppShowCustItem);
    if(cell){
      cell->SetText(text);
      cell->SetId(resId);
      if(cl)
        cl->Parent  = this;
      cell->SetCell(cl);
    }
  }
  else{
    Clear(ppShowCustItem);
    if(cell)
      cell->Bounds.SetEmpty();
  }

  ApplyChanges();
}

//
void
TGridPicker::ModifyPopStyle(uint off, uint on)
{
  if(on)
    Set(on);
  if(off)
    Clear(off);

  if(off & psLocalTips){
    TLocalTipCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_STATITEM], TLocalTipCell);
    if(cell)
      cell->Bounds.SetEmpty();
  }
  ApplyChanges();
}

//
int
TGridPicker::Add(TPickerCell* cell)
{
  cell->Parent = this;
  return Cells->Add(cell);
}

//
TPickerCell*
TGridPicker::Remove(int loc)
{
  if(loc >= (int)Cells->Size())
    return 0;

  TPickerCell* cell = (*Cells)[loc];
  Cells->Detach(loc);

  return cell;
}

//
TPickerCell*
TGridPicker::GetCell(int loc)
{
  if(loc < (int)Cells->Size())
    return (*Cells)[loc];
  return 0;
}

//
uint
TGridPicker::Size()
{
  return Cells->Size();
}

//
void
TGridPicker::AddCell(TPickerCell* cell, bool select)
{
  int idx = Add(cell);
  if(select)
    SetSelection(idx-ID_DELTA);
  SetColumns(NColumns);
}

//
TPickerCell*
TGridPicker::RemoveCell(int loc)
{
  loc += ID_DELTA;
  return Remove(loc);
}

//
TPickerCell*
TGridPicker::GetDefaultCell()
{
  TDefTextCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_DEFITEM], TDefTextCell);
  if(!cell)
    return 0;
  return cell->GetCell();
}

//
// return child of default custom cell
//
TPickerCell*
TGridPicker::GetCustomCell()
{
  TDefTextCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_CUSTITEM], TDefTextCell);
  if(!cell)
    return 0;
  return cell->GetCell();
}

//
LPCTSTR
TGridPicker::GetClassName()
{
  return OWL_PICKER;
}

//
void
TGridPicker::GetWindowClass(WNDCLASS& wndClass)
{
  TControl::GetWindowClass(wndClass);
  wndClass.style = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
}

//
void
TGridPicker::SetupWindow()
{
  TControl::SetupWindow();

  Layout();

  if (Tooltip && !Tooltip->GetHandle()){
    // Make sure tooltip is disabled so it does not take input focus
    Tooltip->ModifyStyle(0,WS_DISABLED);
    Tooltip->Create();
  }

  if(Tooltip){
    owl_string text;
    for (int i = 0; i < (int)Cells->Size(); i++) {
      if((*Cells)[i]->GetId()){
        text = LoadTipText((*Cells)[i]->GetId());
        TToolInfo ti (*this, (*Cells)[i]->GetBounds(), (*Cells)[i]->GetId(),
#if !defined(BI_STD_STRING)
        text.is_null() ? LPSTR_TEXTCALLBACK : text.c_str());
#else
        text.empty() ? LPSTR_TEXTCALLBACK : text.c_str());
#endif
        Tooltip->AddTool(ti);
      }
    }
  }
}
//
void       
TGridPicker::EnableTooltip(bool enable)
{
  if (!Tooltip) {
    // Create and initialize tooltip
    //
    SetTooltip(new TTooltip(this));
  }
  else {
    if (Tooltip->GetHandle())
      Tooltip->Activate(enable);
  }
}
//
void      
TGridPicker::SetTooltip(TTooltip* tooltip)
{
  // Cleanup; via Condemned list if tooltip was created
  //
  if (Tooltip) {
    if (Tooltip->GetHandle())
      Tooltip->SendMessage(WM_CLOSE);
    else
      delete Tooltip;
  }

  // Store new tooltip and create if necessary
  //
  Tooltip = tooltip;
  if (Tooltip) {
    if(GetHandle() && !Tooltip->GetHandle()) {
      // Make sure tooltip is disabled so it does not take input focus
      Tooltip->ModifyStyle(0,WS_DISABLED);
      Tooltip->Create();
    }
  }
}

//
// Relay 'interesting' messages to the tooltip window
//
bool
TGridPicker::PreProcessMsg(MSG& msg)
{
  // Check if this message might be worth relaying to the tooltip window
  //
  if (Tooltip && Tooltip->IsWindow()) {
    if (msg.hwnd == *this || IsChild(msg.hwnd)) {
      if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
        Tooltip->RelayEvent(msg);
      }
    }
  }

  // Always let event go through.
  //
  return TControl::PreProcessMsg(msg);
}

//
//
const int CellSpacing = 2;

//
void
TGridPicker::Layout()
{
  if(!GetHandle())
    return;

  TRect rect;
  GetClientRect(rect);

  // 1. Check Max size
  TRect crect;
  GetCellRect(crect);
  if(crect.IsEmpty())
    crect = TRect(TPoint(0,0),TSize(rect.Width()/NColumns,rect.Height()/NRows));

  TSize size(crect.Width()*NColumns,crect.Height()*NRows);
  int addHeight = 0;
  if(IsSet(ppShowDefItem))
    addHeight += TextHeight+CellSpacing;
  if(IsSet(ppShowCustItem))
    addHeight += TextHeight+CellSpacing*3;
  if(IsSet(ppShowTipsCell))
    addHeight += TextHeight+CellSpacing*3;

  size.cy += addHeight;

  int leftM, rightM, topM, bottomM;
  Margins.GetPixels(leftM, rightM, topM, bottomM, TextHeight);
  crect = TRect(TPoint(0,0),TSize(size.cx+leftM+rightM,size.cy+topM+bottomM));

  bool move = false;
  if(crect.Width() > rect.Width()){
    rect.right = rect.left + crect.Width();
    move = true;
  }
  if(crect.Height() > rect.Height()){
    rect.bottom = rect.top + crect.Height();
    move = true;
  }

  // 2. Reposition Window
  if(move)
    SetWindowPos(HWND_TOPMOST, rect, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);

  int w = (rect.Width()-leftM-rightM )/NColumns;
  int h = (rect.Height()-addHeight-topM-bottomM)/NRows;

  // 3. Find max rectangle
  TRect cellRect;
  if(IsSet(ppShrCells))
    GetCellRect(cellRect);
  if(cellRect.IsEmpty())
    cellRect = TRect(0,0, w,h);
   cellRect.Offset(leftM,topM);

  addHeight = 0;
  if(IsSet(ppShowDefItem)){
    (*Cells)[ID_DEFITEM]->Bounds =
                       TRect(TPoint(leftM,topM),
                             TSize(rect.Width()-leftM-rightM,TextHeight));
    cellRect.Offset(0,TextHeight+CellSpacing);
    addHeight +=TextHeight+CellSpacing;
  }

  if(IsSet(ppShowCustItem)){
    addHeight += CellSpacing*3;
    (*Cells)[ID_CUSTITEM]->Bounds =
                        TRect(TPoint(leftM,h*NRows+topM+addHeight),
                              TSize(rect.Width()-leftM-rightM,TextHeight));
    addHeight +=TextHeight;
  }

  if(IsSet(ppShowTipsCell)){
    addHeight += CellSpacing*3;
    (*Cells)[ID_STATITEM]->Bounds =
                        TRect(TPoint(leftM,h*NRows+topM+addHeight),
                              TSize(rect.Width()-leftM-rightM,TextHeight));
  }

  // 4. Layout Cells
  rect = cellRect;
  for(int i = 0; i < NRows; i++){
    for(int j = 0; j < NColumns; j++){
      int cell  =  i*NColumns+j + ID_DELTA;
      if(cell < (int)Cells->Size()){
        (*Cells)[cell]->Bounds = cellRect;
        cellRect.Offset(w,0);
      }
    }
    cellRect = rect.Offset(0,cellRect.Height());
  }
}

//
int
TGridPicker::Contains(const TPoint& point)
{
  for(int i = 0; i < (int)Cells->Size(); i++){
    if((*Cells)[i]->PtIn(point))
      return i;
  }
  return (int)NPOS;
}

//
void
TGridPicker::GetCellRect(TRect& cellRect)
{
  for(int i = ID_DELTA; i < (int)Cells->Size(); i++){
    TRect rec;
    (*Cells)[i]->QueryBounds(rec);
    cellRect |= rec;
  }
}

//
void
TGridPicker::Paint(TDC& dc, bool /*erase*/, TRect& rect)
{
  TPoint viewport = dc.GetViewportOrg();
  for(int i = 0; i < (int)Cells->Size(); i++){
    if ((*Cells)[i]->GetBounds().Touches(rect)){
      dc.SetViewportOrg((TPoint&)(*Cells)[i]->GetBounds());
      (*Cells)[i]->Paint(dc);
    }
  }
  dc.SetViewportOrg(viewport);

  if(IsSet(ppShowCustItem)){
     TRect tr((*Cells)[ID_CUSTITEM]->GetBounds());
     PaintDivider(dc, tr);
  }

  if(IsSet(ppShowTipsCell)){
     TRect tr((*Cells)[ID_STATITEM]->GetBounds());
     PaintDivider(dc, tr);
  }
}

//
void
TGridPicker::PaintDivider(TDC& dc, TRect& rect)
{
  if(IsFlatStyle())
    rect.Inflate(-1, -1);
  rect.Offset(0,-(CellSpacing*3)-1);
  rect.top     += CellSpacing;
  rect.bottom = rect.top + CellSpacing;
  TUIBorder(rect, TUIBorder::Recessed, TUIBorder::Adjust|TUIBorder::Rect).Paint(dc);
}

//
bool
TGridPicker::CallCustomBox()
{
  return false;
}

//
void
TGridPicker::ExitPicker(bool /*status*/)
{
}

//
void
TGridPicker::RefocusCell(int loc)
{
  if(Focused >= 0){
    (*Cells)[Focused]->LeaveMouse();
    InvalidateRect((*Cells)[Focused]->Bounds,true);
  }
  Focused = loc;
  (*Cells)[loc]->EnterMouse();
  InvalidateRect((*Cells)[loc]->Bounds,true);
  // set text cell string
  if(IsSet(ppShowTipsCell)){
    TLocalTipCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_STATITEM], TLocalTipCell);
    if(cell && (*Cells)[loc]->GetId()){
      cell->SetText(LoadTipText((*Cells)[loc]->GetId()).c_str());
      InvalidateRect(cell->Bounds,true);
    }
  }
  // Set/reset message box string
  if(IsSet(ppMessageTips)){
    if(loc > 0 && (*Cells)[loc]->GetId())
      SetStatusBarMessage((*Cells)[loc]->GetId());
    else
      SetStatusBarMessage(-1);
  }
  if(GetStyle() & BS_NOTIFY)
    NotifyParent(BN_HILITE);
}

//
//
void
TGridPicker::NotifyParent(uint code)
{
  if(Target)
    SendNotification(*Target, GetDlgCtrlID(), code, *this);
  else
    SendNotification(::GetParent(*this), GetDlgCtrlID(), code, *this);
}

//
// WM_SETFOCUS handler
//
void
TGridPicker::EvSetFocus(THandle hWndLostFocus)
{
  TControl::EvSetFocus(hWndLostFocus);

  Set(ppFocus);
  Invalidate(true);
}

//
// WM_KILLFOCUS handler
//
void
TGridPicker::EvKillFocus(THandle hWndGetFocus)
{
  TControl::EvKillFocus(hWndGetFocus);

  Clear(ppFocus);
  ClearCell();
  if (IsSet(ppPushed))
    ClearCapture();
}

//
// WM_LBUTTONDOWN handler. Grab focus and update button's state to be in
// 'pushed' mode.
//
void
TGridPicker::EvLButtonDown(uint modKeys, TPoint& point)
{
  TControl::EvLButtonDown(modKeys, point);

  SetCapture();
  if(!IsSet(ppFocus))
    SetFocus();
  int loc = Contains(point);
  if(loc != (int)NPOS){
    SetSelection(loc-ID_DELTA);
    Set(ppPushed);
  }
}


//
// WM_LBUTTONDBLCLK handler. Simply forward to LBUTTONDOWN handler
//
void
TGridPicker::EvLButtonDblClk(uint modKeys, TPoint& point)
{
  EvLButtonDown(modKeys, point);
}

//
// WM_LBUTTONUP handler. Restore state of button and notify parent with a
// CLICKED message if necessary.
//
void
TGridPicker::EvLButtonUp(uint /*modKeys*/, TPoint& point)
{
  if (GetCapture() == *this) {
    ReleaseCapture();

    if(IsSet(ppButtonSel)){
      Clear(ppButtonSel);
      (*Cells)[ID_CUSTITEM]->Select(false);
      InvalidateRect((*Cells)[ID_CUSTITEM]->Bounds,true);

      Set(ppCustomOpen);
      if(!CallCustomBox()){
        Clear(ppCustomOpen);
        return;
      }
      Clear(ppCustomOpen);

      if(Selected >= 0)
        (*Cells)[Selected]->Select(false);
      Selected = ID_CUSTITEM;
      (*Cells)[ID_CUSTITEM]->Select(true);

      NotifyParent(BN_CLICKED);
      ExitPicker(true);
      return;
    }
    TRect rect;
    GetClientRect(rect);
    if (rect.Contains(point) && Contains(point) >= ID_MINITEM){
      NotifyParent(BN_CLICKED);
      ExitPicker(true);
    }
  }
}

//
// WM_MOUSEMOVE handler. Update state of button if we're in 'capture' mode.
//
void
TGridPicker::EvMouseMove(uint modKeys, TPoint& point)
{
  if(IsSet(ppFocus) && IsSet(ppTrackMouse)){
    TRect rect;
    GetClientRect(rect);

    int loc;
    if (!rect.Contains(point) || (loc = Contains(point))==(int)NPOS)
      ClearCell();
    else if(Focused != loc){
      RefocusCell(loc);

       if (modKeys & MK_LBUTTON && GetCapture() == *this)
        SetSelection(loc-ID_DELTA);
    }
  }
}

//
// WM_KEYDOWN handler. Update state of button upon detecting that user
// pressed the space bar.
//
void
TGridPicker::EvKeyDown(uint key, uint repeatCount, uint flags)
{
  TControl::EvKeyDown(key, repeatCount, flags);

  switch (key){
    case VK_ESCAPE:
      ExitPicker(false);
    break;

    case VK_RETURN:
    case VK_SPACE:
      Set(ppPushed);
      if(Focused < 0)
        Focused = 0;
      break;

    case VK_LEFT:{
        int loc = Focused-1;
        if(Focused == -1 || Focused == ID_DEFITEM)
          loc = IsSet(ppShowCustItem) ? ID_CUSTITEM : Cells->Size()-1;
        else if(Focused == ID_CUSTITEM)
          loc = Cells->Size()-1;
        else if(loc < ID_DELTA && !IsSet(ppShowDefItem))
          loc = IsSet(ppShowCustItem) ? ID_CUSTITEM : Cells->Size()-1;
        else if( loc < ID_DELTA && IsSet(ppShowDefItem))
          loc = ID_DEFITEM;
        RefocusCell(loc);
      }
      break;

    case VK_RIGHT: {
        int loc = Focused+1;
        int maxcell = Cells->Size()-1;
        if(Focused == -1 || Focused == ID_CUSTITEM || 
           (loc > maxcell && !IsSet(ppShowCustItem)))
          loc = IsSet(ppShowDefItem) ? ID_DEFITEM : (ID_MINITEM+1);
        else if(loc > maxcell && IsSet(ppShowCustItem))
          loc = ID_CUSTITEM;
        RefocusCell(loc);
      }
      break;

    case VK_DOWN: {
        int loc = Focused+NColumns;
        int maxcell = Cells->Size()-1;
        if(Focused == -1)
          loc = IsSet(ppShowDefItem) ? ID_DEFITEM : (ID_MINITEM+1);
        else if(Focused == ID_DEFITEM)
          loc = ID_MINITEM+1;
        else if(Focused == ID_CUSTITEM && IsSet(ppShowDefItem))
          loc = ID_DEFITEM;
        else if(loc >= maxcell && IsSet(ppShowCustItem)){
          loc = ID_CUSTITEM;
        }
        else if(loc >= maxcell && IsSet(ppShowDefItem)){
          loc = ID_DEFITEM;
        }
        else{
          if(loc > maxcell)
            loc -= NRows*NColumns;
          if(loc < ID_DELTA)
            loc += NRows;
        }
        RefocusCell(loc);
      }
      break;

    case VK_UP:{
        int loc = Focused-NColumns;
        int maxcell = Cells->Size()-1;
        if(Focused == -1 || Focused == ID_DEFITEM)
          loc = IsSet(ppShowCustItem) ? ID_CUSTITEM : maxcell;
        else if(Focused == ID_CUSTITEM)
          loc = (maxcell-ID_DELTA)/NColumns*NColumns+ID_DELTA;
        else if(loc < ID_DELTA && IsSet(ppShowDefItem))
          loc = ID_DEFITEM;
        else if(loc < ID_DELTA)
          loc += NRows*NColumns;
        RefocusCell(loc);
      }
      break;

    case VK_HOME:
      RefocusCell(IsSet(ppShowDefItem) ? ID_DEFITEM : (ID_MINITEM+1));
      break;

    case VK_END:
      RefocusCell(IsSet(ppShowCustItem) ? ID_CUSTITEM : Cells->Size()-1);
      break;
  }
}

//
// WM_KEYUP handler. Restore state of button and notify parent
//
void
TGridPicker::EvKeyUp(uint key, uint /*repeatCount*/, uint /*flags*/)
{
  if (IsSet(ppPushed) && (key == VK_SPACE || key == VK_RETURN)){
    SetSelection(Focused-ID_DELTA);
    Clear(ppPushed);
    if(IsSet(ppButtonSel)){
      Clear(ppButtonSel);
      (*Cells)[ID_CUSTITEM]->Select(false);
      InvalidateRect((*Cells)[ID_CUSTITEM]->Bounds,true);

      Set(ppCustomOpen);
      if(!CallCustomBox()){
        Clear(ppCustomOpen);
        NotifyParent(BN_CLICKED);
        ExitPicker(true);
        return;
      }
      Clear(ppCustomOpen);

      if(Selected >= 0)
        (*Cells)[Selected]->Select(false);
      Selected = ID_CUSTITEM;
      (*Cells)[ID_CUSTITEM]->Select(true);
    }
    else{
      NotifyParent(BN_CLICKED);
      ExitPicker(true);
    }
  }
}

//
//
//
void
TGridPicker::EvCancelMode()
{
  ClearCell();
  ClearCapture();
}

//
//
//
void
TGridPicker::ClearCell()
{
   if(Focused >= 0){
     (*Cells)[Focused]->LeaveMouse();
    InvalidateRect((*Cells)[Focused]->GetBounds(),true);
    Focused = -1;
  }
   if(IsSet(ppButtonSel)){
    Clear(ppButtonSel);
    (*Cells)[ID_CUSTITEM]->Select(false);
    InvalidateRect((*Cells)[ID_CUSTITEM]->Bounds,true);
  }
  if(IsSet(ppMessageTips))
     SetStatusBarMessage(-1);
  if(IsSet(ppShowTipsCell)){
    TLocalTipCell* cell = TYPESAFE_DOWNCAST((*Cells)[ID_STATITEM], TLocalTipCell);
    if(cell){
      cell->SetText(_T(""));
      InvalidateRect(cell->GetBounds(),true);
    }
  }
}

//
// Release caption if we are in 'capture' mode. Reset internal flags
// appropriately.
//
void
TGridPicker::ClearCapture()
{
  if (GetCapture() == *this)
    ReleaseCapture();
  Clear(ppPushed);
}

//
// ??????????????????????????????
// Maeby better to use TEnabler mechanism
//
owl_string
TGridPicker::LoadTipText(int resId)
{
  _TCHAR tempStr[256];
  int len = GetModule()->LoadString(resId, tempStr, sizeof(tempStr));

  if(len == 0){
    TModule* module = TModule::FindResModule(resId/16+1, RT_STRING);
    if(module)
     len = module->LoadString(resId, tempStr, COUNTOF(tempStr));
  }

  if (len > 0){
    _TCHAR* ptr = _tcschr(tempStr, _T('\n'));
    if (ptr)
      return owl_string(ptr+1);
     else{

      TWindow* parent = GetParentO();
      TDecoratedFrame* frame= parent ? TYPESAFE_DOWNCAST(parent, TDecoratedFrame) : 0;
      while (parent && !frame){
        parent = parent->GetParentO();
        if (parent)
          frame = TYPESAFE_DOWNCAST(parent, TDecoratedFrame);
      }

      tempStr[0] = _T('\0');
      if (frame)
        frame->GetHintText(resId, tempStr, COUNTOF(tempStr), htTooltip);
      return owl_string(tempStr);
    }
  }
  return owl_string(_T(""));
}

//
// We display a message on the status bar.
//
void
TGridPicker::SetStatusBarMessage(int id)
{
  TWindow* parent= GetParentO();
  TDecoratedFrame* frame= parent ? TYPESAFE_DOWNCAST(parent, TDecoratedFrame) : 0;
  while (parent && !frame){
    parent = parent->GetParentO();
    if (parent)
      frame = TYPESAFE_DOWNCAST(parent, TDecoratedFrame);
  }
  if (frame){
    if (id > 0)
      frame->HandleMessage(WM_MENUSELECT, id, 0);
    else
      frame->HandleMessage(WM_MENUSELECT, 0xFFFF0000, 0);
    frame->HandleMessage(WM_ENTERIDLE, MSGF_MENU);
  }
}
#endif
#if !defined(SECTION) || SECTION == 4

//------------------------------------------------------------------------------
// class TPopupPicker
// ~~~~~ ~~~~~~~~~~~~
//
DEFINE_RESPONSE_TABLE1(TPopupPicker, TGridPicker)
  EV_WM_KILLFOCUS,
  EV_WM_ACTIVATEAPP,
END_RESPONSE_TABLE;

//
TPopupPicker::TPopupPicker(TWindow* parent, int id, int x, int y, int w, int h,
             TModule* module)
:
  TGridPicker(parent, id, x, y, w, h, module)
{
  ModifyStyle(WS_VISIBLE|WS_CHILD,WS_POPUP);
  ModifyExStyle(0, WS_EX_TOPMOST);
}

//
TPopupPicker::TPopupPicker(TWindow* parent, int resourceId, TModule* module)
:
  TGridPicker(parent, resourceId, module)
{
}

//
void
TPopupPicker::GetWindowClass(WNDCLASS& wndClass)
{
  TGridPicker::GetWindowClass(wndClass);
  wndClass.style |= CS_SAVEBITS;
}

//
void
TPopupPicker::Paint(TDC& dc, bool erase, TRect& rect)
{
  TRect crect = GetWindowRect();
  ::MapWindowPoints(HWND_DESKTOP, *this, LPPOINT(&crect), 2);
  TUIBorder(crect, TUIBorder::ButtonUp, TUIBorder::Adjust|TUIBorder::Rect).Paint(dc);

  TGridPicker::Paint(dc, erase, rect);
}


//
// WM_LBUTTONDOWN handler. Grab focus and update button's state to be in
// 'pushed' mode.
//
void
TPopupPicker::EvLButtonDown(uint modKeys, TPoint& point)
{
  TGridPicker::EvLButtonDown(modKeys, point);

  if(!GetClientRect().Contains(point))
    ExitPicker(false);
}

//
void
TPopupPicker::EvKillFocus(THandle hWndGetFocus )
{
  TGridPicker::EvKillFocus(hWndGetFocus);
  ReleaseCapture();

  if (!IsSet(ppCustomOpen))
    ExitPicker(false);
}

// Another application has the focus, so we hide the picker window.
void
TPopupPicker::EvActivateApp(bool active, HTASK hTask)
{
  TGridPicker::EvActivateApp(active, hTask);

  if ((!active) && !IsSet(ppCustomOpen))
    ExitPicker(false);
}

//
void
TPopupPicker::ExitPicker(bool status)
{
  ReleaseCapture();
  ShowWindow(SW_HIDE);
  if(status)
    Set(ppSelected);
  else
    Clear(ppSelected);
}

//
bool
TPopupPicker::ShowPicker(TPoint& p, TRect& rect) // (pt, rect)
{
  TSize   size(Attr.W, Attr.H);
  TSize   checksize(size.cx + 1, size.cy + 1);
  TPoint   point(p);
  TSize   scrSize(TUIMetric::CxScreen,TUIMetric::CyScreen);

  // Check the vertical position.
  //
  if (point.y + size.cy > scrSize.cy) {
    if ((rect.top >= 0) && (rect.bottom >= 0) && (rect.top - checksize.cy >= 0))
      point.y = rect.top - checksize.cy;
    else point.y = scrSize.cy - checksize.cy;
  }

  // Check the horizontal position.
  //
  if (point.x + checksize.cx > scrSize.cx)
    point.x = scrSize.cx - checksize.cx;
  SetWindowPos(HWND_TOPMOST, point.x, point.y, size.cx, size.cy, SWP_SHOWWINDOW);
  SetCapture();

  HWND hWnd = GetHandle();
  while(::IsWindow(hWnd) && ::IsWindowVisible(hWnd))
    GetApplication()->PumpWaitingMessages();

  if(!::IsWindow(hWnd))
    return false;

  bool Ok = IsSet(ppSelected);
  Clear(ppSelected);
  return Ok;
}
#endif
#if !defined(SECTION) || SECTION == 5
//------------------------------------------------------------------------------
//
// class TPickerGadget
// ~~~~~ ~~~~~~~~~~~~~
//
TPickerGadget::TPickerGadget(TPopupPicker&  picker, TResId glyphResIdOrIndex,
  TStyle style, TPopupType poptype, TType type, bool enabled, TState state,
  bool sharedGlyph, uint numChars)
:
  TPopupButtonGadget(picker.Attr.Id, glyphResIdOrIndex, style, poptype, type,
                     enabled, state, sharedGlyph, numChars),
  Picker(&picker),
  Selected(0)
{
  // it is not have style WS_CHILD, so this member should be
  // menu handle or 0;
  picker.Attr.Id = 0;
}

//
//
//
TPickerGadget::~TPickerGadget()
{
  if(Picker)
    Picker->Destroy(0);
  delete Picker;
}

//
//
//
void
TPickerGadget::PopupAction()
{
  // do something
  TRect rect = GetBounds();
  TPoint p2(rect.TopLeft());
  Window->ClientToScreen(p2);
  rect.right = p2.x + rect.Width();
  rect.bottom = p2.y + rect.Height();
  rect.left = p2.x;
  rect.top = p2.y;
  p2.y = rect.bottom;

  // We tell the picker where the picker has to be displayed.
  HWND hWnd = *Picker;
  if(Picker->ShowPicker(p2, rect))
    GetSelection();

  if(!IsWindow(hWnd))
    return;

  // and call inherited class
  //
  TPopupButtonGadget::PopupAction();
   TPoint tp;
   Activate(tp);// send command message to parent ?????????????
}

//
//
//
void
TPickerGadget::Created()
{
  TPopupButtonGadget::Created();
  if (Window->GetHandle() && !Picker->GetHandle()){
    Picker->Create();
    Picker->ShowWindow(SW_HIDE);
  }
  if(Picker->GetHandle())
    Picker->Attr.Id = GetId(); 
}

//
//
//
void
TPickerGadget::Inserted()
{
  TPopupButtonGadget::Inserted();
  Picker->SetParent(Window);
  GetSelection();
}

//
//
//
void
TPickerGadget::GetSelection()
{
  TPickerCell* cell = Picker->GetSelectedCell();
  int sel = Picker->GetSelection();
  if(sel == -1) // default selection
    cell = Picker->GetDefaultCell();

  if(!cell)
    cell = Picker->GetCustomCell();
  if(cell)
    Selected = cell;
}

#endif
#if !defined(SECTION) || SECTION == 6
//------------------------------------------------------------------------------
//
// class TColorPicker
// ~~~~~ ~~~~~~~~~~~~
//
// Gray default custom colors for SelectColor Dlg
struct TCustomColors
#if defined(BI_MULTI_THREAD_RTL)
                : public TLocalObject
#endif
{
  TCustomColors();

  TColor Colors[16];
#if defined(BI_MULTI_THREAD_RTL)
//    TMRSWSection  Lock;
#endif
};

TCustomColors::TCustomColors()
{
  Colors[0] = 0x010101L;
  Colors[1] = 0x101010L;
  Colors[2] = 0x202020L;
  Colors[3] = 0x303030L;
  
  Colors[4] = 0x404040L; 
  Colors[5] = 0x505050L; 
  Colors[6] = 0x606060L; 
  Colors[7] = 0x707070L;
  
  Colors[8] = 0x808080L;
  Colors[9] = 0x909090L;
  Colors[10] = 0xA0A0A0L; 
  Colors[11] = 0xB0B0B0L;

  Colors[12] = 0xC0C0C0L; 
  Colors[13] = 0xD0D0D0L; 
  Colors[14] = 0xE0E0E0L; 
  Colors[15] = 0xF0F0F0L;
}

//
// Static instance of the colors 
//
static TCustomColors& GetCustomColors() 
{
#if defined(BI_MULTI_THREAD_RTL)
  static TTlsContainer<TCustomColors> __CustomColors;
#else
  static TCustomColors __CustomColors;
#endif
  return __CustomColors;
};

#if defined(BI_MULTI_THREAD_RTL)
#define LOCKCOLORS //TMRSWSection::TLock Lock(GetBrushCache().Lock);
#else
#define LOCKCOLORS 
#endif

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TColorPicker, TPopupPicker)
  EV_WM_PALETTECHANGED,
  EV_WM_QUERYNEWPALETTE,
END_RESPONSE_TABLE;

//
TColorPicker::TColorPicker(TWindow* parent, int id, int x, int y, int w, int h,
             TModule* module)
:
  TPopupPicker(parent, id, x, y, w, h, module),
  Palette(0),
  CustomColors(GetCustomColors().Colors),
  WorkPalette(0),
  PaletteRealized(false)
{
}

//
TColorPicker::TColorPicker(TWindow* parent, int resourceId, TModule* module)
:
  TPopupPicker(parent, resourceId, module),
  Palette(0),
  CustomColors(GetCustomColors().Colors),
  WorkPalette(0),
  PaletteRealized(false)
{
}

//
TColorPicker::~TColorPicker()
{
  delete Palette;
}

//
void
TColorPicker::SetupWindow()
{
/* // palette not working for now ??????????????????
  if (TScreenDC().GetDeviceCaps(RASTERCAPS) & RC_PALETTE){
    TAPointer<char> __clnObj = new char[sizeof(LOGPALETTE)+sizeof(PALETTEENTRY)*Cells->Size()];
    LOGPALETTE* lpPalette = (LOGPALETTE*)(char*)__clnObj;

    lpPalette->palVersion = 0x300;
    lpPalette->palNumEntries = (uint16)Cells->Size();
    for(int i = 0; i < Cells->Size(); i++){
      TColorCell* cell = TYPESAFE_DOWNCAST(GetSelectedCell(), TColorCell);
      if(cell){
        lpPalette->palPalEntry[i].peRed   = cell->GetColor().Red();
        lpPalette->palPalEntry[i].peGreen = cell->GetColor().Green();
        lpPalette->palPalEntry[i].peBlue  = cell->GetColor().Blue();
        lpPalette->palPalEntry[i].peFlags = PC_RESERVED;
      }
    }
    Palette = new TPalette(lpPalette);
  }
*/
  TPopupPicker::SetupWindow();
}

//
void
TColorPicker::SetCustomColors(TColor* colors)
{
  if(colors)
    CustomColors = colors;
  else
    CustomColors = GetCustomColors().Colors;
}

//
void
TColorPicker::NotifyParent(uint code)
{
  TColorCell* cell = TYPESAFE_DOWNCAST(GetSelectedCell(), TColorCell);
  if(cell){
    if(Target)
      SendNotification(*Target, GetDlgCtrlID(), code,
                       (THandle)(COLORREF)cell->GetColor());
    else
      SendNotification(::GetParent(*this), GetDlgCtrlID(), code,
                       (THandle)(COLORREF)cell->GetColor());
  }
}

//
void
TColorPicker::Paint(TDC& dc, bool erase, TRect& rect)
{
  if (Palette){
    WorkPalette = new TPalette(*Palette);
    dc.SelectObject(*WorkPalette);
    PaletteRealized = (dc.RealizePalette() > 0);

    // set palette index value
    if(PaletteRealized){
      for(int i = 3; i < (int)Cells->Size(); i++){
        TColorCell* cell = TYPESAFE_DOWNCAST((*Cells)[i], TColorCell);
        if(cell)
          cell->SetColor(TColor(i));
      }
    }
  }

  TPopupPicker::Paint(dc, erase, rect);

  // and reset colors
  if(WorkPalette){
    if(PaletteRealized){
      uint ncolors = WorkPalette->GetNumEntries();
      if(ncolors){
        TAPointer<PALETTEENTRY> _entryes = new PALETTEENTRY[ncolors];
        WorkPalette->GetPaletteEntries(0, (uint16)ncolors, _entryes);

        for(int i = 3; i < (int)Cells->Size(); i++){
          TColorCell* cell = TYPESAFE_DOWNCAST((*Cells)[i], TColorCell);
          if(cell)
            cell->SetColor(TColor(_entryes[i]));
        }
      }
    }
    dc.RestorePalette();
    delete WorkPalette;
    WorkPalette = 0;
  }
}

// Someone has changed the palette. We must realize our palette again.
//
void
TColorPicker::EvPaletteChanged(THandle hWndPalChg)
{
  TPopupPicker::EvPaletteChanged(hWndPalChg);
  if (hWndPalChg != GetHandle())
    Invalidate();
}

// Give us the chance to realize our palette.
//
bool
TColorPicker::EvQueryNewPalette()
{
  Invalidate();
  return TPopupPicker::EvQueryNewPalette() | PaletteRealized;
}

//
//
bool
TColorPicker::CallCustomBox()
{
  TColorCell* cell = TYPESAFE_DOWNCAST(GetSelectedCell(), TColorCell);
  TColor color = TColor::Black;
  if(cell)
    color = cell->GetColor();

  TChooseColorDialog::TData choose;

  choose.Flags = CC_RGBINIT;
  choose.Color = color;
  choose.CustColors = CustomColors;
  if (TChooseColorDialog(this, choose).Execute() == IDOK) {
    CustomColor = choose.Color;
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
//
// class TColorPickerGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~~
//
TColorPickerGadget::TColorPickerGadget(TColorPicker& picker,
    TResId glyphResIdOrIndex, TStyle style, TPopupType poptype, TType type,
    bool enabled, TState state, bool sharedGlyph, uint numChars)
:
  TPickerGadget(picker, glyphResIdOrIndex, style, poptype, type, enabled,
                state,sharedGlyph, numChars),
  ColorSel(TColor::None)
{
}

void
TColorPickerGadget::GetSelection()
{
  int sel = Picker->GetSelection();
  if(sel == -2){
    TColorPicker* cPicker = TYPESAFE_DOWNCAST(Picker, TColorPicker);
    if(cPicker)
      ColorSel = cPicker->GetCustomColor();
  }
  else{
    TPickerGadget::GetSelection();
    TColorCell* cell = TYPESAFE_DOWNCAST(Selected, TColorCell);
    if(cell)
      ColorSel = cell->GetColor();
  }
}

//
void
TColorPickerGadget::GetFillRect(TRect& rect, const TRect& faceRect)
{
  TSize size = CelArray ? CelArray->CelSize() : Window->GetCelArray().CelSize();

  rect.left   = faceRect.left+(faceRect.Width()-size.cx)/2;
  rect.top     = faceRect.bottom-5;
  rect.right   = rect.left + size.cx;
  rect.bottom = faceRect.bottom-1;
}

//
void
TColorPickerGadget::PaintFace(TDC& dc, TRect& rect)
{
  TPickerGadget::PaintFace(dc, rect);

  // now add color rectangle
  if(GetEnabled() && ColorSel != TColor::None){

    // Copy only the RGB values for the color that will be draw
    //
    TColor color(ColorSel.Red(), ColorSel.Green(), ColorSel.Blue());

    TRect fillRect;
    GetFillRect(fillRect, rect);

    dc.FillRect(Pressed ? fillRect.Offset(1,1) : fillRect, TBrush(color));

    TBrush shbr(TColor::Sys3dShadow);
    dc.FrameRect(fillRect, shbr);
  }
}

#endif
#if !defined(SECTION) || SECTION == 7
//
// class TBitmapPickerGadget
//
//
// class TBitmapPickerGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
//
TBitmapPickerGadget::TBitmapPickerGadget(TPopupPicker& picker,
    TResId glyphResIdOrIndex, TStyle style, TPopupType poptype, TType type,
    bool enabled, TState state, bool sharedGlyph, uint numChars)
:
  TPickerGadget(picker, glyphResIdOrIndex, style, poptype, type, enabled,
                state,sharedGlyph, numChars)
{
}

//
// helper functions
//
void
TBitmapPickerGadget::CopyCellBitmap()
{
  TBitmapCell* cell = TYPESAFE_DOWNCAST(Selected, TBitmapCell);
  if(cell){
    TCelArray& dstArray = CelArray ? *CelArray : Window->GetCelArray();
    TCelArray& srcArray = *cell->GetCelArray();
    TMemoryDC srcDC((TBitmap&)srcArray);
    TMemoryDC dstDC((TBitmap&)dstArray);
    
    TSize size(dstArray.CelSize().cx,dstArray.CelSize().cx*dstArray.NumRows());
    TRect dstRect(dstArray.CelOffset(GlyphIndex), size);

    int curRow = srcArray.CurRow();
    TRect srcRect(srcArray.CelRect(cell->GetIndex()));
    srcArray.SetCurRow(curRow);
    
    size = TSize(srcRect.Size());
    srcRect = TRect(srcRect.TopLeft(),TSize(size.cx, size.cy*srcArray.NumRows()));
    
    //rect.Inflate(-2,-2);
    
    dstDC.StretchBlt(dstRect, srcDC, srcRect);
  }
}

//
//
//
void
TBitmapPickerGadget::BuildCelArray()
{
  TPickerGadget::BuildCelArray();
  CopyCellBitmap();
}

//
//
//
void
TBitmapPickerGadget::GetSelection()
{
  TPickerCell* cell = Selected;

  TPickerGadget::GetSelection();
  if(Selected && cell != Selected)
    CopyCellBitmap();
}
#endif // !defined(SECTION) || SECTION == 7


__OWL_END_NAMESPACE

//==============================================================================

