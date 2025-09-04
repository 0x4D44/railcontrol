//------------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of TTextGadget.
// Implementation of TDynamicTextGadget.
//------------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_TEXTGADG_H)
# include <owl/textgadg.h>
#endif
#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif
#if !defined(OWL_UIHELPER_H)
#include <owl/uihelper.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

//
// Construct a text gadget
//
TTextGadget::TTextGadget(int          id,
                         TBorderStyle borderStyle,
                         TAlign       align,
                         uint         numChars,
                         LPCTSTR      text,
                         TFont*       font)
:
  TGadget(id, borderStyle),
  Font(font)
{
  Margins.Left = Margins.Right = TUIMetric::CxFixedFrame;
  Align = align;
  NumChars = numChars;
  Text = 0;
  TextLen = 0;
  // init colors to same as reset methods
  ResetBkgndColor();
  ResetTextColor(); 

  SetShrinkWrap(false, true);
  SetText(text);
}

//
// Destruct this text gadget and delete the text it is holding
//
TTextGadget::~TTextGadget()
{
  delete[] Text;
  delete Font;
}

//
// Invalidate the working portion of this gadget--in this case just the
// InnerRect
//
void
TTextGadget::Invalidate()
{
  TRect  innerRect;

  GetInnerRect(innerRect);
  bool transparent = Window->GetFlatStyle() & TGadgetWindow::FlatXPTheme;
  InvalidateRect(innerRect, transparent); // Erase (redraw background) if transparent.
}

//
// Set the text for this gadget
//
void
TTextGadget::SetText(LPCTSTR text)
{
  // Skip processing if new text is the same as current
  //
  if (text && Text && _tcscmp(text, Text) == 0)
    return;

  delete[] Text;

  if (text) {
    Text = strnewdup(text);
    TextLen = ::_tcslen(Text);
  }
  else {
    Text = 0;
    TextLen = 0;
  }

  if (Window)
    Window->GadgetChangedSize(*this);

  Invalidate();
}

//
// Respond to the virtual call to let this gadget's Window know how big this
// text gadget wants to be based on the text size.
//
void
TTextGadget::GetDesiredSize(TSize& size)
{
  TGadget::GetDesiredSize(size);
  TFont* font = Font;
  if (font == 0)
    font = &(Window->GetFont());

  if (font == 0)
    return;

  if (ShrinkWrapWidth)
    size.cx += font->GetTextExtent(Text).cx;
  else {
    int  left, right, top, bottom;
    GetOuterSizes(left, right, top, bottom);

    int newW = font->GetMaxWidth() * NumChars;
    size.cx += newW + left + right - Bounds.Width();  // Old bounds already considered
  }

  if (ShrinkWrapHeight)
    size.cy += font->GetHeight() + 2;
}

//
// Paint the text gadget by painting gadget borders, & then painting text in
// the InnerRect. Empty or 0 text blanks the gadget.
//
void
TTextGadget::Paint(TDC& dc)
{
  PaintBorder(dc);

  TRect  innerRect;
  GetInnerRect(innerRect);

  if (!Font)
    dc.SelectObject(Window->GetFont());
  else
    dc.SelectObject(*Font);

  TColor textColor = GetEnabledColor();
  if(!GetEnabled())
    textColor = TColor::Sys3dHilight;

  bool transparent = Window->GetFlatStyle() & TGadgetWindow::FlatXPTheme;
  if(!Text){
    if (!transparent)
    {
    TColor color = dc.SetBkColor(TColor::Sys3dFace);
    dc.ExtTextOut(0,0, ETO_OPAQUE, &innerRect, 0, 0);
    dc.SetBkColor(color);
  }
  }
  else
  {
    // Create a UI Face object for this button & let it paint the button face
    //
    uint align[] = {DT_LEFT, DT_CENTER, DT_RIGHT};
    uint format =  DT_SINGLELINE | DT_VCENTER | align[Align];
            TUIFace face(innerRect, Text, BkgndColor, format);

     TPoint  dstPt(innerRect.TopLeft());

      dc.SetBkColor(BkgndColor);

     TColor oldTxColor  = dc.SetTextColor(textColor);
    if (!GetEnabled())
      face.Paint(dc, dstPt, TUIFace::Disabled, false, !transparent);
    else
      face.Paint(dc, dstPt, TUIFace::Normal, false, !transparent);
    dc.SetTextColor(oldTxColor);
  }
  dc.RestoreFont();
}
void TTextGadget::SetBkgndColor(TColor& color)
{
  BkgndColor = color;
}

void TTextGadget::SetTextColor(TColor& color)
{
  TextColor = color;
}

void TTextGadget::ResetBkgndColor()
{
  BkgndColor = TColor::Sys3dFace;
}

void TTextGadget::ResetTextColor()
{
  TextColor = TColor::SysBtnText;
}
//------------------------------------------------------------------------------
// class TDynamicTextGadgetEnabler 
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~~
//
class TDynamicTextGadgetEnabler : public TCommandEnabler {
  public:
    TDynamicTextGadgetEnabler(TWindow::THandle hReceiver, 
                              TDynamicTextGadget* g)
    :
      TCommandEnabler(g->GetId(), hReceiver),
      Gadget(g)
    {
    }

    // Override TCommandEnabler virtuals
    //
    void  Enable(bool);
    void  SetText(LPCTSTR);
    void  SetCheck(int){}

  protected:
    TDynamicTextGadget*  Gadget;
};
//
void TDynamicTextGadgetEnabler::Enable(bool enable)
{
  TCommandEnabler::Enable(enable);
  if(Gadget->GetEnabled() != enable)
    Gadget->SetEnabled(enable);
}
//
void TDynamicTextGadgetEnabler::SetText(LPCTSTR text)
{
  if(!Gadget->GetText() || _tcscmp(Gadget->GetText(), text) != 0)
    Gadget->SetText(text);
}
//
//--------------------------------------------------------
// TDynamicTextGadget
//
TDynamicTextGadget::TDynamicTextGadget(int id, TBorderStyle style, TAlign alighn,uint numChars, LPCTSTR text,TFont* font) 
: 
  TTextGadget(id, style, alighn, numChars, text,font)
{
}
//
void TDynamicTextGadget::CommandEnable()
{ 
  PRECONDITION(Window);

  // Must send, not post here, since a ptr to a temp is passed
  //
  // This might be called during idle processing before the
  // HWND has created.  Therefore, confirm handle exists.
  //
  if (Window->GetHandle()){
    TDynamicTextGadgetEnabler ge(*Window, this);
    Window->HandleMessage(WM_COMMAND_ENABLE,0,TParam2(&ge));
  }
}
//
__OWL_END_NAMESPACE
/* ========================================================================== */

