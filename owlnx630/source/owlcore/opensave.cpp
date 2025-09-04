//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:32:03 $
//
// Implementation of OpenSave abstract, FileOpen, FileSave Common Dialog
// classes
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION
#endif
#include <owl/pch.h>
#if !defined(OWL_OPENSAVE_H)
# include <owl/opensave.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

//
//
//
TOpenSaveDialog::TData::TData(uint32 flags,
                              LPTSTR filter,
                              LPTSTR customFilter,
                              LPTSTR initialDir,
                              LPTSTR defExt,
                              int    maxPath,
                              int    filterIndex,
                              uint32 flagsEx)
:
  Flags(flags), FlagsEx(flagsEx), Error(0), FileName(0), Filter(0),
  CustomFilter(customFilter), FilterIndex(filterIndex),
  InitialDir(initialDir), DefExt(defExt),
  MaxPath(maxPath ? maxPath : _MAX_PATH)
{
  FileName = new TCHAR[MaxPath];
  *FileName = 0;
  SetFilter(filter);
  Flags |= OFN_LONGNAMES;

  //
  // We currently cannot mix OFN_ENABLEHOOK & OFN_EXPLORER
  // when running under NT 3.51[no new shell]
  //
  Flags |= OFN_EXPLORER;
}

//
//
//
TOpenSaveDialog::TData::TData(const TData& src)
:
  Flags(src.Flags), FlagsEx(src.FlagsEx), Error(0), FileName(0), Filter(0),
  CustomFilter(src.CustomFilter), FilterIndex(src.FilterIndex),
  InitialDir(src.InitialDir), DefExt(src.DefExt),
  MaxPath(src.MaxPath)
{
  FileName = strnewdup(src.FileName, MaxPath);
  SetFilter(src.Filter);
}

//
//
//
TOpenSaveDialog::TData::~TData()
{
  delete[] FileName;
  delete[] Filter;
}

//
//
//
TOpenSaveDialog::TData&
TOpenSaveDialog::TData::operator =(const TData& src)
{
  Flags = src.Flags;
  FlagsEx = src.FlagsEx;
  Error = 0;
  CustomFilter = src.CustomFilter;
  FilterIndex = src.FilterIndex;
  InitialDir = src.InitialDir;
  DefExt = src.DefExt;
  MaxPath = src.MaxPath;

  delete[] FileName;
  FileName = strnewdup(src.FileName, MaxPath);

  SetFilter(src.Filter);

  return *this;
}

//
// Set the file list box filter strings. Translates '|'s into 0s so that the
// string can be kept as a resource with imbeded '|'s like:
// "Text Files(*.txt)|*.TXT|All Files(*.*)|*.*|"
// Can also handle already processed filter strings.
//
void
TOpenSaveDialog::TData::SetFilter(LPCTSTR filter)
{
  // Copy filter string
  //
  if (filter) {
    delete[] Filter;
    if (_tcschr(filter, _T('|'))) {
      uint len = ::_tcslen(filter) + 2; // one for each terminating 0
      Filter = ::_tcscpy(new TCHAR[len], filter);
      Filter[len-1] = 0;             // in case trailing '|' is missing
    }
    else {
      LPCTSTR p = filter;
      while (*p)
        p += ::_tcslen(p) + 1;             // scan for 00 at end
      uint len = uint(p - filter) + 1;  // one more for last 0
      Filter = new TCHAR[len];
      memcpy(Filter, filter, sizeof(TCHAR) * len);
    }
  }
  // Stomp |s with 0s
  //
  if (Filter) {
    for (TCharIterator<TCHAR> i(Filter); ; i++) {
      if (!*i.Current())
        break;
      if (*i.Current() == _T('|'))
        *i.Current() = 0;
    }
  }
}


//----------------------------------------------------------------------------

DEFINE_RESPONSE_TABLE1(TOpenSaveDialog, TCommonDialog)
END_RESPONSE_TABLE;

uint TOpenSaveDialog::ShareViMsgId = 0;


//
//
//
void
TOpenSaveDialog::Init(TResId templateId)
{
  if (!ShareViMsgId)
    ShareViMsgId = ::RegisterWindowMessage(SHAREVISTRING);

  memset(&ofn, 0, sizeof(OPENFILENAME));
#if defined(OPENFILENAME_SIZE_VERSION_400)
  ofn.lStructSize = TSystem::GetMajorVersion() < 5 ?
                    OPENFILENAME_SIZE_VERSION_400 : sizeof(OPENFILENAME);
#else
  ofn.lStructSize = sizeof(OPENFILENAME);
#endif
  ofn.hwndOwner = Parent ? Parent->GetHandle() : 0;
  ofn.hInstance = *GetModule();
  ofn.Flags = OFN_ENABLEHOOK | Data.Flags;
  if (templateId) {
    ofn.lpTemplateName = templateId;
    ofn.Flags |= OFN_ENABLETEMPLATE;
  }
  else
    ofn.Flags &= ~OFN_ENABLETEMPLATE;
#if WINVER >= 0x500 && !defined(WINELIB)
  ofn.FlagsEx = Data.FlagsEx;
#endif
  ofn.lpfnHook = 0;

  ofn.lpstrFilter = Data.Filter;
  ofn.nFilterIndex = Data.FilterIndex;
  ofn.lpstrFile = Data.FileName;
  ofn.nMaxFile = Data.MaxPath;
  ofn.lpstrInitialDir = Data.InitialDir;
  ofn.lpstrDefExt = Data.DefExt;
}

