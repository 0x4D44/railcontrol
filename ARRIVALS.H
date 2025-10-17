// OWLCVT 05/11/95 22:33:58
/* ARRIVALS.H
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

#if !defined(_ARRIVALS_H_)
#define _ARRIVALS_H_

// Text display offsets
#define ARR_OFFSET_DUE        10
#define ARR_OFFSET_ON         (ARR_OFFSET_DUE     + 40)
#define ARR_OFFSET_LATE       (ARR_OFFSET_ON      + 30)
#define ARR_OFFSET_LOCO       (ARR_OFFSET_LATE    + 20)
#define ARR_OFFSET_ARRDESC    (ARR_OFFSET_LOCO    + 125)
#define ARR_OFFSET_STATUS     (ARR_OFFSET_ARRDESC + 135)
#define ARR_OFFSET_DEPTIME    (ARR_OFFSET_STATUS  + 130)
#define ARR_OFFSET_DEPDESC    (ARR_OFFSET_DEPTIME + 40)


// Define TArrivals window class
class TArrivals;
typedef TArrivals* PArrivals;

class TArrivals : public TFrameWindow
{
public:
  TArrivals(TWindow * AParent, int XPos, int YPos);
  ~TArrivals();
  virtual void GetWindowClass(WNDCLASS& WndClass);
  auto GetWindowClassName() -> owl::TWindowClassName override;
  virtual void SetupWindow(void);
  virtual bool CanClose();
  void EvSize(owl::uint sizeType, const TSize& size);
  void EvMove(const TPoint& point);
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
  virtual void UpdateDisplay(HDC TheDC, BOOL Redraw);
  virtual void TimeTabInfo(HDC TheDC, int TabVal, int YPos);
  virtual void ConvertTime(int TimeVal, char *TimeStr);

  // Friend classes
DECLARE_RESPONSE_TABLE( TArrivals );
};


#endif

