//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//$Author: jogybl $
//$Date: 2009-08-05 10:31:53 $
//
// Definition of Common Dialog abstract base class
//----------------------------------------------------------------------------

#if !defined(OWL_COMMDIAL_H)
#define OWL_COMMDIAL_H

#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#if !defined(_INC_COMMDLG)
#if defined(BI_NEED_COMMDLG_H)
# include <api_upd/commdlg.h>
#else
# include <commdlg.h>
#endif
#endif
#if !defined(__CDERR_H)
# include <cderr.h>
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
// class TCommonDialog
// ~~~~~ ~~~~~~~~~~~~~
class _OWLCLASS TCommonDialog : public TDialog {
  public:
    TCommonDialog(TWindow* parent, LPCTSTR title = 0, TModule* module = 0);
   ~TCommonDialog();

    virtual void PerformCreate(int menuOrId);
    virtual int  DoExecute();

  protected:
    LPCTSTR GetCDTitle() const;

    void SetupWindow();

    // Default behavior inline for message response functions
    //
    void CmOkCancel(); //EV_COMMAND(IDOK or IDCANCEL
    void EvClose();    //EV_CLOSE
    void CmHelp();     //EV_COMMAND(pshHelp,

  protected_data:
    LPCTSTR CDTitle;

  private:
    TCommonDialog(const TCommonDialog&);
    TCommonDialog& operator=(const TCommonDialog&);

  DECLARE_RESPONSE_TABLE(TCommonDialog);
  DECLARE_CASTABLE;
};

//
// class TCommDlg
// ~~~~~ ~~~~~~~~
// delay loading COMDLG32.DLL/COMMDLG.DLL
class _OWLCLASS TCommDlg {
  public:
    static  BOOL  ChooseColor(LPCHOOSECOLOR);
    static  BOOL  ChooseFont(LPCHOOSEFONT);
    static  DWORD CommDlgExtendedError();
    static  HWND  FindText(LPFINDREPLACE);
    static  short GetFileTitle(LPCTSTR,LPTSTR,WORD);
    static  BOOL  GetOpenFileName(LPOPENFILENAME);
    static  BOOL  PageSetupDlg(LPPAGESETUPDLG);
#if WINVER >= 0x500
    static  BOOL  PrintDlgEx(LPPRINTDLGEX);
#endif
    static  BOOL  PrintDlg(LPPRINTDLG);
    static  HWND  ReplaceText(LPFINDREPLACE);
    static  BOOL  GetSaveFileName(LPOPENFILENAME);
    static  TModule&  GetModule();
};


// Generic definitions/compiler options (eg. alignment) following the 
// definition of classes
#include <owl/posclass.h>



//----------------------------------------------------------------------------
// Inline implementations
//

//
// Return the title from the common dialog.
//
inline LPCTSTR TCommonDialog::GetCDTitle() const
{
  return CDTitle;
}

//
inline void TCommonDialog::CmOkCancel()
{
  DefaultProcessing();
}

//
inline void TCommonDialog::EvClose()
{
  DefaultProcessing();
}

//
inline void TCommonDialog::CmHelp()
{
  DefaultProcessing();
}

__OWL_END_NAMESPACE


#endif  // OWL_COMMDIAL_H
