//----------------------------------------------------------------------------
//  Project ApxMdiDv
//  Borland International
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ApxMdiDv Application
//  FILE:         apxmddva.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TApxMdiDvApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <owl/docmanag.h>
#include <owl/filedoc.h>
#include <owl/template.h>

#include <owl/cmdline.h>
#include <owl/filename.h>
#include <owl/registry.h>

#include "apxMDdva.h"
#include "apmdmdic.h"
#include "apmdmdi1.h"
#include "apxmddev.h"
#include "apxmddad.h"                        // Definition of about dialog.

//{{TApxMdiDvApp Implementation}}



//{{DOC_VIEW}}
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, TApxMdiDvEditView, DocType1);
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, TApxMdiDvRichEditView, DocType2);

//{{DOC_VIEW_END}}

//{{DOC_MANAGER}}
//DocType1 __dvt1("All Files", "*.*", 0, "txt", dtAutoDelete | dtUpdateDir | dtOverwritePrompt);
DocType1 __dvt1(_T("Text Files"), _T("*.txt"), 0, _T("txt"), dtAutoDelete | dtUpdateDir | dtOverwritePrompt);
DocType2 __dvt2(_T("RichEdit Files"), _T("*.rtf"), 0, _T("rtf"), dtAutoDelete | dtUpdateDir | dtOverwritePrompt);
//{{DOC_MANAGER_END}}


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE2(TApxMdiDvApp, TRecentFiles, TApplication)
//{{TApxMdiDvAppRSP_TBL_BEGIN}}
  EV_OWLVIEW(dnCreate, EvNewView),
  EV_OWLVIEW(dnClose,  EvCloseView),
  EV_COMMAND(CM_FILESEND, CmFileSend),
  EV_COMMAND_ENABLE(CM_FILESEND, CeFileSend),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_WM_DROPFILES,
  EV_WM_WININICHANGE,
  EV_OWLDOCUMENT(dnCreate, EvOwlDocument),
  EV_OWLDOCUMENT(dnRename, EvOwlDocument),
  EV_REGISTERED(MruFileMessage, CmFileSelected),
//{{TApxMdiDvAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
//
TApxMdiDvApp::TApxMdiDvApp()
:
	TApplication(_T("AppExpert Rich Edit MDI DocView Example")),
  TRecentFiles(_T("Software\\OWLNext\\ApxRevDv"), 4, 30, true)
{
  Printer = 0;
  Printing = 0;

  SetDocManager(new TDocManager(dmMDI, this));

  ApxMail = new TMailer();
}


TApxMdiDvApp::~TApxMdiDvApp()
{
  delete Printer;
  delete ApxMail;
}


void TApxMdiDvApp::CreateGadgets(TDockableControlBar* cb, bool server)
{
  if (!server) {
    cb->Insert(*new TButtonGadget(CM_MDIFILENEW, CM_MDIFILENEW));
    cb->Insert(*new TButtonGadget(CM_MDIFILEOPEN, CM_MDIFILEOPEN));
    cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
    cb->Insert(*new TSeparatorGadget(6));
  }

  cb->Insert(*new TButtonGadget(CM_EDITCUT, CM_EDITCUT));
  cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY));
  cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_EDITUNDO, CM_EDITUNDO));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
  cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));

  if (!server) {
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_FILEPRINT, CM_FILEPRINT));
		cb->Insert(*new TButtonGadget(CM_FILEPRINTPREVIEW, CM_FILEPRINTPREVIEW));
  }

  cb->EnableFlatStyle();

  // Add caption and fly-over help hints.
  //
  cb->SetCaption(_T("Toolbar"));
  cb->SetHintMode(TGadgetWindow::EnterHints);
}


