#include "pch.h"
#pragma hdrstop

#include <owl/celarray.h>
#include <owl/glyphbtn.h>

#include "cooldemogrid.h"

#include "resource.h"

using namespace owl;

static PALETTEENTRY gStaticColors[] = {
  /*000*/    {  0,  0,  0,0},  // White
  /*001*/    {128,  0,  0,0},  // Dark Red
  /*002*/    {  0,128,  0,0},  // Dark Green
  /*003*/    {128,128,  0,0},  // Dark Yellow
  /*004*/    {  0,  0,128,0},  // Dark Blue 
  /*005*/    {128,  0,128,0},  // Dark magenta
  /*006*/    {  0,128,128,0},  // Dark Cyan
  /*007*/    {192,192,192,0},  // Light Gray
  /*008*/    {192,220,192,0},  // Money Gray
  /*009*/    {166,202,240,0},  // Sky Blue

  /*246*/    {255,251,240,0},  // Cream
  /*247*/    {160,160,164,0},  // Light gray
  /*248*/    {128,128,128,0},  // Medium Gray
  /*249*/    {255,  0,  0,0},  // Red
  /*250*/    {  0,255,  0,0},  // Green
  /*251*/    {255,255,  0,0},  // Yellow
  /*252*/    {  0,  0,255,0},  // Blue
  /*253*/    {255,  0,255,0},  // Magenta
  /*254*/    {  0,255,255,0},  // Cyan
  /*255*/    {255,255,255,0},  // White 
};

// Custom Cell component
class TColorCell : public TCoolGrid::TCell {
  typedef TCoolGrid::TCell Inherited;
public:
  TColorCell(const TColor& color) :Color(color) {}

  virtual void Paint(TDC& dc, const TRect& rect);

protected:
  TColor Color;
};
//
void TColorCell::Paint(TDC& dc, const TRect& rect)
{
  TColor oldClr = dc.SetBkColor(Color);
  dc.ExtTextOut(rect.TopLeft(), ETO_OPAQUE, &rect, _T(""), 0, 0);
  dc.SetBkColor(oldClr);
}
//---------------------------------------------------------
// Custom InPlaceEdit component
class TInPlaceBoolEdit : public TCoolGrid::TInPlaceComboBox {
  typedef TCoolGrid::TInPlaceComboBox Inherited;
public:
  TInPlaceBoolEdit(TCoolGrid& parent, uint textLimit = 255);
};
TInPlaceBoolEdit::TInPlaceBoolEdit(TCoolGrid& parent, uint textLimit)
  :
  TCoolGrid::TInPlaceComboBox(parent, textLimit)
{
  GetData().AddString(_T("true"), true);
  GetData().AddString(_T("false"), false);
  SetReadOnly(true);
}

DEFINE_RESPONSE_TABLE1(TCoolDemoGrid, TCoolGrid)
END_RESPONSE_TABLE;

TCoolDemoGrid::TCoolDemoGrid(
  owl::TWindow* parent,
  int id,
  LPCTSTR text,
  int x, int y, int w, int h,
  owl::TModule* module) : TCoolGrid{ parent, id, text, x, y, w, h, module }
{
  ModifyStyle(0, WS_VSCROLL | WS_HSCROLL);
  ModifyExStyle(0, WS_EX_CLIENTEDGE);

  // set shared cell array for buttons
  TCelArray* celArray = new TCelArray(new TBtnBitmap(*GetModule(), IDB_BITMAPLIST), 24);
  SetCelArray(celArray);

  SetGridDim(20, 20);

  EnableVResize();

  int ColumnIndex = 0;
  _TCHAR buffer[256];

#if 0
  for (int e = 0; e < 5; e++) {
    for (int f = 0; f < 10; f++) {
      TButtonCell* cell = new TButtonCell(e + f, 1, 0, 255, 0, true);
      wsprintf(buffer, _T("Button%d"), e);
      cell->SetText(buffer);
      SetCell(TCellPos(e, f), *cell);
    }
  }
#endif

#if 0
  for (int g = 0; g < 5; g++) {
    for (int h = 0; h < 10; h++)
      SetCell(TCellPos(g, h), *new TBitmapCell(h + g, 1, 0, true));
  }
#endif


#if 0
  for (int i = 0; i < 5; i++) {
    //SetColumnWidth(i, 100);
    for (int j = 0; j < 10; j++)
      SetCell(TCellPos(i, j), *new TColorCell(gStaticColors[i + j]));
  }
#endif


#if 1
  SetHorTitle(ColumnIndex, _T("Colors"));
  SetColumnWidth(ColumnIndex, 40);
  for (int i = 0; i < 10; i++)
    SetCell(TCellPos(ColumnIndex, i), *new TColorCell(gStaticColors[i]));
  ColumnIndex++;
#endif

#if 1
  SetColumnWidth(ColumnIndex, 20);
  for (int i1 = 0; i1 < 10; i1++)
    SetCell(TCellPos(ColumnIndex, i1), *new TBitmapCell(ColumnIndex + i1, 1, 0, true));
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("Text"));
  SetColumnWidth(ColumnIndex, 70);
  for (int k = 0; k < 10; k++) {
    wsprintf(buffer, _T("TextItem%d"), k);
    SetCell(TCellPos(ColumnIndex, k), *new TTextCell(buffer));
  }
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("Bitmap+Text"));
  SetColumnWidth(ColumnIndex, 80);
  for (int i2 = 0; i2 < 10; i2++) {
    TBitmapTextCell* cell = new TBitmapTextCell(ColumnIndex + i2, 1, 0, 255, 0, true);
    wsprintf(buffer, _T("Text%d"), i2);
    cell->SetText(buffer);
    SetCell(TCellPos(ColumnIndex, i2), *cell);
  }
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("Edit"));
  SetColumnEdit(ColumnIndex, new TInPlaceEditCtrl(*this));
  SetColumnWidth(ColumnIndex, 80);
  for (int l = 0; l < 10; l++) {
    wsprintf(buffer, _T("EditItem%d"), l);
    SetCell(TCellPos(ColumnIndex, l), *new TEditCell(buffer));
  }
  ColumnIndex++;
