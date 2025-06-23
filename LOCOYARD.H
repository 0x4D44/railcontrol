// OWLCVT 05/11/95 22:33:59
/* LOCOYARD.H
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
*  Version  : 2.0
*  Language : C++ (BORLAND v3.1)
*
*/

#if !defined(_LOCOYARD_H_)
#define _LOCOYARD_H_


// Define TLocoyard window class
class TLocoyard;
typedef TLocoyard* PLocoyard;

class TLocoyard : public TFrameWindow
{
private:
  int        OldSelect;              // The reference of the currently selected loco
  int        OldXPos,                // Old selected loco XPos
             OldYPos;                // Old selected loco YPos
  int        OldYardLoco[16];        // Old locoyard locos
  int        TheRoute;               // Passed route number parameter
  BOOL       InWhichLoco;            // We are in the which loco routine


public:
  TLocoyard(TWindow * AParent, int XPos, int YPos);
  ~TLocoyard();
  void GetWindowClass(WNDCLASS& WndClass);
  LPSTR GetClassName();
  void SetupWindow(void);
  BOOL CanClose();
  void EvSize( UINT, TSize& );
  void EvMove( TPoint& );
  void EvLButtonDown( UINT, TPoint& );
  void EvLButtonUp( UINT, TPoint& );
  void EvMouseMove( UINT, TPoint& );
  void Paint(TDC& dc, BOOL erase, TRect& rect);
  void UpdateDisplay(HDC TheDC, BOOL Redraw);
  void WhichLoco(int RouteNo);

  DECLARE_RESPONSE_TABLE( TLocoyard );
};


#endif


