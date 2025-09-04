//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1991, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of class TRadioButton.  This defines the basic behavior for
// all radio buttons.
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_RADIOBUT_H)
# include <owl/radiobut.h>
#endif
#if !defined(OWL_GROUPBOX_H)
# include <owl/groupbox.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif

#if defined(BI_COMP_BORLANDC)
#  if (__BORLANDC__ < 0x530)
#   include <bwcc.h>
#  else
#   include <owl/private/bwcc.h>
#  endif
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

DEFINE_RESPONSE_TABLE1(TRadioButton, TCheckBox)
  EV_NOTIFY_AT_CHILD(BN_CLICKED, BNClicked),
END_RESPONSE_TABLE;

//
// constructor for a TRadioButton object
//
TRadioButton::TRadioButton(TWindow*        parent,
                           int             id,
                           LPCTSTR         title,
                           int x, int y, int w, int h,
                           TGroupBox*      group,
                           TModule*        module)
:
  TCheckBox(parent, id, title, x, y, w, h, group, module)
{
  Attr.Style = WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON;
}

//
//
//
TRadioButton::TRadioButton(TWindow*   parent,
                           int        resourceId,
                           TGroupBox* group,
                           TModule*   module)
:
  TCheckBox(parent, resourceId, group, module)
{
}

//
// Return name of predefined BWCC or Windows radio button class
//
LPCTSTR
TRadioButton::GetClassName()
{
#if defined(BI_COMP_BORLANDC) && !defined(_UNICODE)
  if (GetApplication()->BWCCEnabled())
    return RADIO_CLASS;
  else
#endif
    return _T("BUTTON");
}

//
// responds to an incoming BN_CLICKED message.
//
// need to see if it's checked because Windows generates two BN_CLICKED
// messages on keyboard input such as up arrow(but only one on mouse input),
// and we should only handle the one after it's checked
//
void
TRadioButton::BNClicked()
{
  if (GetCheck())
    TCheckBox::BNClicked();

  else
    DefaultProcessing();
}

#endif
#if !defined(SECTION) || SECTION == 2

IMPLEMENT_STREAMABLE1(TRadioButton, TCheckBox);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TRadioButton from the passed ipstream
//
void*
TRadioButton::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  ReadBaseObject((TCheckBox*)GetObject(), is);
  return GetObject();
}

//
// writes the TRadioButton to the passed opstream
//
void
TRadioButton::Streamer::Write(opstream& os) const
{
  WriteBaseObject((TCheckBox*)GetObject(), os);
}
#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

