//----------------------------------------------------------------------------
//  Project ApxMdiDv
//  Borland International
//  Copyright © 1996. All Rights Reserved.
//
//  SUBSYSTEM:    ApxMdiDv Application
//  FILE:         apxmddad.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TApxMdiDvAboutDlg (TDialog).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include <stdio.h>

#include "apxmddva.h"
#include "apxMDdad.h"


TProjectRCVersion::TProjectRCVersion(TModule* module)
{
  uint32  fvHandle;
  uint    vSize;
  _TCHAR    appFName[255];
  TAPointer<_TCHAR> subBlockName(new _TCHAR[255]);

  FVData = 0;

  module->GetModuleFileName(appFName, sizeof(appFName) / sizeof(_TCHAR));

#if !defined(UNICODE)
  OemToChar(appFName, appFName);
#endif

  uint32 dwSize = ::GetFileVersionInfoSize(appFName, &fvHandle);
  if (dwSize) {
    FVData  = (void *)new _TCHAR[(uint)dwSize];
    if (::GetFileVersionInfo(appFName, fvHandle, dwSize, FVData)) {
      // Copy string to buffer so if the -dc compiler switch(Put constant strings in code segments)
      // is on VerQueryValue will work under Win16.  This works around a problem in Microsoft's ver.dll
      // which writes to the string pointed to by subBlockName.
      //
      _tcscpy(subBlockName, _T("\\VarFileInfo\\Translation"));
      if (!::VerQueryValue(FVData, subBlockName,(void * *)&TransBlock, &vSize)) {
        delete[] FVData;
        FVData = 0;
      }
      else
        // Swap the words so sprintf will print the lang-charset in the correct format.
        //
        *(uint32 *)TransBlock = MAKELONG(HIWORD(*(uint32 *)TransBlock), LOWORD(*(uint32 *)TransBlock));
    }
  }
}


TProjectRCVersion::~TProjectRCVersion()
{
  if (FVData)
    delete[] FVData;
}


bool TProjectRCVersion::GetProductName(LPTSTR& prodName)
{
  uint    vSize;
  TAPointer<_TCHAR> subBlockName(new _TCHAR[255]);

  if (FVData) {
    wsprintf(subBlockName, _T("\\StringFileInfo\\%08lx\\%s"), *(uint32 *)TransBlock,(LPTSTR)_T("ProductName"));
    return FVData ? ::VerQueryValue(FVData, subBlockName,(void * *)&prodName, &vSize) : false;
  } else
    return false;
}


bool TProjectRCVersion::GetProductVersion(LPTSTR& prodVersion)
{
  uint    vSize;
  TAPointer<_TCHAR> subBlockName(new _TCHAR[255]);

  if (FVData) {
    wsprintf(subBlockName, _T("\\StringFileInfo\\%08lx\\%s"), *(uint32 *)TransBlock,(LPTSTR)_T("ProductVersion"));
    return FVData ? ::VerQueryValue(FVData, subBlockName,(void * *)&prodVersion, &vSize) : false;
  } else
    return false;
}


bool TProjectRCVersion::GetCopyright(LPTSTR& copyright)
{
  uint    vSize;
  TAPointer<_TCHAR> subBlockName(new _TCHAR[255]);

  if (FVData) {
    wsprintf(subBlockName, _T("\\StringFileInfo\\%08lx\\%s"), *(uint32 *)TransBlock,(LPTSTR)_T("LegalCopyright"));
    return FVData ? ::VerQueryValue(FVData, subBlockName,(void * *)&copyright, &vSize) : false;
  } else
    return false;
}


bool TProjectRCVersion::GetDebug(LPTSTR& debug)
{
  uint    vSize;
  TAPointer<_TCHAR> subBlockName(new _TCHAR[255]);

  if (FVData) {
    wsprintf(subBlockName, _T("\\StringFileInfo\\%08lx\\%s"), *(uint32 *)TransBlock,(LPTSTR)_T("SpecialBuild"));
    return FVData ? ::VerQueryValue(FVData, subBlockName,(void * *)&debug, &vSize) : false;
  } else
    return false;
}


//{{TApxMdiDvAboutDlg Implementation}}


//--------------------------------------------------------
// TApxMdiDvAboutDlg
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TApxMdiDvAboutDlg::TApxMdiDvAboutDlg(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
}


TApxMdiDvAboutDlg::~TApxMdiDvAboutDlg()
{
  Destroy();

  // INSERT>> Your destructor code here.
}


void TApxMdiDvAboutDlg::SetupWindow()
{
  LPTSTR prodName = 0, prodVersion = 0, copyright = 0, debug = 0;

  // Get the static text for the value based on VERSIONINFO.
  //
  TStatic* versionCtrl = new TStatic(this, IDC_VERSION, 255);
  TStatic* copyrightCtrl = new TStatic(this, IDC_COPYRIGHT, 255);
  TStatic* debugCtrl = new TStatic(this, IDC_DEBUG, 255);

  TDialog::SetupWindow();

  // Process the VERSIONINFO.
  //
  TProjectRCVersion applVersion(GetModule());

  // Get the product name and product version strings.
  //
  if (applVersion.GetProductName(prodName) && applVersion.GetProductVersion(prodVersion)) {
    // IDC_VERSION is the product name and version number, the initial value of IDC_VERSION is
    // the word Version(in whatever language) product name VERSION product version.
    //
    _TCHAR buffer[255];
    _TCHAR versionName[128];

    buffer[0] = '\0';
    versionName[0] = '\0';

    versionCtrl->GetText(versionName, sizeof(versionName)/sizeof(_TCHAR));
    wsprintf(buffer, _T("%s %s %s"), prodName, versionName, prodVersion);

    versionCtrl->SetText(buffer);
  }

  // Get the legal copyright string.
  //
  if (applVersion.GetCopyright(copyright))
    copyrightCtrl->SetText(copyright);

  // Only get the SpecialBuild text if the VERSIONINFO resource is there.
  //
  if (applVersion.GetDebug(debug))
    debugCtrl->SetText(debug);
}
