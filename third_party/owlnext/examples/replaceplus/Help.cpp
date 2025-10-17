#include "pch.h"
#pragma hdrstop

#include "Help.h"
#include "App.rh"
#include <owl/dialog.h>

using namespace owl;

void ExecuteHelpDialog(TWindow* parent)
{
  TDialog d(parent, IDD_HELP);
  d.Execute();
}
