// OWLCVT 05/11/95 22:37:40
/* START.CPP
*  =========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     :
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"

/*************************************************************************/
/* Methods of TStart follow...*/


TStart::TStart(TFrameWindow* parent, LPSTR name)
 : TDialog(parent, name)
{
  Stat1 = new TStatic(this, ID_STRTTEXT1, 0);
  Stat2 = new TStatic(this, ID_STRTTEXT2, 0);
  Stat3 = new TStatic(this, ID_STRTTEXT3, 0);
}


void TStart::SetupWindow()
{
  // Call ancestor method
  TDialog::SetupWindow();
  TMainWindow* frame = TYPESAFE_DOWNCAST(Parent, TMainWindow);

  Stat1->SetText(frame->DisplayHan->StartText[0]);
  Stat2->SetText(frame->DisplayHan->StartText[1]);
  Stat3->SetText(frame->DisplayHan->StartText[2]);
}






