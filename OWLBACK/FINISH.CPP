/* FINISH.CPP
*  ==========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     : 01/11/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#include "classdef.h"

/*************************************************************************/
/* Methods of TFinish follow...*/


TFinish::TFinish(PTWindowsObject AParent, LPSTR AName)
 : TDialog(AParent, AName)
{
  Stat1 = new TStatic(this, ID_FINIACHIE, 0);
  Stat2 = new TStatic(this, ID_FINIARRDE, 0);
  Stat3 = new TStatic(this, ID_FINIDEPDE, 0);
  Stat4 = new TStatic(this, ID_FINIRATIN, 0);
}


void TFinish::SetupWindow()
{
  char  szText1[100];
  int   Delay;

  // Call ancestor method
  TDialog::SetupWindow();

  // Set up static controls
  wsprintf(szText1, "During your %d hour shift you achieved", PLayout(Parent)->ShiftLength);
  Stat1->SetText(szText1);
  wsprintf(szText1, "Average arrival delay: %d mins (%d trains)", (PLayout(Parent)->ArrDelay)/(2*PLayout(Parent)->ArrNum), PLayout(Parent)->ArrNum);
  Stat2->SetText(szText1);
  wsprintf(szText1, "Average departure delay: %d mins (%d trains)", (PLayout(Parent)->DepDelay)/(2*PLayout(Parent)->DepNum), PLayout(Parent)->DepNum);
  Stat3->SetText(szText1);

  Delay = (PLayout(Parent)->ArrDelay)/(PLayout(Parent)->ArrNum) +
	  (PLayout(Parent)->DepDelay)/(PLayout(Parent)->DepNum);
  if ((PLayout(Parent)->ArrNum > 0) && (PLayout(Parent)->DepNum > 0))
  {
    if (Delay == 0)
    {
      Stat4->SetText("Performance rating: Outstanding!");
    }
    if ((Delay > 0) && (Delay <= 5))
    {
      Stat4->SetText("Performance rating: Excellent");
    }
    if ((Delay > 5) && (Delay <= 10))
    {
      Stat4->SetText("Performance rating: Competent");
    }
    if ((Delay > 10) && (Delay <= 20))
    {
      Stat4->SetText("Performance rating: Unsatisfactory");
    }
    if (Delay > 20)
    {
      Stat4->SetText("Performance rating: Disastrous");
    }
  }
  else
  {
    Stat4->SetText("Performance rating: Atrocious");
  }
}






