#pragma once
#include <owl/dialog.h>
#include <owl/slider.h>
#include <owl/gauge.h>
#include <owl/static.h>


class TSliderDialog : public owl::TDialog
{
public:
  TSliderDialog(owl::TWindow* parent);
  virtual ~TSliderDialog();

protected:

  virtual void SetupWindow(); // override
  void EvTimer(owl::uint timerId);

private:

  double Temp;
  bool IsHeaterOn;
  owl::TStatic* TempStatic;
  owl::TGauge* ThermometerGauge;
  owl::THSlider* ThermostatSlider;
  owl::TStatic* HysteresisStatic;
  owl::TVSlider* HysteresisSlider;
  owl::TStatic* OutsideTempStatic;
  owl::TVSlider* OutsideTempSlider;

  void UpdateTemp();
  void UpdateHysteresis(owl::uint = 0);
  void UpdateOutsideTemp(owl::uint = 0);

  void SimulateHeater();

  DECLARE_RESPONSE_TABLE(TSliderDialog);

};

