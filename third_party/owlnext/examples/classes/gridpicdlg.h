#pragma once
#include <owl/dialog.h>
#include <owl/picker.h>
#include <owl/celarray.h>

class TGridPickerDialog : public owl::TDialog
{
public:
  TGridPickerDialog(owl::TWindow* parent);
  ~TGridPickerDialog();

protected:
  void SetupWindow();

private:
  owl::TCelArray*    CelArray;
  owl::TGridPicker*  Picker1;
  owl::TGridPicker*  Picker2;
  owl::TGridPicker*  Picker3;
  owl::TGridPicker*  Picker4;
  owl::TGridPicker*  Picker5;
  owl::TGridPicker*  Picker6;
  owl::TGridPicker*  Picker7;
  owl::TGridPicker*  Picker8;
  owl::TColorPicker* ColorPicker;
};

