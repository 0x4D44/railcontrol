#pragma once

#include <owl/dialog.h>
#include <owl/radiobut.h>
#include <owl/checkbox.h>
#include <owl/slider.h>

#include "ledclock.h"

class TLedClockDialog : public owl::TDialog
{
public:
  TLedClockDialog(owl::TWindow* parent);
  ~TLedClockDialog();

protected:
  void SetupWindow();
  void CleanupWindow();

  void SettingChanged();
  void SliderSettingChanged(owl::uint);

  void EvTimer(owl::uint);

  void BnForeground();
  void BnBackground();

  owl::TResult EvNotify(owl::uint ctlId, owl::TNotify& notifyInfo);

private:
  TLedClock* ledClock;
  owl::TRadioButton* display24Hour;
  owl::TRadioButton* display12Hour;
  owl::TCheckBox* displayHourLeadingZero;
  owl::TCheckBox* displaySeconds;

  owl::TSlider* horizontalMargin;
  owl::TSlider* verticalMargin;

  DECLARE_RESPONSE_TABLE(TLedClockDialog);
};

