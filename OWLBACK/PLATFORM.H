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

class TPlatform : public TWindow
{
private:
  LOGFONT    Font;                   // Handle to the main window font record
  HFONT      HFont;                  // Handle to the main window font
  int        Timetab[15];            // What is in the platforms at the moment?
  char       Currently[50][40];      // Strings currently displayed on the screen

public:
  TPlatform(PTWindowsObject AParent, int XPos, int YPos);
  ~TPlatform();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow(void);
  virtual BOOL CanClose();
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
  virtual void WMMove(RTMessage Msg) = [WM_FIRST + WM_MOVE];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void UpdateDisplay(HDC TheDC, BOOL Redraw);

  // Friend classes
};


#endif
