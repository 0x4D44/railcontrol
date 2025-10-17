#include "pch.h"
#include "checklstdlg.h"
#include "resource.h"

using namespace owl;


const int CheckListId = 100;
const int NumItems = 15;

const int BufSize = 4096;

_TCHAR Buffer[BufSize];

//
// CheckListXWindow constructor
//
TCheckListXDialog::TCheckListXDialog(TWindow* parent)
  :
  TDialog(parent, TResId(IDD_CHECKLISTDIALOG)),
  Items(InitItems()),
  CheckList(this, CheckListId, 10, 10, 300, 300, Items.data(), static_cast<int>(Items.size()))
{}

auto TCheckListXDialog::CanClose() -> bool
{
  auto s = tostringstream{};
  s << _T("You've selected:\r\n\r\n");
  for (auto& i : Items)
  {
    if (i.IsChecked() || i.IsIndeterminate())
    {
      s << i.GetText();
      if (i.IsIndeterminate())
        s << _T(" (ind)");
      s << _T("\r\n");
    }
  }
  return MessageBox(s.str(), _T("Okay to close?"), MB_OKCANCEL) == IDOK;
}

auto TCheckListXDialog::InitItems() -> TItems
{
  auto items = TItems{};
  auto n = 0;
  for (auto& i : items)
  {
    auto s = tostringstream{};
    s << _T("Item ") << n++;
    i.SetText(s.str());
  }
  items[0].Toggle();
  items[1].SetIndeterminate();
  items[2].SetThreeStates(true);
  return items;
}

