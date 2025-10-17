#include "pch.h"
#include "resizabledialog.h"
#include <owl/configfl.h>

using namespace owl;

DEFINE_RESPONSE_TABLE2(TResizableDialog, TDialog, TLayoutWindow)
  EV_WM_SIZE,
  EV_WM_GETMINMAXINFO,
  EV_WM_WINDOWPOSCHANGED,
END_RESPONSE_TABLE;

TResizableDialog::TResizableDialog(owl::TWindow* parent, owl::TResId resId, owl::TModule* module)
  : TDialog{parent, resId, module}, TLayoutWindow{parent},
  LayoutInitialized{false}, MinSize{}, AppRegKeyName{}, RegValuePrefix{}
{}

void TResizableDialog::Anchor(TWindow& child, TAnchorX anchorX, TAnchorY anchorY, TAnchorX anchorW, TAnchorY anchorH)
{
  const auto rw = GetClientRect();
  const auto rc = GetChildRect(child);
  auto m = TLayoutMetrics{};
  m.SetMeasurementUnits(TMeasurementUnits::lmPixels);

  switch (anchorX)
  {
  case TAnchorX::PercentWidth: m.X.Set(lmLeft, lmPercentOf, lmParent, lmRight, rc.left * 100 / rw.right); break;
  case TAnchorX::EdgeRight: m.X.Set(lmRight, lmLeftOf, lmParent, lmRight, rw.right - rc.right); break;
  }

  switch (anchorY)
  {
  case TAnchorY::PercentHeight: m.Y.Set(lmTop, lmPercentOf, lmParent, lmBottom, rc.top * 100 / rw.bottom); break;
  case TAnchorY::EdgeBottom: m.Y.Set(lmBottom, lmAbove, lmParent, lmBottom, rw.bottom - rc.bottom); break;
  }

  switch (anchorW)
  {
  case TAnchorX::PercentWidth: m.Width.Set(lmRight, lmPercentOf, lmParent, lmRight, rc.right * 100 / rw.right); break;
  case TAnchorX::EdgeRight: m.Width.Set(lmRight, lmLeftOf, lmParent, lmRight, rw.right - rc.right); break;
  }

  switch (anchorH)
  {
  case TAnchorY::PercentHeight: m.Height.Set(lmBottom, lmPercentOf, lmParent, lmBottom, rc.bottom * 100 / rw.bottom); break;
  case TAnchorY::EdgeBottom: m.Height.Set(lmBottom, lmAbove, lmParent, lmBottom, rw.bottom - rc.bottom); break;
  }

  SetChildLayoutMetrics(child, m);
}

void TResizableDialog::AnchorRight(TWindow& child)
{ Anchor(child, TAnchorX::EdgeRight, TAnchorY::EdgeTop, TAnchorX::EdgeLeft, TAnchorY::EdgeTop); }

void TResizableDialog::AnchorRightBottom(TWindow& child)
{ Anchor(child, TAnchorX::EdgeRight, TAnchorY::EdgeBottom, TAnchorX::EdgeLeft, TAnchorY::EdgeTop); }

void TResizableDialog::AnchorBottom(TWindow& child)
{ Anchor(child, TAnchorX::EdgeLeft, TAnchorY::EdgeBottom, TAnchorX::EdgeLeft, TAnchorY::EdgeTop); }

void TResizableDialog::AnchorAll(TWindow& child)
{ Anchor(child, TAnchorX::EdgeLeft, TAnchorY::EdgeTop, TAnchorX::EdgeRight, TAnchorY::EdgeBottom); }

void TResizableDialog::AnchorLeftRight(TWindow& child)
{ Anchor(child, TAnchorX::EdgeLeft, TAnchorY::EdgeTop, TAnchorX::EdgeRight, TAnchorY::EdgeTop); }

void TResizableDialog::AnchorTopBottom(TWindow& child)
{ Anchor(child, TAnchorX::EdgeLeft, TAnchorY::EdgeTop, TAnchorX::EdgeLeft, TAnchorY::EdgeBottom); }

void TResizableDialog::Anchor(int id, TAnchorX anchorX, TAnchorY anchorY, TAnchorX anchorW, TAnchorY anchorH)
{ Anchor(*GetControl(id), anchorX, anchorY, anchorW, anchorH); }

void TResizableDialog::AnchorRight(int id)
{ AnchorRight(*GetControl(id)); }

void TResizableDialog::AnchorRightBottom(int id)
{ AnchorRightBottom(*GetControl(id)); }

void TResizableDialog::AnchorBottom(int id)
{ AnchorBottom(*GetControl(id)); }

void TResizableDialog::AnchorLeftRight(int id)
{ AnchorLeftRight(*GetControl(id)); }

void TResizableDialog::AnchorAll(int id)
{ AnchorAll(*GetControl(id)); }

void TResizableDialog::AnchorTopBottom(int id)
{ AnchorTopBottom(*GetControl(id)); }

bool TResizableDialog::EvInitDialog(THandle hFocus)
{
  const auto res = TDialog::EvInitDialog(hFocus);
  if (AppRegKeyName.length() > 0)
  {
    const auto config = TRegConfigFile{AppRegKeyName};
    auto r = TRect{};
    const auto ok = config.ReadRect(_T("UI"), RegValuePrefix + _T("_Extent"), r);
    if (ok && !r.IsEmpty())
    {
      InitializeLayout();
      MoveWindow(r);
      ClientSize = GetClientRect().Size();
      Layout();
    }
  }
  return res;
}

void TResizableDialog::EvSize(uint sizeType, const TSize& size)
{
  if (size.cx > 0 && size.cy > 0)
    InitializeLayout();
  TDialog::EvSize(sizeType, size);
  TLayoutWindow::EvSize(sizeType, size);
}

void TResizableDialog::EvGetMinMaxInfo(MINMAXINFO& m)
{
  TDialog::EvGetMinMaxInfo(m);
  if (LayoutInitialized)
  {
    m.ptMinTrackSize.x = MinSize.X();
    m.ptMinTrackSize.y = MinSize.Y();
  }
}

void TResizableDialog::EvWindowPosChanged(const WINDOWPOS& p)
{
  TDialog::EvWindowPosChanged(p);
  TLayoutWindow::EvWindowPosChanged(p);
  if (AppRegKeyName.length() > 0 && LayoutInitialized && p.cx > 0 && p.cy > 0)
  {
    auto config = TRegConfigFile{AppRegKeyName};
    config.WriteRect(_T("UI"), RegValuePrefix + _T("_Extent"),
      TRect{{p.x, p.y}, TSize{p.cx, p.cy}});
  }
}

void TResizableDialog::InitializeLayout()
{
  const auto rect = GetWindowRect();
  if (!rect.IsEmpty() && !LayoutInitialized)
  {
    MinSize = rect.Size();
    SetupLayout();
    LayoutInitialized = true;
    Layout();
    Invalidate(true);
  }
}

//
/// Creates an alias, if need be.
//
auto TResizableDialog::GetControl(int id) -> TWindow*
{
  const auto h = GetDlgItem(id);
  const auto w = TWindow::GetWindowPtr(h);
  return w ? w : new TWindow{h, GetModule()};
}
