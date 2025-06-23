// OWLCVT 05/11/95 22:33:59
/* PLATFORM.H
*  ==========
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

#if !defined(_PLATFORM_H_)
#define _PLATFORM_H_ 


// Define TPlatform window class
_CLASSDEF(TPlatform)

class TPlatform;
typedef TPlatform* PPlatform;

class TPlatform : public TFrameWindow
{
private:
  int        Timetab[15];            // What is in the platforms at the moment?
  char       Currently[50][40];      // Strings currently displayed on the screen
// OWLCVT: Implementation of TListBoxData::Strings and TComboBoxData::Strings has changed.

public:
  TPlatform(TWindow * AParent, int XPos, int YPos);
  ~TPlatform();
  void GetWindowClass(WNDCLASS& WndClass);
  LPSTR GetClassName();
  void SetupWindow(void);
  BOOL CanClose();
  void EvSize(UINT, TSize&);
  void EvMove(TPoint&);
  void Paint(TDC& dc, BOOL erase, TRect& rect);
  void UpdateDisplay(HDC TheDC, BOOL Redraw);

  DECLARE_RESPONSE_TABLE( TPlatform );
};


#endif
