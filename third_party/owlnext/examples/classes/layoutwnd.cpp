#include "pch.h"
#include "layoutwnd.h"

#include <owl/dialog.h>
#include <owl/combobox.h>
#include <owl/edit.h>

#include "layout.rh"
#include "resource.h"

using namespace owl;

class TMyChildWindow : public TWindow {
public:
  TMyChildWindow(TWindow* parent, int id, int x, tchar* title,
    TColor color);
};


#define WM_SET_X (WM_USER + 100)

const int MaxWindows = 10;

class TLayoutDialog : public TDialog {
public:
  TLayoutDialog(TLayoutWindow* parent, TResId resId, TChildInfo* info);
  enum Constraint { X1, X2, Y1, Y2 };

protected:
  char*          Title;
  TLayoutWindow* LayoutWindow;

  void    SetupWindow();
  void    HandleLayout();
  void    HandleWindowChange();

  void    SetLayoutConstraint(TLayoutConstraint& lc, Constraint which);
  void    GetLayoutConstraint(TLayoutConstraint& lc, Constraint which);

  TChildInfo* ChildInfo;
  int         ChildNum;

  TListBox*  WindowList;

  TComboBox* X1EdgeCombo;
  TComboBox* X2EdgeCombo;
  TComboBox* Y1EdgeCombo;
  TComboBox* Y2EdgeCombo;

  TComboBox* X1OtherEdgeCombo;
  TComboBox* X2OtherEdgeCombo;
  TComboBox* Y1OtherEdgeCombo;
  TComboBox* Y2OtherEdgeCombo;

  TComboBox* X1UnitsCombo;
  TComboBox* X2UnitsCombo;
  TComboBox* Y1UnitsCombo;
  TComboBox* Y2UnitsCombo;

  TEdit*     X1ValueEdit;
  TEdit*     X2ValueEdit;
  TEdit*     Y1ValueEdit;
  TEdit*     Y2ValueEdit;

  TComboBox* X1RelationCombo;
  TComboBox* X2RelationCombo;
  TComboBox* Y1RelationCombo;
  TComboBox* Y2RelationCombo;

  TComboBox* X1RelWinCombo;
  TComboBox* X2RelWinCombo;
  TComboBox* Y1RelWinCombo;
  TComboBox* Y2RelWinCombo;

  DECLARE_RESPONSE_TABLE(TLayoutDialog);
};


TMyChildWindow::TMyChildWindow(TWindow* parent, int id, int x, tchar* title,
  TColor color)
  :
  TWindow(parent, title)
{
  SetBkgndColor(color);
  Attr.Style = WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_CLIPSIBLINGS;
  Attr.Id = id;
  static int i = 0;
  Attr.X = 10 + x;
  Attr.Y = 10;
  Attr.W = 100;
  Attr.H = 100;
}

//----------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TLayoutDemoWindow, TLayoutWindow)
EV_COMMAND(CM_LAYOUT, CmLayout),
EV_COMMAND(CM_RELAYOUT, CmReLayout),
END_RESPONSE_TABLE;

TLayoutDemoWindow::TLayoutDemoWindow(TWindow* parent)
  :
  TLayoutWindow(parent, 0)
{
  Attr.Style |= WS_BORDER;

  static TColor ChildColor[] = {
    RGB(0xFF, 0x00, 0x00),
    RGB(0x00, 0xFF, 0x00),
    RGB(0x00, 0x00, 0xFF),
    RGB(0xFF, 0xFF, 0x00),
    RGB(0x00, 0xFF, 0xFF),
    RGB(0xFF, 0x00, 0xFF),
  };
  static tchar* ChildName[] = {
    _T("Red"),
    _T("Green"),
    _T("Blue"),
    _T("Yellow"),
    _T("Cyan"),
    _T("Magenta"),
  };

  int i = 0;
  for (; i < MaxChildren; i++)
    ChildInfo[i].Child = new TMyChildWindow(this, i + 1, i * 100, ChildName[i], ChildColor[i]);
  ChildInfo[i].Child = 0;

  LayoutDialog = new TLayoutDialog(this, _T("IDD_LAYOUT"), ChildInfo);

}

void
TLayoutDemoWindow::CmLayout()
{
  // Only one layout dialog at a time please
  //
  if (!LayoutDialog->GetHandle())
    LayoutDialog->Create();
}

