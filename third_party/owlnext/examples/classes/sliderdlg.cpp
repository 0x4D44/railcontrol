#include "pch.h"
#include "sliderdlg.h"
#include "resource.h"

using namespace owl;


enum
{
  IDC_THERMOSTAT = 201,
  IDC_HEATERTIME,
  IDC_OUTSIDETEMP,
  IDC_STATICTEMP,
  IDC_STATICTIME,
  IDC_STATICOTEMP,
  IDC_THERMOMETER,
};

enum { IDT_REFRESH = 1 };

DEFINE_RESPONSE_TABLE1(TSliderDialog, TDialog)
EV_WM_TIMER,
EV_CHILD_NOTIFY_ALL_CODES(IDC_HEATERTIME, UpdateHysteresis),
EV_CHILD_NOTIFY_ALL_CODES(IDC_OUTSIDETEMP, UpdateOutsideTemp),
END_RESPONSE_TABLE;

TSliderDialog::TSliderDialog(TWindow* parent)
  : TDialog(parent, TResId(IDD_SLIDERDIALOG)),
  Temp(40.0),
  IsHeaterOn(false)
{
  TempStatic = new TStatic(this, IDC_STATICTEMP, _T(""), 110, 30, 160, 17, 0);
  ThermometerGauge = new TGauge(this, _T("%d\xB0"), IDC_THERMOMETER, 70, 70, 240, 24, true, 2);
  ThermostatSlider = new THSlider(this, IDC_THERMOSTAT, 70, 110, 240, 40);
  HysteresisStatic = new TStatic(this, IDC_STATICTIME, _T(""), 4, 10, 160, 17, 0);
  HysteresisSlider = new TVSlider(this, IDC_HEATERTIME, 20, 30, 32, 160);
  OutsideTempStatic = new TStatic(this, IDC_STATICOTEMP, _T(""), 216, 10, 160, 17, 0);
  OutsideTempSlider = new TVSlider(this, IDC_OUTSIDETEMP, 330, 30, 32, 160);

  TempStatic->ModifyStyle(0, SS_CENTER);
  ThermostatSlider->ModifyStyle(0, TBS_AUTOTICKS);
  HysteresisStatic->ModifyStyle(0, SS_LEFT);
  HysteresisSlider->ModifyStyle(0, TBS_AUTOTICKS);
  OutsideTempStatic->ModifyStyle(0, SS_RIGHT);
  OutsideTempSlider->ModifyStyle(0, TBS_AUTOTICKS);
}


TSliderDialog::~TSliderDialog()
{
}

void
TSliderDialog::SetupWindow()
{
  TDialog::SetupWindow();

  ThermometerGauge->SetRange(40, 120);
  ThermometerGauge->SetValue(80);

  ThermostatSlider->SetRange(40, 120);
  ThermostatSlider->SetRuler(10, false);
  ThermostatSlider->SetPosition(80);

  HysteresisSlider->SetRange(0, 10);
  HysteresisSlider->SetRuler(5, false);
  HysteresisSlider->SetPosition(5);

  OutsideTempSlider->SetRange(20, 90);
  OutsideTempSlider->SetRuler(10, false);
  OutsideTempSlider->SetPosition(40);

  SetTimer(IDT_REFRESH, 1000);

  UpdateTemp();
  UpdateHysteresis();
  UpdateOutsideTemp();
}

void
TSliderDialog::EvTimer(uint /*timerId*/)
{
  SimulateHeater();
  UpdateTemp();
}

void
TSliderDialog::UpdateTemp()
{
  tostringstream s;
  s << _T("Heater is ") << (IsHeaterOn ? _T("on") : _T("off"));
  TempStatic->SetText(s.str());
  ThermometerGauge->SetValue(static_cast<int>(Temp + 0.5));
}

void
TSliderDialog::UpdateHysteresis(uint)
{
  tostringstream s;
  s << HysteresisSlider->GetPosition() << _T("\xB0 hysteresis");
  HysteresisStatic->SetText(s.str());
}

void
TSliderDialog::UpdateOutsideTemp(uint)
{
  tostringstream s;
  s << OutsideTempSlider->GetPosition() << _T("\xB0 outside");
  OutsideTempStatic->SetText(s.str());
}

void
TSliderDialog::SimulateHeater()
{
  Temp += (IsHeaterOn ? 2.0 : 0.0) + // heater output
    (OutsideTempSlider->GetPosition() - Temp) / 40.0; // heat transfer

  int tempSetting = ThermostatSlider->GetPosition();
  int hysteresis = HysteresisSlider->GetPosition();
  double lowTriggerTemp = tempSetting - hysteresis;
  double highTriggerTemp = tempSetting + hysteresis;

  if (!IsHeaterOn && Temp <= lowTriggerTemp)
    IsHeaterOn = true;
  else if (IsHeaterOn && Temp >= highTriggerTemp)
    IsHeaterOn = false;
}
