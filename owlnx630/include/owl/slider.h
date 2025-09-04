//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.27 $
//$Author: jogybl $
//$Date: 2007-07-20 10:47:35 $
//
// Definition of classes TSlider & TVSlider.  This defines the basic behavior
// of slider controls.
//----------------------------------------------------------------------------

#if !defined(OWL_SLIDER_H)
#define OWL_SLIDER_H

#if !defined(OWL_SCROLLBA_H)
# include <owl/scrollba.h>
#endif
#if !defined(OWL_COLOR_H)
# include <owl/color.h>
#endif
#if !defined(OWL_COMMCTRL_H)
# include <owl/commctrl.h>
#endif
#include <owl/slider.rh>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

class _OWLCLASS TDC;
class _OWLCLASS TRegion;
class _OWLCLASS TBrush;

//
// class TSlider
// ~~~~~ ~~~~~~~
class _OWLCLASS TSlider : public TScrollBar {
  public:
    // Constructors and destructor
    //
    TSlider(TWindow*        parent,
            int             id,
            int x, int y, int w, int h,
            TResId          thumbResId,
            TModule*        module = 0);
    TSlider(TWindow* parent, int resId, TResId thumbResId, TModule* module = 0);
   ~TSlider();

    // Overload TScrollBar virtual functions
    //
    void          GetRange(int& min, int& max) const;
    int           GetPosition() const;
    void          SetRange(int min, int max, bool redraw = true);
    void          SetPosition(int thumbPos);
    void          SetPosition(int thumbPos, bool redraw);

    // New settings introduced by sliders sliders
    //
    void          SetRuler(int ticGap, bool snap = false);
    void          SetRuler(int tics[], int ticCount, bool snap = false);
    void          SetSel(int start, int end, bool redraw = true);
    void          GetSel(int& start, int& end);

    // New messages
    //
    HWND          GetBuddy(bool loc=true) const;        // Version 4.70
    void          SetBuddy(HWND buddy, bool loc=true);  // Version 4.70
    HWND           GetTooltips() const;                  // Version 4.70
    void          SetTooltips(HWND tooltip);            // Version 4.70
    void          SetTipSide(int loc);                  // Version 4.70


    static void   SetNativeUse(TNativeUse nu);

  protected:

    // Override TWindow virtual member functions
    //
    LPCTSTR        GetClassName();
    void           SetupWindow();

    int            SnapPos(int pos);

    // OWL's slider implementation
    //
#if !defined(OWL_NATIVECTRL_ALWAYS)

    // TSlider virtual functions for implementation
    //
    virtual int    PointToPos(const TPoint& point) = 0;  
    virtual TPoint PosToPoint(int pos) = 0;
    virtual void   NotifyParent(int scrollCode, int pos=0) = 0;

    virtual void   SetupThumbRgn();
    virtual int    HitTest(TPoint& point) = 0;
    virtual void   SlideThumb(TDC& dc, int thumbPos);

    virtual void   PaintRuler(TDC& dc) = 0;
    virtual void   PaintSlot(TDC& dc) = 0;
    virtual void   PaintThumb(TDC& dc);

    void           GetBkColor(TDC& dc);

    // Message response functions
    //
    uint          EvGetDlgCode(MSG *);
    void          EvPaint();
    void          EvLButtonDown(uint modKeys, TPoint& point);
    void          EvMouseMove(uint modKeys, TPoint& point);
    void          EvLButtonUp(uint modKeys, TPoint& point);
    void          EvLButtonDblClk(uint modKeys, TPoint& point);
    void          EvKeyDown(uint key, uint repeatCount, uint flags);
    void          EvSetFocus(HWND hWndLostFocus);
    void          EvKillFocus(HWND hWndGetFocus);
#endif

  protected_data:
    int           Min;        // Minimum position value
    int           Max;        // Maximum position value
    uint          Range;      // Positive range
    int           Pos;        // Current position value
    TResId        ThumbResId; // Bitmap res id for thumb knob
    TRect         ThumbRect;  // Bounding rect of Thumb in pixels
    TRegion*      ThumbRgn;   // Optional clipping/hit test region
    TRect         CaretRect;  // Bounding rect of Thumb's blink caret
    int           SlotThick;  // Thickness(width or height) of slot
    int           TicGap;     // Gap between tics in pos units
    int*          Tics;       // Or array of specific tics
    int           TicCount;   //  and its size
    bool          Snap;       // Snap Thumb to tics
    int           SelStart;   // Selection start & end
    int           SelEnd;

