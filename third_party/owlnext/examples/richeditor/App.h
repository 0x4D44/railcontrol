#ifndef APP_H
#define APP_H

#include "Workspace.h"
#include <owl/applicat.h>
#include <owl/rcntfile.h>
#include <owl/docking.h>
#include <owl/printer.h>
#include <owl/mailer.h>
#include <owl/decmdifr.h>

class TRichEditorApp
  : public owl::TApplication, public owl::TRecentFiles
{
public:

  TRichEditorApp();
  virtual ~TRichEditorApp();

  void CreateGadgets(owl::TDockableControlBar* cb, bool server = false);

  // Public data members used by the print menu commands and Paint routine in MDIChild.
  //
  owl::TPrinter* Printer;
  int Printing; // Printing in progress.
  owl::TMailer* ApxMail;

protected:

  virtual void InitMainWindow(); // override
  virtual void InitInstance(); // override

  void CmFilePrint();
  void CmFilePrintSetup();
  void CmFilePrintPreview();
  void CePrintEnable(owl::TCommandEnabler& tce);
  void CmFileSend();
  void CeFileSend(owl::TCommandEnabler& ce);
  void CmHelpAbout();

  void EvNewView(owl::TView& view);
  void EvCloseView(owl::TView& view);
  void EvDropFiles(owl::TDropInfo drop);
  void EvSettingChange(owl::uint, LPCTSTR section);
  void EvOwlDocument(owl::TDocument& doc);
  owl::TResult CmFileSelected(owl::TParam1 wp, owl::TParam2 lp);

private:

  owl::THarbor* ApxHarbor;

  void SetupSpeedBar(owl::TDecoratedMDIFrame* frame);
  void AddFiles(owl::TFileDropletList& files);
  void ProcessCmdLine();
  void RegisterInfo();
  void UnRegisterInfo();

  DECLARE_RESPONSE_TABLE(TRichEditorApp);
};

#endif
