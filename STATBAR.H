// OWLCVT 05/11/95 22:33:58
/* STATBAR.H
*  =========
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

/* Header file for status bar class */

#if !defined(_STATBAR_H_)
#define _STATBAR_H_



#define SB_SETTEXT  WM_USER + 1

// Define STATBAR window class
_CLASSDEF(TStatbar)
typedef TStatbar* PStatbar;

class TStatbar : public TFrameWindow
{
  private:
	 char      DisplayedText[100];    // Text to display in the status bar
	 LOGFONT   TheFontLog;            // Logical font structure used
	 HFONT     TheFont;               // Handle to the font

  public:
	 TStatbar(TWindow* AParent);
	 ~TStatbar();
	 void SetupWindow();
	 void Paint(TDC& dc, bool erase, TRect& rect);
	 void DrawScreen(HDC TheDC, BOOL TextChanged);
	 void GetWindowClass(WNDCLASS &WndClass);
	 LPSTR GetClassName();
	 void EvSize( UINT, TSize& );
	 LRESULT SetText(WPARAM, LPARAM);
// OWLCVT: Unrecognized DDVT value 1025 for 'SetText', generic signature used
	 virtual char* GetText();

	 DECLARE_RESPONSE_TABLE( TStatbar );
};


#endif
