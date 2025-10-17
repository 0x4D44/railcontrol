//----------------------------------------------------------------------------
//  Project themes
//  
//  (C) 2007 Sebastian Ledesma
//
//  SUBSYSTEM:    themes.apx Application
//  FILE:         moredilg.h
//  AUTHOR:       Sebastian Ledesma
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TMoreDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(moredilg_h)              // Sentry, use file only if it's not already included.
#define moredilg_h

#include <owl/dialog.h>

#include "themsapp.rh"            // Definition of all resources.
#include <owl/treeviewctrl.h>
#include <owl/listviewctrl.h>
#include <owl/colmnhdr.h>
#include <owl/imagelst.h>

#include <owl/controlb.h>
#include <owl/statusba.h>
#include <owl/buttonga.h>

#include <owl/glyphbtn.h>

class TMoreDialog : public owl::TDialog {
  public:
    TMoreDialog(owl::TWindow* parent, owl::TResId resId = IDD_MOREDIALOG, owl::TModule* module = 0);
    virtual ~TMoreDialog();

  protected:
    owl::TColumnHeader* pColumnHeader;
    owl::TListViewCtrl* pListWindow;
    owl::TTreeViewCtrl* pTreeView;

    owl::TControlBar *pControlBar;
    owl::TGlyphButton *pBtn_Glyph;
    owl::TImageList *LgImageList;

  public:
    virtual void SetupWindow();
};


#endif  // moredilg_h sentry.

