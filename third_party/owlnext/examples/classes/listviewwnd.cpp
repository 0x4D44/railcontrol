//
// This example program demonstrates simple use of TListViewCtrl.
// This code is based on "EXAMPLES/OWL/CLASSES/ListWIND" in OWL 5.
//
// Copyright (c) 1995, 1995 by Borland International, All Rights Reserved
// Copyright (c) 2013 Vidar Hasfjord
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).
//

#include "pch.h"
#include "listviewwnd.h"

#include <commctrl.h>

#include <string>

#include "resource.h"

using namespace owl;

#if defined(_UNICODE)
#define _ttostring(a) std::to_sstring(a)
#else
#define _ttostring(a) std::to_string(a)
#endif

DEFINE_RESPONSE_TABLE1(TListViewWindow, TWindow)
EV_WM_SETFOCUS,
EV_WM_SIZE,
EV_NM_CUSTOMDRAW(ListId, NmCustomDraw),
EV_LVN_GETINFOTIP(ListId, LvnGetInfoTip),
END_RESPONSE_TABLE;

TListViewWindow::TListViewWindow(TWindow* parent)
  : TWindow(parent),
  List(this, ListId, 0, 0, 0, 0),
  oddLineColor(TColor::White),
  evenLineColor(230, 230, 230)
{
  ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
  List.ModifyStyle(0, ListStyle);
}

TListViewWindow::~TListViewWindow()
{
}

void TListViewWindow::SetupWindow() // override
{
  TWindow::SetupWindow();

  List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, ListStyleEx, ListStyleEx);

  List.InsertColumn(0, TLvColumn(_T("Column 1"), 80));
  List.InsertColumn(1, TLvColumn(_T("Column 2"), 100));
  List.InsertColumn(2, TLvColumn(_T("Column 3"), 150, TLvColumn::Right));

  for (int index = 0; index < 10; ++index)
  {
    tostringstream sItem;
    sItem << _T("Text ") << index + 1;
    TLvItem item(sItem.str());

    int row = List.AddItem(item);

    tostringstream sSubItem;
    sSubItem << _T("Subitem text ") << index + 1;

    List.SetItem(TLvItem(sSubItem.str()), row, 1);
    List.SetItem(TLvItem(sSubItem.str()), row, 2);
  }
  
  List.EnableTooltip(true);

  TRect headerRect;
  ::GetClientRect(List.GetHeaderCtrl(), &headerRect);

  int columnCount = Header_GetItemCount(List.GetHeaderCtrl());
  int left = 0;
  for (int index = 0; index < columnCount; ++index)
  {
    TLvColumn column;
    if (List.GetColumn(index, column))
    {
      tostringstream sTooltip;
      sTooltip << _T("Tooltip for ") << column.GetText();

      TToolInfo* ti = new TToolInfo(List.GetHeaderCtrl(), TRect(left, 0, left + column.GetWidth(), headerRect.Height()), index, sTooltip.str());
      List.GetTooltip()->AddTool(*ti);

      headerTooltips.Add(ti);

      left += column.GetWidth();
    }
  }
}

void TListViewWindow::EvSetFocus(HWND lostFocus)
{
  List.SetFocus();
}

void TListViewWindow::EvSize(uint sizeType, const TSize& size)
{
  TWindow::EvSize(sizeType, size);

  // Resize the List control inside the available client area.
  //
  TSize margin(-GetSystemMetrics(SM_CXHSCROLL), -GetSystemMetrics(SM_CXVSCROLL));
  TRect r = GetClientRect().Inflate(margin);
  List.MoveWindow(r, true);
}

int TListViewWindow::NmCustomDraw(owl::TNmCustomDraw& nm)
{
  TLvCustomDraw& lvnm = *((TLvCustomDraw*)&nm);
  int row = (int)nm.dwItemSpec;

  switch (nm.dwDrawStage)
  {
  case CDDS_PREPAINT:
    return CDRF_NOTIFYITEMDRAW;

  case CDDS_ITEMPREPAINT:
  {
    if (row % 2 == 1)
      lvnm.clrTextBk = oddLineColor;
    else
      lvnm.clrTextBk = evenLineColor;

    return CDRF_DODEFAULT;
  }
  
  case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
    return CDRF_DODEFAULT;

  default:
    return CDRF_DODEFAULT;
  }
}

void TListViewWindow::LvnGetInfoTip(TLvGetInfoTip& git)
{
  TLvItem item = List.GetItem(git.iItem);

  tostringstream sTooltip;
  sTooltip << _T("Tooltip for item #") << git.iItem << _T(" : ") << item.GetText();

  _tcscpy_s(git.pszText, git.cchTextMax, sTooltip.str().c_str());
}

TResult TListViewWindow::EvNotify(uint id, TNotify & notifyInfo)
{
  if (notifyInfo.hwndFrom == List.GetHeaderCtrl())
  {
    if ((int)notifyInfo.code == (int)HDN_ITEMCHANGEDW || (int)notifyInfo.code == (int)HDN_ITEMCHANGEDA)
    {
      UpdateTooltips();
    }
    else if ((int)notifyInfo.code == (int)NM_CUSTOMDRAW)
    {
      TNmCustomDraw& nm = *((TNmCustomDraw*)& notifyInfo);

      switch (nm.dwDrawStage)
      {
      case CDDS_PREPAINT:
        return CDRF_NOTIFYITEMDRAW;

      case CDDS_ITEMPREPAINT:
      {
        TDC dc(nm.hdc);

        TRect rect(nm.rc);
        dc.SelectObject(TPen(TColor::Gray));
        dc.SelectObject(TBrush(TColor::LtGray));
        dc.FillSolidRect(rect, TColor::LtGray);
        dc.Rectangle(rect);

        TLvColumn column;
        if (List.GetColumn(static_cast<int>(nm.dwItemSpec), column))
        {
          tstring str(column.GetText());

          //TSize size = dc.GetTextExtent(str, str.length());
          dc.ExtTextOut(TPoint(nm.rc.left + 4, nm.rc.top + 2), ETO_CLIPPED, &rect, str);
        }

        return CDRF_SKIPDEFAULT;
      }

      default:
        return CDRF_DODEFAULT;
      }
    }
  }

  return TWindow::EvNotify(id, notifyInfo);
}

void TListViewWindow::UpdateTooltips()
{
  TRect headerRect;
  ::GetClientRect(List.GetHeaderCtrl(), &headerRect);

  int left = 0;
  const int n = static_cast<int>(headerTooltips.size());
  for (int index = 0; index != n; ++index)
  {
    TLvColumn column;
    if (List.GetColumn(index, column))
    {
      headerTooltips[index]->SetRect(TRect(left, 0, left + column.GetWidth(), headerRect.Height()));
      List.GetTooltip()->NewToolRect(*headerTooltips[index]);
      left += column.GetWidth();
    }
  }
}
