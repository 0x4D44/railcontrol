#include "pch.h"
#pragma hdrstop

#include "Client.h"
#include "Help.h"
#include "Util.h"
#include "Replacer.h"
#include "App.rh"
#include <owl/memcbox.h>
#include <owl/glyphbtn.h>
#include <owl/tooltip.h>
#include <owl/configfl.h>
#include <filesystem>

using namespace owl;

//-----------------------------------------------------------------------------

// Specializes TMemCombox to store settings in the Windows Registry.
//
class TRegComboBox
  : public TMemComboBox
{
public:

  tstring RegKey;

  TRegComboBox(TWindow* parent, int resId, const tstring& name, const tstring& regKey)
    : TMemComboBox(parent, resId, name), RegKey(regKey) {}

  virtual TConfigFile* CreateConfigFile() // override
  { return new TRegConfigFile(RegKey); }
};

// Specializes TRegComboBox to accept a dropped file or folder.
//
class TFolderComboBox
  : public TRegComboBox
{
public:

  TFolderComboBox(TWindow* parent, int resId, const tstring& name, const tstring& regKey)
    : TRegComboBox(parent, resId, name, regKey)
  {}

protected:

  void SetupWindow() override
  {
    TRegComboBox::SetupWindow();
    DragAcceptFiles(true);
  }

private:

  // Enters the dropped folder name in the edit field.
  //
  void EvDropFiles(TDropInfo i)
  {
    PRECONDITION(i.DragQueryFileCount() > 0);
    using namespace std::filesystem;
    const auto f = path{i.DragQueryFile(0)};
    i.DragFinish(); // We're done with TDropInfo, so release it.

    // Note: We use the non-throwing overload of is_directory below, as not to let exceptions
    // escape and crash this handler.
    //
    auto e = std::error_code{};
    const auto folder = is_directory(f, e) ? f : f.parent_path();
    SetText(to_tstring(folder));
  }

  DECLARE_RESPONSE_TABLE(TFolderComboBox);
};

DEFINE_RESPONSE_TABLE1(TFolderComboBox, TRegComboBox)
  EV_WM_DROPFILES,
END_RESPONSE_TABLE;

//-----------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TReplaceClient, TDialog)
  EV_BN_CLICKED(IDC_BROWSE, BnBrowseClicked),
  EV_BN_CLICKED(IDC_ACTIONFIND, BnActionClicked),
  EV_BN_CLICKED(IDC_ACTIONTOUCH, BnActionClicked), 
  EV_BN_CLICKED(IDOK, BnReplaceClicked),
  EV_BN_CLICKED(IDC_HLP, BnHelpClicked),
END_RESPONSE_TABLE;

//-----------------------------------------------------------------------------

TReplaceClient::TReplaceClient(TWindow* parent, TReplaceData& data, const tstring& regKey)
  : TTransferDialog<TReplaceData>(parent, IDD_CLIENT, data)
{
  new TGlyphButton(this, IDOK, TGlyphButton::btOk);
  new TGlyphButton(this, IDCANCEL, TGlyphButton::btCancel);
  new TGlyphButton(this, IDC_BROWSE, TGlyphButton::btBrowse);
  new TGlyphButton(this, IDC_HLP, TGlyphButton::btHelp);

  new TFolderComboBox(this, IDC_FOLDER, _T("Folder"), regKey);
  new TRegComboBox(this, IDC_FILTER, _T("Filter"), regKey);
  new TRegComboBox(this, IDC_FIND, _T("Find"), regKey);
  new TRegComboBox(this, IDC_REPLACE, _T("Replace"), regKey);
}

//-----------------------------------------------------------------------------

void TReplaceClient::SetupWindow()
{
  TDialog::SetupWindow();

  // Add tooltips to some of the controls.
  //
  auto& t = *TTooltip::Make(this);
  const auto h = GetHandle();
  t.AddTool(h, IDOK, _T("Perform the action"));
  t.AddTool(h, IDCANCEL, _T("Close the program"));
  t.AddTool(h, IDC_BROWSE, _T("Browse for folder"));
  t.AddTool(h, IDC_SUBFOLDERS, _T("Recurse subfolders of chosen folder"));
  t.AddTool(h, IDC_ACTIONFIND, _T("Find and replace text in files"));
  t.AddTool(h, IDC_ACTIONTOUCH, _T("Change file date/time"));
  t.AddTool(h, IDC_FOLDER, _T("Enter start folder or select from recent list"));
  t.AddTool(h, IDC_FILTER, _T("Enter filters like *.cpp;*.c;*.h or select from recent list"));
}

//-----------------------------------------------------------------------------

void TReplaceClient::DoTransferData(const TTransferInfo& i, TReplaceData& d)
{
  TransferDlgItemText(i, IDC_FOLDER, d.Folder);
  TransferDlgItemText(i, IDC_FILTER, d.Filter);
  TransferDlgItemText(i, IDC_FIND, d.SearchTerm);
  TransferDlgItemText(i, IDC_REPLACE, d.Replacement);
  TransferCheckBoxData(i, IDC_SUBFOLDERS, d.RecurseFlag);
  TransferRadioButtonData(i, IDC_ACTIONFIND, d.Action);
  TransferDateTimePickerData(i, IDC_EDITTIME, d.Time);
  TransferDateTimePickerData(i, IDC_EDITDATE, d.Date);

  if (i.Operation == tdSetData)
  {
    // Update the control state.
    //
    BnActionClicked();
  }
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnBrowseClicked()
{
  tstring folder = GetDlgItemText(IDC_FOLDER);
  folder = BrowseForFolder(GetHandle(), folder, _T("Select root folder"));
  if (!folder.empty())
    SetDlgItemText(IDC_FOLDER, folder);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnActionClicked()
{
  const int idFind[] = {IDC_ST_FIND, IDC_FIND, IDC_REPLACE, IDC_ST_REPLACE};
  const int idTouch[] = {IDC_ST_TIME, IDC_EDITTIME, IDC_EDITDATE};

  bool shouldFind = IsChecked(IDC_ACTIONFIND);
  EnableDlgItem(idFind, shouldFind);
  DisableDlgItem(idTouch, shouldFind);
  ShowDlgItem(idFind, shouldFind);
  HideDlgItem(idTouch, shouldFind);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnReplaceClicked()
{
  // Validate dialog contents.
  //
  try 
  {
    if (GetDlgItemText(IDC_FOLDER).empty()) throw tstring(_T("Enter folder."));
    if (GetDlgItemText(IDC_FILTER).empty()) throw tstring(_T("Enter filter."));
    if (IsChecked(IDC_ACTIONFIND)) 
    {
      if (GetDlgItemText(IDC_FIND).empty()) throw tstring(_T("Enter search string."));
    }
  }
  catch (const tstring& m)
  {
    MessageBox(m, _T("Validation Error"), MB_ICONINFORMATION);
    return;
  }

  // Call TransferData, which will call DoTransferData, to update the arguments.
  // Then send the command to the parent window; this will be handled by the application.
  // See TReplaceApp::CmReplace.
  //
  TransferData(tdGetData);
  GetParent()->PostMessage(WM_COMMAND, CM_REPLACE);
}

//-----------------------------------------------------------------------------

void TReplaceClient::BnHelpClicked()
{
  ExecuteHelpDialog(this);
}
