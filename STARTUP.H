/* STARTUP.H
*  =========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date	   : 25/08/1994
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

/* Header file for startup dialog class */


#if !defined(_STARTUP_H_)
#define _STARTUP_H_


// Define STARTUP dialog class
_CLASSDEF(TStartup)

class TStartup;
typedef TStartup* PStartup;

class TStartup : public TWindow
{
public:
  HBITMAP   HLoader;

  TStartup(TFrameWindow* parent);
  ~TStartup();
  void SetupWindow(void);
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
};


#endif   // _STARTUP_H_
