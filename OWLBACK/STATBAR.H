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

class TStatbar : public TWindow
{
  private:
    char      DisplayedText[100];    // Text to display in the status bar
    LOGFONT   TheFontLog;            // Logical font structure used
    HFONT     TheFont;               // Handle to the font

  public:
    TStatbar(PTWindowsObject AParent);
    ~TStatbar();
    virtual void SetupWindow();
    virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo);
    virtual void DrawScreen(HDC TheDC, BOOL TextChanged);
    virtual void GetWindowClass(WNDCLASS &WndClass);
    virtual LPSTR GetClassName();
    virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
    virtual void SetText(RTMessage Msg) = [WM_FIRST + SB_SETTEXT];
    virtual char* GetText();
};


#endif
