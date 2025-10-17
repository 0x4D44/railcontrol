// Based on https://www.codeproject.com/articles/177603/win-editable-treeview-and-listview-merged-as-one
// Not all functionality is finished yet

#include "pch.h"
#include "treelist.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TTreeList, TLayoutWindow)
  EV_NM_RELEASEDCAPTURE(1002, NmReleasedCapture),
  EV_NM_CUSTOMDRAW(1001, NmCustomDraw),
END_RESPONSE_TABLE;

TTreeList::TTreeList(TWindow* parent, LPCTSTR title, TModule* module)
  : TLayoutWindow(parent, title, module)
{
  Init();
}

TTreeList::TTreeList(TWindow* parent, const tstring& title, TModule* module)
  : TLayoutWindow(parent, title, module)
{
  Init();
}

TTreeList::~TTreeList()
{
}

void TTreeList::Init()
{
  tree = new TTreeViewCtrl(this, 1001, 0, 0, 0, 0);
  tree->Attr.Style |= TVS_FULLROWSELECT | TVS_NOHSCROLL | TVS_NOTOOLTIPS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;

  header = new TColumnHeader(this, 1002, 0, 0, 0, 16);
  header->Attr.Style |= HDS_FULLDRAG;

  TLayoutMetrics layoutMetricsHeader;
  layoutMetricsHeader.SetMeasurementUnits(TMeasurementUnits::lmPixels);

  layoutMetricsHeader.Width.Set(lmRight, lmLeftOf, lmParent, lmRight, 0);
  SetChildLayoutMetrics(*header, layoutMetricsHeader);

  TLayoutMetrics layoutMetricsTree;
  layoutMetricsTree.SetMeasurementUnits(TMeasurementUnits::lmPixels);

  layoutMetricsTree.Y.Set(lmTop, lmBelow, header, lmBottom, 0);
  layoutMetricsTree.Width.Set(lmRight, lmLeftOf, lmParent, lmRight, 0);
  layoutMetricsTree.Height.Set(lmBottom, lmAbove, lmParent, lmBottom, 0);

  SetChildLayoutMetrics(*tree, layoutMetricsTree);
}

void TTreeList::SetupWindow()
{
  TLayoutWindow::SetupWindow();

  header->Add(THdrItem(_T("Column 1")));
  header->Add(THdrItem(_T("Column 2")));
  header->Add(THdrItem(_T("Column 3")));

  TTreeNode root = tree->GetRoot();
  root.AddChild(TTvItem(_T("Item 1")));
  TTreeNode child = root.AddChild(TTvItem(_T("Item 2")));
  child.AddChild(TTvItem(_T("Item 3")));
  child.AddChild(TTvItem(_T("Item 4")));
}

void TTreeList::NmReleasedCapture()
{
  tree->Invalidate(true);
}

int TTreeList::NmCustomDraw(TNmCustomDraw& nm)
{
  switch (nm.dwDrawStage)
  {
  case CDDS_PREPAINT:
    return CDRF_NOTIFYITEMDRAW;

  case CDDS_ITEMPREPAINT:
    return (CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT);

  case CDDS_ITEMPOSTPAINT:
    return ItemPostPaint(nm);
  }

  return CDRF_DODEFAULT;
}

int TTreeList::ItemPostPaint(owl::TNmCustomDraw& nm)
{
  if (::IsRectEmpty(&nm.rc) || !nm.hdc)
    return CDRF_DODEFAULT;

  HTREEITEM hTreeItem = (HTREEITEM)nm.dwItemSpec;

  TTreeNode treeNode(*tree, hTreeItem);

  TRect rectLabel;
  treeNode.GetItemRect(rectLabel, true);
  if (rectLabel.IsEmpty())
    return CDRF_DODEFAULT;

  TRect rectItem(nm.rc);
  if (rectItem.IsEmpty())
    return CDRF_DODEFAULT;

  TRect rectClient = GetClientRect();

  TTvCustomDraw& tvnm = *(TTvCustomDraw*)&nm;

  TDC dc(nm.hdc);

  dc.SetBkMode(TRANSPARENT);

  dc.SetTextColor(TColor::Black);

  TColor clWnd = tree->GetBkColor();
  TColor clTextBk = tvnm.GetBkColor();

  TBrush brWnd(clWnd);
  TBrush brTextBk(clTextBk);

  rectLabel.right = rectClient.right;

  dc.FillRect(rectLabel, brWnd);

  int columnCount = header->GetCount();
  if (columnCount <= 0)
    return CDRF_DODEFAULT;

  int iOffset = 0;

  std::vector<int> widths(columnCount);

  for (int iCol = 0; iCol < columnCount; ++iCol)
  {
    THdrItem hdrItem;
    if (header->GetItem(hdrItem, iCol, HDI_HEIGHT | HDI_WIDTH))
    {
      widths[iCol] = hdrItem.cxy;
      iOffset += hdrItem.cxy;
      rectItem.right = iOffset - 1;
      dc.DrawEdge(rectItem, BDR_SUNKENINNER, BF_RIGHT);
    }
    else
    {
      widths[iCol] = 0;
    }
  }

  dc.DrawEdge(rectItem, BDR_SUNKENINNER, BF_BOTTOM);

  tstring label(_T("Label"));

  TRect rectText;
  dc.DrawText(label, static_cast<int>(label.size()), rectText, DT_NOPREFIX | DT_CALCRECT);
  rectLabel.right = std::min(rectLabel.Left() + rectLabel.Right() + 4, widths[0] - 4);

  //if ((rectLabel.right - rectLabel.left) < 0)
  //  brTextBk = brWnd;

  bool selectedLine = false;

  if (clTextBk != clWnd)
  {
    if ((tvnm.nmcd.uItemState & CDIS_SELECTED) == CDIS_SELECTED)
    {
      selectedLine = true;
      dc.SetTextColor(TColor::White);
      rectLabel.right = rectClient.right;
      dc.FillRect(rectLabel, brTextBk);
    }
  }

  rectText = rectLabel;

  TRect rectHeaderItem;
  if (!header->GetItemRect(rectHeaderItem, 0))
    return CDRF_DODEFAULT;

  rectText.right = rectHeaderItem.right;
  rectText.DeflateRect(2, 1);
  dc.DrawText(label, static_cast<int>(label.size()), rectText, DT_NOPREFIX | DT_END_ELLIPSIS);

  iOffset = widths[0];

  for (int iCol = 1; iCol < columnCount; ++iCol)
  {
    tstring cell(_T("Cell"));

    rectText = rectLabel;
    rectText.left = iOffset;
    rectText.right = iOffset + widths[iCol];

    rectText.DeflateRect(2, 1);
    rectText.left += 8;

    dc.DrawText(cell, static_cast<int>(cell.size()), rectText, DT_NOPREFIX | DT_END_ELLIPSIS);

    iOffset += widths[iCol];
  }

  return CDRF_DODEFAULT;
}


