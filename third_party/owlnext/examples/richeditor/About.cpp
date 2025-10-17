#include <owl/pch.h>

#include "About.h"
#include "App.rh"
#include <owl/static.h>
#include <owl/module.h>
#include <strsafe.h>

using namespace owl;

TAboutDlg::TAboutDlg(TWindow* parent, TModule* module)
  : TDialog(parent, IDD_ABOUT, module)
{}

void TAboutDlg::SetupWindow()
{
  TDialog::SetupWindow();

  // Get the product name and product version strings from the module.
  // Note: The initial value of IDC_VERSION is the word Version (in whatever language).
  //
  auto v = TModuleVersionInfo{ GetModule()->GetHandle() };
  auto s = tostringstream{};
  s << v.GetProductName() << _T(' ') << GetDlgItemText(IDC_VERSION) << _T(' ') << v.GetProductVersion();
  SetDlgItemText(IDC_VERSION, s.str());
  SetDlgItemText(IDC_COPYRIGHT, v.GetLegalCopyright());
  if (v.IsSpecialBuild())
    SetDlgItemText(IDC_DEBUG, v.GetSpecialBuild());
}