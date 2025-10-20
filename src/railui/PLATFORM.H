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
  int        mRowHeight;
  int        mRowSpacing;
  int        mClientWidth;
  int        mClientHeight;
  int        mContentRight;
  int        mContentBottom;

  void ResetContentExtents();
  void UpdateMetrics(HDC dc);
  void TrackContent(int right, int bottom);
  void EnsureContentFits();

public:
  TPlatform(TWindow * AParent, int XPos, int YPos);
  ~TPlatform();
  void GetWindowClass(WNDCLASS& WndClass);
  auto GetWindowClassName() -> owl::TWindowClassName override;
  void SetupWindow(void);
  virtual bool CanClose();
  void EvSize(owl::uint sizeType, const TSize& size);
  void EvMove(const TPoint&);
  virtual void Paint(TDC& dc, bool erase, TRect& rect);
  void UpdateDisplay(HDC TheDC, BOOL Redraw);

  DECLARE_RESPONSE_TABLE( TPlatform );
};


#endif