//
// Re-layout all of the children. Not really needed
//
void
TLayoutDemoWindow::CmReLayout()
{
  for (int i = 0; i < MaxChildren; i++)
    SetChildLayoutMetrics(*ChildInfo[i].Child, ChildInfo[i].LayoutMetrics);
  try {
    Layout();
  }
  catch (TXOwl& exception) {
    MessageBox(exception.why().c_str(), _T("Exception"));
  }
}

void
TLayoutDemoWindow::SetupWindow()
{
  TLayoutWindow::SetupWindow();
  PostMessage(WM_COMMAND, CM_RELAYOUT);
}


//----------------------------------------------------------------------------


DEFINE_RESPONSE_TABLE1(TLayoutDialog, TDialog)
EV_COMMAND(ID_LAYOUT, HandleLayout),
EV_LBN_SELCHANGE(ID_WINDOWLIST, HandleWindowChange),
END_RESPONSE_TABLE;

TLayoutDialog::TLayoutDialog(TLayoutWindow* parent,
  TResId         resId,
  TChildInfo*    childInfo)
  :
  TDialog(parent, resId),
  LayoutWindow(parent),
  ChildInfo(childInfo)
{
  WindowList = new TListBox(this, ID_WINDOWLIST);

  X1EdgeCombo = new TComboBox(this, ID_X1_MYEDGE);
  X2EdgeCombo = new TComboBox(this, ID_X2_MYEDGE);
  Y1EdgeCombo = new TComboBox(this, ID_Y1_MYEDGE);
  Y2EdgeCombo = new TComboBox(this, ID_Y2_MYEDGE);

  X1OtherEdgeCombo = new TComboBox(this, ID_X1_OTHEREDGE);
  X2OtherEdgeCombo = new TComboBox(this, ID_X2_OTHEREDGE);
  Y1OtherEdgeCombo = new TComboBox(this, ID_Y1_OTHEREDGE);
  Y2OtherEdgeCombo = new TComboBox(this, ID_Y2_OTHEREDGE);

  X1UnitsCombo = new TComboBox(this, ID_X1_UNITS);
  X2UnitsCombo = new TComboBox(this, ID_X2_UNITS);
  Y1UnitsCombo = new TComboBox(this, ID_Y1_UNITS);
  Y2UnitsCombo = new TComboBox(this, ID_Y2_UNITS);

  X1ValueEdit = new TEdit(this, ID_X1_VALUE);
  X2ValueEdit = new TEdit(this, ID_X2_VALUE);
  Y1ValueEdit = new TEdit(this, ID_Y1_VALUE);
  Y2ValueEdit = new TEdit(this, ID_Y2_VALUE);

  X1RelationCombo = new TComboBox(this, ID_X1_RELATIONSHIP);
  X2RelationCombo = new TComboBox(this, ID_X2_RELATIONSHIP);
  Y1RelationCombo = new TComboBox(this, ID_Y1_RELATIONSHIP);
  Y2RelationCombo = new TComboBox(this, ID_Y2_RELATIONSHIP);

  X1RelWinCombo = new TComboBox(this, ID_X1_RELWIN);
  X2RelWinCombo = new TComboBox(this, ID_X2_RELWIN);
  Y1RelWinCombo = new TComboBox(this, ID_Y1_RELWIN);
  Y2RelWinCombo = new TComboBox(this, ID_Y2_RELWIN);
}

struct TEnumPair {
  tchar* Name;
  LPARAM Value;
};
#if defined(UNICODE)
#define ENUMPAIR(enum)   {_T(#enum), enum}
#else
#define ENUMPAIR(enum)   {#enum, enum}
#endif

//
// For a valid constraint....
// X2Edge must not be the same as X1 edge
// Y2Edge must not be the same as Y1 edge
// If X2Edge == lmWidth then lmLeft and lmRight relations don't make sense
// if Y2Edge == lmHeight then lmAbove and lmBelow relations don't make sense
//

TEnumPair X1EdgePairs[] = {
  ENUMPAIR(lmLeft),
  ENUMPAIR(lmCenter),
  ENUMPAIR(lmRight),
};

TEnumPair X2EdgePairs[] = {
  ENUMPAIR(lmCenter),
  ENUMPAIR(lmRight),
  ENUMPAIR(lmWidth),
};

