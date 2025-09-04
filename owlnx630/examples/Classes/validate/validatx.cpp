//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1993, 1996 by Borland International, All Rights Reserved
//----------------------------------------------------------------------------
#include <owl/pch.h>
#if !defined(OWL_APPLICAT_H)
# include <owl/applicat.h>
#endif
#if !defined(OWL_DIALOG_H)
# include <owl/dialog.h>
#endif
#if !defined(OWL_FRAMEWIN_H)
# include <owl/framewin.h>
#endif
#if !defined(OWL_EDIT_H)
# include <owl/edit.h>
#endif
#if !defined(OWL_CHECKBOX_H)
# include <owl/checkbox.h>
#endif
#if !defined(OWL_VALIDATE_H)
# include <owl/validate.h>
#endif
#include <string.h>               // For strcpy and strcat
#include <stdlib.h>               // For atoi
#include <ctype.h>                // For isdigit and isalpha
#include "validate.rc"               

#define MAXNAMELEN     35
#define MAXSSLEN       12
#define MAXIDLEN       4
#define MAXDEPTLEN     7
#define MAXSECLEN      3
#define MAXCUSTOMLEN   20

#include <pshpack1.h>
struct TEmployeeStruct {
  TCHAR NameEdit[MAXNAMELEN];
  TCHAR SSEdit[MAXSSLEN];
  TCHAR IDEdit[MAXIDLEN];
  TCHAR DeptEdit[MAXDEPTLEN];
  TCHAR SecEdit[MAXSECLEN];
  uint16 FullTime;
  uint16 Perm;
  uint16 Exempt;
  TCHAR CustomEdit[MAXCUSTOMLEN];
};
#include <poppack.h>

//
// class TEmployeeDlg
// ~~~~~ ~~~~~~~~~~~~
class TEmployeeDlg : public TDialog {
  public:
    TEmployeeDlg(TWindow* parent, TResId resId, TEmployeeStruct& transfer);

  private:
    void    CmSetCustom();
    TEdit*  CustomEdit;

  DECLARE_RESPONSE_TABLE(TEmployeeDlg);
};


DEFINE_RESPONSE_TABLE1(TEmployeeDlg, TDialog)
  EV_COMMAND(IDC_CUSTOM, CmSetCustom),
END_RESPONSE_TABLE;


//
//
//
TEmployeeDlg::TEmployeeDlg(TWindow* parent, TResId resid,
  TEmployeeStruct& transfer)
:
  TDialog(parent, resid)
{
  TEdit* edit;
  edit = new TEdit(this, IDC_NAME, sizeof(transfer.NameEdit) / sizeof(TCHAR));
  edit->SetValidator(new TFilterValidator(_T("A-Za-z. ")));
  edit = new TEdit(this, IDC_SS, sizeof(transfer.SSEdit) / sizeof(TCHAR));
  edit->SetValidator(new TPXPictureValidator(_T("###-##-####")));
  edit = new TEdit(this, IDC_EMPID, sizeof(transfer.IDEdit) / sizeof(TCHAR));
  edit->SetValidator(new TRangeValidator(1, 999));
  edit = new TEdit(this, IDC_DEPT, sizeof(transfer.DeptEdit) / sizeof(TCHAR));
  edit->SetValidator(new TPXPictureValidator(_T("Sales,Dev,Mfg")));
  edit = new TEdit(this, IDC_SECURITY, sizeof(transfer.SecEdit) / sizeof(TCHAR));
  edit->SetValidator(new TPXPictureValidator(_T("11,12,13,14,15")));
  new TCheckBox(this, IDC_FTIME, 0);
  new TCheckBox(this, IDC_PERMANENT, 0);
  new TCheckBox(this, IDC_EXEMPT, 0);
  CustomEdit = new TEdit(this, IDC_EDIT2, sizeof(transfer.CustomEdit) / sizeof(TCHAR));
  TValidator* v = new TPXPictureValidator(_T("------"), true);
  v->UnsetOption(voOnAppend);
  CustomEdit->SetValidator(v);

  SetTransferBuffer(&transfer);
}

void
TEmployeeDlg::CmSetCustom()
{
  TCHAR buff[40];
  TValidator* v;
  
  GetDlgItemText(IDC_EDIT1, buff, sizeof(buff) / sizeof(TCHAR));
  
  TRY {
    v = new TPXPictureValidator(buff, true);
  }
  CATCH( (TXValidator x) {                    // catches syntax errors
    MessageBox(x.why().c_str(), GetApplication()->GetName(), MB_OK);
    return;
  })
  
  v->UnsetOption(voOnAppend);
  CustomEdit->SetValidator(v);
  CustomEdit->SetFocus();
}


//
// class TTestWindow
// ~~~~~ ~~~~~~~~~~~
class TTestWindow : public TWindow {
  public:
    TTestWindow(TWindow* parent = 0);
    void CmEmpInput();

  private:
    TEmployeeStruct EmployeeStruct;

  DECLARE_RESPONSE_TABLE(TTestWindow);
};

DEFINE_RESPONSE_TABLE1(TTestWindow, TWindow)
  EV_COMMAND(CM_EMPINPUT, CmEmpInput),
END_RESPONSE_TABLE;


//
//
//
TTestWindow::TTestWindow(TWindow* parent)
:
  TWindow(parent)
{
  memset(&EmployeeStruct, 0, sizeof EmployeeStruct);
}

void
TTestWindow::CmEmpInput()
{
  TCHAR empInfo[sizeof(TEmployeeStruct)+ 10 + 11 + 11];

  if (TEmployeeDlg(this, IDD_EMPLOYEEINFO, EmployeeStruct).Execute() == IDOK) {
	  
    _tcscpy(empInfo, EmployeeStruct.NameEdit);
    _tcscat(empInfo, _T("\n"));
    _tcscat(empInfo, EmployeeStruct.SSEdit);
    _tcscat(empInfo, _T("\n"));
    _tcscat(empInfo, EmployeeStruct.IDEdit);
    _tcscat(empInfo, _T("\n"));
    _tcscat(empInfo, EmployeeStruct.DeptEdit);
    _tcscat(empInfo, _T("\n"));
    _tcscat(empInfo, EmployeeStruct.SecEdit);
    _tcscat(empInfo, _T("\n"));
    _tcscat(empInfo, EmployeeStruct.FullTime ? _T("FullTime ") : _T("PartTime "));
    _tcscat(empInfo, EmployeeStruct.Perm ? _T("Permanent ") : _T("Temporary "));
    _tcscat(empInfo, EmployeeStruct.Exempt ? _T("Exempt ") : _T("NonExempt "));
    MessageBox(empInfo, _T("Information Stored"), MB_OK);
  }
}

//
// class TValidateApp
// ~~~~~ ~~~~~~~~~~~~
class TValidateApp : public TApplication {
  public:
    TValidateApp() : TApplication(_T("ValidateApp")) {}
    void InitMainWindow() {
      TFrameWindow* frame = new TFrameWindow(0, _T("Validate Dialog Input"),
        new TTestWindow);
      frame->AssignMenu(200);
      frame->SetWindowPos(0, 0, 0, 400, 200, SWP_NOMOVE);
      SetMainWindow(frame);
    }
};

int
OwlMain(int /*argc*/, TCHAR* /*argv*/ [])
{
  return TValidateApp().Run();
}
