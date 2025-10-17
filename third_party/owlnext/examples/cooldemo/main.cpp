//
// OWLNext Application Entry Point
//
#include "pch.h"
#pragma hdrstop

#include "resource.h"

#include <owl/applicat.h>
#include <owl/rcntfile.h>
#include <owl/statusba.h>
#include <owl/bitmapga.h>
#include <owl/profile.h>
#include <owl/transfer.h>
#include <owl/memcbox.h>
#include <owl/opensave.h>
#include <owl/shellitm.h>
#include <owl/clipboar.h>
#include <filesystem>
#include <chrono>

#include <owl/editfile.rh>
#include <coolprj/cooledit.rh>

#include "cooldemoedit.h"
#include "cooldemogrid.h"

using namespace owl;
using namespace std;

class TCoolDemoApp 
  : public TApplication, public owl::TRecentFiles
{
public:
  TCoolDemoApp() 
    : TApplication(_T("CoolDemo")),
    TRecentFiles{to_tstring(GetIniFileName())},
    FileData{}
  {
    FileData.Flags = OFN_ALLOWMULTISELECT |
      OFN_FILEMUSTEXIST |
      OFN_HIDEREADONLY |
      OFN_OVERWRITEPROMPT |
      OFN_EXPLORER |
      OFN_ENABLESIZING;

    FileData.SetFilter(_T("All Files (*.*)|*.*|"));
    FileData.DefExt = _T("");
  }

  auto GetIniFileName() const -> filesystem::path
  {
    using namespace filesystem;
    const auto getRoamingAppDataFolderPath = []
    {
      auto buf = LPWSTR{};
      const auto r = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &buf);
      CHECK(r == S_OK); InUse(r);
      auto p = unique_ptr<WCHAR, decltype(&CoTaskMemFree)>{buf, &CoTaskMemFree};
      return filesystem::path{p.get()};
    };
    auto v = TModuleVersionInfo{GetHandle()};
    const auto app = v.GetProductName() + _T(' ') + tstring{GetName()};
    const auto appdata = getRoamingAppDataFolderPath() / app;
    if (!exists(appdata))
      create_directory(appdata);
    return appdata / (app + _T(".ini"));
  }

  auto GetMDIClient() -> owl::TMDIClient&
  {
    const auto f = dynamic_cast<owl::TMDIFrame*>(GetMainWindow()); CHECK(f);
    const auto c = f->GetClientWindow(); CHECK(c);
    return *c;
  }

  auto CanClose() -> bool override
  {
    const auto r = TApplication::CanClose();
    if (!r) return false;

    // Save the window position until next run.
    //
    auto s = TProfile{_T("MainWindowPosition"), to_tstring(GetIniFileName())};
    const auto wp = GetMainWindow()->GetWindowPlacement();
    const auto showCmd = (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) ?
      SW_SHOWDEFAULT : wp.showCmd;
    s.WriteInt(_T("showCmd"), showCmd);
    s.WriteInt(_T("left"), wp.rcNormalPosition.left);
    s.WriteInt(_T("top"), wp.rcNormalPosition.top);
    s.WriteInt(_T("right"), wp.rcNormalPosition.right);
    s.WriteInt(_T("bottom"), wp.rcNormalPosition.bottom);
    return true;
  }

