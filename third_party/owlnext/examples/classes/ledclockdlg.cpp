#include "pch.h"
#include "ledclockdlg.h"

#include <owl/chooseco.h>

#include "resource.h"

using namespace owl;

const int ID_TIMER = 1;

static TColor CustomColors[16];

DEFINE_RESPONSE_TABLE1(TLedClockDialog, TDialog)

EV_WM_TIMER,

EV_BN_CLICKED(IDC_DISPLAY12HOUR, SettingChanged),
EV_BN_CLICKED(IDC_DISPLAY24HOUR, SettingChanged),
EV_BN_CLICKED(IDC_DISPLAYHOURLEADINGZERO, SettingChanged),
EV_BN_CLICKED(IDC_DISPLAYSECONDS, SettingChanged),

EV_CHILD_NOTIFY_ALL_CODES(IDC_HORIZONTALMARGIN, SliderSettingChanged),
EV_CHILD_NOTIFY_ALL_CODES(IDC_VERTICALMARGIN, SliderSettingChanged),

EV_BN_CLICKED(IDC_SELECTFOREGROUND, BnForeground),
EV_BN_CLICKED(IDC_SELECTBACKGROUND, BnBackground),

END_RESPONSE_TABLE;

TLedClockDialog::TLedClockDialog(TWindow* parent)
  : TDialog(parent, IDD_LEDDIALOG)
{
  ledClock = new TLedClock(this, IDC_LEDSTATIC);

  display24Hour = new TRadioButton(this, IDC_DISPLAY24HOUR);
  display12Hour = new TRadioButton(this, IDC_DISPLAY12HOUR);
  displayHourLeadingZero = new TCheckBox(this, IDC_DISPLAYHOURLEADINGZERO);
  displaySeconds = new TCheckBox(this, IDC_DISPLAYSECONDS);

  horizontalMargin = new TSlider(this, IDC_HORIZONTALMARGIN, 0);
  verticalMargin = new TSlider(this, IDC_VERTICALMARGIN, 0);
}

TLedClockDialog::~TLedClockDialog()
{
}

void TLedClockDialog::SetupWindow()
{
  TDialog::SetupWindow();

  SetTimer(ID_TIMER, 1000);

  if (ledClock->GetClockDisplay() == TLedClock::Display24Hour)
    display24Hour->SetCheck(BF_CHECKED);
  else
    display12Hour->SetCheck(BF_CHECKED);

  displayHourLeadingZero->SetCheck(ledClock->GetDisplayHourLeadingZero() ? BF_CHECKED : BF_UNCHECKED);
  displaySeconds->SetCheck(ledClock->GetDisplaySeconds() ? BF_CHECKED : BF_UNCHECKED);

  horizontalMargin->SetRange(0, 10, false);
  verticalMargin->SetRange(0, 10, false);

  horizontalMargin->SetRuler(1, true);
  verticalMargin->SetRuler(1, true);

  horizontalMargin->SetPosition(ledClock->GetXMargin());
  verticalMargin->SetPosition(ledClock->GetYMargin());

  //ledClock->SetBkgndColor(TColor::Black);
}

void TLedClockDialog::CleanupWindow()
{
  KillTimer(ID_TIMER);

  TDialog::CleanupWindow();
}

void TLedClockDialog::EvTimer(owl::uint)
{
  ledClock->Invalidate();
}

void TLedClockDialog::SettingChanged()
{
  ledClock->SetClockDisplay(display24Hour->GetCheck() == BF_CHECKED ? TLedClock::Display24Hour : TLedClock::Display12Hour);
  ledClock->SetDisplayHourLeadingZero(displayHourLeadingZero->GetCheck() == BF_CHECKED);
  ledClock->SetDisplaySeconds(displaySeconds->GetCheck() == BF_CHECKED);

  ledClock->Invalidate();
}

void TLedClockDialog::SliderSettingChanged(owl::uint n) 
{ 
  ledClock->SetXMargin(horizontalMargin->GetPosition());
  ledClock->SetYMargin(verticalMargin->GetPosition());

  ledClock->Invalidate();
}

TResult TLedClockDialog::EvNotify(uint ctlId, TNotify& notifyInfo)
{
  // Note that for TTN_NEEDTEXT from a tooltip, TNotify::idFrom contains the HWND of the control
  // for which the tooltip displays. TNotify::hwndFrom is the handle of the tooltip window itself.
  //
  const auto ctrl = GetWindowPtr(reinterpret_cast<HWND>(notifyInfo.idFrom));
  if (notifyInfo.code == TTN_NEEDTEXT && (ctrl == horizontalMargin || ctrl == verticalMargin))
  {
    auto& tt = reinterpret_cast<TTooltipText&>(notifyInfo);
    const auto t = tstring{tt.szText};
    auto s = tstringstream{};
    if (t == _T("0"))
      s << _T("no margin");
    else if (t == _T("1"))
      s << _T("1 pt margin");
    else
      s << t << _T(" pts margin");
    tt.CopyText(s.str());
  }
  return TDialog::EvNotify(ctlId, notifyInfo);
}

void TLedClockDialog::BnForeground()
{
  TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, ledClock->GetTextColor());
  d.CustColors = &CustomColors[0];
  TChooseColorDialog dlg(this, d);
  if (dlg.Execute() != IDOK) return;

  ledClock->SetTextColor(d.Color);
  ledClock->Invalidate();
}

void TLedClockDialog::BnBackground()
{
  TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, ledClock->GetBkgndColor());
  d.CustColors = &CustomColors[0];
  TChooseColorDialog dlg(this, d);
  if (dlg.Execute() != IDOK) return;

  ledClock->SetBkgndColor(d.Color);
  ledClock->Invalidate();
}
