#include <owl/pch.h>

#include "App.h"
#include "Editor.h"
#include "Preview.h"
#include "HexViewer.h"
#include "About.h"
#include "App.rh"
#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <owl/docmanag.h>
#include <owl/filedoc.h>
#include <owl/cmdline.h>
#if !defined(UNICODE)
#include <coolprj/hexdocument.h>
#endif
#include <type_traits>

using namespace owl;

const auto RegistryEntryName = tstring{_T("OWLNext_RichEditor")};

DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, TPlainEditor, DocType1);
DEFINE_DOC_TEMPLATE_CLASS(TFileDocument, TRichEditor, DocType2);

#if !defined(UNICODE)
DEFINE_DOC_TEMPLATE_CLASS(THexDocument, THexViewer, DocType3);
#endif

DocType1 __dvt1(_T("Text Files"), _T("*.txt"), 0, _T("txt"), dtAutoDelete | dtUpdateDir | dtOverwritePrompt);
DocType2 __dvt2(_T("RichEdit Files"), _T("*.rtf"), 0, _T("rtf"), dtAutoDelete | dtUpdateDir | dtOverwritePrompt);

#if !defined(UNICODE)
DocType3 __dvt3(_T("Hex Files"), _T("*.hex"), 0, _T("hex"), dtAutoDelete | dtUpdateDir | dtOverwritePrompt);
#endif

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE2(TRichEditorApp, TRecentFiles, TApplication)
  EV_OWLVIEW(dnCreate, EvNewView),
  EV_OWLVIEW(dnClose, EvCloseView),
  EV_COMMAND(CM_FILEPRINT, CmFilePrint),
  EV_COMMAND_ENABLE(CM_FILEPRINT, CePrintEnable),
  EV_COMMAND(CM_FILEPRINTERSETUP, CmFilePrintSetup),
  EV_COMMAND_ENABLE(CM_FILEPRINTERSETUP, CePrintEnable),
  EV_COMMAND(CM_FILEPRINTPREVIEW, CmFilePrintPreview),
  EV_COMMAND_ENABLE(CM_FILEPRINTPREVIEW, CePrintEnable),
  EV_COMMAND(CM_FILESEND, CmFileSend),
  EV_COMMAND_ENABLE(CM_FILESEND, CeFileSend),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_WM_DROPFILES,
  EV_WM_SETTINGCHANGE,
  EV_OWLDOCUMENT(dnCreate, EvOwlDocument),
  EV_OWLDOCUMENT(dnRename, EvOwlDocument),
  EV_REGISTERED(MruFileMessage, CmFileSelected),
END_RESPONSE_TABLE;

TRichEditorApp::TRichEditorApp()
  : TApplication(_T("RichEditor Example")),
  TRecentFiles(_T("Software\\OWLNext\\") + RegistryEntryName, 4, 30, true)
{
  Printer = 0;
  Printing = 0;
  SetDocManager(new TDocManager(dmMDI, this));
  GetDocManager()->SetAdditionalFileDialogFlags(OFN_ENABLESIZING);
  ApxMail = new TMailer();
}

TRichEditorApp::~TRichEditorApp()
{
  delete Printer;
  delete ApxMail;
}

void TRichEditorApp::CreateGadgets(TDockableControlBar* cb, bool server)
{
  if (!server)
  {
    cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW));
    cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN));
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

  if (!server)
  {
    cb->Insert(*new TSeparatorGadget(6));
    cb->Insert(*new TButtonGadget(CM_FILEPRINTPREVIEW, CM_FILEPRINTPREVIEW));
    cb->Insert(*new TButtonGadget(CM_FILEPRINT, CM_FILEPRINT));
  }

  cb->EnableFlatStyle();

  // Add caption and fly-over help hints.
  //
  cb->SetCaption(_T("Toolbar"));
  cb->SetHintMode(TGadgetWindow::EnterHints);
}

void TRichEditorApp::SetupSpeedBar(TDecoratedMDIFrame* frame)
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

void TRichEditorApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  auto workspace = std::make_unique<TWorkspace>(this);
  TDecoratedMDIFrame* frame = new TDecoratedMDIFrame(GetName(), IDM_MDI, std::move(workspace), true, this);
  frame->ModifyStyle(0, WS_EX_ACCEPTFILES); // Enable acceptance of dropped files.
  frame->SetIcon(this, IDI_MDIAPPLICATION);
  frame->SetIconSm(this, IDI_MDIAPPLICATION);
  frame->SetMenuDescr(TMenuDescr(IDM_MDI));
  frame->Attr.AccelTable = IDA_EDITFILE;
  frame->SetWindowPos(NULL, TRect(0, 0, 640, 640), SWP_NOMOVE | SWP_NOZORDER);

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed,
    TStatusBar::CapsLock |
    TStatusBar::NumLock |
    TStatusBar::ScrollLock);
  frame->Insert(*sb, TDecoratedFrame::Bottom);
  SetupSpeedBar(frame);
  SetMainWindow(frame);
}

void TRichEditorApp::InitInstance()
{
  TApplication::InitInstance();
  ProcessCmdLine();
}

