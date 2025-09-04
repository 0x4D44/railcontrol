//----------------------------------------------------------------------------  
//  Project Urllink
//  
//  Copyright © 1998. All Rights Reserved.
//
//  SUBSYSTEM:    Urllink Application
//  FILE:         urllnkdc.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TUrllinkDlgClient (TDialog).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include <owl/urllink.h>

#include "urllnkap.h"
#include "urllnkdc.h"


//{{TUrllinkDlgClient Implementation}}


//--------------------------------------------------------
// TUrllinkDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TUrllinkDlgClient::TUrllinkDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
	TUrlLink* link = new TUrlLink(this, IDC_LINK);
	link->SetURL(_T("https://sourceforge.net/projects/owlnext/"));
	TUrlLink* link2 = new TUrlLink(this, IDC_LINK2);
	link2->SetURL(_T("https://sourceforge.net/p/owlnext/wiki/Examples/"));
}

