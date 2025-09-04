//----------------------------------------------------------------------------
//  Project Urllink
//  
//  Copyright © 1998. All Rights Reserved.
//
//  SUBSYSTEM:    Urllink Application
//  FILE:         urllnkdc.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TUrllinkDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(urllnkdc_h)              // Sentry, use file only if it's not already included.
#define urllnkdc_h

#include "urllnkap.rh"                  // Definition of all resources.


//{{TDialog = TUrllinkDlgClient}}
class TUrllinkDlgClient : public TDialog {
  public:
    TUrllinkDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);

};    //{{TUrllinkDlgClient}}


#endif  // urllnkdc_h sentry.
