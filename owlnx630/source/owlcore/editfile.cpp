//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29 $                                                              //
//$Author: jogybl $                                                             //
//$Date: 2009-08-05 10:32:03 $                                                    //
//
// Implementation of class TEditFile, a text edit which can find/replace and
// read/write from/to a file.
//----------------------------------------------------------------------------
#if defined(__BORLANDC__)  
#pragma hdrignore SECTION    
#endif
#include <owl/pch.h>
#if !defined(OWL_WINDOW_H) 
# include <owl/window.h>
#endif
#if !defined(OWL_CONTROL_H)
# include <owl/control.h>
#endif
#if !defined(OWL_EDIT_H)
# include <owl/edit.h>
#endif
#if !defined(OWL_EDITSEAR_H)
# include <owl/editsear.h>
#endif
#if !defined(OWL_EDITFILE_H)
# include <owl/editfile.h>
#endif
#if !defined(OWL_FILE_H)
# include <owl/file.h>
#endif
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_SYSTEM_H)
# include <owl/system.h>
#endif
#include <string.h>
#include <stdio.h>

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

DEFINE_RESPONSE_TABLE1(TEditFile, TEditSearch)
// !!! BUG  EV_COMMAND(CM_FILENEW,  CmFileNew),  // BUG !!!!!!!
// !!! BUG  EV_COMMAND(CM_FILEOPEN, CmFileOpen), // BUG !!!!!!!  
  EV_COMMAND(CM_FILESAVE, CmFileSave),
  EV_COMMAND(CM_FILESAVEAS, CmFileSaveAs),
  EV_COMMAND_ENABLE(CM_FILESAVE, CmSaveEnable),
END_RESPONSE_TABLE;

//
// constructor for a TEditFile
//
// initializes its data members using passed parameters and default values
//
TEditFile::TEditFile(TWindow*        parent,
                     int             id,
                     LPCTSTR         text,
                     int x, int y, int w, int h,
                     LPCTSTR         fileName,
                     TModule*        module)
:
  TEditSearch(parent, id, text, x, y, w, h, module)
{
  FileName = fileName ? strnewdup(fileName) : 0;
}

//
// dispose of the file name
//
TEditFile::~TEditFile()
{
  delete[] FileName;
}

//
// performs setup for a TEditFile
//
void
TEditFile::SetupWindow()
{
  TEditSearch::SetupWindow();
  FileData.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;

  FileData.Flags |= OFN_LONGNAMES;
  //
  // We currently cannot mix OFN_ENABLEHOOK & OFN_EXPLORER
  // when running under NT 3.51[no new shell]
  //
  FileData.Flags |= OFN_EXPLORER;

  FileData.SetFilter(GetModule()->LoadString(IDS_FILEFILTER).c_str());

  SetFileName(FileName);
  if (FileName && !Read()) {
    owl_string msgTemplate(GetModule()->LoadString(IDS_UNABLEREAD));
    LPTSTR msg = new _TCHAR[_MAX_PATH + msgTemplate.length()];
    _stprintf(msg, msgTemplate.c_str(), FileName);
    MessageBox(msg, 0, MB_ICONEXCLAMATION | MB_OK);
    delete[] msg;
    SetFileName(0);
  }
}

//
// sets the file name of the window and updates the caption
// replacing an empty name with 'Untitled' in its caption
//
void
TEditFile::SetFileName(LPCTSTR fileName)
{
  if (fileName != FileName) {
    delete[] FileName;
    FileName = fileName ? strnewdup(fileName) : 0;
  }
  owl_string untitled(GetModule()->LoadString(IDS_UNTITLEDFILE));
  SetDocTitle(FileName ? (LPCTSTR)FileName : untitled.c_str(), 0);
}

//
// begins the edit of a new file, after determining that it is Ok to
// clear the TEdit's text
//
void
TEditFile::NewFile()
{
  if (CanClear()) {
    Clear();
    Invalidate();
    ClearModify();
    SetFileName(0);
  }
}

//
// replaces the current file with the given file
//
void
TEditFile::ReplaceWith(LPCTSTR fileName)
{
  if (Read(fileName)) {
    Invalidate();
    SetFileName(fileName);
  }
  else {
    owl_string msgTemplate(GetModule()->LoadString(IDS_UNABLEREAD));
    LPTSTR  msg = new TCHAR[_MAX_PATH + msgTemplate.length()];
    _stprintf(msg, msgTemplate.c_str(), fileName);
    MessageBox(msg, 0, MB_ICONEXCLAMATION | MB_OK);
    delete[] msg;
  }
}

//
// brings up a dialog allowing the user to open a file into this
// window
//
// same as selecting File|Open from the menus
//
void
TEditFile::Open()
{
  if (CanClear()) {
    *FileData.FileName = 0;
    if (TFileOpenDialog(this, FileData).Execute() == IDOK)
      ReplaceWith(FileData.FileName);
  }
}

