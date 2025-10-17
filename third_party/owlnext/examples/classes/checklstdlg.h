#pragma once
#include <owl/dialog.h>
#include <owl/checklst.h>
#include <array>

class TCheckListXDialog : public owl::TDialog {
public:
  TCheckListXDialog(owl::TWindow* parent = nullptr);

  auto CanClose() -> bool override;

private:
  using TItems = std::array<owl::TCheckListItem, 15>;
  TItems Items;
  owl::TCheckList CheckList;
  const int CheckListId = 100;

  static auto InitItems()->TItems;
};