// OWLCVT 05/11/95 22:38:01
/* CONFIGUR.CPP
*  ============
*
*  PROGRAM DESCRIPTION
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


TConfigur::TConfigur(TWindow* parent, LPSTR name)
 : TDialog(parent, name)
{
  TRC_NRM((TB, "Constructing new configuration window"));

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
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  TRC_NRM((TB, "Setting up configuration window"));

  // Set up the timer speed
  TRC_NRM((TB, "Timer speed is:%d", frame->TimerSpeed));
  switch (frame->TimerSpeed)
  {
	 case 1: Radio1->SetCheck(BF_CHECKED); break;
	 case 2: Radio2->SetCheck(BF_CHECKED); break;
	 case 3: Radio3->SetCheck(BF_CHECKED); break;
  }

  // Set up the controls - <DelayEnable>
  TRC_NRM((TB, "Delay is %sabled", frame->DelayEnable ? "En" : "Dis"));
  if (frame->DelayEnable)
  {
	 Check1->SetCheck(BF_CHECKED);
  }
  else
  {
	 Check1->SetCheck(BF_UNCHECKED);
  }

  // <LocoRefuel>
  TRC_NRM((TB, "LocoRefuel is %sabled", frame->LocoRefuel ? "En" : "Dis"));
  if (frame->LocoRefuel)
  {
	 Check2->SetCheck(BF_CHECKED);
  }
  else
  {
	 Check2->SetCheck(BF_UNCHECKED);
  }

  // <SaveOnExit>
  TRC_NRM((TB, "SaveOnExit is %sabled", frame->SaveOnExit ? "En" : "Dis"));
  if (frame->SaveOnExit)
  {
	 Check3->SetCheck(BF_CHECKED);
  }
  else
  {
	 Check3->SetCheck(BF_UNCHECKED);
  }

  // <StartOptim>
  TRC_NRM((TB, "StartOptim is %sabled", frame->StartOptim ? "En" : "Dis"));
  if (frame->StartOptim)
  {
	 Check4->SetCheck(BF_CHECKED);
  }
  else
  {
	 Check4->SetCheck(BF_UNCHECKED);
  }

  // <SoundEnable>
  TRC_NRM((TB, "SoundEnable is %sabled", frame->SoundEnable ? "En" : "Dis"));
  if (frame->SoundEnable)
  {
	 Check5->SetCheck(BF_CHECKED);
  }
  else
  {
	 Check5->SetCheck(BF_UNCHECKED);
  }
}


void TConfigur::CmOk()
{
  // Get handle to main frame window
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  TRC_NRM((TB, "OK pressed - updating configuration information"));

  // Update parent field - <TimerSpeed>
  if (Radio1->GetCheck() == BF_CHECKED)  frame->TimerSpeed = 1;
  if (Radio2->GetCheck() == BF_CHECKED)  frame->TimerSpeed = 2;
  if (Radio3->GetCheck() == BF_CHECKED)  frame->TimerSpeed = 3;

  // Update parent fields - <DelayEnable>
  if (Check1->GetCheck() == BF_CHECKED)
  {
	 frame->DelayEnable = TRUE;
  }
  else
  {
	 frame->DelayEnable = FALSE;
  }

  // <LocoRefuel>
  if (Check2->GetCheck() == BF_CHECKED)
  {
	 frame->LocoRefuel = TRUE;
  }
  else
  {
	 frame->LocoRefuel = FALSE;
  }

  // <SaveOnExit>
  if (Check3->GetCheck() == BF_CHECKED)
  {
	 frame->SaveOnExit = TRUE;
  }
  else
  {
	 frame->SaveOnExit = FALSE;
  }

  // <StartOptim>
  if (Check4->GetCheck() == BF_CHECKED)
  {
	 frame->StartOptim = TRUE;
  }
  else
  {
	 frame->StartOptim = FALSE;
  }

  // <SoundEnable>
  if (Check5->GetCheck() == BF_CHECKED)
  {
	 frame->SoundEnable = TRUE;
  }
  else
  {
	 frame->SoundEnable = FALSE;
  }

  // Call ancestor method
  TDialog::CmOk();
}


void TConfigur::OnHandleHelp()
{
  TRC_NRM((TB, "Handle help window"));

  // Display help file
  WinHelp(HELPFILENAME, HELP_CONTEXT, 101);
}


DEFINE_RESPONSE_TABLE1(TConfigur, TDialog)
  EV_COMMAND(IDOK, CmOk),
  EV_COMMAND(ID_HELP_BUTTON, OnHandleHelp),
END_RESPONSE_TABLE;
