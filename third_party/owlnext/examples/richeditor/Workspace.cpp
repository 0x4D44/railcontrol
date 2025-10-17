#include <owl/pch.h>

#include "Workspace.h"
#include "App.h"
#include "Preview.h"
#include <owl/docmanag.h>
#include <owl/listbox.h>
#include <owl/editfile.rh>

using namespace owl;

DEFINE_RESPONSE_TABLE1(TWorkspace, TMDIClient)
  EV_WM_DROPFILES,
END_RESPONSE_TABLE;

TWorkspace::TWorkspace(TModule* module)
  :
  TMDIClient(module)
{}

void TWorkspace::SetupWindow()
{
  TMDIClient::SetupWindow();
  DragAcceptFiles(true);
}

void TWorkspace::EvDropFiles(TDropInfo)
{
  Parent->ForwardMessage();
}
