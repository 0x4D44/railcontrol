// OWLCVT 05/11/95 22:34:01
/* ABOUT.H
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

#if !defined(_ABOUT_H_)
#define _ABOUT_H_

/* Class definitions for TAbout */
_CLASSDEF(TAbout)

class TAbout;
typedef TAbout* PAbout;

class TAbout : public TDialog
{
private:
  // SECURITY FIX: Use RAII guards for automatic bitmap cleanup
  TBitmapGuard HLoco1;
  TBitmapGuard HLoco2;

public:
  TAbout(TWindow* parent, LPCTSTR name);
  void SetupWindow();
  void CmOk();
  void EvPaint();

  DECLARE_RESPONSE_TABLE(TAbout);
};


#endif