void TRichEditorApp::ProcessCmdLine()
{
  TCmdLine cmd(GetCmdLine().c_str());
  while (cmd.GetTokenKind() != TCmdLine::Done)
  {
    if (cmd.GetTokenKind() == TCmdLine::Option)
    {
      if (cmd.GetToken() == _T("unregister"))
      {
        UnRegisterInfo();
        return;
      }
    }
    cmd.NextToken();
  }
  RegisterInfo();
}

void TRichEditorApp::RegisterInfo()
{
  try
  {
    auto f = GetModuleFileName();
    TRegKey& root = TRegKey::GetClassesRoot();
    TRegKey{root, RegistryEntryName + _T(".Application\\DefaultIcon")}.
      SetValue(nullptr, f); // Default value.

    TRegKey{root, RegistryEntryName + _T(".Document.1\\DefaultIcon")}.
      SetValue(nullptr, f + _T(",1")); // Default value.
  }
  catch (const TXRegistry& x)
  {
    TRACE(_T("RegisterInfo failed: TXRegistry: ") << x.why());
    WARN(x.GetErrorCode() == ERROR_ACCESS_DENIED, _T("Access was denied. "
      "Run the program as an administrator to test RegisterInfo."));
  }
}

void TRichEditorApp::UnRegisterInfo()
{
  try
  {
    const auto tryNukeKey = [&](const tstring& subkey)
    {
      TRegKey& root = TRegKey::GetClassesRoot();
      const auto r = root.NukeKey(subkey);
      const auto key = root.GetName() + tstring{_T("\\")} + subkey;
      if (r != ERROR_SUCCESS && r != ERROR_FILE_NOT_FOUND) throw TXRegistry{_T("NukeKey failed"), key, r};
      WARN(r == ERROR_FILE_NOT_FOUND, _T("UnRegisterInfo: NukeKey failed: Registry key does not exist: ")
        << key << _T(" (the program was probably not registered)"));
    };
    tryNukeKey(RegistryEntryName + _T(".Application"));
    tryNukeKey(RegistryEntryName + _T(".Document.1"));
  }
  catch (const TXRegistry& x)
  {
    TRACE(_T("UnRegisterInfo failed: TXRegistry: ") << x.why());
    WARN(x.GetErrorCode() == ERROR_ACCESS_DENIED, _T("Access was denied. "
      "Run the program as an administrator to test UnRegisterInfo."));
  }
}

void TRichEditorApp::EvNewView(TView& view)
{
  TMDIClient* mdiClient = dynamic_cast<TMDIClient*>(GetMainWindow()->GetClientWindow());
  if (mdiClient)
  {
    TMDIChild* child = new TMDIChild(*mdiClient, 0, view.GetWindow());

    // Set the menu descriptor for this view.
    //
    if (view.GetViewMenu())
      child->SetMenuDescr(*view.GetViewMenu());

    child->SetIcon(this, IDI_DOC);
    child->SetIconSm(this, IDI_DOC);
    child->Create();
  }
}

void TRichEditorApp::EvCloseView(TView&)
{}

void TRichEditorApp::CeFileSend(TCommandEnabler& ce)
{
  ce.Enable((GetDocManager()->GetCurrentDoc() != 0)
    && ApxMail->IsMAPIAvailable());
}

