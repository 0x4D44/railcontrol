// OWLCVT 05/11/95 22:33:58
/* DEPARTUR.H
*  ==========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date     :
*  Version  : 2.1
*  Language : C++ (BORLAND v3.1)
*
*/

#if !defined(_DEPARTUR_H_)
#define _DEPARTUR_H_

// Define TDepartur window class
class TDepartur;
typedef TDepartur* PDepartur;

class TDepartur : public TFrameWindow
{
private:
  int        OldStatus[DISP_NUM_DEPART];    // Old status values

public:
  TDepartur(TWindow * AParent, int XPos, int YPos);
  ~TDepartur();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  virtual LPCTSTR GetClassName();
  virtual void SetupWindow(void);
  virtual bool CanClose();
  void EvSize( UINT, TSize& );
  void EvMove( TPoint& );
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
  virtual void UpdateDisplay(HDC TheDC, BOOL Redraw);
  virtual void TimeTabInfo(HDC TheDC, int TabVal, int YPos);
  virtual void ConvertTime(int TimeVal, char *TimeStr);

  // Friend classes
DECLARE_RESPONSE_TABLE( TDepartur );
};


#endif
