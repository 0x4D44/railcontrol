/* LOCOYARD.H
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

#if !defined(_LOCOYARD_H_)
#define _LOCOYARD_H_


// Define TLocoyard window class
class TLocoyard;
typedef TLocoyard* PLocoyard;

class TLocoyard : public TWindow
{
private:
  LOGFONT    Font;                   // Handle to the main window font record
  HFONT      HFont;                  // Handle to the main window font
  int        OldSelect;              // The reference of the currently selected loco
  int        OldXPos,                // Old selected loco XPos
	     OldYPos;                // Old selected loco YPos
  int        OldYardLoco[16];        // Old locoyard locos
  int        TheRoute;               // Passed route number parameter
  BOOL       InWhichLoco;            // We are in the which loco routine


public:
  TLocoyard(PTWindowsObject AParent, int XPos, int YPos);
  ~TLocoyard();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow(void);
  virtual BOOL CanClose();
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
  virtual void WMMove(RTMessage Msg) = [WM_FIRST + WM_MOVE];
  virtual void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
  virtual void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
  virtual void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void UpdateDisplay(HDC TheDC, BOOL Redraw);
  virtual void WhichLoco(int RouteNo);
};


#endif


