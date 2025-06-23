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
  HBITMAP  HLoco1, HLoco2;

public:
  TAbout(PTWindowsObject AParent, LPSTR AName);
  virtual void SetupWindow();
  virtual void Ok(RTMessage Msg) = [ID_FIRST + IDOK];
  virtual void WMPaint(RTMessage Msg) = [WM_FIRST + WM_PAINT];
};


#endif

