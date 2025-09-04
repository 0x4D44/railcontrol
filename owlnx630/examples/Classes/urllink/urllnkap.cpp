//----------------------------------------------------------------------------   
//  Project Urllink
//  
//  Copyright © 1998. All Rights Reserved.
//
//  SUBSYSTEM:    Urllink Application
//  FILE:         urllnkap.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TUrllinkApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <stdio.h>

#include "urllnkap.h"
#include "urllnkdc.h"                        // Definition of client class.


//{{TUrllinkApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TUrllinkApp, TApplication)
//{{TUrllinkAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TUrllinkAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TUrllinkApp
//
TUrllinkApp::TUrllinkApp() : TApplication(_T("Urllink"))
{

  // INSERT>> Your constructor code here.
}


TUrllinkApp::~TUrllinkApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TUrllinkApp
// ~~~~~
// Application intialization.
//
void TUrllinkApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  TSDIDecFrame* frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);
  frame->Attr.Style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);

  SetMainWindow(frame);

}



//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, LPCTSTR title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TUrllinkDlgClient(0) : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TUrllinkApp
// ~~~~~~~~~~~
// Menu Help About Urllink command
//
void TUrllinkApp::CmHelpAbout()
{
}



int OwlMain(int , _TCHAR* [])
{
  TUrllinkApp   app;
  return app.Run();
}
