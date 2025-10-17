#include "pch.h"
#include "urllinkdlg.h"
#include "resource.h"

#include <owlext/urllink.h>

using namespace owl;
using namespace OwlExt;

TUrlLinkDialog::TUrlLinkDialog(TWindow* parent)
  : TDialog(parent, TResId(IDD_URLLINKDIALOG))
{
  TUrlLink* link = new TUrlLink(this, IDC_LINK);
  link->SetURL(_T("https://sourceforge.net/projects/owlnext/"));
  TUrlLink* link2 = new TUrlLink(this, IDC_LINK2);
  link2->SetURL(_T("https://sourceforge.net/p/owlnext/wiki/Examples/"));

}


TUrlLinkDialog::~TUrlLinkDialog()
{
}
