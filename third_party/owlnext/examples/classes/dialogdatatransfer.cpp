//
// Dialog Data Transfer Demo
// This code demonstrates different mechanisms and programming styles for transferring data to and from controls.
// A simple search dialog is used as a demonstration case. There are two accompanying articles based on this code:
//
//   "Safe Transfer Buffers", http://sourceforge.net/apps/mediawiki/owlnext/index.php?title=Safe_Transfer_Buffers
//   "Dialog Data Transfer", http://sourceforge.net/apps/mediawiki/owlnext/index.php?title=Dialog_Data_Transfer
//
// Copyright (c) 2010 Vidar Hasfjord 
// Distributed under the OWLNext License (see http://owlnext.sourceforge.net).

#if defined(__BORLANDC__)
# pragma option -w-ccc // Disable "Condition is always true/false".
# pragma option -w-inl // Disable warning "Functions containing 'statement' is not expanded inline".
#endif

#include "pch.h"

#include <owl/pch.h>

#include <owl/transfer.h>
#include <owl/transferbuffer.h>
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/checkbox.h>
#include <vector>

#include "client.h"

#include "resource.h"
#include "dialogdatatransfer.rh"

using namespace owl;
using namespace std;

//
// Dialog Data Transfer Demo application
//
class TDialogDataTransferDemo
  : public TEdit
{
public:

  TDialogDataTransferDemo(TWindow* parent, int id, LPCTSTR text, int x, int y, int w, int h,
    uint textLimit = 0, bool multiline = false, TModule* module = 0)
    : TEdit(parent, id, text, x, y, w, h, textLimit, multiline, module)
  {

  }


  //
  // Argument pack for our demo function - Search
  //
  struct TSearchArguments
  {
    tstring SearchString;
    uint Flags;
    enum TFlag
    {
      CaseSensitive = 1,
      WholeWord = 2,
      SearchUp = 4
    };

    TSearchArguments() : SearchString(), Flags(0) {}
    TSearchArguments(const tstring& s, uint f) : SearchString(s), Flags(f) {}

    //
    // Getters
    //
    tstring GetSearchString() const { return SearchString; }
    bool IsCaseSensitive() const { return Flags & CaseSensitive; }
    bool IsWholeWord() const { return Flags & WholeWord; }
    bool IsSearchUp() const { return Flags & SearchUp; }
    bool GetOption(TFlag f) const { return Flags & f; }

    // 
    // Setters
    //
    void SetSearchString(const tstring& s) { SearchString = s; }
    void SetCaseSensitive(bool enable) { SetOption(CaseSensitive, enable); }
    void SetWholeWord(bool enable) { SetOption(WholeWord, enable); }
    void SetSearchUp(bool enable) { SetOption(SearchUp, enable); }
    void SetOption(TFlag f, bool enable) { enable ? (Flags |= f) : (Flags &= ~f); }
  };

  //
  // Demo function
  //
  int DoSearch(TSearchArguments a)
  {
    TRACE(_T("Search string: ") << a.SearchString << _T(", Flags: ") << a.Flags);
    return Search(GetCurrentPosition(),
      a.SearchString,
      a.Flags & a.CaseSensitive,
      a.Flags & a.WholeWord,
      a.Flags & a.SearchUp);
  }

protected:

  //
  // Command handlers
  //
  void CmSearch1();
  void CmSearch2();
  void CmSearch3();
  void CmSearch4();
  void CmSearch5();

  DECLARE_RESPONSE_TABLE(TDialogDataTransferDemo);
};


DEFINE_RESPONSE_TABLE1(TDialogDataTransferDemo, TEdit)
EV_COMMAND(CM_SEARCH1, CmSearch1),
EV_COMMAND(CM_SEARCH2, CmSearch2),
EV_COMMAND(CM_SEARCH3, CmSearch3),
EV_COMMAND(CM_SEARCH4, CmSearch4),
EV_COMMAND(CM_SEARCH5, CmSearch5),
END_RESPONSE_TABLE;