//
//
//
TOpenSaveDialog::TOpenSaveDialog(TWindow* parent, TData& data, TModule*   module)
:
  TCommonDialog(parent, 0, module),
  Data(data)
{
}

//
//
//
TOpenSaveDialog::TOpenSaveDialog(TWindow*        parent,
                                 TData&          data,
                                 TResId          templateId,
                                 LPCTSTR         title,
                                 TModule*        module)
:
  TCommonDialog(parent, 0, module),
  Data(data)
{
  Init(templateId);
  ofn.lpstrTitle = title;
}

//
//
//
bool
TOpenSaveDialog::DialogFunction(uint msg, TParam1 param1, TParam2 param2)
{
  if (TCommonDialog::DialogFunction(msg, param1, param2))
    return true;

  if (msg == TOpenSaveDialog::ShareViMsgId)
    return (bool)ShareViolation();

  return false;
}

//
//
//
int
TOpenSaveDialog::ShareViolation()
{
  return OFN_SHAREWARN;
}

//----------------------------------------------------------------------------

//
//
//
TFileOpenDialog::TFileOpenDialog(TWindow*        parent,
                                 TData&          data,
                                 TResId          templateId,
                                 LPCTSTR         title,
                                 TModule*        module)
:
  TOpenSaveDialog(parent, data, templateId, title, module)
{
}

//
//
//
int
TFileOpenDialog::DoExecute()
{
  ofn.lpfnHook = LPOFNHOOKPROC(StdDlgProc);
  int ret = TCommDlg::GetOpenFileName(&ofn);
  if (ret) {
    Data.Flags = ofn.Flags;
    Data.FilterIndex = (int)ofn.nFilterIndex;
    Data.Error = 0;
  }
  else {
    Data.Error = TCommDlg::CommDlgExtendedError();
  }
  return ret ? IDOK : IDCANCEL;
}


//----------------------------------------------------------------------------

//
//
//
TFileSaveDialog::TFileSaveDialog(TWindow*        parent,
                                 TData&          data,
                                 TResId          templateId,
                                 LPCTSTR         title,
                                 TModule*        module)
:
  TOpenSaveDialog(parent, data, templateId, title, module)
{
}

//
//
//
int
TFileSaveDialog::DoExecute()
{
  ofn.lpfnHook = LPOFNHOOKPROC(StdDlgProc);
  int ret = TCommDlg::GetSaveFileName(&ofn);
  if (ret) {
    Data.Flags = ofn.Flags;
    Data.FilterIndex = ofn.nFilterIndex;
    Data.Error = 0;
  }
  else {
    Data.Error = TCommDlg::CommDlgExtendedError();
  }
  return ret ? IDOK : IDCANCEL;
}

#endif
#if !defined(SECTION) || SECTION == 2

//
//
//
void
TOpenSaveDialog::TData::Read(ipstream& is)
{
  is >> Flags;
#if defined(UNICODE)
  _USES_CONVERSION;

  char* fileName      = is.readString();
  char* filter        = is.freadString();
  char* customFilter  = is.freadString();

  FileName = strnewdup(_A2W(fileName));
  Filter = strnewdup(_A2W(filter));
  CustomFilter = strnewdup(_A2W(customFilter));
  
  delete[] fileName;
  delete[] filter;
  delete[] customFilter;

  is >> FilterIndex;
  
  char* initDir = is.freadString();
  char* defExt  = is.freadString();
  
  InitialDir = strnewdup(_A2W(initDir));
  DefExt = strnewdup(_A2W(defExt));

  delete[] initDir;
  delete[] defExt;

#else
  FileName = is.readString();
  Filter = is.freadString();
  CustomFilter = is.freadString();
  is >> FilterIndex;
  InitialDir = is.freadString();
  DefExt = is.freadString();
#endif
  is >> FlagsEx;
}

//
//
//
void
TOpenSaveDialog::TData::Write(opstream& os)
{
  _USES_CONVERSION;

  os << Flags;
  os.writeString(_W2A(FileName));
  os.fwriteString(_W2A(Filter));
  os.fwriteString(_W2A(CustomFilter));
  os << FilterIndex;
  os.fwriteString(_W2A(InitialDir));
  os.fwriteString(_W2A(DefExt));
  os << FlagsEx;
}

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

