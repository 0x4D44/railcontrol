//
// This example program demonstrates simple use of TTreeViewCtrl.
// This code is based on "EXAMPLES/OWL/CLASSES/TREEWIND" in OWL 5.
//
// Copyright (c) 1995, 1995 by Borland International, All Rights Reserved
// Copyright (c) 2013 Vidar Hasfjord
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).
//

#include "pch.h"
#include "treeviewwnd.h"

#include <commctrl.h>

#include "resource.h"

using namespace owl;

DEFINE_RESPONSE_TABLE1(TTreeViewWindow, TWindow)
  EV_COMMAND(CM_SETSTATE, CmSetState),
  EV_COMMAND(CM_GETSTATE, CmGetState),
  EV_COMMAND(CM_SORTDESCENDING, CmSortDescending),
  EV_WM_SETFOCUS,
  EV_WM_SIZE,
  EV_TVN_ITEMEXPANDING(TreeId, TvnItemExpanding),
END_RESPONSE_TABLE;

TTreeViewWindow::TTreeViewWindow(TWindow* parent)
  : TWindow(parent),
  Tree(this, TreeId, 0, 0, 0, 0, TreeStyle),
  Icons(GetModule()->GetHandle(), IDB_ICONS, 20, 1, TColor::White, IMAGE_BITMAP, 0)
{
  ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
}

void TTreeViewWindow::SetupWindow() // override
{
  TWindow::SetupWindow();

#if (NTDDI_VERSION >= NTDDI_VISTA)
  const uint exStyle = TVS_EX_DOUBLEBUFFER;
  Tree.SendMessage(TVM_SETEXTENDEDSTYLE, exStyle, exStyle);
#endif

  // Initialize the tree with some arbitrary items.
  //
  // Note that, although we do so here for simplicity, it is not efficient to add items at the
  // end of the list (or sub-list) using AddChild. For long lists you should insert the items in 
  // reverse order, i.e. insert items at the top of the list using InsertChild. 
  // See http://blogs.msdn.com/b/oldnewthing/archive/2011/11/25/10241394.aspx
  //
  Tree.SetImageList(TTreeViewCtrl::Normal, Icons);
  const int folder = 1; // image list index

  TTreeNode root = Tree.GetRoot();
  root.AddChild(TTvItem(_T("Item 1")));
  TTreeNode item2 = root.AddChild(TTvItem(_T("Item 2"), folder, folder));
  item2.AddChild(TTvItem(_T("Item 2.1")));
  TTreeNode item2_2 = item2.AddChild(TTvItem(_T("Item 2.2"), folder, folder));
  item2_2.AddChild(TTvItem(_T("Item 2.2.1")));
  item2_2.AddChild(TTvItem(_T("Item 2.2.2")));
  item2.AddChild(TTvItem(_T("Item 2.3 (with some longer text)")));
  root.AddChild(TTvItem(_T("Item 3")));
  TTreeNode item4 = root.AddChild(TTvItem(_T("Item 4"), folder, folder));
  TTreeNode item4_1 = item4.AddChild(TTvItem(_T("Item 4.1")));
  item4.AddChild(TTvItem(_T("Item 4.2")));
  root.AddChild(TTvItem(_T("Item 5")));

  // Store the item handle in the item data, so that we can get at it in our compare function for
  // our sort command (see CmSortDescending).
  //
  for (auto n = root.GetChild(); n; n = n.GetNextSibling())
    n.SetItemData(reinterpret_cast<LPARAM>(n.GetHTreeItem()));

  item4.ExpandItem(TVE_EXPAND);
  item4_1.SelectItem(TVGN_CARET);
}

  void TTreeViewWindow::CmSetState()
  {
    TTreeNode n = Tree.GetSelection();
    n.SetState(TVIS_BOLD);
  }

  void TTreeViewWindow::CmGetState()
  {
    TTreeNode node = Tree.GetSelection();
    uint state;
    bool r = node.GetState(state); CHECK(r); InUse(r);
    tstring m = (state & TVIS_EXPANDED) ?
      _T("Selected node is expanded.") :
      _T("Selected node is not expanded.");
    MessageBox(m, _T("Item state"));
  }

  void TTreeViewWindow::CmSortDescending()
  {
    // Define the sort predicate.
    //
    struct TComparator : public TTvComparator
    {
      TTreeViewCtrl& Tree;

      TComparator(TTreeViewCtrl& t) : TTvComparator{}, Tree{ t } {}

      int Compare(LPARAM i1, LPARAM i2, LPARAM) const override
      {
        PRECONDITION(i1 != 0 && i2 != 0);

        // Extract the handles we stored earlier (see SetupWindow), and use the handles to retrieve
        // the item text.
        //
        auto s1 = tstring{ TTreeNode{ Tree, reinterpret_cast<HTREEITEM>(i1) }.GetText() };
        auto s2 = tstring{ TTreeNode{ Tree, reinterpret_cast<HTREEITEM>(i2) }.GetText() };
        return s2.compare(s1); // descending
      }
    };
    Tree.GetRoot().SortChildren(TComparator{ Tree });
  }

  void TTreeViewWindow::EvSetFocus(HWND lostFocus)
  {
    Tree.SetFocus();
  }

  void TTreeViewWindow::EvSize(uint sizeType, const TSize& size)
  {
    TWindow::EvSize(sizeType, size);

    // Resize the tree control inside the available client area.
    //
    TSize margin(-GetSystemMetrics(SM_CXHSCROLL), -GetSystemMetrics(SM_CXVSCROLL));
    TRect r = GetClientRect().Inflate(margin);
    Tree.MoveWindow(r, true);
  }

  bool TTreeViewWindow::TvnItemExpanding(TTvNotify& i)
  {
    TTreeNode n(Tree, i.itemNew);
    TRACE(n.GetText() << _T(" is expanding or retracting."));
    return false;
  }