void TApxMdiDvApp::SetupSpeedBar(TDecoratedMDIFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //
  TDockableControlBar* cb = new TDockableControlBar(frame);
  CreateGadgets(cb);

  // Setup the toolbar ID used by OLE 2 for toolbar negotiation.
  //
  cb->Attr.Id = IDW_TOOLBAR;

  ApxHarbor->Insert(*cb, alTop);
}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Application main window construction & intialization.
//
void TApxMdiDvApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  MdiClient = new TApxMdiDvMDIClient(this);
  TDecoratedMDIFrame* frame = new TDecoratedMDIFrame(GetName(), IDM_MDI, *MdiClient, true, this);

	// Enable acceptance of dropped files
  //
  frame->Attr.ExStyle |= WS_EX_ACCEPTFILES;

  nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_MDIAPPLICATION);
  frame->SetIconSm(this, IDI_MDIAPPLICATION);

  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_MDI;

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed,
                                  TStatusBar::CapsLock        |
                                  TStatusBar::NumLock         |
                                  TStatusBar::ScrollLock);
  frame->Insert(*sb, TDecoratedFrame::Bottom);

  SetupSpeedBar(frame);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_MDI));

}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Application instance initialization.
//
void TApxMdiDvApp::InitInstance()
{
  TApplication::InitInstance();
  ProcessCmdLine();
}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Process command line parameters.
//
void TApxMdiDvApp::ProcessCmdLine()
{
  TCmdLine cmd(GetCmdLine().c_str());

  while (cmd.Kind != TCmdLine::Done) {
    if (cmd.Kind == TCmdLine::Option) {
      if (_tcsnicmp(cmd.Token, _T("unregister"), cmd.TokenLen) == 0) {
        UnRegisterInfo();
        return;
      }
    }
    cmd.NextToken();
  }

  RegisterInfo();
}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Register application info.
//
void TApxMdiDvApp::RegisterInfo()
{
  TAPointer<_TCHAR> buffer(new TCHAR[_MAX_PATH]);

  GetModuleFileName(buffer, _MAX_PATH);

	TRegKey(TRegKey::GetClassesRoot(), _T("ApxRevDv.Application\\DefaultIcon")).SetDefValue(0, REG_SZ, buffer, _tcslen(buffer));
	_tcscat(buffer, _T(",1"));
	TRegKey(TRegKey::GetClassesRoot(), _T("ApxRevDv.Document.1\\DefaultIcon")).SetDefValue(0, REG_SZ, buffer, _tcslen(buffer));
	_tcscpy(buffer, _T("ApxRevDv.Document.1"));
	//TRegKey(TRegKey::ClassesRoot, ".txt").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Unregister application info.
//
void TApxMdiDvApp::UnRegisterInfo()
{
	TAPointer<_TCHAR> buffer(new _TCHAR[_MAX_PATH]);

	GetModuleFileName(buffer, _MAX_PATH);

	TRegKey(TRegKey::GetClassesRoot(), _T("ApxRevDv.Application")).DeleteKey(_T("DefaultIcon"));
	TRegKey(TRegKey::GetClassesRoot(), _T("ApxRevDv.Document.1")).DeleteKey(_T("DefaultIcon"));

	TRegKey::GetClassesRoot().DeleteKey(_T("ApxRevDv.Application"));
	TRegKey::GetClassesRoot().DeleteKey(_T("ApxRevDv.Document.1"));
  //TRegKey::GetClassesRoot().DeleteKey(".txt");
}



//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~
// Response Table handlers:
//
void TApxMdiDvApp::EvNewView(TView& view)
{
  TMDIClient* mdiClient = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), TMDIClient);
  if (mdiClient) {
    TApxMdiDvMDIChild* child = new TApxMdiDvMDIChild(*mdiClient, 0, view.GetWindow());

    // Set the menu descriptor for this view.
    //
    if (view.GetViewMenu())
			child->SetMenuDescr(*view.GetViewMenu());

    // Assign icons with this child window.
    //
    child->SetIcon(this, IDI_DOC);
    child->SetIconSm(this, IDI_DOC);

    child->Create();
  }
}


void TApxMdiDvApp::EvCloseView(TView&)
{
}


void TApxMdiDvApp::CeFileSend(TCommandEnabler& ce)
{
  ce.Enable((GetDocManager()->GetCurrentDoc() != 0)
            && ApxMail->IsMAPIAvailable());
}


