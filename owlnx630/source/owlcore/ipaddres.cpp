//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1998 by Bidus Yura, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:19 $
//
// Implementation of the TIPAddress class
//----------------------------------------------------------------------------
#include <owl/pch.h>

#if !defined(OWL_IPADDRES_H)
# include <owl/ipaddres.h>
#endif


__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;
DIAG_DECLARE_GROUP(OwlCommCtrl);        // CommonCtrl Diagnostic group

//
//
//
TIPAddress::TIPAddress(TWindow* parent, int id, int x, int y, int w, int h,
                       TModule* module)
:
  TControl(parent,id,_T(""),x,y,w,h,module)
{
  // Does this apply to extended common controls?
  if (!TCommCtrl::IsAvailable() &&
       TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionIE3)
    TXCommCtrl::Raise();

  TCommCtrl::Dll()->CheckCommonControl(ICC_INTERNET_CLASSES );
}


//
//
//
TIPAddress::TIPAddress(TWindow* parent, int resourceId, TModule* module)
:
  TControl(parent, resourceId, module)
{
  // Does this apply to extended common controls?
  if (!TCommCtrl::IsAvailable() &&
       TCommCtrl::Dll()->GetCtrlVersion() > ComCtlVersionIE3)
    TXCommCtrl::Raise();

  TCommCtrl::Dll()->CheckCommonControl(ICC_INTERNET_CLASSES );
}

//
//
//
TIPAddress::~TIPAddress()
{
}


// Return the proper class name.
// Windows class: WC_PAGESCROLLER is defined in commctrl.h
LPCTSTR
TIPAddress::GetClassName()
{
  return WC_IPADDRESS;
}


//
//
//
uint
TIPAddress::Transfer(void* buffer, TTransferDirection direction)
{

   TIPAddressBits* ipData = (TIPAddressBits*)buffer;
  if (direction == tdGetData)
    GetAddress(*ipData);
  else if (direction == tdSetData)
    SetAddress(*ipData);

  return sizeof(TIPAddressBits);
}
__OWL_END_NAMESPACE
/* ========================================================================== */