//
// Convenience
//
typedef TDialogDataTransferDemo::TSearchArguments TSearchArguments;

//
// Application-specific DDT helper
//
void TransferFlag(const TTransferInfo& i, HWND ctrl, uint& flags, TSearchArguments::TFlag flag)
{
  if (i.Operation == tdSetData)
    CheckDlgButton(ctrl, (flags & flag) != 0);
  else if (i.Operation == tdGetData)
    flags = (flags & ~flag) | (IsChecked(ctrl) ? flag : 0);
}

//
// Application-specific DDT helper; overload
//
void TransferFlag(const TTransferInfo& i, int ctrl, uint& flags, TSearchArguments::TFlag flag)
{
  TransferFlag(i, GetDlgItem(i.Window, ctrl), flags, flag);
}

//-----------------------------------------------------------------------------

//
// Traditional OWL transfer buffer (unsafe and not recommended)
// NB! Byte packing is required!
//
#include <pshpack1.h>
struct TSearchDialog1Buffer
{
  enum { SearchStringSize = 7 }; // Artificially limited to demonstrate safety issues.
  _TCHAR SearchString[SearchStringSize]; // TEdit data
  WORD CaseSensitive; // TCheckBox data
  WORD WholeWord; // TCheckBox data
  WORD SearchUp; // TCheckBox data
};
#include <poppack.h>

//
// Search dialog using OWL transfer buffers
// Note that the size argument to SetTransferBuffer is only needed for BC++ 5.02.
// Standard-compliant compilers will automatically deduce the size from the first argument.
// The second argument was introduced as a safety measure in OWLNext 6.32. You can compile in
// OWL 5 compatibility mode to avoid it, but note that doing so will deactivate the new buffer
// size check, and is thus not recommended.
//
class TSearchDialog1
  : public TDialog
{
public:

  typedef TSearchDialog1Buffer TTransferBuffer;

  TSearchDialog1(TWindow* parent, TTransferBuffer& b)
    : TDialog(parent, IDD_SEARCH_DIALOG)
  {
    new TEdit(this, IDC_SEARCH_STRING, TTransferBuffer::SearchStringSize);
    new TCheckBox(this, IDC_CASE_SENSITIVE);
    new TCheckBox(this, IDC_WHOLE_WORD);
    new TCheckBox(this, IDC_SEARCH_UP);
    SetTransferBuffer(&b, sizeof(TTransferBuffer));
  }

};

//
// Test OWL transfer buffers.
// Notice the error-prone transfer between application data and transfer buffer.
// Text fields (char arrays) in particular need attention to avoid buffer overruns.
//
void TDialogDataTransferDemo::CmSearch1()
{
  static TSearchArguments a;

  TSearchDialog1::TTransferBuffer b;
  b.SearchString[0] = _T('\0');
#if defined(__BORLANDC__)
  _tcsncat(b.SearchString, a.SearchString.c_str(), b.SearchStringSize - 1); // tstring -> char[]
#else
  _tcsncat_s(b.SearchString, a.SearchString.c_str(), b.SearchStringSize - 1); // tstring -> char[]
#endif
  WARN(b.SearchStringSize <= a.SearchString.length(), _T("The search string was truncated!"));

  b.CaseSensitive = (a.Flags & a.CaseSensitive) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD
  b.WholeWord = (a.Flags & a.WholeWord) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD
  b.SearchUp = (a.Flags & a.SearchUp) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD

  TSearchDialog1 dlg(this, b);
  dlg.SetCaption(_T("Search1 (OWL transfer buffer)"));
  if (dlg.Execute() == IDOK)
  {
    a.SearchString = b.SearchString; // tstring <- char[]
    a.Flags = ((b.CaseSensitive == BST_CHECKED) ? a.CaseSensitive : 0) | // TFlag <- WORD
      ((b.WholeWord == BST_CHECKED) ? a.WholeWord : 0) | // TFlag <- WORD
      ((b.SearchUp == BST_CHECKED) ? a.SearchUp : 0); // TFlag <- WORD
    DoSearch(a);
  }
}