#endif

#if 1
  wsprintf(buffer, _T("Command button"));
  SetCell(TCellPos(ColumnIndex, 0), *new TTextCell(buffer));
  //SetHorTitle(ColumnIndex, _T("Command button"));
  SetColumnWidth(ColumnIndex, 80);

  TCommandCell* bcell = new TCommandCell(CM_HELPABOUT, 9, 1, 0, 255, 0, true);
  wsprintf(buffer, _T("Button%d"), 1);
  bcell->SetText(buffer);
  SetCell(TCellPos(ColumnIndex, 1), *bcell);

  //    for(int k1 = 1; k1 < 10; k1++){
  //        wsprintf(buffer, "TextItem%d",k1);
  //        SetCell(TCellPos(ColumnIndex,k1), *new TTextCell(buffer));
  //    }
  ColumnIndex++;
#endif

#if 0
  SetColumnEdit(ColumnIndex, new TInPlaceEditCtrl(*this));
  SetColumnWidth(ColumnIndex, 80);
  for (int l1 = 0; l1 < 10; l1++) {
    wsprintf(buffer, _T("EditItem%d"), l1);
    SetCell(TCellPos(ColumnIndex, l1), *new TEditCell(buffer));
  }
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("ListBox"));
  TInPlaceListBox* edit = new TInPlaceListBox(*this);
  edit->GetData().AddString(_T("ListItem1"), true);
  edit->GetData().AddString(_T("ListItem2"), false);
  edit->GetData().AddString(_T("ListItem3"), false);
  edit->GetData().AddString(_T("ListItem4"), false);
  edit->GetData().AddString(_T("ListItem5"), false);
  edit->GetData().AddString(_T("ListItem6"), false);
  edit->GetData().AddString(_T("ListItem7"), false);

  SetColumnEdit(ColumnIndex, edit);
  SetColumnWidth(ColumnIndex, 100);
  for (int m = 0; m < 10; m++) {
    wsprintf(buffer, _T("ListItem%d"), m);
    SetCell(TCellPos(ColumnIndex, m), *new TComboBoxCell(buffer));
  }
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("ComboBox"));
  TInPlaceComboBox* cedit = new TInPlaceComboBox(*this);
  cedit->GetData().AddString(_T("CBoxItem0"), true);
  cedit->GetData().AddString(_T("CBoxItem1"), false);
  cedit->GetData().AddString(_T("CBoxItem2"), false);
  cedit->GetData().AddString(_T("CBoxItem3"), false);
  cedit->GetData().AddString(_T("CBoxItem4"), false);
  cedit->GetData().AddString(_T("CBoxItem5"), false);
  cedit->GetData().AddString(_T("CBoxItem6"), false);
  cedit->GetData().AddString(_T("CBoxItem7"), false);
  cedit->GetData().AddString(_T("CBoxItem8"), false);
  cedit->GetData().AddString(_T("CBoxItem9"), false);

  SetColumnEdit(ColumnIndex, cedit);
  SetColumnWidth(ColumnIndex, 100);
  for (int n = 0; n < 10; n++) {
    wsprintf(buffer, _T("CBoxItem%d"), n);
    SetCell(TCellPos(ColumnIndex, n), *new TComboBoxCell(buffer));
  }
  ColumnIndex++;
#endif

#if 0
  SetColumnWidth(ColumnIndex, 50);
  for (int o = 0; o < 10; o++) {
    wsprintf(buffer, "bool%d", o);
    SetCell(TCellPos(ColumnIndex, o), *new TTextCell(buffer));
  }
  ColumnIndex++;
#endif

#if 1
  SetHorTitle(ColumnIndex, _T("Booleans"));
  SetColumnEdit(ColumnIndex, new TInPlaceBoolEdit(*this));
  SetColumnWidth(ColumnIndex, 50);
  for (int p = 0; p < 10; p++) {
    TComboBoxCell* cell = new TComboBoxCell(_T("true"));
    cell->SetBgColor(TColor::LtGray);
    SetCell(TCellPos(ColumnIndex, p), *cell);
  }
  ColumnIndex++;
#endif
}

