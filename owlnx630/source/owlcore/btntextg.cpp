//----------------------------------------------------------------------------//
// ObjectWindows 1998 Copyright by Yura Bidus                                 //
//                                                                            //
// Used code and ideas from Dieter Windau and Joseph Parrello                 //
//                                                                            //
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//                                                                            //
// THIS CLASS AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF         //
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO        //
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A             //
// PARTICULAR PURPOSE.                                                        //
//                                                                            //
// Implementation of class TButtonTextGadget.                                 //
//----------------------------------------------------------------------------//
#include <owl/pch.h>

#if !defined(OWL_CELARRAY_H)
# include <owl/celarray.h>
#endif
#if !defined(OWL_GADGETWI_H)
# include <owl/gadgetwi.h>
#endif
#if !defined(OWL_UIHELPER_H)
#include <owl/uihelper.h>
#endif
#if !defined(OWL_BTNTEXTG_H)
# include <owl/btntextg.h>
#endif
#if !defined(OWL_DECFRAME_H)
#include <owl/decframe.h>
#endif

__OWL_USING_STD_NAMESPACE

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlGadget);

//------------------------------------------------------------------------------
//
// class TButtonTextGadgetEnabler
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Command enabler for button text gadget
//
#  if defined(BI_COMP_BORLANDC)
#    pragma warn -inl
#  endif
class /*_OWLCLASS*/ TButtonTextGadgetEnabler : public TCommandEnabler {
  public:
    TButtonTextGadgetEnabler(TWindow::THandle hReceiver, TButtonTextGadget* g)
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
    TButtonTextGadget*  Gadget;
};
#  if defined(BI_COMP_BORLANDC)
#    pragma warn .inl
#  endif

//
// Enable or disable the button gadget
//
void
TButtonTextGadgetEnabler::Enable(bool enable)
{
  TCommandEnabler::Enable(enable);
  Gadget->SetEnabled(enable);
}

//
// Handle the SetText request for a button-text gadget.
//
void
TButtonTextGadgetEnabler::SetText(LPCTSTR text)
{
  if(Gadget->GetStyle()& TButtonTextGadget::sText)
    Gadget->SetText(text);
}

//
// Set the check-state for the button gadget
//
void
TButtonTextGadgetEnabler::SetCheck(int state)
{
  Gadget->SetButtonState(TButtonTextGadget::TState(state));
}


//------------------------------------------------------------------------------
//
// class TButtonTextGadget
// ~~~~~ ~~~~~~~~~~~~~~~~~
//

const  int TextSpaceV = 0;
const  int TextSpaceH = 0;

//
// Constructor
//
TButtonTextGadget::TButtonTextGadget(int id, TResId glyphResIdOrIndex,
                                     TStyle style, TType type,
                                     bool enabled, TState state,
                                     bool sharedGlyph, uint numChars)
:
  TButtonGadget(glyphResIdOrIndex, id, type, enabled, state, sharedGlyph),
  Text(0),
  TextLen(0),
  NumChars(numChars),
  Align(aCenter),
  Style(style),
  LayoutStyle(lTextBottom),
  Font(0)
{
}

//
TButtonTextGadget::~TButtonTextGadget()
{
  delete[] Text;
  delete Font;
}

//
// Initiate a command enable for this button gadget
//
void
TButtonTextGadget::CommandEnable()
{
  PRECONDITION(Window);

  // Must send, not post here, since a ptr to a temp is passed
  //
  // This might be called during idle processing before the
  // HWND has created.  Therefore, confirm handle exists.
  //
  if (Window->GetHandle()) {
    TButtonTextGadgetEnabler ge(*Window, this);
    Window->HandleMessage(WM_COMMAND_ENABLE, 0,TParam2(&ge));
  }
}

//
// Set the text for this gadget
//
void
TButtonTextGadget::SetText(LPCTSTR text, bool repaint)
{
  // Skip processing if new text is the same as current
  //
  if (text && Text && _tcscmp(text, Text) == 0)
    return;

  delete[] Text;

  if (text){
    Text = strnewdup(text);
    TextLen = ::_tcslen(Text);
  }
  else {
    Text = 0;
    TextLen = 0;
  }

  // Load text from menu or tooltip
  if(!Text && (Style&sText) && Window){
    TWindow* parent = Window->GetParentO();
    TDecoratedFrame* frame= parent ? TYPESAFE_DOWNCAST(parent, TDecoratedFrame) : 0;
    while (parent && !frame){
      parent = parent->GetParentO();
      if (parent)
        frame = TYPESAFE_DOWNCAST(parent, TDecoratedFrame);
    }
    _TCHAR tempStr[256];
    tempStr[0] = _T('\0');
    if (frame)
      TextLen = frame->GetHintText((uint)(LPTSTR)ResId, tempStr, COUNTOF(tempStr), htTooltip);
    if (TextLen)
      Text = strnewdup(tempStr);
  }

  if (Window && repaint)
    Window->GadgetChangedSize(*this);
}

