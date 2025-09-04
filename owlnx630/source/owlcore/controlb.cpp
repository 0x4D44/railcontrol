//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:18 $
//
// Implementation of class TControlBar.
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_CONTROLB_H)
# include <owl/controlb.h>
#endif
#if !defined(OWL_UIMETRIC_H)
# include <owl/uimetric.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlWin);

IMPLEMENT_CASTABLE(TControlBar);

//
//
//
TControlBar::TControlBar(TWindow*        parent,
                         TTileDirection  direction,
                         TFont*          font,
                         TModule*        module)
:
  TGadgetWindow(parent, direction, font, module)
{
  Margins.Units = TMargins::BorderUnits;

  if (Direction == Horizontal){
    Margins.Left = Margins.Right = TUIMetric::CxFixedFrame;
    Margins.Top = Margins.Bottom = TUIMetric::CyFixedFrame;
  }
  else {
    Margins.Left = Margins.Right = TUIMetric::CxFixedFrame;
    Margins.Top = Margins.Bottom = TUIMetric::CyFixedFrame;
  }


//  Margins.Left = Margins.Right = TUIMetric::CxSizeFrame + 2;  // (6) fixed?
//  Margins.Top = Margins.Bottom = TUIMetric::CyFixedFrame;     // (2) fixed?

  // Toolbars default to having tooltips
  //
  WantTooltip = true;

  TRACEX(OwlWin, OWL_CDLEVEL, "TControlBar constructed @" << (void*)this);
}

//
//
//
TControlBar::~TControlBar()
{
  TRACEX(OwlWin, OWL_CDLEVEL, "TControlBar destructed @" << (void*)this);
}

__OWL_END_NAMESPACE
/* ========================================================================== */

