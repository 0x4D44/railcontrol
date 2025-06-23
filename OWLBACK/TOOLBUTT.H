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
  int   XPos[20];           // Button X positions
  WORD  WParam[20];         // Button response message WParam value
  long  LParam[20];         // Button response message LParam value
  HWND  SentWindow[20],     // Handle of window to send message to
	StatWindow[20];     // Handle of window to send status bar info to
  char  UpText[20][100],    // Button up position text
	DownText[20][100];  // Button down position text
} Toolbuttondata;


// Define TOOLBUTTON window class
_CLASSDEF(TToolbutton)
typedef TToolbutton* PToolbutton;

class TToolbutton : public TWindow
{
  private:
    BOOL             UpPosition,     // Button is in the 'Up' position
		     Enabled,        // Is the button enabled?
		     LMseButton;     // Is the left mouse button selected
    char	     UpText[100],    // Text for button in up position
		     DownText[100];  // Text for button in down position
    WORD	     WParam;	     // Message WParam value
    long             LParam;         // Message LParam value
    HWND             SentWindow,     // Handle of window to send message to
		     StatWindow;     // Handle of window to send SetText message to
    int              ButtonNum;      // Tool button number


  public:
    TToolbutton(PTWindowsObject AParent, int XPos, int YPos, int ButtNum, WORD WPAR,
		long LPAR, HWND SentWin, HWND StatWin, char* UpTxt,
		char* DownTxt);
    ~TToolbutton();
    virtual void SetupWindow();
    virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo);
    virtual void DrawScreen(HDC TheDC);
    virtual void GetWindowClass(WNDCLASS &WndClass);
    virtual LPSTR GetClassName();
    virtual void Enable(BOOL GivEnabled);
    virtual void WMLButtonDown(RTMessage Msg) = [WM_FIRST + WM_LBUTTONDOWN];
    virtual void WMLButtonUp(RTMessage Msg) = [WM_FIRST + WM_LBUTTONUP];
    virtual void WMMouseMove(RTMessage Msg) = [WM_FIRST + WM_MOUSEMOVE];
    virtual void SetDownText(char* TextString);
    virtual void SetUpText(char* TextString);
};


#endif