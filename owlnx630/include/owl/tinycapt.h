//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
//----------------------------------------------------------------------------

#if !defined(OWL_TINYCAPT_H)
#define OWL_TINYCAPT_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

class _OWLCLASS TFont;

// Generic definitions/compiler options (eg. alignment) preceeding the 
// definition of classes
#include <owl/preclass.h>

//
// class TTinyCaption
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TTinyCaption : public virtual TWindow {
  protected:
    TTinyCaption();
   ~TTinyCaption();

    // Pass closeBox=true to replace SystemMenu box with a box that will 
    // close window when clicked
    // Used for floating palettes, etc.
    //
    void        EnableTinyCaption(int ch=58, bool closeBox=false);

    // Controller class must handle events that call these mixin handlers
    //
    TEventStatus DoNCHitTest(TPoint& screenPt, uint& evRes);
    TEventStatus DoNCPaint();
    TEventStatus DoNCCalcSize(bool calcValidRects, 
                              NCCALCSIZE_PARAMS & calcSize, uint& evRes);
    TEventStatus DoNCLButtonDown(uint hitTest, TPoint& screenPt);
    TEventStatus DoMouseMove(uint hitTest, TPoint& screenPt);
    TEventStatus DoLButtonUp(uint hitTest, TPoint& screenPt);
    TEventStatus DoNCActivate(bool active, bool& evRes);
    TEventStatus DoCommand(uint id, HWND hWndCtl, uint notifyCode, TResult& evRes);
    TEventStatus DoSysCommand(uint cmdType, TPoint& p);

    void        PaintButton(TDC& dc, TRect& boxRect, bool pressed);
    void        PaintCloseBox(TDC& dc, TRect& boxRect, bool pressed);
    void        PaintSysBox(TDC& dc, TRect& boxRect, bool pressed);
    void        PaintMinBox(TDC& dc, TRect& boxRect, bool pressed);
    void        PaintMaxBox(TDC& dc, TRect& boxRect, bool pressed);
    void        PaintCaption(bool active);
    void        DoSysMenu();

    TRect       GetCaptionRect();  // Get caption area for hit test or paint
    TRect       GetSysBoxRect();   // Same for close box
    TRect       GetMinBoxRect();
    TRect       GetMaxBoxRect();

    uint        EvNCHitTest(TPoint& screenPt);
    void     EvNCPaint(HRGN);
    uint        EvNCCalcSize(bool calcValidRects, NCCALCSIZE_PARAMS & calcSize);
    void        EvNCLButtonDown(uint hitTest, TPoint& screenPt);
    void        EvMouseMove(uint hitTest, TPoint& screenPt);
    void        EvLButtonUp(uint hitTest, TPoint& screenPt);
    bool        EvNCActivate(bool active);
    TResult     EvCommand(uint id, HWND hWndCtl, uint notifyCode);
    void        EvSysCommand(uint cmdType, TPoint& p);

  protected_data:
    // Cache system metrics 
    //
    TSize    Border;        // Thin frame border size for dividers
    TSize    Frame;         // Actual Left and Right, Top and Bottom frame size

    bool     CloseBox;      // True for special close box
    bool     TCEnabled;
    int      CaptionHeight;
    TFont*   CaptionFont;   // Tiny font for caption bar
    uint     DownHit;       // Mouse down hit test result
    bool     IsPressed;     // Is a button currently pressed
    bool     WaitingForSysCmd;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TTinyCaption(const TTinyCaption&);
    TTinyCaption& operator =(const TTinyCaption&);

  DECLARE_RESPONSE_TABLE(TTinyCaption);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TTinyCaption, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TTinyCaption, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TTinyCaption );

// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>


__OWL_END_NAMESPACE


#endif  // OWL_TINYCAPT_H