//
// reads the contents of a  specified file, or the previously-specified file
// if no name passed, into the TEdit control
// The caller is responsible for any error UI
//
bool
TEditFile::Read(LPCTSTR fileName)
{
  if (!fileName)
    if (FileName)
      fileName = FileName;
    else
      return false;

  bool   success = false;
  TFile file(fileName, TFile::ReadOnly|TFile::OpenExisting);

  if (file.IsOpen()) {
    long  charsToRead = file.Length();
    if (charsToRead >= 0) {
      Clear();

      // Lock and resize Editor's buffer to the size of the file
      // Then if OK, read the file into editBuffer
      //
      LPTSTR editBuffer = LockBuffer(uint(charsToRead+1));
      if (editBuffer) {
        if (file.Read(editBuffer, uint(charsToRead)) == uint(charsToRead)) {

          // 0 terminate Editor's buffer
          //
          editBuffer[int(charsToRead)] = 0;
          success = true;
          ClearModify();
        }
        UnlockBuffer(editBuffer, true);
      }
    }
  }
  return success;
}

//
// saves the contents of the TEdit child control into the file currently
// being editted
//
// returns true if the file was saved or IsModified returns false
//(contents already saved)
//
bool
TEditFile::Save()
{
  if (IsModified()) {
    if (!FileName)
      return SaveAs();

    if (!Write()) {
      owl_string msgTemplate(GetModule()->LoadString(IDS_UNABLEWRITE));
      LPTSTR msg = new TCHAR[_MAX_PATH + msgTemplate.length()];
      _stprintf(msg, msgTemplate.c_str(), FileName);
      MessageBox(msg, 0, MB_ICONEXCLAMATION | MB_OK);
      delete[] msg;
      return false;
    }
  }
  return true;  // editor's contents haven't been changed
}

//
// saves the contents of the TEdit child control into a file whose name
// is retrieved from the user, through execution of a "Save" file dialog
//
// returns true if the file was saved
//
bool
TEditFile::SaveAs()
{
  if (FileName)
    ::_tcscpy(FileData.FileName, FileName);

  else
    *FileData.FileName = 0;

  if (TFileSaveDialog(this, FileData).Execute() == IDOK) {
    if (Write(FileData.FileName)) {
      SetFileName(FileData.FileName);
      return true;
    }
    owl_string msgTemplate(GetModule()->LoadString(IDS_UNABLEWRITE));
    LPTSTR  msg = new TCHAR[_MAX_PATH + msgTemplate.length()];
    _stprintf(msg, msgTemplate.c_str(), FileName);
    MessageBox(msg, 0, MB_ICONEXCLAMATION | MB_OK);
    delete[] msg;
  }
  return false;
}

//
// Enables save command only if text is modified
//
void
TEditFile::CmSaveEnable(TCommandEnabler& commandHandler)
{
  commandHandler.Enable(IsModified());
}

//
// writes the contents of the TEdit child control to a specified file, or
// the previously-specified file if none passed.
// The caller is responsible for any error UI
//
bool
TEditFile::Write(LPCTSTR fileName)
{
  if (!fileName)
    if (FileName)
      fileName = FileName;
    else
      return false;

  TFile file(fileName, TFile::WriteOnly|TFile::PermExclusive|TFile::CreateAlways);
  if (!file.IsOpen())
    return false;

  bool success = false;
  LPTSTR editBuffer = LockBuffer();
  if (editBuffer) {
    success = file.Write(editBuffer, ::_tcslen(editBuffer));
    UnlockBuffer(editBuffer);
    if (success)
      ClearModify();
  }
  return success;
}

//
// returns a bool value indicating whether or not it is Ok to clear
// the TEdit's text
//
// returns true if the text has not been changed, or if the user Oks the
// clearing of the text
//
bool
TEditFile::CanClear()
{
  if (IsModified()) {
    owl_string msgTemplate(GetModule()->LoadString(IDS_FILECHANGED));
    owl_string untitled(GetModule()->LoadString(IDS_UNTITLEDFILE));
    LPTSTR  msg = new TCHAR[_MAX_PATH+msgTemplate.length()];

    _stprintf(msg, msgTemplate.c_str(),
             FileName ? (LPCTSTR)FileName : untitled.c_str());

    int result = MessageBox(msg, 0, MB_YESNOCANCEL|MB_ICONQUESTION);
    delete[] msg;
    return result==IDYES ? Save() : result != IDCANCEL;
  }
  return true;
}

//
//
//
bool
TEditFile::CanClose()
{
  return CanClear();
}

#endif
#if !defined(SECTION) || SECTION == 2


IMPLEMENT_STREAMABLE1(TEditFile, TEditSearch);

#if !defined(BI_NO_OBJ_STREAMING)

//
// reads an instance of TEditFile from the passed ipstream
//
void*
TEditFile::Streamer::Read(ipstream& is, uint32 /*version*/) const
{
  TEditFile* o = GetObject();
  ReadBaseObject((TEditSearch*)o, is);

#if defined(UNICODE)
  _USES_CONVERSION;
  char * fileName = is.freadString();

  o->FileName = _A2W(fileName);

  delete[] fileName;
#else
  o->FileName = is.freadString();
#endif  
  if (!*o->FileName) {
    delete o->FileName;
    o->FileName = 0;
  }
  return o;
}

//
// writes the TEditFile to the passed opstream
//
void
TEditFile::Streamer::Write(opstream& os) const
{
  TEditFile* o = GetObject();
  WriteBaseObject((TEditSearch*)o, os);
  _USES_CONVERSION;
  os.fwriteString(o->FileName ? _W2A(o->FileName) : "");
}

#endif  // if !defined(BI_NO_OBJ_STREAMING)

#endif


__OWL_END_NAMESPACE

