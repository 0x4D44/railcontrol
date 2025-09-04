//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Revised by Yura Bidus 11/23/1998
//
// Implementation of class TButtonGadget.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_BUTTONGA_H)
# include <owl/buttonga.h>
#endif
#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_CELARRAY_H)
# include <owl/celarray.h>
#endif
#if !defined(OWL_UIHELPER_H)
# include <owl/uihelper.h>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif
#if !defined(OWL_SYSTEM_H)
# include <owl/system.h>
#endif
#if !defined(OWL_THEME_H)
# include <owl/theme.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGadget);

//
// Command enabler for button gadgets
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class /*_OWLCLASS*/ TButtonGadgetEnabler : public TCommandEnabler {
  public:
    TButtonGadgetEnabler(TWindow::THandle hReceiver, TButtonGadget* g)
    :
      TCommandEnabler(g->GetId(), hReceiver),
      Gadget(g)
    {
    }

    // Override TCommandEnabler virtuals
    //
    void  Enable(bool);
    void  SetText(LPCTSTR);
    void  SetCheck(int);

  protected:
    TButtonGadget*  Gadget;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Enable or disable the button gadget
//
void
TButtonGadgetEnabler::Enable(bool enable)
{
  TCommandEnabler::Enable(enable);
  Gadget->SetEnabled(enable);
}

//
// Handle the SetText request for a button gadget. Currently does nothing.
//
void
TButtonGadgetEnabler::SetText(LPCTSTR)
{
//# pragma warn -ccc
//# pragma warn -rch
//  CHECK(true);
//# pragma warn .rch
//# pragma warn .ccc
}

//
// Set the check-state for the button gadget
//
void
TButtonGadgetEnabler::SetCheck(int state)
{
  Gadget->SetButtonState(TButtonGadget::TState(state));
}

//----------------------------------------------------------------------------

//
// Construct a button gadget that loads its own bitmap resource
//
TButtonGadget::TButtonGadget(TResId glyphResIdOrIndex,
                             int    id,
                             TType  type,
                             bool   enabled,
                             TState state,
                             bool   sharedGlyph)
:
  TGadget(id, ButtonUp),
  SharingGlyph(sharedGlyph)
{
  // If we are sharing the glyph held by our window, then we won't need to load
  // the resource
  //
  if (sharedGlyph) {
    ResId = 0;
    GlyphIndex = (int)(LPCTSTR)glyphResIdOrIndex;
    GlyphCount = 1;
  }
  else {
    ResId = glyphResIdOrIndex.IsString() ?
                strnewdup(glyphResIdOrIndex) : (LPCTSTR)glyphResIdOrIndex;
    GlyphIndex = -1;   // Need to get during BuildCelArray
    GlyphCount = 4;
  }

  CelArray = 0;
  BitmapOrigin.x = BitmapOrigin.y = 0;
  Type = type;
//  Repeat = repeat;
  TrackMouse = true;
  State = state;
  NotchCorners = true;
  Pressed = false;
  SetEnabled(enabled);
  SetAntialiasEdges(true);

  TRACEX(OwlGadget, OWL_CDLEVEL, "TButtonGadget constructed @" << (void*)this);
}

//
// Destruct this button gadget, freeing up allocated resources
//
TButtonGadget::~TButtonGadget()
{
  delete CelArray;
  if (ResId.IsString())
    delete[] (LPSTR)(LPCTSTR)ResId;

  TRACEX(OwlGadget, OWL_CDLEVEL, "TButtonGadget destructed @" << (void*)this);
}

//
// Initiate a command enable for this button gadget
//
void
TButtonGadget::CommandEnable()
{
  PRECONDITION(Window);

  // Must send, not post here, since a ptr to a temp is passed
  //
  // This might be called during idle processing before the
  // HWND has created.  Therefore, confirm handle exists.
  //
  if (Window->GetHandle()) {
    TButtonGadgetEnabler ge(*Window, this);
    Window->HandleMessage(WM_COMMAND_ENABLE, 0, TParam2(&ge));
  }
}

//
// Respond to a WM_SYSCOLORCHANGE, in this case to rebuild the CelArray with
// possibly new 3d colors
//
void
TButtonGadget::SysColorChange()
{
  TGadget::SysColorChange();
  delete CelArray;
  CelArray = 0;
  BuildCelArray();
}

//
// Perform an exclusive checking of this gadget by unchecking the neighboring
// exclusive button gadgets
//
void
TButtonGadget::CheckExclusively()
{
  PRECONDITION(Window);
#if defined(BI_COMP_GNUC)  
  TState state = State;
  if (state != Down) {
#else
  if ((TState)State != Down) {
#endif
    if (Window) {
      TButtonGadget*  first = 0;
      TButtonGadget*  last = this;

      // Look for the start of the group in which this button is located
      //
      for (TGadget* g = Window->FirstGadget(); g && g != this;
        g = Window->NextGadget(*g)) {

        TButtonGadget* bg = TYPESAFE_DOWNCAST(g, TButtonGadget);

        if (!bg || (bg->Type != Exclusive && bg->Type != SemiExclusive))
          first = 0;
        else if (!first)
          first = bg;
      }

      if (!first)
        first = this;

      // Look for the end of the group in which this button is located
      //
      while (Window->NextGadget(*last)) {
        TButtonGadget* bg = TYPESAFE_DOWNCAST(Window->NextGadget(*last), TButtonGadget);

        if (!bg || (bg->Type != Exclusive && bg->Type != SemiExclusive))
          break;
        else
          last = bg;
      }

      // Walk all the gadget between first and last (inclusive) and pop them up
      // if they are down
      //
      while (true) {
#if defined(BI_COMP_GNUC)  
        TState state = first->State;
        if (state == Down) {
#else
        if (first->State == Down) {
#endif
          first->State = Up;
          first->Invalidate();
          first->Update();
        }

        if (first == last)  // All done
          break;

        first = TYPESAFE_DOWNCAST(first->NextGadget(), TButtonGadget);
      }
    }

    State = Down;
  }
}

//
// Set the state of a button. Handles setting down on an exclusive button to
// pop out the neighbors
//
void
TButtonGadget::SetButtonState(TState newState)
{
#if defined(BI_COMP_GNUC)  
  TState state = State;
  if (newState != state) {
#else
  if (newState != (TState) State) {
#endif
    if ((Type == Exclusive || Type == SemiExclusive) && newState == Down)
      CheckExclusively();
    else
      State = newState;
    Invalidate();
    Update();
  }
}

//
// Set the bounding rect for this button gadget. Also takes care of
// re-centering the glyph
//
void
TButtonGadget::SetBounds(const TRect& bounds)
{
  PRECONDITION(Window);
  TRACEX(OwlGadget, 1, _T("TButtonGadget::SetBounds() enter @") << this <<
    _T(" bounds ") << bounds);

  TGadget::SetBounds(bounds);

  // Center the glyph within the inner bounds
  //
  TRect  innerRect;
  GetInnerRect(innerRect);

  TSize  bitmapSize = CelArray ? CelArray->CelSize() : Window->GetCelArray().CelSize();
  BitmapOrigin.x = innerRect.left + (innerRect.Width() - bitmapSize.cx) / 2;
  BitmapOrigin.y = innerRect.top + (innerRect.Height() - bitmapSize.cy) / 2;
  TRACEX(OwlGadget, 1, _T("TButtonGadget::SetBounds() leave @") << this <<
    _T(" bounds ") << bounds);
}

//
// Find out how big this button gadget wants to be. Calculated using the base
// size to get the borders, etc. plus the glyph size.
//
void
TButtonGadget::GetDesiredSize(TSize& size)
{
  PRECONDITION(Window);
  TRACEX(OwlGadget, 1, _T("TButtonGadget::GetDesiredSize() enter @") << this <<
    _T(" size ") << size);

  TGadget::GetDesiredSize(size);

  // Build our CelArray or CelArray entry if not done already
  //
  if (!CelArray && GlyphIndex < 0)
    BuildCelArray();

  size += CelArray ? CelArray->CelSize() : Window->GetCelArray().CelSize();
  TRACEX(OwlGadget, 1, _T("TButtonGadget::GetDesiredSize() leave @") << this <<
    _T(" size ") << size);
}


//
// Virtual function responsible for supplying the dib for the glyph. Can be
// overriden to get dib from elsewhere, cache it, map colors differently, etc.
//
// Returns 0 if no resource was specified for this button
//
TDib*
TButtonGadget::GetGlyphDib()
{
  PRECONDITION(Window);

  if (ResId) {
    TDib* glyph = new TDib(*Window->GetModule(), ResId);
    glyph->MapUIColors( TDib::MapFace | TDib::MapText | TDib::MapShadow |
                          TDib::MapHighlight );
    return glyph;
  }
  return 0;
}

//
// Virtual function responsible for releasing glyph dib as needed based on how
// GetGlyphDib() got it (if different from new/delete).
//
void
TButtonGadget::ReleaseGlyphDib(TDib* glyph)
{
  delete glyph;
}

//
// Build the CelArray member using the resource bitmap as the base glyph
// CelArray may contain an existing cel array that should be deleted if
// replaced.
//
// The CelArray and glyph painting can work in one of the following ways:
//   1. ResId ctor is used, ...., glyph states are cached in this CelArray
//   2. ResId ctor is used, CelArray holds single glyph and paints state on
//      the fly
//   3. ResId ctor is used, glyph is added to Window's CelArray and paints
//      state on the fly
//   4. glyphIndex ctor is used, uses Window's CelArray glyph and paints state
//      on the fly
//
void
TButtonGadget::BuildCelArray()
{
  //PRECONDITION(Window);

  TDib*  glyph = GetGlyphDib();

  // Case 4, no resource of our own--using a cel in our Window's CelArray
  //
  if (!glyph)
    return;

  // Case 3, add (or replace old) our glyph to our Window's CelArray
  //
  
	bool case2 = true; 
	if (SharingGlyph) 
	{ 
		CHECK(Window); 
		TSize glyphSize(glyph->Width(), glyph->Height()); 
		TCelArray& celArray = Window->GetCelArray(glyph->Width(), glyph->Height()); 
 
		if (glyphSize == TSize(celArray.CelSize().cx,celArray.CelSize().cy*celArray.NumRows())) 
		{ 
			case2 = false; 
			if (GlyphIndex >= 0) 
				celArray.Replace(GlyphIndex, *glyph); 
			else 
				GlyphIndex = celArray.Add(*glyph); 
		} 
	} 
 
	if (case2) 
	{   
    // Case 2, build a CelArray containing only the normal glyph state
    //
    delete CelArray;
    CelArray = new TCelArray(*glyph, 1);
    GlyphIndex = 0;
    SharingGlyph = false;
  }

  ReleaseGlyphDib(glyph);

  // Case 1, for compatibility for now...
  //
}

//
//
//
void          
TButtonGadget::SetGlyphIndex(int index)
{
  if(SharingGlyph)
    GlyphIndex = index;
}

//
// Perform all of the painting for this button gadget
//
void
TButtonGadget::Paint(TDC& dc)
{
  PRECONDITION(Window);

  PaintBorder(dc);

  // Get the inner rect to use as the button face
  //
  TRect  faceRect;
  GetInnerRect(faceRect);

  PaintFace(dc, faceRect);
}

void
TButtonGadget::PaintBorder(TDC& dc)
{
  // Paint the button frame depending on the active styles.
  //
  bool flat = Window->GetFlatStyle() & TGadgetWindow::FlatStandard;
  if (flat)
  {
    // Calculate state and border/background style.
    //
    bool ok = GetEnabled();
    bool down = ok && (Pressed || State == Down);
    bool hover = ok && (IsHaveMouse() || Window->GadgetGetCaptured() == this);
    BorderStyle = down ? Recessed : hover ? Raised : None;

    bool xpstyle = Window->IsThemed();
    if (xpstyle)
    {
      // Draw themed background. This ignores all our border styles.
      //
      int state = down ? TS_PRESSED : hover ? TS_HOT : 0;
      TThemePart part(Window->GetHandle(), (LPCWSTR)L"TOOLBAR", TP_BUTTON, state);
      TRect rect(0, 0, Bounds.Width(), Bounds.Height());
      part.DrawTransparentBackground(Window->GetHandle(), dc, rect);
    }
    else 
    {
      // Themes are not available. Draw old flat style.
      //
      TGadget::PaintBorder(dc);
    }
  }
  else // !flat
  {
    BorderStyle = (Pressed || State == Down) ? ButtonDn : ButtonUp;
    TGadget::PaintBorder(dc);
  }
}

//
// Paint face of button
//
void
TButtonGadget::PaintFace(TDC& dc, TRect& rect)
{
  // Determine which CelArray to paint from: ours or our Window's
  //
  TCelArray& celArray = CelArray ? *CelArray : Window->GetCelArray();

  // set Current Row if enabled style FlatGrayButtons
  if(Window->GetFlatStyle()&TGadgetWindow::FlatGrayButtons){
    if(IsHaveMouse())
      celArray.SetCurRow(0);// normal
    else if(celArray.NumRows()>1)
      celArray.SetCurRow(1); // set gray row
  }

  // Calc the source rect from the celarray. The dest point of the glyph is
  // relative to the face rect.
  //
  TRect  srcRect(celArray.CelRect(GlyphIndex));
  TPoint dstPt(BitmapOrigin - rect.TopLeft());

  // Create a UI Face object for this button & let it paint the button face
  //
  TUIFace face(rect, celArray, TColor::Sys3dFace);
  bool flat = Window->GetFlatStyle() & TGadgetWindow::FlatStandard;
  TUIFace::TState state = TUIFace::Disabled;
  bool pressed = false;
  if (GetEnabled()) switch (State)
  {
    case Indeterminate:
      state = TUIFace::Indeterm;
      pressed = Pressed;
      break;

    case Down:
      if (flat)
      {
        bool hover = !IsHaveMouse() && Window->GadgetGetCaptured() != this;
        state = hover ? TUIFace::Down : TUIFace::Normal;
        pressed = hover ? true : Pressed;
  }
  else
      {
        state = Pressed ? TUIFace::Down : TUIFace::Normal;
        pressed = Pressed;
    }
      break;

    default:
      state = TUIFace::Normal;
      pressed = Pressed;
  }
  face.Paint(dc, srcRect, dstPt, state, pressed, false); // no fill (transparent)
}

//
// Begin button pressed state, repaint & enter menuselect state
//
void
TButtonGadget::BeginPressed(TPoint&)
{
  Pressed = true;
  SetInMouse(true);
  Invalidate();
  Update();
  if (Window->GetHintMode() == TGadgetWindow::PressHints)
    Window->SetHintCommand(GetId());
}

//
// Cancel pressed state, repaint & end menuselect state
//
void
TButtonGadget::CancelPressed(TPoint&, bool mstate)
{
  Pressed = false;
  SetInMouse(mstate);
  Invalidate(true);
  Update();
  if (Window->GetHintMode() == TGadgetWindow::PressHints)
    Window->SetHintCommand(-1);
}

//
// The action method called on a completed 'click', generates WM_COMMAND
//
// Invoked by mouse-up event inside the Gadget. Sets member data "Pressed"
// to false, changes state for attribute buttons, and paints the button
// in its current state.
//
// Command buttons just send the command
// Exclusive buttons check themselves and uncheck neighbors.
// NonExclusive buttons toggle their check state.
// SemiExclusive uncheck neighbors on press, but can also be unpressed.
//
void
TButtonGadget::Activate(TPoint& pt)
{
#if defined(BI_COMP_GNUC)  
  TState state = State;
  TType type   = Type;
#endif
  
  switch (Type) {
    case Exclusive:
#if defined(BI_COMP_GNUC)  
      if (state != Down)
#else
      if ((TState)State != Down)
#endif
        CheckExclusively();
      break;

    case NonExclusive:
#if defined(BI_COMP_GNUC)  
      State = state == Up ? Down : Up;
#else
      State = State == Up ? Down : Up;
#endif
      break;

    case SemiExclusive:
#if defined(BI_COMP_GNUC)  
      if (state != Down)
#else
      if ((TState)State != Down)
#endif
        CheckExclusively();
      else
        State = Up;
      break;
    default:
    case Command:
    case RepeatCmd:
      break;
  }

  // Unpress the button
  //
  CancelPressed(pt, true);


  // Send the associated command for all enabled buttons except for exclusive
  // buttons that are being poped up
  //
#if defined(BI_COMP_GNUC)  
  if (!((type == Exclusive || type == SemiExclusive) && state != Down) &&
#else
  if (!((Type == Exclusive || Type == SemiExclusive) && (TState)State != Down) &&
#endif
      GetEnabled())
    Window->PostMessage(WM_COMMAND, GetId());
}

//
// Handle the mouse left button down & enter the pressed state
//
void
TButtonGadget::LButtonDown(uint modKeys, TPoint& pt)
{
  TGadget::LButtonDown(modKeys, pt);
  BeginPressed(pt);
}

//
// Mouse has entered this button, (not pressed). Show hints if that style
// is enabled
//
void
TButtonGadget::MouseEnter(uint modKeys, TPoint& pt)
{
  TGadget::MouseEnter(modKeys, pt);
  if (Window->GetHintMode() == TGadgetWindow::EnterHints)
    Window->SetHintCommand(GetId());
}

//
// Mouse has moved (back) into this gadget. Push or pull the button up & down
// as needed.
//
void
TButtonGadget::MouseMove(uint modKeys, TPoint& pt)
{
  TGadget::MouseMove(modKeys, pt);

  bool hit = PtIn(pt);
  if (Pressed) {
    if (!hit)
      CancelPressed(pt, false);
  }
  else if (hit)
    BeginPressed(pt);
}

//
// Mouse has left this button, (not pressed). Hide hints if that style
// is enabled
//
void
TButtonGadget::MouseLeave(uint modKeys, TPoint& pt)
{
  TGadget::MouseLeave(modKeys, pt);
  if (Window->GetHintMode() == TGadgetWindow::EnterHints)
    Window->SetHintCommand(-1);
}

//
// Handle the mouse left button up & leave the pressed state. If the mouse is
// still on this button, i.e. it is still pressed, then perform the button
// action.
//
void
TButtonGadget::LButtonUp(uint modKeys, TPoint& pt)
{
  TGadget::LButtonUp(modKeys, pt);
  if (Pressed)
    Activate(pt);
  else if(Window->GetFlatStyle()&TGadgetWindow::FlatStandard)
    Invalidate();
}

__OWL_END_NAMESPACE

//==============================================================================