//-----------------------------------------------------------------------------

#define BIND_EXISTING 0
#define BIND_AND_INITIALIZE 1
#define BIND_ANONYMOUS 2
#define SAFE_TRANSFER_BUFFER_DEMO_OPTION BIND_EXISTING // BIND_EXISTING | BIND_AND_INITIALIZE | BIND_ANONYMOUS

//
// Safe transfer buffer
// Any alignment and packing is supported.
//
struct TSearchDialog2Buffer
{
  tstring SearchString; // TEdit data
  WORD CaseSensitive; // TCheckBox data
  WORD WholeWord; // TCheckBox data
  WORD SearchUp; // TCheckBox data
};

//
// Search dialog using safe transfer buffer
//
class TSearchDialog2
  : public TDialog,
  virtual public TTransferBufferWindow<TSearchDialog2Buffer>
{
public:

#if SAFE_TRANSFER_BUFFER_DEMO_OPTION == BIND_EXISTING

  //
  // Bind controls after creation.
  // TTransferBuffer is a typedef provided by the base class.
  //
  TSearchDialog2(TWindow* parent, TTransferBuffer& b)
    : TDialog(parent, IDD_SEARCH_DIALOG),
    SearchStringEdit(this, IDC_SEARCH_STRING),
    CaseSensitiveCheckBox(this, IDC_CASE_SENSITIVE),
    WholeWordCheckBox(this, IDC_WHOLE_WORD),
    SearchUpCheckBox(this, IDC_SEARCH_UP)
  {
    Bind(&TTransferBuffer::SearchString, SearchStringEdit);
    Bind(&TTransferBuffer::CaseSensitive, CaseSensitiveCheckBox);
    Bind(&TTransferBuffer::WholeWord, WholeWordCheckBox);
    Bind(&TTransferBuffer::SearchUp, SearchUpCheckBox);
    SetTransferBuffer(&b);
  }

protected:

  TEdit SearchStringEdit;
  TCheckBox CaseSensitiveCheckBox;
  TCheckBox WholeWordCheckBox;
  TCheckBox SearchUpCheckBox;

#elif SAFE_TRANSFER_BUFFER_DEMO_OPTION == BIND_AND_INITIALIZE

  // Initialize named references to controls created by binders.
  // TTransferBuffer is a typedef provided by the base class.
  // Note that this option fails for Borland C++ 5.02. See the BIND_EXISTING option above for an
  // alternative that is compatible with Borland C++ 5.02.
  //
  TSearchDialog2(TWindow* parent, TTransferBuffer& b)
    : TDialog(parent, IDD_SEARCH_DIALOG),
    SearchStringEdit(Bind<TEdit>(&TTransferBuffer::SearchString, IDC_SEARCH_STRING)),
    CaseSensitiveCheckBox(Bind<TCheckBox>(&TTransferBuffer::CaseSensitive, IDC_CASE_SENSITIVE)),
    WholeWordCheckBox(Bind<TCheckBox>(&TTransferBuffer::WholeWord, IDC_WHOLE_WORD)),
    SearchUpCheckBox(Bind<TCheckBox>(&TTransferBuffer::SearchUp, IDC_SEARCH_UP))
  {
    SetTransferBuffer(&b);
  }

protected:

  TEdit& SearchStringEdit;
  TCheckBox& CaseSensitiveCheckBox;
  TCheckBox& WholeWordCheckBox;
  TCheckBox& SearchUpCheckBox;

#elif SAFE_TRANSFER_BUFFER_DEMO_OPTION == BIND_ANONYMOUS

  //
  // Use binders to create anonymous controls.
  // TTransferBuffer is a typedef provided by the base class.
  // Note that this option fails for Borland C++ 5.02. See the BIND_EXISTING option above for an
  // alternative that is compatible with Borland C++ 5.02.
  //
  TSearchDialog2(TWindow* parent, TTransferBuffer& b)
    : TDialog(parent, IDD_SEARCH_DIALOG)
  {
    Bind<TEdit>(&TTransferBuffer::SearchString, IDC_SEARCH_STRING, 0);
    Bind<TCheckBox>(&TTransferBuffer::CaseSensitive, IDC_CASE_SENSITIVE);
    Bind<TCheckBox>(&TTransferBuffer::WholeWord, IDC_WHOLE_WORD);
    Bind<TCheckBox>(&TTransferBuffer::SearchUp, IDC_SEARCH_UP);
    SetTransferBuffer(&b); // type-safe overload
  }

#endif

};

