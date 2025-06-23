// OWLCVT 05/11/95 22:36:42
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


TFinish::TFinish(TWindow* parent, LPSTR name)
 : TDialog(parent, name)
{
  Stat1 = new TStatic(this, ID_FINIACHIE, 0);
  Stat2 = new TStatic(this, ID_FINIARRDE, 0);
  Stat3 = new TStatic(this, ID_FINIDEPDE, 0);
  Stat4 = new TStatic(this, ID_FINIRATIN, 0);
}


void TFinish::SetupWindow()
{
  char  szText1[100];
  float lArrDelay  = 0;
  float lDepDelay  = 0;
  float lAvDelay   = 0;

  // Call ancestor method
  TDialog::SetupWindow();
  TLayout* layout = TYPESAFE_DOWNCAST(Parent, TLayout);

  /**************************************************************************/
  /* Calculate average delays.                                              */
  /**************************************************************************/
  lArrDelay = layout->ArrDelay;
  lArrDelay /= (float) (2 * layout->ArrNum);
  lDepDelay = layout->DepDelay;
  lDepDelay /= (float) (2 * layout->DepNum);
  lAvDelay = (lArrDelay + lDepDelay) / 2;

  // Set up static controls
  sprintf(szText1, "During your %d hour shift you achieved:", layout->ShiftLength);
  Stat1->SetText(szText1);
  sprintf(szText1, "Average arrival delay: %.2f mins (%d trains)", lArrDelay, layout->ArrNum);
  Stat2->SetText(szText1);
  sprintf(szText1, "Average departure delay: %.2f mins (%d trains)", lDepDelay, layout->DepNum);
  Stat3->SetText(szText1);

  if ((layout->ArrNum > 0) || (layout->DepNum > 0))
  {
    if (lAvDelay == 0)
    {
      Stat4->SetText("Performance rating: Outstanding!");
    }
    if ((lAvDelay > 0) && (lAvDelay <= 5))
    {
      Stat4->SetText("Performance rating: Excellent");
    }
    if ((lAvDelay > 5) && (lAvDelay <= 10))
    {
      Stat4->SetText("Performance rating: Competent");
    }
    if ((lAvDelay > 10) && (lAvDelay <= 20))
    {
      Stat4->SetText("Performance rating: Unsatisfactory");
    }
    if (lAvDelay > 20)
    {
      Stat4->SetText("Performance rating: Disastrous");
    }
  }
  else
  {
    Stat4->SetText("Performance rating: Atrocious");
  }
}






