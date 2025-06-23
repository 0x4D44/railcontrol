/* START.CPP
*  =========
*
*  PROGRAM DECSRIPTION
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


TStart::TStart(PTWindowsObject AParent, LPSTR AName)
 : TDialog(AParent, AName)
{
  Stat1 = new TStatic(this, ID_STRTTEXT1, 0);
  Stat2 = new TStatic(this, ID_STRTTEXT2, 0);
  Stat3 = new TStatic(this, ID_STRTTEXT3, 0);
}


void TStart::SetupWindow()
{
  // Call ancestor method
  TDialog::SetupWindow();

  Stat1->SetText(PMainWindow(Parent)->DisplayHan->StartText[0]);
  Stat2->SetText(PMainWindow(Parent)->DisplayHan->StartText[1]);
  Stat3->SetText(PMainWindow(Parent)->DisplayHan->StartText[2]);
}