//
// Test safe transfer buffers.
// Note how much easier and safer it is to handle strings in the buffer compared to arrays.
//
void TDialogDataTransferDemo::CmSearch2()
{
  static TSearchArguments a;

  TSearchDialog2::TTransferBuffer b;
  b.SearchString = a.SearchString; // tstring -> tstring // Nice!
  b.CaseSensitive = (a.Flags & a.CaseSensitive) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD
  b.WholeWord = (a.Flags & a.WholeWord) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD
  b.SearchUp = (a.Flags & a.SearchUp) ? BST_CHECKED : BST_UNCHECKED; // TFlag -> WORD

  TSearchDialog2 dlg(this, b);
  dlg.SetCaption(_T("Search2 (safe transfer buffer)"));
  if (dlg.Execute() == IDOK)
  {
    a.SearchString = b.SearchString;
    a.Flags = ((b.CaseSensitive == BST_CHECKED) ? a.CaseSensitive : 0) | // TFlag <- WORD
      ((b.WholeWord == BST_CHECKED) ? a.WholeWord : 0) | // TFlag <- WORD
      ((b.SearchUp == BST_CHECKED) ? a.SearchUp : 0); // TFlag <- WORD
    DoSearch(a);
  }
}

//-----------------------------------------------------------------------------

#define USE_ORTHODOX_IDIOM 0
#define USE_TRANSFER_MECHANISM 1
#define MANUAL_DEMO_OPTION USE_ORTHODOX_IDIOM // USE_ORTHODOX_IDIOM | USE_TRANSFER_MECHANISM