//
//  Set new font
//
void
TButtonTextGadget::SetFont(TFont* font, bool repaint)
{
  delete Font;
  Font = font;
  if (Window && repaint)
    Window->GadgetChangedSize(*this);
}


//
//  Set new style
//
void
TButtonTextGadget::SetStyle(TStyle style, bool repaint)
{
  if(Style != style){
    Style = style;
    if (Window && repaint)
      Window->GadgetChangedSize(*this);
  }
}

//
void
TButtonTextGadget::SetAlign(const TAlign align, bool repaint)
{
  if(Align != align){
    Align = align;
    if(repaint)
      Invalidate();
  }
}

//
void
TButtonTextGadget::SetLayoutStyle(const TLayoutStyle style, bool repaint)
{
  if(LayoutStyle != style){
    LayoutStyle = style;
    if (Window && repaint)
      Window->GadgetChangedSize(*this);
  }
}


//
// Virtual called after the window holding a gadget has been created
//
void
TButtonTextGadget::Created()
{
  PRECONDITION(Window);
  PRECONDITION(Window->GetHandle());

  TButtonGadget::Created();

  // if Style == sText and text isn't set -> get it now from menu/resource
  if(!Text && (Style&sText)){
    SetText(0);
    Invalidate();
  }
}

//
// Respond to a WM_SYSCOLORCHANGE, in this case to rebuild the CelArray with
// possibly new 3d colors
//
void
TButtonTextGadget::SysColorChange()
{
  // Baypass TButtonGadget handler if CellArray = 0
  if(Style&sBitmap)
    TButtonGadget::SysColorChange();
  else
    TGadget::SysColorChange();
}

//
void
TButtonTextGadget::GetTextSize(TSize& size)
{
  TFont* font = Font;
  if (font == 0)
    font = &(Window->GetFont());

  if (font == 0)
    return;

  TEXTMETRIC tm;
  font->GetTextMetrics(tm);

  size.cx += tm.tmAveCharWidth * NumChars;
  size.cy += tm.tmHeight + 2;
}

//
void
TButtonTextGadget::GetDesiredSize(TSize& size)
{
  PRECONDITION(Window);
  TRACEX(OwlGadget, 1, _T("TButtonTextGadget::GetDesiredSize() enter @") << this <<
    _T(" size ") << size);

  if(Style&sBitmap)
    TButtonGadget::GetDesiredSize(size);
  else
    TGadget::GetDesiredSize(size);

  // if paint text -> add text size
  if(Style&sText){
    TSize textSize;
    GetTextSize(textSize);

    TSize gsize;
    TGadget::GetDesiredSize(gsize);
    switch(LayoutStyle){
       case lTextLeft:
       case lTextRight:
         size.cx += textSize.cx + TextSpaceV;
         size.cy =  max(size.cy,textSize.cy+gsize.cy);
         break;
       case lTextTop:
       case lTextBottom:
         size.cx += max(size.cx,textSize.cx+gsize.cx);
         size.cy += textSize.cy + TextSpaceH;
         break;
    }
  }
  TRACEX(OwlGadget, 1, _T("TButtonTextGadget::GetDesiredSize() leave @") << this <<
    _T(" size ") << size);
}

//
void
TButtonTextGadget::SetBounds(const TRect& rect)
{
  PRECONDITION(Window);

  if(Style&sBitmap)
    TButtonGadget::SetBounds(rect);
  else
    TGadget::SetBounds(rect);

  if(Style&sBitmap && Style&sText){
    TRect  faceRect, textRect, btnRect;
    GetInnerRect(faceRect);

    Layout(faceRect, textRect, btnRect);

    TSize  bitmapSize = CelArray ? CelArray->CelSize() : Window->GetCelArray().CelSize();
    BitmapOrigin.x = btnRect.left + (btnRect.Width() - bitmapSize.cx) / 2;
    BitmapOrigin.y = btnRect.top + (btnRect.Height() - bitmapSize.cy) / 2;
  }
}

//
void
TButtonTextGadget::Paint(TDC& dc)
{
  PRECONDITION(Window);

  PaintBorder(dc);

  TRect  faceRect, textRect, btnRect;
  GetInnerRect(faceRect);

  Layout(faceRect, textRect, btnRect);

  if(Style&sText)
    PaintText(dc, textRect, Text);
  if(Style&sBitmap)
    PaintFace(dc, btnRect);
}