TEnumPair Y1EdgePairs[] = {
  ENUMPAIR(lmTop),
  ENUMPAIR(lmCenter),
  ENUMPAIR(lmBottom),
};

TEnumPair Y2EdgePairs[] = {
  ENUMPAIR(lmCenter),
  ENUMPAIR(lmBottom),
  ENUMPAIR(lmHeight),
};

TEnumPair AllEdgePairs[] = {
  ENUMPAIR(lmLeft),
  ENUMPAIR(lmCenter),
  ENUMPAIR(lmRight),
  ENUMPAIR(lmTop),
  ENUMPAIR(lmBottom),
  ENUMPAIR(lmWidth),
  ENUMPAIR(lmHeight),
};


// Relations for all X edges
//
TEnumPair XRelationPairs[] = {
  ENUMPAIR(lmAsIs),
  ENUMPAIR(lmAbsolute),
  ENUMPAIR(lmPercentOf),
  ENUMPAIR(lmSameAs),
  ENUMPAIR(lmLeftOf),     // These should be disabled for lmWidth
  ENUMPAIR(lmRightOf),
};

TEnumPair YRelationPairs[] = {
  ENUMPAIR(lmAsIs),
  ENUMPAIR(lmAbsolute),
  ENUMPAIR(lmPercentOf),
  ENUMPAIR(lmSameAs),
  ENUMPAIR(lmAbove),      // These should be disabled for lmHeight
  ENUMPAIR(lmBelow),
};

TEnumPair RelWinPairs[MaxWindows] = {
  {_T("lmParent"), reinterpret_cast<LPARAM>(lmParent)}  // lmParent was changed from 0 to nullptr to resolve C++ Builder warnings
};

TEnumPair UnitsPairs[] = {
  ENUMPAIR(lmPixels),
  ENUMPAIR(lmLayoutUnits),
};

static void
FillCombo(TComboBox* combo, TEnumPair* pairs, int count)
{
  while (combo->DeleteString(0) > 0)
    ;
  for (int i = 0; i < count; i++) {
    int index = combo->AddString(pairs[i].Name);
    combo->SetItemData(index, pairs[i].Value);
  }
}

void
TLayoutDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // Fill the window list
  //
  int i = 0;
  for (; ChildInfo[i].Child && i < MaxWindows - 1; i++) {
    WindowList->AddString(ChildInfo[i].Child->Title);
    RelWinPairs[i + 1].Name = ChildInfo[i].Child->Title;
    RelWinPairs[i + 1].Value = reinterpret_cast<LPARAM>(ChildInfo[i].Child);
  }

  FillCombo(X1EdgeCombo, X1EdgePairs, COUNTOF(X1EdgePairs));
  FillCombo(X2EdgeCombo, X2EdgePairs, COUNTOF(X2EdgePairs));
  FillCombo(Y1EdgeCombo, Y1EdgePairs, COUNTOF(Y1EdgePairs));
  FillCombo(Y2EdgeCombo, Y2EdgePairs, COUNTOF(Y2EdgePairs));

  FillCombo(X1OtherEdgeCombo, AllEdgePairs, COUNTOF(AllEdgePairs));
  FillCombo(X2OtherEdgeCombo, AllEdgePairs, COUNTOF(AllEdgePairs));
  FillCombo(Y1OtherEdgeCombo, AllEdgePairs, COUNTOF(AllEdgePairs));
  FillCombo(Y2OtherEdgeCombo, AllEdgePairs, COUNTOF(AllEdgePairs));

  FillCombo(X1UnitsCombo, UnitsPairs, COUNTOF(UnitsPairs));
  FillCombo(X2UnitsCombo, UnitsPairs, COUNTOF(UnitsPairs));
  FillCombo(Y1UnitsCombo, UnitsPairs, COUNTOF(UnitsPairs));
  FillCombo(Y2UnitsCombo, UnitsPairs, COUNTOF(UnitsPairs));

  X1ValueEdit->SetText(_T(""));
  X2ValueEdit->SetText(_T(""));
  Y1ValueEdit->SetText(_T(""));
  Y2ValueEdit->SetText(_T(""));

  FillCombo(X1RelationCombo, XRelationPairs, COUNTOF(XRelationPairs));
  FillCombo(X2RelationCombo, XRelationPairs, COUNTOF(XRelationPairs));
  FillCombo(Y1RelationCombo, YRelationPairs, COUNTOF(YRelationPairs));
  FillCombo(Y2RelationCombo, YRelationPairs, COUNTOF(YRelationPairs));

  FillCombo(X1RelWinCombo, RelWinPairs, i + 1);
  FillCombo(X2RelWinCombo, RelWinPairs, i + 1);
  FillCombo(Y1RelWinCombo, RelWinPairs, i + 1);
  FillCombo(Y2RelWinCombo, RelWinPairs, i + 1);

  // This call will result in a LBN_SELCHANGE Message being sent to the
  // window, which will result in HandleWindowChange being called.
  // This call can't be made until the above controls are setup, so wait until
  // the end here.
  //
  ChildNum = -1;  // Flag to signal initial call to HandleWindowChange
  WindowList->SetSelIndex(0);
  HandleWindowChange();
}