void TRichEditorApp::CmFileSend()
{
  // Check to see if a document exists
  //
  TDocument* currentDoc = GetDocManager()->GetCurrentDoc();
  if (currentDoc)
  {
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

void TRichEditorApp::CmFilePrint()
{
  // Create Printer object if not already created.
  //
  if (!Printer)
    Printer = new TPrinter;
  TAPointer<_TCHAR> docName(new _TCHAR[_MAX_PATH]);
  TDocument* currentDoc = GetDocManager()->GetCurrentDoc();
  if (currentDoc->GetTitle())
    _tcscpy(docName, currentDoc->GetTitle());
  else
    _tcscpy(docName, _T("Document"));

  // Create Printout window and set characteristics.
  //
  TMDIClient* workspace = dynamic_cast<TMDIClient*>(GetMainWindow()->GetClientWindow()); CHECK(workspace);
  TMDIChild* child = workspace->GetActiveMDIChild();
  if (!child) return;
  TTextPrintout printout(Printer, docName, child->GetClientWindow(), true);

  // Bring up the Print dialog and print the document.
  //
  Printing++;
  Printer->Print(GetWindowPtr(GetActiveWindow()), printout, true);
  Printing--;
}

void TRichEditorApp::CmFilePrintSetup()
{
  if (!Printer)
    Printer = new TPrinter;

  // Bring up the Print Setup dialog.
  //
  Printer->Setup(GetMainWindow());
}

void TRichEditorApp::CmFilePrintPreview()
{
  if (!Printer)
    Printer = new TPrinter;

  Printing++;

  TMDIClient* workspace = dynamic_cast<TMDIClient*>(GetMainWindow()->GetClientWindow()); CHECK(workspace);
  TMDIChild* child = workspace->GetActiveMDIChild();
  if (!child) return;
  TPreview* prevW = new TPreview(GetMainWindow(), Printer, child->GetClientWindow(), _T("Print Preview"), new TLayoutWindow(0, 0, 0));
  prevW->Create();

  // Here we resize the preview window to take the size of the MainWindow, then
  // hide the MainWindow.
  //
  TFrameWindow * mainWindow = GetMainWindow();
  TRect r = mainWindow->GetWindowRect();
  prevW->MoveWindow(r);
  prevW->ShowWindow(SW_SHOWNORMAL);
  mainWindow->ShowWindow(SW_HIDE);

  BeginModal(GetMainWindow());

  // After the user closes the preview Window, we take its size and use it
  // to size the MainWindow, then show the MainWindow again.
  //
  r = prevW->GetWindowRect();
  mainWindow->MoveWindow(r);
  mainWindow->ShowWindow(SW_SHOWNORMAL);

  // We must destroy the preview window explicitly.  Otherwise, the window will
  // not be destroyed until it's parent the MainWindow is destroyed.
  //
  prevW->Destroy();
  delete prevW;

  Printing--;
}

void TRichEditorApp::CePrintEnable(TCommandEnabler& tce)
{
  TMDIClient* workspace = dynamic_cast<TMDIClient*>(GetMainWindow()->GetClientWindow()); CHECK(workspace);
  TMDIChild* child = workspace->GetActiveMDIChild();
  if (child)
  {
    // If we have a Printer already created just test if all is okay.
    // Otherwise create a Printer object and make sure the printer really
    // exists and then delete the Printer object.
    //
    if (!Printer)
    {
      Printer = new TPrinter;
      tce.Enable(!Printer->GetSetup().Error);
    }
    else
      tce.Enable(!Printer->GetSetup().Error);
  }
  else
    tce.Enable(false);
}

void TRichEditorApp::CmHelpAbout()
{
  TAboutDlg(GetMainWindow()).Execute();
}

void TRichEditorApp::EvDropFiles(TDropInfo drop)
{
  // Iterate thru the entries in drop and create FileDrops objects for each one.
  //
  TFileDropletList files;
  int fileCount = drop.DragQueryFileCount();  // Number of files dropped.
  for (int i = 0; i < fileCount; i++)
    files.Add(new TFileDroplet(drop, i));

  AddFiles(files);
  drop.DragFinish();
}

void TRichEditorApp::AddFiles(TFileDropletList& files)
{
  // Open all files dragged in.
  //
  for (TFileDropletListIter fileIter(files); fileIter; fileIter++)
  {
    TDocTemplate* tpl = GetDocManager()->MatchTemplate(fileIter.Current()->GetName());
    if (tpl)
      GetDocManager()->CreateDoc(tpl, fileIter.Current()->GetName());
  }
}

void TRichEditorApp::EvOwlDocument(TDocument& doc)
{
  if (doc.GetDocPath())
    SaveMenuChoice(doc.GetDocPath());
}

TResult TRichEditorApp::CmFileSelected(TParam1 wp, TParam2)
{
  TAPointer<_TCHAR> text(new _TCHAR[_MAX_PATH]);
  GetMenuText(wp, text, _MAX_PATH);
  TDocTemplate* tpl = GetDocManager()->MatchTemplate(text);
  if (tpl)
    GetDocManager()->CreateDoc(tpl, text);
  return 0;
}

void TRichEditorApp::EvSettingChange(uint /*flags*/, LPCTSTR section)
{
  if (_tcscmp(section, _T("windows")) == 0)
  {
    // If the device changed in the WIN.INI file then the printer
    // might have changed.  If we have a TPrinter(Printer) then
    // check and make sure it's identical to the current device
    // entry in WIN.INI.
    //
    if (Printer)
    {
      const int bufferSize = 255;
      _TCHAR printDBuffer[bufferSize];
      LPTSTR printDevice = printDBuffer;
      LPTSTR devName;
      LPTSTR driverName = 0;
      LPTSTR outputName = 0;
      if (::GetProfileString(_T("windows"), _T("device"), _T(""), printDevice, bufferSize))
      {
        // The string which should come back is something like:
        //
        //      HP LaserJet III,hppcl5a,LPT1:
        //
        // Where the format is:
        //
        //      devName,driverName,outputName
        //
        devName = printDevice;
        while (*printDevice)
        {
          if (*printDevice == _T(','))
          {
            *printDevice++ = 0;
            if (!driverName)
              driverName = printDevice;
            else
              outputName = printDevice;
          }
          else
            printDevice = ::CharNext(printDevice);
        }

        if (Printer->GetSetup().Error != 0 ||
          _tcscmp(devName, Printer->GetSetup().GetDeviceName()) != 0 ||
          _tcscmp(driverName, Printer->GetSetup().GetDriverName()) != 0 ||
          _tcscmp(outputName, Printer->GetSetup().GetOutputName()) != 0)
        {
          // New printer installed so get the new printer device now.
          //
          delete Printer;
          Printer = new TPrinter;
        }
      }
      else
      {
        // No printer installed(GetProfileString failed).
        //
        delete Printer;
        Printer = new TPrinter;
      }
    }
  }
}

int OwlMain(int, _TCHAR*[])
{
  TRichEditorApp app;
  return app.Run();
}