//
// Layout Text and Bitmap
//
void
TButtonTextGadget::Layout(TRect& faceRect, TRect& textRect, TRect& btnRect)
{
  if(Style == sBitmap)
    btnRect = faceRect;
  else if(Style == sText)
    textRect = faceRect;
  else{
    TSize textSize;
    if(Style&sText)
      GetTextSize(textSize);

    TSize btnSize;
    if(Style&sBitmap)
      TButtonGadget::GetDesiredSize(btnSize);

    switch(LayoutStyle){

       case lTextLeft:
        if(Style&sText)
          textRect = TRect(faceRect.TopLeft(), TSize(textSize.cx,faceRect.Height()));
        if(Style&sBitmap)
          btnRect = TRect(TPoint(faceRect.left+textSize.cx+TextSpaceV,faceRect.top),
                          faceRect.BottomRight());
         break;

       case lTextRight:
        if(Style&sBitmap)
          btnRect = TRect(faceRect.left, faceRect.top,
                          faceRect.left+btnSize.cx, faceRect.bottom);
        if(Style&sText)
          textRect = TRect(btnRect.right+TextSpaceV,faceRect.top,
                           faceRect.right, faceRect.bottom);

         break;

       case lTextTop:
        if(Style&sText)
          textRect = TRect(faceRect.TopLeft(), TSize(faceRect.Width(),textSize.cy));
        if(Style&sBitmap)
          btnRect = TRect(TPoint(faceRect.left,faceRect.top+textSize.cy+TextSpaceH),
                          TSize(faceRect.Width(), faceRect.Height()-textSize.cy));
         break;

       case lTextBottom:
        if(Style&sText)
          textRect = TRect(faceRect.left,faceRect.bottom-textSize.cy,
                           faceRect.right,faceRect.bottom);
        if(Style&sBitmap)
          btnRect = TRect(faceRect.TopLeft(),
                          TSize(faceRect.Width(), faceRect.Height()-textSize.cy));
         break;
    }
  }
}

//
// Paint Text
//
void
TButtonTextGadget::PaintText(TDC& dc, TRect& rect, LPCTSTR text)
{
  if (!Font)
    dc.SelectObject(Window->GetFont());
  else
    dc.SelectObject(*Font);

  TColor textColor = TColor::SysBtnText;
  if(!GetEnabled())
    textColor = TColor::Sys3dHilight;
  else if((Window->GetFlatStyle()&TGadgetWindow::FlatHotText) && IsHaveMouse())
     textColor = TColor::LtBlue;

   TColor oldTxColor  = dc.SetTextColor(textColor);

  uint format =  DT_SINGLELINE|DT_NOCLIP|DT_END_ELLIPSIS;
  switch(Align){
    case aLeft:
      format |= DT_LEFT;
      break;
    case aRight:
      format |= DT_RIGHT;
      break;
    case aCenter:
      format |= DT_CENTER;
      break;
  }
  switch(LayoutStyle){
    case lTextLeft:
    case lTextRight:
      format |= DT_VCENTER;
      break;
    case lTextTop:
      format |= DT_VCENTER;//DT_BOTTOM;
      break;
    case lTextBottom:
      format |= DT_VCENTER;//DT_TOP;
      break;
  }

  // Create a UI Face object for this button & let it paint the button face
  //
  TPoint  dstPt(rect.TopLeft());

  if (State == Down && GetEnabled() &&
      Window->GetFlatStyle()&TGadgetWindow::FlatStandard){
    if(IsHaveMouse()){
      if(Pressed){
        rect.Offset(1,1);
        if(format == DT_VCENTER && format == DT_CENTER)
          rect.Offset(1,1);
        else if(format == DT_VCENTER)
          rect.Offset(0,1);
        else if(format == DT_CENTER)
          rect.Offset(1,0);
      }
      TUIFace(rect, text, TColor::Sys3dFace,format).Paint(dc, dstPt,
                TUIFace::Normal, true, true);
    }
    else{
      TUIFace face(rect, text, TColor::Sys3dFace,format);
      if(Window->GadgetGetCaptured()==this)
        face.Paint(dc, dstPt,TUIFace::Normal, true);
      else
        face.Paint(dc, dstPt, TUIFace::Down, Pressed, false);
    }
  }
  else{
    TUIFace face(rect, text, TColor::Sys3dFace,format);
    if (!GetEnabled())
      face.Paint(dc, dstPt, TUIFace::Disabled, false, false);
    else if (State == Indeterminate)
      face.Paint(dc, dstPt, TUIFace::Indeterm, Pressed, false);
    else if (State == Down) // Down and not flat
      face.Paint(dc, dstPt, TUIFace::Down, Pressed, false);
    else
      face.Paint(dc, dstPt, TUIFace::Normal, Pressed, false);
  }

  dc.SetTextColor(oldTxColor);

  dc.RestoreFont();
}

__OWL_END_NAMESPACE

//==============================================================================
