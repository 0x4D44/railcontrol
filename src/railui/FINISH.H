/* FINISH.H
*  ========
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

#if !defined(_FINISH_H_)
#define _FINISH_H_

#include <owl\static.h>
#include "general.h"


/* Class definitions for TFinish */
_CLASSDEF(TFinish)

class TFinish;
typedef TFinish* PFinish;

class TFinish : public TDialog
{
public:
  TStatic       *Stat1,
					 *Stat2,
					 *Stat3,
					 *Stat4;     // Handles to four static text controls 

  TFinish(TWindow* parent, LPCTSTR AName);
  virtual void SetupWindow();
};


#endif  // _FINISH_H_

