/* TOOLBAR.H
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

/* Header file for tool bar class */

#if !defined(_TOOLBAR_H_)
#define _TOOLBAR_H_


// Define TOOLBAR window class
_CLASSDEF(TToolbar)
typedef TToolbar* PToolbar;

class TToolbar : public TWindow
{
  public:
    int              NumButt;             // Number of toolbar buttons
    Toolbuttondata   ButtData;            // Struct containing button X position data
    PToolbutton      PToolbutt[20];       // Handles to up to 20 toolbar buttons

    HBITMAP          ButtBitmap,          // Handle to the button bitmaps
         	     HUpButt,	          // Handle to 'Up' button bitmap
		     HDownButt;           // Handle to 'Down' button bitmap

    TToolbar(PTWindowsObject AParent, int NumButtons, Toolbuttondata ButtonData, HBITMAP ButtonBitmap);
    ~TToolbar();
    virtual void SetupWindow();
    virtual void Paint(HDC PaintDC, PAINTSTRUCT _FAR &PaintInfo);
    virtual void DrawScreen(HDC TheDC);
    virtual void GetWindowClass(WNDCLASS &WndClass);
    virtual LPSTR GetClassName();
    virtual void WMSize(RTMessage Msg) = [WM_FIRST + WM_SIZE];
};

#endif


