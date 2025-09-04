//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of class TEditFile, a text edit which can find/replace and
// read/write from/to a file.
//----------------------------------------------------------------------------

#if !defined(OWL_EDITFILE_H)
#define OWL_EDITFILE_H

#if !defined(OWL_EDITSEAR_H)
# include <owl/editsear.h>
#endif
#if !defined(OWL_OPENSAVE_H)
# include <owl/opensave.h>
#endif
#include <owl/editfile.rh>
#if defined(BI_HAS_PRAGMA_ONCE)
# pragma once
#endif
#if defined(BI_COMP_WATCOM)
# pragma read_only_file
#endif


__OWL_BEGIN_NAMESPACE

// Generic definitions/compiler options (eg. alignment) preceeding the
// definition of classes
#include <owl/preclass.h>

//
// class TEditFile
// ~~~~~ ~~~~~~~~~
class _OWLCLASS TEditFile : public TEditSearch {
  public:
    TEditFile(TWindow*        parent = 0,
              int             id = 0,
              LPCTSTR         text = 0,
              int x = 0, int y = 0, int w = 0, int h = 0,
              LPCTSTR         fileName = 0,
              TModule*        module = 0);

    // NOTE: The following constructor, which aliases an Edit control created
    //       from a dialog resource is 'unconventional' in that it expects
    //       a TModule reference instead of the traditional 'TModule* = 0'.
    //       This is, however, to avoid ambiguities between the two forms
    //       of constructor. Since it is traditionally created as a child
    //       of a TDialog-derived class, you can simply use the module of the
    //       parent object.
    //       For example,
    //             TMyDialog::TMyDialog(....) {
    //                edit = new TEditFile(this, ID_EDIT1, *GetModule());
    //             }
    //
    TEditFile(TWindow*   parent,
              int        resourceId,
              TModule&   module);
   ~TEditFile();

    virtual bool  CanClear();
    virtual bool  CanClose();

    virtual void  NewFile();
    virtual void  Open();
    virtual bool  Read(LPCTSTR fileName = 0);
    virtual void  ReplaceWith(LPCTSTR fileName);
    virtual bool  Save();
    virtual bool  SaveAs();
    virtual bool  Write(LPCTSTR fileName=0);

    LPCTSTR       GetFileName() const;
    void          SetFileName(LPCTSTR fileName);

    TOpenSaveDialog::TData& GetFileData();
    void   SetFileData(const TOpenSaveDialog::TData& fd);

  protected:  //!CQ these command handlers were public...

    // Command response functions
    //
    void          CmFileNew();     // CM_FILENEW
    void          CmFileOpen();    // CM_FILEOPEN
    void          CmFileSave();    // CM_FILESAVE
    void          CmFileSaveAs();  // CM_FILESAVEAS

    // Command enabler functions
    //
    void          CmSaveEnable(TCommandEnabler& commandHandler);

  protected:
    // Override virtual functions defined by TWindow
    //
    void          SetupWindow();

  public_data:
    TOpenSaveDialog::TData FileData;
    LPTSTR FileName;

  private:
    // Hidden to prevent accidental copying or assignment
    //
    TEditFile(const TEditFile&);
    TEditFile& operator =(const TEditFile&);

  DECLARE_RESPONSE_TABLE(TEditFile);
  //DECLARE_STREAMABLE(_OWLCLASS, NS_OWL::TEditFile, 1);
  DECLARE_STREAMABLE_OWL(NS_OWL::TEditFile, 1);
};
// define streameable inlines (VC)
DECLARE_STREAMABLE_INLINES( NS_OWL::TEditFile );

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
inline
TEditFile::TEditFile(TWindow* parent, int resourceId, TModule& module)
          :TEditSearch(parent, resourceId, module),
           FileName(0)
{
}

// Return the filename for this buffer.
//
inline LPCTSTR TEditFile::GetFileName() const {
  return FileName;
}

//
// Return the FileData data member used for the common dialogs.
//
inline TOpenSaveDialog::TData& TEditFile::GetFileData() {
  return FileData;
}

//
inline void TEditFile::SetFileData(const TOpenSaveDialog::TData& fd) {
  FileData = fd;
}

//
inline void TEditFile::CmFileNew() {
  NewFile();
}

//
inline void TEditFile::CmFileOpen() {
  Open();
}

//
inline void TEditFile::CmFileSave() {
  Save();
}

//
inline void TEditFile::CmFileSaveAs() {
  SaveAs();
}


__OWL_END_NAMESPACE


#endif  // OWL_EDITFILE_H
