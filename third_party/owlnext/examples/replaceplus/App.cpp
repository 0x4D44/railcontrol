#include "pch.h"
#pragma hdrstop

#include "App.h"
#include "Client.h"
#include "Replacer.h"
#include "App.rh"
#include <owl/cmdline.h>
#include <owl/framewin.h>
#include <owl/configfl.h>

using namespace owl;

namespace
{

  const tchar ReplacePlusRegKeyName[] = _T("ReplacePlus");

  //-----------------------------------------------------------------------------

  //
  // Loads arguments from the Windows Registry.
  //
  TReplaceData LoadArguments()
  {
    TReplaceData d;
    TRegConfigFile f(ReplacePlusRegKeyName);
    TConfigFileSection s(f, _T("Options"));
    d.Action = s.ReadInteger(_T("Action"), d.Action);
    s.ReadString(_T("Folder"), d.Folder, d.Folder);
    s.ReadString(_T("Filter"), d.Filter, d.Filter);
    s.ReadString(_T("SearchTerm"), d.SearchTerm, d.SearchTerm);
    s.ReadString(_T("Replacement"), d.Replacement, d.Replacement);
    d.RecurseFlag = s.ReadBool(_T("RecurseFlag"), d.RecurseFlag);
    return d;
  }

  //-----------------------------------------------------------------------------

  //
  // Saves the given arguments to the Windows Registry.
  //
  void SaveArguments(const TReplaceData& d)
  {
    TRegConfigFile f(ReplacePlusRegKeyName);
    TConfigFileSection s(f, _T("Options"));
    s.WriteInteger(_T("Action"), d.Action);
    s.WriteString(_T("Folder"), d.Folder);
    s.WriteString(_T("Filter"), d.Filter);
    s.WriteString(_T("SearchTerm"), d.SearchTerm);
    s.WriteString(_T("Replacement"), d.Replacement);
    s.WriteBool(_T("RecurseFlag"), d.RecurseFlag);
  }

  //-----------------------------------------------------------------------------

  //
  // Extracts and returns the arguments from the command line used to start this process.
  //
  TReplaceData ProcessCommandLine()
  {
    //
    // This local helper constructs the arguments string by string.
    //
    struct TLocal
    {
      TReplaceData d;

      void AddString(const tstring& s)
      {
        if (!d.Replacement.empty()) return; // Full.
        tstring& m = d.Folder.empty() ? d.Folder :
          d.Filter.empty() ? d.Filter :
          d.SearchTerm.empty() ? d.SearchTerm :
          d.Replacement;
        m = s;
      }
    } local;

    TCmdLine c(::GetCommandLine());
    c.NextToken(); // Skip Start token.
    c.NextToken(); // Skip Name token (program name).
    while (c.GetTokenKind() != TCmdLine::Done) 
    {
      if (c.GetTokenKind() == TCmdLine::Option) 
      {
        if (c.GetToken() == _T("r+"))
          local.d.RecurseFlag = true;
        else if (c.GetToken() == _T("r-"))
          local.d.RecurseFlag = false;
      }
      else if (c.GetTokenKind() == TCmdLine::Name) 
        local.AddString(c.GetToken());
      c.NextToken();
    }
    return local.d;
  }

} // namespace

//-----------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TReplaceApp, TApplication)
  EV_COMMAND(CM_REPLACE, CmReplace),
END_RESPONSE_TABLE;

//-----------------------------------------------------------------------------

TReplaceApp::TReplaceApp()
  : TApplication(_T("ReplacePlus 6.32")),
  Arguments(LoadArguments())
{}

//-----------------------------------------------------------------------------

TReplaceApp::~TReplaceApp()
{
  SaveArguments(Arguments);
}

//-----------------------------------------------------------------------------

void TReplaceApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  TReplaceClient* client = new TReplaceClient(0, Arguments, ReplacePlusRegKeyName);
  TFrameWindow* frame = new TFrameWindow(0, GetName(), client, true);
  frame->SetIcon(this, IDI_SDIAPPLICATION);
  frame->SetIconSm(this, IDI_SDIAPPLICATION);
  frame->ModifyStyle(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME, WS_DLGFRAME);
  SetMainWindow(frame);
}

//-----------------------------------------------------------------------------

void TReplaceApp::CmReplace()
{
  bool r = PerformCommand(Arguments, GetMainWindow());
  if (!r) 
    GetMainWindow()->MessageBox(_T("Command failed."), _T("ReplacePlus Error"), MB_ICONERROR);
}

//-----------------------------------------------------------------------------

int OwlMain(int argc, tchar* [])
{
  // If arguments are passed on the command line, then skip the GUI.
  //
  if (argc > 1)
  {
    TReplaceData d = ProcessCommandLine();
    return PerformCommand(d) ? 0 : 1; // Return 'error level' 1 on errors.
  }
  else
  {
    TReplaceApp a;
    return a.Run();
  }
}
