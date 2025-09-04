//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2009-10-23 10:49:25 $
//
// Definition of TGadgetList, TGadgetWindow & TGadgetWindowFont
// A list holding gadgets, & a window class owning & managing tiled gadgets.
//----------------------------------------------------------------------------

#if !defined(OWL_GADGETWI_H)
#define OWL_GADGETWI_H

#if !defined(OWL_WINDOW_H)
# include <owl/window.h>
#endif
#if !defined(OWL_GADGET_H)
# include <owl/gadget.h>
#endif
#if !defined(OWL_GDIOBJEC_H)
# include <owl/gdiobjec.h>
#endif
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif

__OWL_BEGIN_NAMESPACE

class _OWLCLASS TCelArray;
class _OWLCLASS TTooltip;

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TGadgetList
// ~~~~~ ~~~~~~~~~~~
// A list of Gadgets with management functions
//
class _OWLCLASS TGadgetList {
  public:
    TGadgetList();
    virtual ~TGadgetList();

    // Insert a Gadget or a list of gadgets. Gadgets are removed from
    // the source list
    //
    enum TPlacement {Before, After};
    virtual void  Insert(TGadget& gadget, TPlacement = After,
                         TGadget* sibling = 0);
    virtual void  InsertFrom(TGadgetList& list, TPlacement = After,
                         TGadget* sibling = 0);

    // Removes (unlinks) a gadget.
    //
    virtual TGadget*  Remove(TGadget& gadget);

    // Callbacks invoked when a gadget is removed or inserted
    //
    virtual void  Inserted(TGadget& gadget);
    virtual void  Removed(TGadget& gadget);

    // Insert/remove methods that take a source/dest TGadgetList&

    // Locate and iterate over gadgets
    //
    uint          GadgetCount() const;
    TGadget*      FirstGadget() const;
    TGadget*      NextGadget(TGadget& gadget) const;
    TGadget*      GadgetFromPoint(TPoint& point) const;
    TGadget*      GadgetWithId(int id) const;
    TGadget*      operator [](uint index);

  protected_data:
    TGadget*        Gadgets;        // Linked list of gadgets
    uint            NumGadgets;     // Total number of gadgets
};

//
// class TGadgetWindowFont
// ~~~~~ ~~~~~~~~~~~~~~~~~
// Specify the point size of the font (not the size in pixels) and whether it
// is bold and/or italic. you get a variable width sans serif font(typically
// Helvetica)
//
class _OWLCLASS TGadgetWindowFont : public TFont {
  public:
    TGadgetWindowFont(int  pointSize = 10,
                      bool bold = false,
                      bool italic = false);
    TGadgetWindowFont(const TFont& font);
};