    bool          Sliding;
    TColor        BkColor;    // Color to use to paint background

    static TNativeUse ClassNativeUse;  // Default use of native control impl

    // Statics used while the mouse is down & Thumb is sliding
    //
    static TSize  MouseOffset;
    static TDC*   SlideDC;

  DECLARE_RESPONSE_TABLE(TSlider);
  //DECLARE_ABSTRACT_STREAMABLE(_OWLCLASS, NS_OWL::TSlider, 2);
  DECLARE_ABSTRACT_STREAMABLE_OWL(NS_OWL::TSlider, 2);
};

// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TSlider );

//
// class THSlider
// ~~~~~ ~~~~~~~~
class _OWLCLASS THSlider : public TSlider {
  public:
    THSlider(TWindow*        parent,
             int             id,
             int x, int y, int w, int h,
             TResId          thumbResId = IDB_HSLIDERTHUMB,
             TModule*        module = 0);
    THSlider(TWindow* parent, int resId, TResId thumbResId = IDB_HSLIDERTHUMB,
             TModule* module = 0);

  protected:
#if !defined(OWL_NATIVECTRL_ALWAYS)
    // Override TSlider virtual member functions
    //
    int    PointToPos(const TPoint& point);
    TPoint PosToPoint(int pos);
    void   NotifyParent(int scrollCode, int pos=0);
    int    HitTest(TPoint& point);
    void   PaintRuler(TDC& dc);
    void   PaintSlot(TDC& dc);
#endif

  private:
    // Hidden to prevent accidental copying or assignment
    //
    THSlider(const THSlider&);
    THSlider& operator =(const THSlider&);

  //DECLARE_STREAMABLE(_OWLCLASS, THSlider, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::THSlider, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::THSlider );

//
// class TVSlider
// ~~~~~ ~~~~~~~~
class _OWLCLASS TVSlider : public TSlider {
  public:
    TVSlider(TWindow*        parent,
             int             id,
             int x, int y, int w, int h,
             TResId          thumbResId = IDB_VSLIDERTHUMB,
             TModule*        module = 0);
    TVSlider(TWindow* parent, int resId, TResId thumbResId = IDB_VSLIDERTHUMB,
             TModule* module = 0);

  protected:
#if !defined(OWL_NATIVECTRL_ALWAYS)
    // Override TSlider virtual member functions
    //
    int    PointToPos(const TPoint& point);
    TPoint PosToPoint(int pos);
    void   NotifyParent(int scrollCode, int pos=0);
    int    HitTest(TPoint& point);
    void   PaintRuler(TDC& dc);
    void   PaintSlot(TDC& dc);
#endif

  private:
    TVSlider(const TVSlider&);
    TVSlider& operator =(const TVSlider&);

  //DECLARE_STREAMABLE(_OWLCLASS, TVSlider, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TVSlider, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TVSlider );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>


//----------------------------------------------------------------------------
// Inline implementations
//

//
inline void TSlider::GetRange(int& min, int& max) const
{
  min = Min; max = Max;
}

//
inline int TSlider::GetPosition() const
{
  return Pos;
}

//
// Specifies whether the class uses the native (operating system) implementation
// or emulates it.
//
inline void TSlider::SetNativeUse(TNativeUse nu)
{
  ClassNativeUse = nu;
}

//
// Version 4.70
//
inline HWND TSlider::GetBuddy(bool loc) const
{
  return HWND(CONST_CAST(TSlider*,this)->SendMessage(TBM_GETBUDDY,TParam1(loc)));
}

//
// Version 4.70
//
inline void  TSlider::SetBuddy(HWND buddy, bool loc)
{
  SendMessage(TBM_SETBUDDY, TParam1(loc), TParam2(buddy));
}

//
// Version 4.70
//
inline HWND TSlider::GetTooltips() const
{
  return HWND(CONST_CAST(TSlider*,this)->SendMessage(TBM_GETTOOLTIPS));
}

//
// Version 4.70
//
inline void  TSlider::SetTooltips(HWND tooltip)
{
  SendMessage(TBM_SETTOOLTIPS, TParam1(tooltip));
}

//
// Version 4.70
//
inline void  TSlider::SetTipSide(int loc)
{
  SendMessage(TBM_SETTIPSIDE, loc);
}

__OWL_END_NAMESPACE


#endif  // OWL_SLIDER_H
