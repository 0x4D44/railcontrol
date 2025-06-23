// OWLCVT 05/11/95 22:33:59
/* CONFIGUR.H
*  ==========
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

#if !defined(_CONFIGUR_H_)
#define _CONFIGUR_H_

#include <owl\checkbox.h>
#include <owl\radiobut.h>
#include "general.h"


/* Class definitions for TConfiguration */
_CLASSDEF(TConfigur)

class TConfigur;
typedef TConfigur* PConfigur;

class TConfigur : public TDialog
{
public:
  TRadioButton  *Radio1,
                *Radio2,
					 *Radio3;    // Handles to three radio buttons
  TCheckBox     *Check1,
					 *Check2,
					 *Check3,
					 *Check4,
					 *Check5;    // Handles to five check boxes

  TConfigur(TWindow * parent, LPSTR name);
  void SetupWindow();
  void CmOk();
  void OnHandleHelp();

  DECLARE_RESPONSE_TABLE(TConfigur);
};


#endif

