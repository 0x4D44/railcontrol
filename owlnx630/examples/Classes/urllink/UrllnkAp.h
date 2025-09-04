//----------------------------------------------------------------------------
//  Project Urllink
//  
//  Copyright © 1998. All Rights Reserved.
//
//  SUBSYSTEM:    Urllink Application
//  FILE:         urllnkap.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TUrllinkApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(urllnkap_h)              // Sentry, use file only if it's not already included.
#define urllnkap_h

#include <owl/decframe.h>
#include <owl/opensave.h>


#include "urllnkap.rh"            // Definition of all resources.


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, LPCTSTR title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TUrllinkApp}}
class TUrllinkApp : public TApplication {
  private:


  public:
    TUrllinkApp();
    virtual ~TUrllinkApp();

//{{TUrllinkAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TUrllinkAppVIRTUAL_END}}

//{{TUrllinkAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TUrllinkAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TUrllinkApp);
};    //{{TUrllinkApp}}


#endif  // urllnkap_h sentry.
