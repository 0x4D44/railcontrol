//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.26 $
//$Author: jogybl $
//$Date: 2007-05-11 15:29:13 $
//
// Definition of FindReplace- abstract, Find-, Replace- common Dialog classes
//----------------------------------------------------------------------------

#if !defined(OWL_FINDREPL_H)
#define OWL_FINDREPL_H

#if !defined(OWL_COMMDIAL_H)
# include <owl/commdial.h>
#endif

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
// class TFindReplaceDialog
// ~~~~~ ~~~~~~~~~~~~~~~~~~
class _OWLCLASS TFindReplaceDialog : public TCommonDialog {
  public:
    class _OWLCLASS TData {
      public:
        uint32   Flags;
        int      BuffSize;
        uint32   Error;
        LPTSTR   FindWhat;
        LPTSTR   ReplaceWith;

        TData(uint32 flags = 0, int buffSize = 81);
        TData(const TData& src);
       ~TData();

        TData& operator =(const TData& src);

        void     Write(opstream& os);
        void     Read(ipstream& is);
    };

    TFindReplaceDialog(TWindow*        parent,
                       TData&          data,
                       TResId          templateId = 0,
                       LPCTSTR title = 0,
                       TModule*        module = 0);

    void         UpdateData(TParam2 param = 0);

  protected:
    virtual void PerformCreate(int menuOrId);

    TFindReplaceDialog(TWindow*        parent,
                       TResId          templateId = 0,
                       LPCTSTR title = 0,
                       TModule*        module = 0);

    void     Init(TResId templateId);
    bool     DialogFunction(uint message, TParam1, TParam2);

    TData&   GetData();
    void     SetData(TData& data);

    FINDREPLACE&  GetFR();
    void   SetFR(const FINDREPLACE& _fr);

    // Default behavior inline for message response functions
    //
    void     CmFindNext();   //EV_COMMAND(IDOK,
    void     CmReplace();    //EV_COMMAND(psh1,
    void     CmReplaceAll(); //EV_COMMAND(psh2,
    void     CmCancel();     //EV_COMMAND(IDCANCEL,

    void     EvNCDestroy();

  protected_data:
    FINDREPLACE  fr;
    TData&       Data;

  DECLARE_RESPONSE_TABLE(TFindReplaceDialog);
  DECLARE_CASTABLE;
};

//
// class TFindDialog
// ~~~~~ ~~~~~~~~~~~
class _OWLCLASS TFindDialog : public TFindReplaceDialog {
  public:
    TFindDialog(TWindow*        parent,
                TData&          data,
                TResId          templateId = 0,
                LPCTSTR         title = 0,
                TModule*        module = 0);

  protected:
    virtual void PerformCreate(int menuOrId);

  private:
    TFindDialog();
    TFindDialog(const TFindDialog&);

  DECLARE_CASTABLE;
};

//
// class TReplaceDialog
// ~~~~~ ~~~~~~~~~~~~~~
class _OWLCLASS TReplaceDialog : public TFindReplaceDialog {
  public:
    TReplaceDialog(TWindow*        parent,
                   TData&          data,
                   TResId          templateId = 0,
                   LPCTSTR         title = 0,
                   TModule*        module = 0);

  protected:
    virtual void PerformCreate(int menuOrId);

  private:
    TReplaceDialog(const TReplaceDialog&);
    TReplaceDialog& operator=(const TReplaceDialog&);

  DECLARE_CASTABLE;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return the transfer data for the find and replace dialog.
//
inline TFindReplaceDialog::TData& TFindReplaceDialog::GetData() {
  return Data;
}

//
// Set the transfer data for the dialog.
//
inline void  TFindReplaceDialog::SetData(TData& data) {
  Data = data;
}

//
// Return the underlying system structure for the find and replace dialog.
//
inline FINDREPLACE& TFindReplaceDialog::GetFR() {
  return fr;
}

//
// Set the underlying system structure for the dialog.
//
inline void TFindReplaceDialog::SetFR(const FINDREPLACE& _fr) {
  fr = _fr;
}

//
// Default behavior inline for message response functions
//
inline void TFindReplaceDialog::CmFindNext() {
  DefaultProcessing();
}

//
// Default behavior inline for message response functions
//
inline void TFindReplaceDialog::CmReplace() {
  DefaultProcessing();
}

//
// Default behavior inline for message response functions
//
inline void TFindReplaceDialog::CmReplaceAll() {
  DefaultProcessing();
}

//
// Default behavior inline for message response functions
//
inline void TFindReplaceDialog::CmCancel() {
  DefaultProcessing();
}

__OWL_END_NAMESPACE

#endif  // OWL_FINDREPL_H