void TApxMdiDvApp::CmFileSend ()
{
  // Check to see if a document exists
  //
  TDocument* currentDoc = GetDocManager()->GetCurrentDoc();

  if (currentDoc) {
    TAPointer<_TCHAR> savedPath(new _TCHAR[_MAX_PATH]);
    TAPointer<_TCHAR> docName(new _TCHAR[_MAX_PATH]);

    bool dirtyState = currentDoc->IsDirty();

    if (currentDoc->GetDocPath())
      _tcscpy(savedPath, currentDoc->GetDocPath());
    else
      _tcscpy(savedPath, _T(""));

    if (currentDoc->GetTitle())
      _tcscpy(docName, currentDoc->GetTitle());
    else
      _tcscpy(docName, _T("Document"));

    TFileName tempFile(TFileName::TempFile);

    currentDoc->SetDocPath(tempFile.Canonical().c_str());
    currentDoc->Commit(true);

    currentDoc->SetDocPath(savedPath);
    currentDoc->SetDirty(dirtyState);

    ApxMail->SendDocuments(GetMainWindow(), tempFile.Canonical().c_str(), docName, false);

    tempFile.Remove();
  }
}


//--------------------------------------------------------
// TApxMdiDvApp
// ~~~~~~~~~~~
// Menu Help About ApxMdiDv command
void TApxMdiDvApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TApxMdiDvAboutDlg(GetMainWindow()).Execute();
}


void TApxMdiDvApp::EvDropFiles(TDropInfo drop)
{
  TFileDropletList files;

  // Iterate thru the entries in drop and create FileDrops objects for each
  // one.
  //
  int fileCount = drop.DragQueryFileCount();  // Number of files dropped.
  for (int i = 0; i < fileCount; i++)
    files.Add(new TFileDroplet(drop, i));

  // Open the files that were dropped.
  //
  AddFiles(files);

  // Release the memory allocated for this handle with DragFinish.
  //
  drop.DragFinish();
}


void TApxMdiDvApp::AddFiles(TFileDropletList& files)
{
  // Open all files dragged in.
  //
  for (TFileDropletListIter fileIter(files); fileIter; fileIter++) {
    TDocTemplate* tpl = GetDocManager()->MatchTemplate(fileIter.Current()->GetName());
    if (tpl)
      GetDocManager()->CreateDoc(tpl, fileIter.Current()->GetName());
  }
}


void TApxMdiDvApp::EvOwlDocument(TDocument& doc)
{
  if (doc.GetDocPath())
    SaveMenuChoice(doc.GetDocPath());
}


TResult TApxMdiDvApp::CmFileSelected(TParam1 wp, TParam2)
{
  TAPointer<_TCHAR> text(new _TCHAR[_MAX_PATH]);

  GetMenuText(wp, text, _MAX_PATH);
  TDocTemplate* tpl = GetDocManager()->MatchTemplate(text);
  if (tpl)
    GetDocManager()->CreateDoc(tpl, text);
  return 0;
}


void TApxMdiDvApp::EvWinIniChange(LPTSTR section)
{
  if (_tcscmp(section, _T("windows")) == 0) {
    // If the device changed in the WIN.INI file then the printer
    // might have changed.  If we have a TPrinter(Printer) then
    // check and make sure it's identical to the current device
    // entry in WIN.INI.
    //
    if (Printer) {
      const int bufferSize = 255;
      _TCHAR printDBuffer[bufferSize];
      LPTSTR printDevice = printDBuffer;
      LPTSTR devName;
      LPTSTR driverName = 0;
      LPTSTR outputName = 0;
      if (::GetProfileString(_T("windows"), _T("device"), _T(""), printDevice, bufferSize)) {
        // The string which should come back is something like:
        //
        //      HP LaserJet III,hppcl5a,LPT1:
        //
        // Where the format is:
        //
        //      devName,driverName,outputName
        //
        devName = printDevice;
        while (*printDevice) {
          if (*printDevice == _T(',')) {
            *printDevice++ = 0;
            if (!driverName)
              driverName = printDevice;
            else
              outputName = printDevice;
          }
          else
            printDevice = ::CharNext(printDevice);
        }

        if (Printer->GetSetup().Error != 0                ||
            _tcscmp(devName, Printer->GetSetup().GetDeviceName()) != 0  ||
            _tcscmp(driverName, Printer->GetSetup().GetDriverName()) != 0 ||
            _tcscmp(outputName, Printer->GetSetup().GetOutputName()) != 0 ) {
          // New printer installed so get the new printer device now.
          //
          delete Printer;
          Printer = new TPrinter(this);
        }
      }
      else {
        // No printer installed(GetProfileString failed).
        //
        delete Printer;
        Printer = new TPrinter(this);
      }
    }
  }
}


int OwlMain(int , _TCHAR* [])
{
  TApxMdiDvApp   app;
  return app.Run();
}
