/* SELECTOR.H
*  ==========
*
*  PROGRAM DECSRIPTION
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

#if !defined(_SELECTOR_H_)
#define _SELECTOR_H_


/* Class definitions for TSelector */
class TSelector;
typedef TSelector* PSelector;

class TSelector : public TWindow
{
private:
  BOOL        SelectOld,            // Old selector in or out position?
              SelectCur,            // Current selector in or out position?
              LMseButton,           // Is the left mouse button down?
              LockedOut;            // Is the button locked out?
  int         SelectNum,            // Selector number
	      Type,                 // Selector type
	      PlatformRef,          // PPlatDataInfo array number (index ref)
              WPAR, LPAR;           // Message to be sent values
  char        MainText[5];         // Textstring for the button window
  LOGFONT     Font;	            // Default font record
  int         DXPos, DYPos,         // Default X and Y positions
	      DWidth, DHeight;      // Default height and width
  static int  Holdpoint;            // Selector number of the hold point
  static int  Locoyard;             // Selector number of the loco yard
  static int  HoldSection;          // Section number of the hold point


public:
  TSelector(PTWindowsObject AParent, int XPos, int YPos, int Width, int Height,
	    int GSelectNum, int GType, int PlatRef, int WPR, int LPR, BOOL En);
  ~TSelector();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow();
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void PaintButton(HDC TheDC);
  virtual void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
  virtual void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
  virtual void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];
  virtual void Release();
  static  void Reset();
  virtual void SetText(char *TextString);
  virtual BOOL IsSelected();
  virtual void GetText(char *TextString, int NumChars);
  virtual int  GetDXPos();
  virtual int  GetDYPos();
  virtual int  GetDWidth();
  virtual int  GetDHeight();
  static  int  GetHoldPoint();
  static  int  GetLocoyard();
  static  int  GetHoldSection();
  virtual int  GetType();
  virtual int  GetPlatRef();
};

#endif