static void
SelectItemData(TComboBox* combo, LPARAM itemData)
{
  int count = combo->GetCount();
  combo->SetSelIndex(-1);  // Clear the selection
  for (int i = 0; i < count; i++)
    if (combo->GetItemData(i) == itemData) {
      combo->SetSelIndex(i);
      return;
    }
  TRACE("Couldn't find lmXxx (" << itemData << ") in list");
}

static void
SelectItemData(TComboBox* combo, TWindow* win)
{
  SelectItemData(combo, reinterpret_cast<LPARAM>(win));
}

//
// Take a given layout constraint & shove it into the dialog
//
void
TLayoutDialog::SetLayoutConstraint(TLayoutConstraint& lc, Constraint which)
{
  tchar buff[20];

  switch (which) {
  case X1:
    SelectItemData(X1EdgeCombo, lc.MyEdge);
    SelectItemData(X1RelationCombo, lc.Relationship);
    SelectItemData(X1RelWinCombo, lc.RelWin);
    SelectItemData(X1OtherEdgeCombo, lc.OtherEdge);
    SelectItemData(X1UnitsCombo, lc.Units);
    wsprintf(buff, _T("%d"), lc.Value);
    X1ValueEdit->SetText(buff);
    break;

  case X2:
    SelectItemData(X2EdgeCombo, lc.MyEdge);
    SelectItemData(X2RelationCombo, lc.Relationship);
    SelectItemData(X2RelWinCombo, lc.RelWin);
    SelectItemData(X2OtherEdgeCombo, lc.OtherEdge);
    SelectItemData(X2UnitsCombo, lc.Units);
    wsprintf(buff, _T("%d"), lc.Value);
    X2ValueEdit->SetText(buff);
    break;

  case Y1:
    SelectItemData(Y1EdgeCombo, lc.MyEdge);
    SelectItemData(Y1RelationCombo, lc.Relationship);
    SelectItemData(Y1RelWinCombo, lc.RelWin);
    SelectItemData(Y1OtherEdgeCombo, lc.OtherEdge);
    SelectItemData(Y1UnitsCombo, lc.Units);
    wsprintf(buff, _T("%d"), lc.Value);
    Y1ValueEdit->SetText(buff);
    break;

  case Y2:
    SelectItemData(Y2EdgeCombo, lc.MyEdge);
    SelectItemData(Y2RelationCombo, lc.Relationship);
    SelectItemData(Y2RelWinCombo, lc.RelWin);
    SelectItemData(Y2OtherEdgeCombo, lc.OtherEdge);
    SelectItemData(Y2UnitsCombo, lc.Units);
    wsprintf(buff, _T("%d"), lc.Value);
    Y2ValueEdit->SetText(buff);
    break;
  }
}