//
// class TGadgetWindow
// ~~~~~ ~~~~~~~~~~~~~
// - Maintains a list of tiled Gadgets
// - You specify whether the Gadgets are tiled horizontally or vertically
// - A font that the Gadgets should use
// - You can specify left, right, top, and bottom margins. The Gadgets are
//   situated within the inner rectangle (area excluding borders and
//   margins). Margins can be specified in pixels, layout units (based on
//   the window font), or border units (width or height of a thin window
//   border)
// - You can specify that the Gadget window shrink wrap itself to "fit"
//   around its Gadgets in either the width or height or both (default for
//   horizontally tiled Gadgets is shrink wrap height and default for
//   vertically tiled Gadgets is shrink wrap width)
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class _OWLCLASS TGadgetWindow : virtual public TWindow, public TGadgetList {
  public:

    // Enumeration describing how gadgets should be laid out within the
    // gadget window.
    //
    enum TTileDirection {
      Horizontal,       // Arrange gadgets in a row
      Vertical,         // Arrange gadgets in a column
      Rectangular       // Arrange gadgets in rows & columns [2-D]
    };

    TGadgetWindow(TWindow*        parent = 0,
                  TTileDirection  direction = Horizontal,
                  TFont*          font = new TGadgetWindowFont,
                  TModule*        module = 0);
   ~TGadgetWindow();

    // Override TWindow member function and choose initial size if shrink
    // wrapping was requested
    //
    bool          Create();

    // Changes the margins and initiates a layout session
    //
    void          SetMargins(TMargins& margins);

    // Get or set the direction. Setting the direction swaps dimentional
    // members & initiates a layout session
    //
    TTileDirection GetDirection() const;
    virtual void  SetDirection(TTileDirection direction);

    void          SetRectangularDimensions(int width, int height, int rowMargin= -1);

    TFont&        GetFont();
    uint          GetFontHeight() const;

    // Retrieves/assigns tooltip of/to window
    //
    TTooltip*     GetTooltip() const;
    void           EnableTooltip(bool enable=true);
    void          SetTooltip(TTooltip* tooltip);

    // Getting & releasing of the mouse by gadgets.
    //
    bool          GadgetSetCapture(TGadget& gadget);
    void          GadgetReleaseCapture(TGadget& gadget);
    TGadget*      GadgetGetCaptured();

    // Hint mode settings & action used by contained gadgets
    //
    enum THintMode {
      NoHints,
      PressHints,
      EnterHints
    };
    void          SetHintMode(THintMode hintMode);
    THintMode     GetHintMode();
    void          SetHintCommand(int id);  // (id <= 0) to clear

    enum TFlatStyle{
      NonFlatNormal    =  0x0000L, // Not flat type
      FlatStandard    = 0x0001L, // standard IE 3.0 look ->
      FlatGrayButtons = 0x0002L, // grayed buttons
      FlatHotText     = 0x0004L, // hot text
      FlatSingleDiv   = 0x0008L, // single grip divider
      FlatXPTheme     = 0x0010L, // Windows XP theme styles
      FlatDefault      = 0xFFFFL, // System depended type
    };

    static void   EnableFlatStyle(uint style = FlatDefault);
    /*virtual*/ static uint  GetFlatStyle();

    // Windows XP theme styles
    //
    bool IsThemed() const;
    void EnableThemeBackground(bool enable = true);
    bool IsThemeBackgroundEnabled() const;
    bool IsBackgroundThemed() const;

    // TGadgetList list management overrides
    // Insert & remove a Gadget, update their Window ptr to this window
    //
    void     Insert(TGadget& gadget, TPlacement = After, TGadget* sibling = 0);
    void     InsertFrom(TGadgetList& list, TPlacement = After, TGadget* sibling = 0);
    TGadget* Remove(TGadget& gadget);

    // Override callbacks invoked when a gadget is removed or inserted
    // Locate and iterate over gadgets
    //
    void     Inserted(TGadget& gadget);
    void     Removed(TGadget& gadget);

    // Shared CelArray management
    //
    virtual void          SetCelArray(TCelArray* sharedCels);
    virtual TCelArray&    GetCelArray(int minX = 0, int minY = 0);

    // During idle time, iterates over the Gadgets invoking their
    // CommandEnable() member function
    //
    bool          IdleAction(long idleCount);

    // Set timer - useful for gadgets that need regular update [Time/Date]
    //
    bool          EnableTimer();

    // Sent by a Gadget when its size has changed. Initiates a layout session
    //
    void          GadgetChangedSize(TGadget& gadget);

    // Begins a layout session which tiles the Gadgets & repaints
    //
    virtual void  LayoutSession();

    // Simply sets the corresponding member data
    //
    void          SetShrinkWrap(bool shrinkWrapWidth, bool shrinkWrapHeight);

    // Get the desired size for this gadget window.
    //
    virtual void  GetDesiredSize(TSize& size);

  protected:
    // Called by Paint(). Iterates over the Gadgets and asks each one to draw
    //
    virtual void  PaintGadgets(TDC& dc, bool erase, TRect& rect);

    // Computes the area inside of the borders and margins
    //
    virtual void  GetInnerRect(TRect& rect);

    int           LayoutUnitsToPixels(int units);

    void          GetMargins(const TMargins& margins,
                             int& left, int& right, int& top, int& bottom);
    void          UseDesiredSize();

    // Gadget layout information used during the layout process
    //
    class TLayoutInfo {
      public:
        TLayoutInfo(int gadgetCount)
            : DesiredSize(0,0), GadgetBounds(new TRect[gadgetCount]) {}
        TSize            DesiredSize;
        TAPointer<TRect> GadgetBounds;
    };

    // Calculates the layout of the Gadgets in the specified direction
    // Returns the TLayoutInfo to describe the result but does not move the
    // gadgets.
    //
    virtual void  LayoutGadgets(TTileDirection dir, TLayoutInfo& layout);

    // Tiles the Gadgets in the current direction
    //
    virtual TRect TileGadgets();

    // Selects the font into the DC and calls PaintGadgets()
    //
    void          Paint(TDC& dc, bool erase, TRect& rect);

    virtual void  PositionGadget(TGadget* previous, TGadget* next,
                                 TPoint& point);

    // Overrides of TWindow virtuals
    //
    void          SetupWindow();
    void          CleanupWindow();
    bool          PreProcessMsg(MSG& msg);

    // Message response functions
    //
    void          EvLButtonDown(uint modKeys, TPoint& point);
    void          EvLButtonUp(uint modKeys, TPoint& point);
    void          EvLButtonDblClk(uint modKeys, TPoint& point);
    void          EvRButtonDown(uint modKeys, TPoint& point);
    void          EvRButtonUp(uint modKeys, TPoint& point);
    void          EvMouseMove(uint modKeys, TPoint& point);
    void          EvWindowPosChanging(WINDOWPOS & windowPos);
    void          EvSysColorChange();

    void          EvCreateTooltips();

    // The overridden event handler functions are virtual, and not dispatched
    // thru response tables
    //
    TResult       EvCommand(uint id, HWND hWndCtl, uint notifyCode);
    void          EvCommandEnable(TCommandEnabler& ce);

  // Protected data members
  //
  protected_data:
    TFont*          Font;           // Font used for size calculations
//! TBrush*         BkgndBrush;     // OBSOLETE: background brush
    TGadget*        Capture;        // Gadget that has captured the mouse
    TGadget*        AtMouse;        // Last Gadget at mouse position
    TMargins        Margins;
    uint            FontHeight       : 8;
    bool            ShrinkWrapWidth  : 8;
    bool            ShrinkWrapHeight : 8;
    uint            WideAsPossible   : 8;  // # of "WideAsPossible" gadgets
    bool            DirtyLayout      : 8;
    TTileDirection  Direction        : 8;
    THintMode       HintMode         : 8;
    TCelArray*      SharedCels;     // CelArray that can be shared by gadgets
    TTooltip*        Tooltip;        // Tooltip
    bool            WantTooltip;    // Flags whether to create a tooltip
    int             RowWidth;       // Requested max width of each row
    int             RowMargin;      // Settable inter-row margin
    bool            WantTimer;      // Flags whether to start a timer
    uint            TimerID;        // Timer identifier
    static uint     FlatStyle;      // FlatStyle
    bool            ThemeBackgroundMode; //Indicate enablement of themed backgrounds

  private:
    // Calculate layouts metrics for window and all gadgets for a given
    // direction
    //
    void          LayoutHorizontally(TLayoutInfo&);
    void          LayoutVertically(TLayoutInfo&);
    void          LayoutRectangularly(TLayoutInfo&);
    void          FinishRow(int, TGadget*, TGadget*, int, TLayoutInfo&, int&);

    // Hidden to prevent accidental copying or assignment
    //
    TGadgetWindow(const TGadgetWindow&);
    TGadgetWindow& operator =(const TGadgetWindow&);

  friend class TGadget;

  DECLARE_RESPONSE_TABLE(TGadgetWindow);
  DECLARE_CASTABLE;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif


#define EV_WM_CREATETOOLTIP\
  {{WM_OWLCREATETTIP}, 0, (TAnyDispatcher)::v_Dispatch,\
   (TMyPMF)NS_OWL::v_Sig(&TMyClass::EvCreateTooltips)}


//
// class TGadgetControl
// ~~~~~ ~~~~~~~~~~~~~~
// Specialized gadget window that is easy to construct holding one gadget for
// use as a control in a window.
//
class _OWLCLASS TGadgetControl : public TGadgetWindow {
  public:
    TGadgetControl(TWindow*        parent = 0,
                   TGadget*        soleGadget = 0,
                   TFont*          font = new TGadgetWindowFont,
                   TModule*        module = 0);
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations

//
// Return the number of gadgets in the list.
//
inline uint TGadgetList::GadgetCount() const {
  return NumGadgets;
}

//
// Return the first gadget in the list.
//
inline TGadget* TGadgetList::FirstGadget() const {
  return Gadgets;
}

//
// Return the next gadget in the list relative to a given gadget
//
inline TGadget* TGadgetList::NextGadget(TGadget& gadget) const {
  return gadget.NextGadget();
}

//----------------------------------------------------------------------------

//
// Return the direction of tiling for this gadget window.
//
inline TGadgetWindow::TTileDirection TGadgetWindow::GetDirection() const {
  return Direction;
}

//
// Return the font being used by this gadget window.
//
inline TFont& TGadgetWindow::GetFont() {
  return *Font;
}

//
// Return Tooltip
//
inline TTooltip* TGadgetWindow::GetTooltip() const {
  return Tooltip;
}

//
// Return the height of the font being used by this gadget window.
//
inline uint TGadgetWindow::GetFontHeight() const {
  return FontHeight;
}

//
// Set the hint mode of this gadget window to one of the THintMode enum values
//
inline void TGadgetWindow::SetHintMode(THintMode hintMode){
  HintMode = hintMode;
}

//
// Return the current hint mode of this gadget window.
//
inline TGadgetWindow::THintMode TGadgetWindow::GetHintMode(){
  return HintMode;
}

//
// return if windows is flat style
//
inline uint TGadgetWindow::GetFlatStyle() {
  return FlatStyle;
}

//
// Retrieve the sizes of the 4 margins in pixels for this gadget window given a
// margin object
//
inline void TGadgetWindow::GetMargins(const TMargins& margins,
                int& left, int& right, int& top, int& bottom) {
  margins.GetPixels(left, right, top, bottom, FontHeight);
}

//
// Retrieve gadget with capture
//
inline TGadget* TGadgetWindow::GadgetGetCaptured(){
  return Capture;
}

__OWL_END_NAMESPACE


#endif  // OWL_GADGETWI_H
