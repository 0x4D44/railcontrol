//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Defines TMenuGadget
//
// A TMenuGadget is a text gadget that when pressed, it acts as a popup menu.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_MENUGADG_H)
# include <owl/menugadg.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

const int MaxMenuTextLen = 512;

//
// Create the popup menu and initialize the text gadget.
//
TMenuGadget::TMenuGadget(TMenu& menu, TWindow* window, int id,
                         TBorderStyle borderStyle, LPTSTR text, TFont* font)
:
  TTextGadget(id, borderStyle, TTextGadget::Left, 15, text, font),
  CmdTarget(window)
{
  SetShrinkWrap(true, true);
  PopupMenu = new TPopupMenu(menu);

  // initialize Text data member
  //
  TCHAR menuText[MaxMenuTextLen];
  if (PopupMenu->GetMenuString(0, menuText, MaxMenuTextLen, MF_BYPOSITION))
    SetText(menuText);
}

//
// Delete the allocated popup menu.
//
TMenuGadget::~TMenuGadget()
{
  delete PopupMenu;
}

//
// Popup menu on lbuttondown
//
void
TMenuGadget::LButtonDown(uint modKeys, TPoint& p)
{
  TGadget::LButtonDown(modKeys, p);
  TRect rect = GetBounds();
//  GetInnerRect(rect);
  TPoint p2(rect.TopLeft());
  Window->ClientToScreen(p2);
  PopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, p2, 0, *CmdTarget);
}

//
//
//
void
TMenuGadget::GetDesiredSize(TSize& size)
{
  // for flat style set borders minimum 1
  if((Window->GetFlatStyle()&TGadgetWindow::FlatStandard) &&
      GetBorderStyle()==None && Borders.Left == 0 && Borders.Top == 0 &&
      Borders.Right==0 && Borders.Bottom==0)
    Borders.Left = Borders.Top = Borders.Right = Borders.Bottom = 1;

  TTextGadget::GetDesiredSize(size);
}

//
//
//
void
TMenuGadget::PaintBorder(TDC& dc)
{
  if(Window->GetFlatStyle()&TGadgetWindow::FlatStandard){
    TBorderStyle oldStyle = BorderStyle;
    if(GetEnabled() && IsHaveMouse())
      BorderStyle = Raised;
    TTextGadget::PaintBorder(dc);
    BorderStyle = oldStyle;
  }
  else
    TTextGadget::PaintBorder(dc);
}

//
//
//
TColor
TMenuGadget::GetEnabledColor() const
{
  if((Window->GetFlatStyle()&TGadgetWindow::FlatHotText) && IsHaveMouse())
     return TColor::LtBlue;
  return TColor::SysBtnText;
}

__OWL_END_NAMESPACE
/* ========================================================================== */