//
// Retrieve a layout constraint from the dialog
//
void
TLayoutDialog::GetLayoutConstraint(TLayoutConstraint& lc, Constraint which)
{
  tchar buff[20];
  switch (which) {
  case X1:
    lc.MyEdge = (TEdge)X1EdgeCombo->GetItemData(X1EdgeCombo->GetSelIndex());
    lc.Relationship = (TRelationship)X1RelationCombo->GetItemData(X1RelationCombo->GetSelIndex());
    lc.RelWin = (TWindow*)X1RelWinCombo->GetItemData(X1RelWinCombo->GetSelIndex());
    lc.OtherEdge = (TEdge)X1OtherEdgeCombo->GetItemData(X1OtherEdgeCombo->GetSelIndex());
    lc.Units = (TMeasurementUnits)X1UnitsCombo->GetItemData(X1UnitsCombo->GetSelIndex());
    X1ValueEdit->GetText(buff, sizeof buff);
    lc.Value = _ttoi(buff);
    break;

  case X2:
    lc.MyEdge = (TEdge)X2EdgeCombo->GetItemData(X2EdgeCombo->GetSelIndex());
    lc.Relationship = (TRelationship)X2RelationCombo->GetItemData(X2RelationCombo->GetSelIndex());
    lc.RelWin = (TWindow*)X2RelWinCombo->GetItemData(X2RelWinCombo->GetSelIndex());
    lc.OtherEdge = (TEdge)X2OtherEdgeCombo->GetItemData(X2OtherEdgeCombo->GetSelIndex());
    lc.Units = (TMeasurementUnits)X2UnitsCombo->GetItemData(X2UnitsCombo->GetSelIndex());
    X2ValueEdit->GetText(buff, sizeof buff);
    lc.Value = _ttoi(buff);
    break;

  case Y1:
    lc.MyEdge = (TEdge)Y1EdgeCombo->GetItemData(Y1EdgeCombo->GetSelIndex());
    lc.Relationship = (TRelationship)Y1RelationCombo->GetItemData(Y1RelationCombo->GetSelIndex());
    lc.RelWin = (TWindow*)Y1RelWinCombo->GetItemData(Y1RelWinCombo->GetSelIndex());
    lc.OtherEdge = (TEdge)Y1OtherEdgeCombo->GetItemData(Y1OtherEdgeCombo->GetSelIndex());
    lc.Units = (TMeasurementUnits)Y1UnitsCombo->GetItemData(Y1UnitsCombo->GetSelIndex());
    Y1ValueEdit->GetText(buff, sizeof buff);
    lc.Value = _ttoi(buff);
    break;

  case Y2:
    lc.MyEdge = (TEdge)Y2EdgeCombo->GetItemData(Y2EdgeCombo->GetSelIndex());
    lc.Relationship = (TRelationship)Y2RelationCombo->GetItemData(Y2RelationCombo->GetSelIndex());
    lc.RelWin = (TWindow*)Y2RelWinCombo->GetItemData(Y2RelWinCombo->GetSelIndex());
    lc.OtherEdge = (TEdge)Y2OtherEdgeCombo->GetItemData(Y2OtherEdgeCombo->GetSelIndex());
    lc.Units = (TMeasurementUnits)Y2UnitsCombo->GetItemData(Y2UnitsCombo->GetSelIndex());
    Y2ValueEdit->GetText(buff, sizeof buff);
    lc.Value = _ttoi(buff);
    break;
  }
}

//
// Window changed
// Ask user to save current layout constraints?
//
void TLayoutDialog::HandleWindowChange()
{
  if (ChildNum >= 0) {
    // Save all of the current settings, unless this is the first call
    //
    GetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.X, X1);
    GetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Width, X2);
    GetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Y, Y1);
    GetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Height, Y2);
  }

  // Switch to new window
  //
  ChildNum = WindowList->GetSelIndex();
  if (ChildNum >= 0) {
    SetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.X, X1);
    SetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Width, X2);
    SetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Y, Y1);
    SetLayoutConstraint(ChildInfo[ChildNum].LayoutMetrics.Height, Y2);
  }
}

//
// Get the constraints from each window, update child window
//
void
TLayoutDialog::HandleLayout()
{
  TLayoutMetrics& lm = ChildInfo[ChildNum].LayoutMetrics;

  GetLayoutConstraint(lm.X, X1);
  GetLayoutConstraint(lm.Width, X2);
  GetLayoutConstraint(lm.Y, Y1);
  GetLayoutConstraint(lm.Height, Y2);

  // Check some basic restrictions
  //
  if (lm.X.MyEdge == lm.Width.MyEdge) {
    MessageBox(_T("X1.MyEdge can not be the same as X2.MyEdge"));
    return;
  }
  if (lm.Y.MyEdge == lm.Height.MyEdge) {
    MessageBox(_T("Y1.MyEdge can not be the same as Y2.MyEdge"));
    return;
  }

  Parent->PostMessage(WM_COMMAND, CM_RELAYOUT);
}
