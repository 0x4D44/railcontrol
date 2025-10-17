//----------------------------------------------------------------------------
//  Project themes
//
//  (C) 2007 Sebastian Ledesma
//
//  SUBSYSTEM:    themes.apx Application
//  FILE:         moredilg.cpp
//  AUTHOR:       Sebastian Ledesma
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TMoreDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "pch.h"
#include <owl/pch.h>

#include "moredlg.h"
#include <stdio.h>

#include "resource.h"

using namespace owl;

TMoreDialog::TMoreDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  pColumnHeader = new TColumnHeader(this, IDC_HEADER1);
  pListWindow = new TListViewCtrl(this, IDC_LISTVIEW1);
  pTreeView = new TTreeViewCtrl(this, IDC_TREEVIEW1);

#if OWLVersion<=0x0520 //asumo classic OWL
  pControlBar = new TControlBar(this, IDC_TOOLBAR1);
#else //asumo OWLNext
  pControlBar = new TControlBar(this);
#endif
  pBtn_Glyph= new TGlyphButton (this, IDOK);
  pBtn_Glyph->SetGlyph(IDB_OWLGLYPH);

  SetBkgndColor(RGB(255,198, 116));

  //pListWindow->Attr.Style |= LVS_SHAREIMAGELISTS | LVS_SMALLICON;

  // Initialize ImageLists
  //
  LgImageList = new TImageList(TSize(32, 32), ILC_COLOR4, 15, 5);
  LgImageList->Add(TIcon(*GetApplication(), (TResId)IDI_ICON1));
  LgImageList->Add(TIcon(*GetApplication(), (TResId)IDI_ICON2));
  LgImageList->Add(TIcon(*GetApplication(), (TResId)IDI_ICON3));
  LgImageList->Add(TIcon(*GetApplication(), (TResId)IDI_ICON4));

/*  pControlBar->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW));
  pControlBar->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN));
  pControlBar->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));             */
}


TMoreDialog::~TMoreDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete LgImageList;

}


void TMoreDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  THdrItem hdrItem(_T("THdrItem 1"));
  THdrItem hdrItem2(_T("THdrItem 2"));
  THdrItem hdrItem3(_T("THdrItem 3"));
  pColumnHeader->Add(hdrItem);
  pColumnHeader->Add(hdrItem2);
  pColumnHeader->Add(hdrItem3);


  pListWindow->SetImageList(*LgImageList, TListViewCtrl::Normal);


  //TListWindColumn column("Column", 100);
  //pListWindow->InsertColumn(0, column);

   _TCHAR Buffer[64];
  for (int i = 0; i < LgImageList->GetImageCount(); i++) {
    wsprintf(Buffer, _T("Item %d"), i);
    TLvItem item(Buffer);
    item.SetImageIndex(i);
    pListWindow->InsertItem(item);
  }

}

