#include "pch.h"
#include "glyphbtndlg.h"

#include "resource.h"

#include <CommCtrl.h>

#include <owl/glyphbtn.h>

using namespace owl;

TGlyphButtonDialog::TGlyphButtonDialog(TWindow* parent)
  : TDialog(parent, TResId(IDD_GLYPHBUTTONDIALOG))
{
  new TGlyphButton(this, IDOK, TGlyphButton::btOk);
  new TGlyphButton(this, IDCANCEL, TGlyphButton::btCancel);
  new TGlyphButton(this, IDYES, TGlyphButton::btYes);
  new TGlyphButton(this, IDNO, TGlyphButton::btNo);
  new TGlyphButton(this, IDABORT, TGlyphButton::btAbort);
  new TGlyphButton(this, IDIGNORE, TGlyphButton::btIgnore);
  new TGlyphButton(this, IDCLOSE, TGlyphButton::btClose);
  new TGlyphButton(this, IDHELP, TGlyphButton::btHelp);
  new TGlyphButton(this, IDC_APPLY, TGlyphButton::btApply);
  new TGlyphButton(this, IDC_REVERT, TGlyphButton::btRevert);
  new TGlyphButton(this, IDC_ADD, TGlyphButton::btAdd);
  new TGlyphButton(this, IDC_DELETE, TGlyphButton::btDelete);
  new TGlyphButton(this, IDC_EDIT, TGlyphButton::btEdit);
  new TGlyphButton(this, IDC_SETUP, TGlyphButton::btSetup);
  new TGlyphButton(this, IDC_BROWSE, TGlyphButton::btBrowse);
  new TGlyphButton(this, IDC_KEY, TGlyphButton::btKey);

  TGlyphButton* btnCustom = new TGlyphButton(this, IDC_CUSTOMGLYPH);
  btnCustom->SetGlyph(TResId(IDB_OWLGLYPH));

}


TGlyphButtonDialog::~TGlyphButtonDialog()
{
}

void TGlyphButtonDialog::SetupWindow()
{
  TDialog::SetupWindow();

  SendDlgItemMessage(IDC_SHIELDBUTTON, BCM_SETSHIELD, 0, 1);
}
