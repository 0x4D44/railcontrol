/* START.H
*  =======
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

#if !defined(_START_H_)
#define _START_H_

#include <owl\static.h>
#include "general.h"


/* Class definitions for TStart */
_CLASSDEF(TStart)

class TStart;
typedef TStart* PStart;

class TStart : public TDialog
{
public:
  TStatic       *Stat1,
					 *Stat2,
		          *Stat3;     // Handles to four static text controls

  TStart(TFrameWindow* parent, LPSTR AName);
  virtual void SetupWindow();
};


#endif  // _START_H_

