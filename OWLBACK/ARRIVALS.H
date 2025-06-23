/* ARRIVALS.H
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
*  Version  : 1.0
*  Language : C++ (BORLAND v3.1)
*
*/

#if !defined(_ARRIVALS_H_)
#define _ARRIVALS_H_

// Define TArrivals window class
class TArrivals;
typedef TArrivals* PArrivals;

class TArrivals : public TWindow
{
private:
  LOGFONT    Font;                   // Handle to the main window font record
  HFONT      HFont;                  // Handle to the main window font

public:
  TArrivals(PTWindowsObject AParent, int XPos, int YPos);
  ~TArrivals();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPSTR GetClassName();
  virtual void SetupWindow(void);
  virtual BOOL CanClose();
  virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
  virtual void WMMove(RTMessage Msg) = [WM_FIRST + WM_MOVE];
  virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR & PaintInfo);
  virtual void UpdateDisplay(HDC TheDC, BOOL Redraw);
  virtual void TimeTabInfo(HDC TheDC, int TabVal, int YPos);
  virtual void ConvertTime(int TimeVal, char *TimeStr);

  // Friend classes
};


#endif

