#pragma once

#include "resizabledialog.h"

#include <owl/edit.h>
#include <owl/combobox.h>
#include <owl/button.h>
#include <owl/listbox.h>
#include <owl/listviewctrl.h>

class TSampleResizableDialog1 : public TResizableDialog
{
public:
  TSampleResizableDialog1(owl::TWindow *parent, owl::TResId resId = 0, owl::TModule *module = 0);
  ~TSampleResizableDialog1();

protected:
  void SetupWindow();
  void SetupLayout();

private:
  owl::TEdit* edit1;
  owl::TComboBox* combo1;

  owl::TListBox* listBox1;
  owl::TListViewCtrl* listView1;

  owl::TButton* button1;
  owl::TButton* button2;
  owl::TButton* button3;
  owl::TButton* button4;
  owl::TButton* button5;
  owl::TButton* button6;
  owl::TButton* button7;

  DECLARE_RESPONSE_TABLE(TSampleResizableDialog1);
};

