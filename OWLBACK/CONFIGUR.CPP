/* CONFIGUR.CPP
*  ============
*
*  PROGRAM DECSRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 28/12/1993
*  Version  : 1.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"

/***************************************************************/
/* Methods of TConfigur follow...  */


TConfigur::TConfigur(PTWindowsObject AParent, LPSTR AName)
 : TDialog(AParent, AName)
{
  Radio1 = new TRadioButton(this, ID_CONFSLOWS, 0);
  Radio2 = new TRadioButton(this, ID_CONFNORMA, 0);
  Radio3 = new TRadioButton(this, ID_CONFFASTS, 0);
  Check1 = new TCheckBox(this, ID_CONFDELAY, 0);
  Check2 = new TCheckBox(this, ID_CONFREFUE, 0);
  Check3 = new TCheckBox(this, ID_CONFSEXIT, 0);
  Check4 = new TCheckBox(this, ID_CONFAUOPT, 0);
  Check5 = new TCheckBox(this, ID_CONFSOUND, 0);
}


void TConfigur::SetupWindow()
{
  // Call ancestor method
  TDialog::SetupWindow();

  // Set up the timer speed
  switch (PMainWindow(Parent)->TimerSpeed)
  {
    case 1: Radio1->SetCheck(BF_CHECKED); break;
    case 2: Radio2->SetCheck(BF_CHECKED); break;
    case 3: Radio3->SetCheck(BF_CHECKED); break;
  }

  // Set up the controls - <DelayEnable>
  if (PMainWindow(Parent)->DelayEnable)
  {
    Check1->SetCheck(BF_CHECKED);
  }
  else
  {
    Check1->SetCheck(BF_UNCHECKED);
  }

  // <LocoRefuel>
  if (PMainWindow(Parent)->LocoRefuel)
  {
    Check2->SetCheck(BF_CHECKED);
  }
  else
  {
    Check2->SetCheck(BF_UNCHECKED);
  }

  // <SaveOnExit>
  if (PMainWindow(Parent)->SaveOnExit)
  {
    Check3->SetCheck(BF_CHECKED);
  }
  else
  {
    Check3->SetCheck(BF_UNCHECKED);
  }

  // <StartOptim>
  if (PMainWindow(Parent)->StartOptim)
  {
    Check4->SetCheck(BF_CHECKED);
  }
  else
  {
    Check4->SetCheck(BF_UNCHECKED);
  }

  // <SoundEnable>
  if (PMainWindow(Parent)->SoundEnable)
  {
    Check5->SetCheck(BF_CHECKED);
  }
  else
  {
    Check5->SetCheck(BF_UNCHECKED);
  }
}


void TConfigur::Ok(RTMessage Msg)
{
  // Update parent field - <TimerSpeed>
  if (Radio1->GetCheck() == BF_CHECKED)  PMainWindow(Parent)->TimerSpeed = 1; 
  if (Radio2->GetCheck() == BF_CHECKED)  PMainWindow(Parent)->TimerSpeed = 2;
  if (Radio3->GetCheck() == BF_CHECKED)  PMainWindow(Parent)->TimerSpeed = 3;

  // Update parent fields - <DelayEnable>
  if (Check1->GetCheck() == BF_CHECKED)
  {
    PMainWindow(Parent)->DelayEnable = TRUE;
  }
  else
  {
    PMainWindow(Parent)->DelayEnable = FALSE;
  }

  // <LocoRefuel>
  if (Check2->GetCheck() == BF_CHECKED)
  {
    PMainWindow(Parent)->LocoRefuel = TRUE;
  }
  else
  {
    PMainWindow(Parent)->LocoRefuel = FALSE;
  }

  // <SaveOnExit>
  if (Check3->GetCheck() == BF_CHECKED)
  {
    PMainWindow(Parent)->SaveOnExit = TRUE;
  }
  else
  {
    PMainWindow(Parent)->SaveOnExit = FALSE;
  }

  // <StartOptim>
  if (Check4->GetCheck() == BF_CHECKED)
  {
    PMainWindow(Parent)->StartOptim = TRUE;
  }
  else
  {
    PMainWindow(Parent)->StartOptim = FALSE;
  }

  // <SoundEnable>
  if (Check5->GetCheck() == BF_CHECKED)
  {
    PMainWindow(Parent)->SoundEnable = TRUE;
  }
  else
  {
    PMainWindow(Parent)->SoundEnable = FALSE;
  }

  // Call ancestor method
  TDialog::Ok(Msg);
}


void TConfigur::HandleHelp(RTMessage)
{
  WinHelp(HWindow, HELPFILENAME, HELP_CONTEXT, 101);
}