protected:
  void InitMainWindow() override
  {
    auto c = make_unique<TMDIClient>();
    auto f = make_unique<TDecoratedMDIFrame>(GetName(), TResId{}, move(c));
    f->SetMenuDescr(TMenuDescr(IDM_MAINMENU));
    f->SetAcceleratorTable(IDM_MAINMENU);

    const auto modeIndicators = static_cast<uint>(TStatusBar::CapsLock | TStatusBar::NumLock | TStatusBar::ScrollLock);
    auto sb = make_unique<TStatusBar>(f.get(), TGadget::TBorderStyle::None, modeIndicators);

    // ID_LINEINDICATOR
    TGadget* textGadget =
      new TDynamicTextGadget(ID_LINEINDICATOR, TTextGadget::Recessed,
        TTextGadget::Left, 4);
    sb->Insert(*textGadget);
    // ID_DIRTYINDICATOR
    TGadget* bmpGadget =
      new TDynamicBitmapGadget(ID_DIRTYINDICATOR, ID_DIRTYINDICATOR,
        TTextGadget::Recessed, 3);
    sb->Insert(*bmpGadget, TStatusBar::Before);
    //ID_INSERTINDICATOR
    textGadget = new TDynamicTextGadget(ID_INSERTINDICATOR,
      TTextGadget::Recessed, TTextGadget::Left, 2);
    TGadget* gd = sb->GadgetWithId(IDG_STATUS_NUM);
    sb->Insert(*textGadget, TStatusBar::After, gd);


    sb->SetMargins({ TMargins::TUnits::Pixels, 4, 4, 4, 4 });
    f->Insert(*sb, TDecoratedFrame::Bottom);
    sb.release(); // TMainWindow took ownership.

    // Restore the window position since last run.
    //
    auto s = TProfile{_T("MainWindowPosition"), to_tstring(GetIniFileName())};
    const auto r = f->GetWindowRect();
    if (nCmdShow == SW_SHOWDEFAULT)
      nCmdShow = s.GetInt(_T("showCmd"), nCmdShow);
    f->MoveWindow(TRect{
      s.GetInt(_T("left"), r.left),
      s.GetInt(_T("top"), r.top),
      s.GetInt(_T("right"), r.right),
      s.GetInt(_T("bottom"), r.bottom)});
    SetMainWindow(std::move(f));
  }

  void CmNewCoolEditWindow()
  {
    const auto client = new TCoolDemoEdit{ nullptr, 0, nullptr, 0, 0, 100, 100, nullptr };
    const auto child = new TMDIChild{ GetMDIClient(), _T("File"), client};
    SetupChildWindow(child);
    child->Create();
  }

  void CmNewCoolGridWindow()
  {
    const auto client = new TCoolDemoGrid{ nullptr, 0, nullptr, 0, 0, 100, 100 };
    const auto child = new TMDIChild{ GetMDIClient(), _T("Grid"), client };
    SetupChildWindow(child);
    child->Create();
  }

  void CmFileOpen()
  {
    *(FileData.FileName) = '\0';
    TFileOpenDialog dlg{ GetMainWindow(), FileData};
    const auto r = dlg.Execute();
    if (CommDlgExtendedError() == FNERR_BUFFERTOOSMALL)
    {
      GetMainWindow()->MessageBox
      (
        _T("Too many files were selected.\n\nOpen a lower number of files at a time."),
        _T("File Open Failure"), MB_OK | MB_ICONEXCLAMATION
      );
      return;
    }
    if (r == IDOK)
      for (const auto& f : FileData.GetFileNames())
        OpenFile(f);
  }

  auto CmFileSelected(TParam1 wp, TParam2) -> TResult
  {
    OpenFile(GetMenuText(static_cast<int>(wp)));
    return 0;
  }

  void CmCoolEditBenchmark()
  {
    static const auto fileNameKey = _T("CoolEdit Benchmark File");

    class TBenchmarkDialog : public TTransferDialog<tstring>
    {
    public:

      TBenchmarkDialog(TWindow* parent, tstring& fileName, filesystem::path iniFileName)
        : TTransferDialog{parent, IDD_COOLEDIT_BENCHMARK, fileName},
        FileNameCombo{this, std::move(iniFileName)}
      {}

      auto Find(TEventInfo& eventInfo, TEqualOperator op) -> bool override // TEventHandler
      {
        // Implement and search response table.
        //
        eventInfo.Object = this; // Important for correct dispatch.
        using TMyClass = TBenchmarkDialog; // Alias used by response table macros.
        using T = TResponseTableEntry;
        static const auto responseTable =
        {
          T EV_BN_CLICKED(IDC_COOLEDIT_BENCHMARK_BROWSE, BnBrowse),
          T{} // Sentinel is required.
        };
        return SearchEntries(responseTable.begin(), eventInfo, op) || TTransferDialog::Find(eventInfo, op);
      }

    protected:

      class TFilenameComboBox : public TMemComboBox
      {
      public:

        TFilenameComboBox(TWindow* parent, filesystem::path iniFileName) : 
          TMemComboBox{parent, IDC_COOLEDIT_BENCHMARK_FILENAME, fileNameKey},
          IniFileName{std::move(iniFileName)}
        {}

      protected:

        filesystem::path IniFileName;

        auto CreateConfigFile() -> TConfigFile* override // TMemComboBox
        {

#if defined(UNICODE)

          return new TIniConfigFile{IniFileName.wstring()};

#else

          return new TIniConfigFile{IniFileName.string()};

#endif

        }

      };

      TFilenameComboBox FileNameCombo;

      void DoTransferData(const TTransferInfo& i, tstring& filename) override // TTransferDialog
      {
        if (i.Operation != tdSetData || FileNameCombo.GetText().empty())
          TransferEditData(i, IDC_COOLEDIT_BENCHMARK_FILENAME, filename);
      }

      void BnBrowse()
      {
        auto data = TOpenSaveDialog::TData{0, _T("All Files (*.*)|*.*|")};
        const auto ok = TFileOpenDialog{this, data, 0, _T("Browse for text file")}.Execute();
        if (ok != IDOK) return;
        FileNameCombo.SetText(data.FileName);
      }

    };

    auto fileName = _T(R"(..\..\..\source\coolprj\cooledit.cpp)"s); // Default.
    const auto ok = TBenchmarkDialog{GetMainWindow(), fileName, GetIniFileName()}.Execute();
    if (ok != IDOK) return;

    TRACE(_T("Starting CoolEdit Benchmark."));

    using TClock = chrono::high_resolution_clock;
    const auto startTime = TClock::now();

    auto& mdi = GetMDIClient();
    const auto c = OpenFile(fileName);
    mdi.GetActiveMDIChild()->ShowWindow(SW_MAXIMIZE);

    const auto openFinishTime = TClock::now();

    c->MoveCtrlEnd();

    const auto moveCtrlEndFinishTime = TClock::now();

    c->SetSelection({{0, 0}, c->GetCursorPos()});
    c->SendMessage(WM_COMMAND, CM_EDITCOPY);
    c->SetSelection({c->GetCursorPos(), c->GetCursorPos()});

    const auto copyFinishTime = TClock::now();

    c->Paste();

    const auto pasteFinishTime = TClock::now();

    while (c->GetCursorPos() != TEditPos{0, 0})
      c->MovePgUp();

    const auto pgUpToTopFinishTime = TClock::now();

    c->SendMessage(WM_COMMAND, CM_EDITUNDO);

    const auto undoFinishTime = TClock::now();

    using TResolution = chrono::milliseconds;
    const auto unit = _T(" ms");
    const auto openTimeTaken = chrono::duration_cast<TResolution>(openFinishTime - startTime);
    const auto moveCtrlEndTimeTaken = chrono::duration_cast<TResolution>(moveCtrlEndFinishTime - openFinishTime);
    const auto copyTimeTaken = chrono::duration_cast<TResolution>(copyFinishTime - moveCtrlEndFinishTime);
    const auto pasteTimeTaken = chrono::duration_cast<TResolution>(pasteFinishTime - copyFinishTime);
    const auto pgUpToTopTimeTaken = chrono::duration_cast<TResolution>(pgUpToTopFinishTime - pasteFinishTime);
    const auto undoTimeTaken = chrono::duration_cast<TResolution>(undoFinishTime - pgUpToTopFinishTime);
    const auto totalTimeTaken = chrono::duration_cast<TResolution>(undoFinishTime - startTime);

    TRACE(_T("Finished CoolEdit Benchmark."));

    auto s = tostringstream{};
    s << _T("CoolEdit Benchmark Results:\n\n")
      << _T("File: ") << fileName << _T("\n\n")
      << _T("Opening the file took ") << openTimeTaken.count() << unit << _T(".\n") 
      << _T("Jumping to the last line took ") << moveCtrlEndTimeTaken.count() << unit << _T(".\n") 
      << _T("Selecting and copying everything took ") << copyTimeTaken.count() << unit << _T(".\n") 
      << _T("Pasting everything took ") << pasteTimeTaken.count() << unit << _T(".\n") 
      << _T("Paging up to the top took ") << pgUpToTopTimeTaken.count() << unit << _T(".\n")
      << _T("Undo took ") << undoTimeTaken.count() << unit << _T(".\n\n")
      << _T("The whole benchmark took ") << totalTimeTaken.count() << unit << _T(".");

    // Copy report to clipboard.
    //
    const auto report = s.str();
    {
      const auto bufSize = (report.size() + 1) * sizeof(tchar);
      using THandle = unique_ptr<void, decltype(&GlobalFree)>;
      auto h = THandle{GlobalAlloc(GMEM_MOVEABLE, bufSize), &GlobalFree};
      if (!h) throw runtime_error{"GlobalAlloc failed"};

      const auto buf = GlobalLock(h.get()); CHECK(buf);
      using TLock = unique_ptr<void, decltype(&GlobalUnlock)>;
      auto lock = TLock{h.get(), &GlobalUnlock};
      memcpy_s(buf, bufSize, report.c_str(), bufSize);
      lock.reset(); // Calls GlobalUnlock.

      const auto f = sizeof(tchar) > 1 ? CF_UNICODETEXT : CF_TEXT;
      auto c = TClipboard{mdi, true};
      c.EmptyClipboard();
      c.SetClipboardData(f, h.get()); 
      h.release(); // SetClipboardData took ownership of the handle.
      c.CloseClipboard();
    }
    s << _T("\n\nThis report has been copied to the clipboard.");

    mdi.MessageBox(s.str(), _T("CoolEdit Benchmark"), MB_ICONINFORMATION);
  }

  void CmHelpAbout()
  {
    auto v = TModuleVersionInfo{GetHandle()};
    const auto caption = tstring{_T("About ")} + GetName();
    auto s = tostringstream{};
    s << v.GetProductName() << _T(' ') << GetName()
      << _T("\nVersion ") << v.GetFileVersion();
    if (__DEBUG + 0)
      s << _T("-debug");
    if (OWL_PRERELEASE)
      s << _T("-prerelease");
    s << _T("\n\n") << v.GetLegalCopyright() <<
      _T("\nAll rights reserved.\n\n")
      _T("Demonstration of TCoolEdit and TCoolGrid.");
    GetMainWindow()->MessageBoxIndirect(TResId{IDI_OWLAPP}, s.str(), caption, MB_USERICON);
  }


  auto OpenFile(const owl::tstring& fileName) -> TCoolDemoEdit*
  {
    PRECONDITION(filesystem::exists(fileName));

    auto findOpenEdit = [&](LPCTSTR fileName) -> TMDIChild*
    {
      if (!fileName) return nullptr;
      for (auto& w : GetMDIClient().GetChildren())
        if (const auto child = dynamic_cast<TMDIChild*>(&w))
          if (const auto edit = dynamic_cast<TCoolDemoEdit*>(child->GetClientWindow()))
            if (edit->IsWindow())
              if (const auto editFileName = edit->GetFileName())
                if (_tcsicmp(fileName, editFileName) == 0)
                  return child;
      return nullptr;
    };

    auto getExistingEdit = [](TMDIChild* child)
    {
      child->BringWindowToTop();
      return dynamic_cast<TCoolDemoEdit*>(child->GetClientWindow());
    };

    auto createEdit = [this](LPTSTR fileName)
    {
      const auto c = new TCoolDemoEdit{ nullptr, 0, nullptr, 0, 0, 100, 100, fileName };
      const auto child = new TMDIChild{ GetMDIClient(), _T("File"), c};
      SetupChildWindow(child);
      child->Create();
      return c;
    };

    _tcscpy_s(FileData.FileName, _MAX_PATH, fileName.c_str());
    const auto child = findOpenEdit(FileData.FileName);
    const auto client = child ? getExistingEdit(child) : createEdit(FileData.FileName); CHECK(client);
    return client;
  }


  void SetupChildWindow(TMDIChild* child)
  {
    // Assign icons for this child window.
    //
    //child->SetIcon(GetApplication(), IDI_DOC);
    //child->SetIconSm(GetApplication(), IDI_DOC);

    // If the current active MDI child is maximize then this one should be also.
    //
    const auto curChild = GetMDIClient().GetActiveMDIChild();
    if (curChild && (curChild->GetWindowLong(GWL_STYLE) & WS_MAXIMIZE))
      child->GetWindowAttr().Style |= WS_MAXIMIZE;
  }

private:
  TOpenSaveDialog::TData FileData;

  DECLARE_RESPONSE_TABLE(TCoolDemoApp);
};

DEFINE_RESPONSE_TABLE2(TCoolDemoApp, TRecentFiles, TApplication)
  EV_COMMAND(CM_FILE_NEWCOOLEDITWINDOW, CmNewCoolEditWindow),
  EV_COMMAND(CM_FILE_NEWCOOLGRIDWINDOW, CmNewCoolGridWindow),
  EV_COMMAND(CM_TOOLS_COOLEDITBENCHMARK, CmCoolEditBenchmark),
  EV_COMMAND(CM_FILEOPEN, CmFileOpen),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  EV_REGISTERED(MruFileMessage, CmFileSelected),
END_RESPONSE_TABLE;

auto OwlMain(int, LPTSTR []) -> int // argc, argv
{
  return TCoolDemoApp().Run();
}
