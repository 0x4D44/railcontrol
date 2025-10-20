// OWLCVT 05/11/95 22:33:58
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

class TToolbar : public TFrameWindow
{
  public:
    int              NumButt;             // Number of toolbar buttons
    Toolbuttondata   ButtData;            // Struct containing button X position data
    TManagedArray<TToolbutton, 20> Buttons; // Managed handles to toolbar buttons

    HBITMAP          ButtBitmap;          // Handle to the button bitmaps
    TBitmapGuard     HUpButt;             // Handle to 'Up' button bitmap
    TBitmapGuard     HDownButt;           // Handle to 'Down' button bitmap

    TToolbar(TWindow * AParent, int NumButtons, const Toolbuttondata& ButtonData, HBITMAP ButtonBitmap);
    ~TToolbar();
	 void SetupWindow();
   virtual void Paint(TDC& dc, bool erase, TRect& rect);
   void DrawScreen(HDC TheDC);
   void GetWindowClass(WNDCLASS &WndClass);
   auto GetWindowClassName() -> owl::TWindowClassName override;
   void EvSize(owl::uint sizeType, const TSize& size);
   TToolbutton* GetButton(int index);
   const TToolbutton* GetButton(int index) const;

	 DECLARE_RESPONSE_TABLE( TToolbar );
};

#endif


