#pragma once

#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/transfer.h>


//
// Employee details; used for dialog data transfer.
//
struct TEmployeeInformation
{
  owl::tstring Name;
  owl::tstring SocialSecurityNumber;
  owl::tstring EmployeeId;
  owl::tstring Department;
  owl::tstring SecurityClearance;
  owl::tstring Coefficient;
  bool FullTime;
  bool Permanent;
  bool Exempt;
  owl::tstring CustomData;
  owl::tstring Address;
};

// 
// Use the dialog data transfer (DDT) class template TTransferDialog as our base dialog.
// This template is new in OWLNext 6.32.
//
typedef owl::TTransferDialog<TEmployeeInformation> TEmployeeBaseDlg;

//
// Employee information entry dialog
//
class TEmployeeDlg
  : public TEmployeeBaseDlg
{
public:

  TEmployeeDlg(owl::TWindow* parent, TEmployeeInformation&);

protected:

  virtual void DoTransferData(const owl::TTransferInfo&, TEmployeeInformation&); // override

private:

  owl::TEdit& CustomData;
  owl::TEdit& CustomPic;

  void CmSetCustom();

  DECLARE_RESPONSE_TABLE(TEmployeeDlg);
};