//
// Search dialog using application data source and traditional manual transfer
//
class TSearchDialog3
  : public TDialog
{
public:

  TSearchDialog3(TWindow* parent, TSearchArguments& a)
    : TDialog(parent, IDD_SEARCH_DIALOG),
    arguments(a)
  {}

protected:

  TSearchArguments& arguments;

#if MANUAL_DEMO_OPTION == USE_ORTHODOX_IDIOM

  virtual void SetupWindow()
  {
    TDialog::SetupWindow();
    SetDlgItemText(IDC_SEARCH_STRING, arguments.SearchString);
    CheckDlgButton(IDC_CASE_SENSITIVE, arguments.Flags & arguments.CaseSensitive ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_WHOLE_WORD, arguments.Flags & arguments.WholeWord ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(IDC_SEARCH_UP, arguments.Flags & arguments.SearchUp ? BST_CHECKED : BST_UNCHECKED);
  }

  void CmOk()
  {
    arguments.SearchString = GetDlgItemText(IDC_SEARCH_STRING);
    arguments.Flags = (IsDlgButtonChecked(IDC_CASE_SENSITIVE) == BST_CHECKED ? arguments.CaseSensitive : 0) |
      (IsDlgButtonChecked(IDC_WHOLE_WORD) == BST_CHECKED ? arguments.WholeWord : 0) |
      (IsDlgButtonChecked(IDC_SEARCH_UP) == BST_CHECKED ? arguments.SearchUp : 0);
    TDialog::CmOk();
  }

#elif MANUAL_DEMO_OPTION == USE_TRANSFER_MECHANISM

  //
  // Hijack the transfer buffer mechanism.
  //
  virtual void TransferData(TTransferDirection d)
  {
    if (d == tdSetData)
    {
      SetDlgItemText(IDC_SEARCH_STRING, arguments.SearchString);
      CheckDlgButton(IDC_CASE_SENSITIVE, arguments.Flags & arguments.CaseSensitive ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(IDC_WHOLE_WORD, arguments.Flags & arguments.WholeWord ? BST_CHECKED : BST_UNCHECKED);
      CheckDlgButton(IDC_SEARCH_UP, arguments.Flags & arguments.SearchUp ? BST_CHECKED : BST_UNCHECKED);
    }
    else if (d == tdGetData)
    {
      arguments.SearchString = GetDlgItemText(IDC_SEARCH_STRING);
      arguments.Flags = (IsDlgButtonChecked(IDC_CASE_SENSITIVE) == BST_CHECKED ? arguments.CaseSensitive : 0) |
        (IsDlgButtonChecked(IDC_WHOLE_WORD) == BST_CHECKED ? arguments.WholeWord : 0) |
        (IsDlgButtonChecked(IDC_SEARCH_UP) == BST_CHECKED ? arguments.SearchUp : 0);
    }
  }

#endif

  DECLARE_RESPONSE_TABLE(TSearchDialog3);
};


DEFINE_RESPONSE_TABLE1(TSearchDialog3, TDialog)
EV_COMMAND(IDOK, CmOk),
END_RESPONSE_TABLE;

//
// Test tradition manual transfer.
//
void TDialogDataTransferDemo::CmSearch3()
{
  static TSearchArguments a;

  TSearchDialog3 dlg(this, a);
  dlg.SetCaption(_T("Search3 (traditional manual transfer)"));
  if (dlg.Execute() == IDOK)
    DoSearch(a);
}

//-----------------------------------------------------------------------------

#define DDT_DEMO_OPTION_USE_CHILD_ID 0
#define DDT_DEMO_OPTION_USE_INSTANCES 1
#define DDT_DEMO_OPTION_USE_MEMBER_FUNCTIONS 2
#define DDT_DEMO_OPTION_USE_FUNCTORS 3
#define DDT_DEMO_OPTION_USE_LAMBDAS 4
#define DDT_DEMO_OPTION_USE_BIND 5
#define DDT_DEMO_OPTION_USE_DDX 6
#define DDT_DEMO_OPTION_USE_SEPARATE_SET_AND_GET 7
#define DDT_DEMO_OPTION DDT_DEMO_OPTION_USE_CHILD_ID // Select option from above.

typedef TTransferDialog<TSearchArguments, TDialog> TSearchDialog4Base;

//
// Dialog using the Dialog Data Transfer framework
//
class TSearchDialog4
  : public TSearchDialog4Base
{
public:

#if DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_CHILD_ID 

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    TransferEditData(i, IDC_SEARCH_STRING, a.SearchString);
    TransferFlag(i, IDC_CASE_SENSITIVE, a.Flags, a.CaseSensitive);
    TransferFlag(i, IDC_WHOLE_WORD, a.Flags, a.WholeWord);
    TransferFlag(i, IDC_SEARCH_UP, a.Flags, a.SearchUp);
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_INSTANCES 

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a),
    SearchString(this, IDC_SEARCH_STRING),
    CaseSensitive(this, IDC_CASE_SENSITIVE),
    WholeWord(this, IDC_WHOLE_WORD),
    SearchUp(this, IDC_SEARCH_UP)
  {}

protected:

  TEdit SearchString;
  TCheckBox CaseSensitive;
  TCheckBox WholeWord;
  TCheckBox SearchUp;

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    TransferEditData(i, SearchString, a.SearchString);
    TransferFlag(i, CaseSensitive, a.Flags, a.CaseSensitive);
    TransferFlag(i, WholeWord, a.Flags, a.WholeWord);
    TransferFlag(i, SearchUp, a.Flags, a.SearchUp);
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_MEMBER_FUNCTIONS

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    typedef TSearchArguments D;
    TransferEditData(i, IDC_SEARCH_STRING, &a, &D::GetSearchString, &D::SetSearchString);
    TransferCheckBoxData(i, IDC_CASE_SENSITIVE, &a, &D::IsCaseSensitive, &D::SetCaseSensitive);
    TransferCheckBoxData(i, IDC_WHOLE_WORD, &a, &D::IsWholeWord, &D::SetWholeWord);
    TransferCheckBoxData(i, IDC_SEARCH_UP, &a, &D::IsSearchUp, &D::SetSearchUp);
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_FUNCTORS

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  struct TOptionProxy
  {
    TSearchArguments& a;
    TSearchArguments::TFlag flag;
    bool Get() const { return a.GetOption(flag); }
    void Set(bool s) { a.SetOption(flag, s); }
  };

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    typedef TSearchArguments D;
    TransferEditData(i, IDC_SEARCH_STRING, &a, &D::GetSearchString, &D::SetSearchString);

    TOptionProxy caseSensitive = { a, a.CaseSensitive };
    TransferCheckBoxData(i, IDC_CASE_SENSITIVE, &caseSensitive, &TOptionProxy::Get, &TOptionProxy::Set);

    TOptionProxy wholeWord = { a, a.WholeWord };
    TransferCheckBoxData(i, IDC_WHOLE_WORD, &wholeWord, &TOptionProxy::Get, &TOptionProxy::Set);

    TOptionProxy searchUp = { a, a.SearchUp };
    TransferCheckBoxData(i, IDC_SEARCH_UP, &searchUp, &TOptionProxy::Get, &TOptionProxy::Set);
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_LAMBDAS // C++0x

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    TransferEditData(i, IDC_SEARCH_STRING,
      [&] {return a.GetSearchString(); },
      [&](string s) {a.SetSearchString(s); }
    );
    TransferCheckBoxData(i, IDC_CASE_SENSITIVE,
      [&] {return a.GetOption(a.CaseSensitive); },
      [&](bool s) {a.SetOption(a.CaseSensitive, s); }
    );
    TransferCheckBoxData(i, IDC_WHOLE_WORD,
      [&] {return a.GetOption(a.WholeWord); },
      [&](bool s) {a.SetOption(a.WholeWord, s); }
    );
    TransferCheckBoxData(i, IDC_SEARCH_UP,
      [&] {return a.GetOption(a.SearchUp); },
      [&](bool s) {a.SetOption(a.SearchUp, s); }
    );
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_BIND

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    using std::placeholders::_1;
    typedef TSearchArguments D;

    TransferEditData(i, IDC_SEARCH_STRING, 
      bind(&D::GetSearchString, &a),
      bind(&D::SetSearchString, &a, _1)
    );
    TransferCheckBoxData(i, IDC_CASE_SENSITIVE, 
      bind(&D::GetOption, &a, a.CaseSensitive), 
      bind(&D::SetOption, &a, a.CaseSensitive, _1)
    );
    TransferCheckBoxData(i, IDC_WHOLE_WORD, 
      bind(&D::GetOption, &a, a.WholeWord), 
      bind(&D::SetOption, &a, a.WholeWord, _1)
    );
    TransferCheckBoxData(i, IDC_SEARCH_UP, 
      bind(&D::GetOption, &a, a.SearchUp), 
      bind(&D::SetOption, &a, a.SearchUp, _1)
    );
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_DDX

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  virtual void DoTransferData(const TTransferInfo& i, TSearchArguments& a)
  {
    UINT caseSensitive = (a.Flags & a.CaseSensitive) ? BST_CHECKED : BST_UNCHECKED;
    UINT wholeWord = (a.Flags & a.WholeWord) ? BST_CHECKED : BST_UNCHECKED;
    UINT searchUp = (a.Flags & a.SearchUp) ? BST_CHECKED : BST_UNCHECKED;

    DDX_Text(i, IDC_SEARCH_STRING, a.SearchString);
    DDX_Check(i, IDC_CASE_SENSITIVE, caseSensitive);
    DDX_Check(i, IDC_WHOLE_WORD, wholeWord);
    DDX_Check(i, IDC_SEARCH_UP, searchUp);

    a.Flags = (caseSensitive == BST_CHECKED ? a.CaseSensitive : 0) |
      (wholeWord == BST_CHECKED ? a.WholeWord : 0) |
      (searchUp == BST_CHECKED ? a.SearchUp : 0);
  }

#elif DDT_DEMO_OPTION == DDT_DEMO_OPTION_USE_SEPARATE_SET_AND_GET

  TSearchDialog4(TWindow* parent, TSearchArguments& a)
    : TSearchDialog4Base(parent, IDD_SEARCH_DIALOG, a)
  {}

protected:

  virtual void DoGetData(const TTransferInfo&, TSearchArguments& a) const
  {
    a.SearchString = GetDlgItemText(IDC_SEARCH_STRING);
    a.Flags = (IsChecked(IDC_CASE_SENSITIVE) ? a.CaseSensitive : 0) |
      (IsChecked(IDC_WHOLE_WORD) ? a.WholeWord : 0) |
      (IsChecked(IDC_SEARCH_UP) ? a.SearchUp : 0);
  }

  virtual void DoSetData(const TTransferInfo&, const TSearchArguments& a)
  {
    SetDlgItemText(IDC_SEARCH_STRING, a.SearchString);
    CheckDlgButton(IDC_CASE_SENSITIVE, a.Flags & a.CaseSensitive);
    CheckDlgButton(IDC_WHOLE_WORD, a.Flags & a.WholeWord);
    CheckDlgButton(IDC_SEARCH_UP, a.Flags & a.SearchUp);
  }

#endif

protected:

  // Provide presets.

  void EvContextMenu(HWND, int x, int y)
  {
    static std::vector<TSearchArguments> presets;

    TPopupMenu menu;
    for (unsigned i = 0; i != presets.size(); ++i)
      menu.AppendMenu(MF_STRING, i + 1, presets[i].SearchString);
    if (!presets.empty())
      menu.AppendMenu(MF_MENUBREAK);
    auto n = static_cast<TMenuItem>(presets.size() + 1);
    menu.AppendMenu(MF_STRING, n, _T("Add preset"));

    int s = menu.TrackPopupMenu(TPM_RETURNCMD, x, y, 0, GetHandle());
    if (s > static_cast<int>(presets.size()))
      presets.push_back(GetData()); // Save preset.
    else if (s > 0)
      SetData(presets[s - 1]); // Use preset.
  }

  DECLARE_RESPONSE_TABLE(TSearchDialog4);
};

DEFINE_RESPONSE_TABLE1(TSearchDialog4, TSearchDialog4Base)
EV_WM_CONTEXTMENU,
END_RESPONSE_TABLE;

//
// Test the Dialog Data Transfer framework.
//
void TDialogDataTransferDemo::CmSearch4()
{
  static TSearchArguments a;

  TSearchDialog4 dlg(this, a);
  dlg.SetCaption(_T("Search4 (dialog data transfer)"));
  if (dlg.Execute() == IDOK)
    DoSearch(a);
}

//-----------------------------------------------------------------------------

#define DELEGATE_DEMO_OPTION_USE_BIND 0
#define DELEGATE_DEMO_OPTION_USE_FUNCTOR 1
#define DELEGATE_DEMO_OPTION_USE_LAMBDA 2
#define DELEGATE_DEMO_OPTION DELEGATE_DEMO_OPTION_USE_BIND // Select option from above.

#if DELEGATE_DEMO_OPTION == DELEGATE_DEMO_OPTION_USE_BIND 

namespace // Delegate function for search arguments transfer
{

  void TransferSearchArguments(const TTransferInfo& i, TSearchArguments& a)
  {
    TransferEditData(i, IDC_SEARCH_STRING, a.SearchString);
    TransferFlag(i, IDC_CASE_SENSITIVE, a.Flags, a.CaseSensitive);
    TransferFlag(i, IDC_WHOLE_WORD, a.Flags, a.WholeWord);
    TransferFlag(i, IDC_SEARCH_UP, a.Flags, a.SearchUp);
  }

}

//
// Test delegating transfer dialog.
// Use std::bind to create a delegate from a free function.
//
void TDialogDataTransferDemo::CmSearch5()
{
  static TSearchArguments a;

  using std::placeholders::_1;
  TDelegatedTransferDialog::TTransferFunction f = bind(&TransferSearchArguments, _1, ref(a));
  TDelegatedTransferDialog dlg(this, IDD_SEARCH_DIALOG, f);
  dlg.SetCaption(_T("Search5 (delegated transfer)"));
  if (dlg.Execute() == IDOK)
    DoSearch(a);
}

#elif DELEGATE_DEMO_OPTION == DELEGATE_DEMO_OPTION_USE_FUNCTOR

//
// Test delegating transfer dialog.
//
void TDialogDataTransferDemo::CmSearch5()
{
  static TSearchArguments a;

  struct TTransferFunction
  {
    TSearchArguments& a;

    void operator()(const TTransferInfo& i)
    {
      TransferEditData(i, IDC_SEARCH_STRING, a.SearchString);
      TransferFlag(i, IDC_CASE_SENSITIVE, a.Flags, a.CaseSensitive);
      TransferFlag(i, IDC_WHOLE_WORD, a.Flags, a.WholeWord);
      TransferFlag(i, IDC_SEARCH_UP, a.Flags, a.SearchUp);
    }

  } f = { a };

  TDelegatedTransferDialog dlg(this, IDD_SEARCH_DIALOG, f);
  dlg.SetCaption(_T("Search5 (delegated transfer)"));
  if (dlg.Execute() == IDOK)
    DoSearch(a);
}

#elif DELEGATE_DEMO_OPTION == DELEGATE_DEMO_OPTION_USE_LAMBDA

//
// Test delegating transfer dialog.
//
void TDialogDataTransferDemo::CmSearch5()
{
  static TSearchArguments a;

  TDelegatedTransferDialog dlg(this, IDD_SEARCH_DIALOG,
    [&](const TTransferInfo& i)
  {
    TransferEditData(i, IDC_SEARCH_STRING, a.SearchString);
    TransferFlag(i, IDC_CASE_SENSITIVE, a.Flags, a.CaseSensitive);
    TransferFlag(i, IDC_WHOLE_WORD, a.Flags, a.WholeWord);
    TransferFlag(i, IDC_SEARCH_UP, a.Flags, a.SearchUp);
  });
  dlg.SetCaption(_T("Search5 (delegated transfer)"));
  if (dlg.Execute() == IDOK)
    DoSearch(a);
}

#endif

//-----------------------------------------------------------------------------

void TClassesMDIClient::CmDialogDataTransfer()
{
  _TCHAR text[] = _T("Dialog Data Transfer\r\n")
    _T("This code demonstrates different mechanisms and programming styles for transferring data to and from controls.");

  TMDIChild* child = new TMDIChild(*this, _T("Dialog Data Transfer"), new TDialogDataTransferDemo(0, 0, text, 0, 0, 0, 0, 0, true));
  child->SetMenuDescr(TMenuDescr(IDM_DIALOGDATATRANSFER, 0, 0, 0, 1, 0, 0));
  child->Create();
  child->ShowWindow(SW_SHOW);

}

