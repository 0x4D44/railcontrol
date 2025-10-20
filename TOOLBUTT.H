// OWLCVT 05/11/95 22:33:57
/* TOOLBUTT.H
*  ==========
*
*  PROGRAM DESCRIPTION
*  ===================
*
*
*  PROGRAM INFORMATION
*  ===================
*  Author   : M G Davidson
*  Date	    : 25/03/1994
*  Version  : 2.02
*  Language : C++ (BORLAND v3.1)
*
*/

/* Header file for tool button class */

#if !defined(_TOOLBUTT_H_)
#define _TOOLBUTT_H_


// Define toolbutton data structure
typedef struct
{
  int   XPos[20];             // Button X positions
  WORD  WParam[20];           // Button response message WParam value
  long  LParam[20];           // Button response message LParam value
  HWND  SentWindow[20],       // Handle of window to send message to
        StatWindow[20];       // Handle of window to send status bar info to
  TBuffer<100> UpText[20];    // Button up position text
  TBuffer<100> DownText[20];  // Button down position text
} Toolbuttondata;


// Define TOOLBUTTON window class
_CLASSDEF(TToolbutton)
class TToolbutton : public TFrameWindow
{
  private:
    BOOL             UpPosition,     // Button is in the 'Up' position
							Enabled,        // Is the button enabled?
			            LMseButton;     // Is the left mouse button selected
	 TBuffer<100>     UpText;           // Text for button in up position
	 TBuffer<100>     DownText;         // Text for button in down position
	 WORD	            WParam;	       // Message WParam value
	 long             LParam;         // Message LParam value
	 HWND             SentWindow,     // Handle of window to send message to
							StatWindow;     // Handle of window to send SetText message to
    int              ButtonNum;      // Tool button number


  public:
    TToolbutton(TWindow * AParent, int XPos, int YPos, int ButtNum, WORD WPAR,
        long LPAR, HWND SentWin, HWND StatWin, const char* UpTxt,
        const char* DownTxt);
    ~TToolbutton();
   void SetupWindow();
   virtual void Paint(TDC& dc, bool erase, TRect& rect);
   void DrawScreen(HDC TheDC);
   void GetWindowClass(WNDCLASS &WndClass);
   auto GetWindowClassName() -> owl::TWindowClassName override;
   void Enable(BOOL GivEnabled);
   void EvLButtonDown(owl::uint modKeys, const TPoint& point);
   void EvLButtonUp(owl::uint modKeys, const TPoint& point);
   void EvMouseMove(owl::uint modKeys, const TPoint& point);
   void SetDownText(const char* TextString);
   void SetUpText(const char* TextString);

    DECLARE_RESPONSE_TABLE(TToolbutton);
};


#endif
