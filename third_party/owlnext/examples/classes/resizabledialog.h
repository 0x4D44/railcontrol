#pragma once

#include <owl/dialog.h>
#include <owl/layoutwi.h>
#include <utility> // std::move

class TResizableDialog
  : public owl::TDialog, virtual public owl::TLayoutWindow
{
public:
  TResizableDialog(owl::TWindow* parent, owl::TResId, owl::TModule* = nullptr);

  void SetAppRegKeyName(owl::tstring n) { AppRegKeyName = std::move(n); }
  void SetRegValuePrefix(owl::tstring k) { RegValuePrefix = std::move(k); }

protected:
  virtual void SetupLayout() = 0;

  enum class TAnchorX { EdgeLeft, EdgeRight, PercentWidth };
  enum class TAnchorY { EdgeTop, EdgeBottom, PercentHeight };

  void Anchor(owl::TWindow& child, TAnchorX anchorX, TAnchorY anchorY, TAnchorX anchorW, TAnchorY anchorH);
  void AnchorRight(owl::TWindow& child);
  void AnchorRightBottom(owl::TWindow& child);
  void AnchorLeftRight(owl::TWindow& child);
  void AnchorTopBottom(owl::TWindow& child);
  void AnchorBottom(owl::TWindow& child);
  void AnchorAll(owl::TWindow& child);

  void Anchor(int resId, TAnchorX anchorX, TAnchorY anchorY, TAnchorX anchorW, TAnchorY anchorH);
  void AnchorRight(int resId);
  void AnchorRightBottom(int resId);
  void AnchorLeftRight(int resId);
  void AnchorTopBottom(int resId);
  void AnchorBottom(int resId);
  void AnchorAll(int resId);

  bool EvInitDialog(THandle hFocus) override;

  void EvSize(owl::uint sizeType, const owl::TSize& size);
  void EvGetMinMaxInfo(MINMAXINFO& minmaxinfo);
  void EvWindowPosChanged(const WINDOWPOS& windowPos);

private:
  bool LayoutInitialized;
  owl::TSize MinSize;
  owl::tstring AppRegKeyName;
  owl::tstring RegValuePrefix;

  void InitializeLayout();
  auto GetControl(int id) -> owl::TWindow*;

  DECLARE_RESPONSE_TABLE(TResizableDialog);
};

