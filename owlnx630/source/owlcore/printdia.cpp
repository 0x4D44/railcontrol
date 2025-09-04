//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.29.2.1 $
//$Author: jogybl $
//$Date: 2010-03-24 11:54:28 $
// 
// Implementation of class TPrintDialog, a Print and PrintSetup common Dialog
// encapsulation
//----------------------------------------------------------------------------
#if  defined(__BORLANDC__)
#pragma hdrignore SECTION 
#endif
#include <owl/pch.h>
#if !defined(OWL_PRINTDIA_H)
# include <owl/printdia.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
#endif

__OWL_BEGIN_NAMESPACE

OWL_DIAGINFO;

#if !defined(SECTION) || SECTION == 1

DEFINE_RESPONSE_TABLE1(TPrintDialog, TCommonDialog)
END_RESPONSE_TABLE;


//
//
//
TPrintDialog::TPrintDialog(TWindow*        parent,
                           TData&          data,
                           LPCTSTR         printTemplateName,
                           LPCTSTR         setupTemplateName,
                           LPCTSTR         title,
                           TModule*        module)
:
  TCommonDialog(parent, title, module),
  Data(data)
{
  memset(&Pd, 0, sizeof Pd);
  Pd.lStructSize = sizeof Pd;
  Pd.hwndOwner = Parent ? Parent->GetHandle() : 0;
  Pd.hInstance = *GetModule();
  Pd.Flags = PD_ENABLEPRINTHOOK | PD_ENABLESETUPHOOK | Data.Flags;
  Pd.Flags &= ~PD_RETURNDEFAULT;

  if (printTemplateName)
  {
    Pd.lpPrintTemplateName = printTemplateName;
    Pd.Flags |= PD_ENABLEPRINTTEMPLATE;
  }
  else
    Pd.Flags &= ~PD_ENABLEPRINTTEMPLATE;

  if (setupTemplateName) {
    Pd.lpSetupTemplateName = setupTemplateName;
    Pd.Flags |= PD_ENABLESETUPTEMPLATE;
  }
  else
    Pd.Flags &= ~PD_ENABLESETUPTEMPLATE;

  Pd.lpfnPrintHook = 0;
  Pd.lpfnSetupHook = 0;

  Pd.nFromPage = (uint16)Data.FromPage;
  Pd.nToPage = (uint16)Data.ToPage;
  Pd.nMinPage = (uint16)Data.MinPage;
  Pd.nMaxPage = (uint16)Data.MaxPage;
  Pd.nCopies = (uint16)Data.Copies;

  memset(&Psd, 0, sizeof Psd);
  Psd.lStructSize = sizeof Psd;
  Psd.hwndOwner = Parent ? Parent->GetHandle() : 0;
  Psd.hInstance = *GetModule();
  Psd.Flags = PSD_ENABLEPAGESETUPHOOK | PSD_ENABLEPAGEPAINTHOOK | Data.PageSetupFlags;

  if (setupTemplateName)
  {
    Psd.lpPageSetupTemplateName = setupTemplateName;
    Psd.Flags |= PSD_ENABLEPAGESETUPTEMPLATE;
  }
  else
    Psd.Flags &= ~PSD_ENABLEPAGESETUPTEMPLATE;

  Psd.lpfnPageSetupHook = 0;
  Psd.lpfnPagePaintHook = 0;

  Psd.ptPaperSize = Data.PaperSize;
  Psd.rtMinMargin = Data.MinMargin;
  Psd.rtMargin = Data.Margin;
  
#if(WINVER >= 0x0500)
  memset(&Pde, 0, sizeof Pde);
  Pde.lStructSize = sizeof Pde;

  // Note: PRINTDLGEX::hwndOwner cannot be NULL (see Windows API).
  // But, we need to maintain compatibility with older versions which allowed Parent == 0.
  // So, if no parent has been passed we'll try to assign the main window handle.
  // If this fails we'll handle the problem in DoExecute by reverting to the old dialog.

  TWindow* main_window = GetApplication() ? GetApplication()->GetMainWindow() : 0;
  Pde.hwndOwner = Parent ? Parent->GetHandle() : main_window ? main_window->GetHandle() : NULL; 

  Pde.hInstance = *GetModule();
  Pde.Flags = Data.Flags;  
  Pde.Flags &= ~PD_RETURNDEFAULT;
	Pde.Flags2 = 0;
  Pde.nStartPage = START_PAGE_GENERAL;   // Needed, won't work otherwise even for PD_RETURNDEFAULT

  if (printTemplateName)
  {
    Pde.lpPrintTemplateName = printTemplateName;
    Pde.Flags |= PD_ENABLEPRINTTEMPLATE;
    Pde.hInstance = module != 0 ? module->GetHandle() : GetModule()->GetHandle();
  }
  else
  {
    Pde.Flags &= ~PD_ENABLEPRINTTEMPLATE;
    Pde.hInstance = 0;
  }
    
/* //?
  if (setupTemplateName) 
  {
    Pde.lpSetupTemplateName = setupTemplateName;
    Pde.Flags |= PD_ENABLESETUPTEMPLATE;
  }
  else
    Pde.Flags &= ~PD_ENABLESETUPTEMPLATE;
*/

  Pde.lpCallback = 0; 

  Pde.nMaxPageRanges = 1;
  Pde.lpPageRanges = &PdeRange;
  Pde.lpPageRanges[0].nFromPage = (uint16)Data.FromPage;
  Pde.lpPageRanges[0].nToPage = (uint16)Data.ToPage;
  Pde.nMinPage = (uint16)Data.MinPage;
  Pde.nMaxPage = (uint16)Data.MaxPage;
  Pde.nCopies = (uint16)Data.Copies;
#endif //#if(WINVER >= 0x0500)
 	
}

//
//
//
bool
TPrintDialog::DialogFunction(uint msg, TParam1 param1, TParam2 param2)
{
  return TCommonDialog::DialogFunction(msg, param1, param2);
}


//
//
//
int
TPrintDialog::DoExecute()
{
  if (Data.DoPageSetup)
  {
    Psd.lpfnPageSetupHook = LPPAGESETUPHOOK(StdDlgProc);
    Psd.lpfnPagePaintHook = LPPAGEPAINTHOOK(StdDlgProc);

    Data.Unlock();

    Psd.hDevMode = Data.HDevMode;
    Psd.hDevNames = Data.HDevNames;

    int ret = TCommDlg::PageSetupDlg(&Psd);
    if (ret)
    {
      Data.PageSetupFlags = Psd.Flags;
      Data.Error = 0;
      Data.PaperSize = Psd.ptPaperSize;
      Data.MinMargin = Psd.rtMinMargin;
      Data.Margin = Psd.rtMargin;
    }
    else
    {
      Data.Error = TCommDlg::CommDlgExtendedError();
    }

    Data.HDevMode = Psd.hDevMode;
    Data.HDevNames = Psd.hDevNames;

    Data.Lock();

    return ret ? IDOK : IDCANCEL;
  }

  Pd.lpfnPrintHook = LPPRINTHOOKPROC(StdDlgProc);
  Pd.lpfnSetupHook = LPSETUPHOOKPROC(StdDlgProc);

  Data.Unlock();
  Pd.hDevMode = Data.HDevMode;
  Pd.hDevNames = Data.HDevNames;

  int result = IDCANCEL;  


#if(WINVER >= 0x0500)
	HWND hDummyWnd = NULL;

  if (!Data.UseOldDialog && TSystem::GetMajorVersion() > 4 && Pde.hwndOwner == NULL)
  {
  	//If no TWindow is provided, create a dummy window to pass to the new print dialog
  	hDummyWnd = ::CreateWindow(_T("STATIC"), _T(""), 0,
       0, 0, 0, 0,
       NULL, NULL,
       GetApplication() ? GetApplication()->GetHandle() : NULL, NULL);

    Pde.hwndOwner = hDummyWnd;
  }

  if (!Data.UseOldDialog && TSystem::GetMajorVersion() > 4 && Pde.hwndOwner != NULL)
  {
  	Pde.hDevMode = Data.HDevMode;
  	Pde.hDevNames = Data.HDevNames;
    CHECK(Pde.hwndOwner); // cannot be NULL
  	HRESULT ret = TCommDlg::PrintDlgEx(&Pde);
    if (ret == S_OK)
	  {
      if (!(Pde.Flags & PD_RETURNDEFAULT) && Pde.dwResultAction == PD_RESULT_CANCEL)
      {
      	Data.Error = 0;
        result = IDCANCEL;
      }
      else
      {
        Data.Flags = Pde.Flags;
        Data.Error = 0;
        Data.HDc = Pde.hDC;
        Data.FromPage = Pde.lpPageRanges[0].nFromPage;
        Data.ToPage = Pde.lpPageRanges[0].nToPage;
        Data.Copies = Pde.nCopies;

        if ((Pde.Flags & PD_RETURNDEFAULT) || Pde.dwResultAction == PD_RESULT_PRINT)
        {
          result = IDOK;
        }
        else
        {
          result = IDCANCEL;
        }

      }

    }
  	else
  	{
    	Data.Error = TCommDlg::CommDlgExtendedError();
      result = IDCANCEL;
  	}
  	Data.HDevMode = Pde.hDevMode;
  	Data.HDevNames = Pde.hDevNames;

    if (hDummyWnd != NULL)
    {
    	::DestroyWindow(hDummyWnd);
    }
  }
  else
#endif //#if(WINVER >= 0x0500)
  {
	  int ret = TCommDlg::PrintDlg(&Pd);
    if (ret)
	  {
  	  Data.Flags = Pd.Flags;
    	Data.Error = 0;
    	Data.HDc = Pd.hDC;
    	Data.FromPage = Pd.nFromPage;
    	Data.ToPage = Pd.nToPage;
    	Data.Copies = Pd.nCopies;
      result = IDOK;
  	}
  	else
  	{
    	Data.Error = TCommDlg::CommDlgExtendedError();
      result = IDCANCEL;
  	}
  	Data.HDevMode = Pd.hDevMode;
  	Data.HDevNames = Pd.hDevNames;
  }

	Data.Lock();
  return result;
}

//
//
//
bool
TPrintDialog::GetDefaultPrinter()
{
  Pd.Flags |= PD_RETURNDEFAULT;
#if(WINVER >= 0x0500)
  int flags = Pde.Flags;
  Pde.Flags = PD_RETURNDEFAULT;
#endif
  Data.ClearDevMode();
  Data.ClearDevNames();
  int result = DoExecute();
#if(WINVER >= 0x0500)
  Pde.Flags = flags;
#endif
  return result == IDOK;
}

//----------------------------------------------------------------------------

//
//
//
TPrintDialog::TData::TData()
:
  Flags(PD_ALLPAGES|PD_COLLATE),
  Error(0),
  FromPage(-1), ToPage(-1),
  MinPage(-1), MaxPage(-1),
  Copies(1),
  PageSetupFlags(PSD_DEFAULTMINMARGINS),
  DoPageSetup(false),
  UseOldDialog(false),
  HDevMode(0),
  HDevNames(0), HDc(0),
  DevMode(0),  DevNames(0)
{
}

//
//
//
TPrintDialog::TData::~TData()
{
  if (HDevMode) {
    ::GlobalUnlock(HDevMode);
    ::GlobalFree(HDevMode);
  }
  if (HDevNames) {
    ::GlobalUnlock(HDevNames);
    ::GlobalFree(HDevNames);
  }
  if (HDc)
    ::DeleteDC(HDc);
}

//
//
//
void
TPrintDialog::TData::Lock()
{
  if (HDevMode)
    DevMode = (DEVMODE *)::GlobalLock(HDevMode);
  else
    DevMode = 0;
  if (HDevNames)
    DevNames = (DEVNAMES *)::GlobalLock(HDevNames);
  else
    DevNames = 0;
}

//
//
//
void
TPrintDialog::TData::Unlock()
{
  if (HDevMode) {
    ::GlobalUnlock(HDevMode);
    DevMode = 0;
  }
  if (HDevNames) {
    ::GlobalUnlock(HDevNames);
    DevNames = 0;
  }
  if (HDc) {
    ::DeleteDC(HDc);
    HDc = 0;
  }
}

//
//
//
void
TPrintDialog::TData::ClearDevMode()
{
  if (HDevMode) {
    ::GlobalUnlock(HDevMode);
    ::GlobalFree(HDevMode);
    HDevMode = 0;
    DevMode = 0;
  }
}

//
//
//
void
TPrintDialog::TData::SetDevMode(const DEVMODE * devMode)
{
  ClearDevMode();
  if (devMode) {

  //!BB The DEVMODE structure now has additional members which
  //!BB are only relevant when running under Windows v4.0. Consequently,
  //!BB it's no longer safe to rely on the compile-time sizeof DEVMODE.
  //!BB The recommended/safe method is to use the dmSize member.
  //!BB
#if 0     //!BB
    int size = sizeof(DEVMODE) + devMode->dmDriverExtra;
#else
    int size = devMode->dmSize + devMode->dmDriverExtra;
#endif
    HDevMode = ::GlobalAlloc(GHND, size);
    DevMode = (DEVMODE *)::GlobalLock(HDevMode);
    memcpy(DevMode, devMode, size);
  }
}

//
//
//
void
TPrintDialog::TData::ClearDevNames()
{
  if (HDevNames) {
    ::GlobalUnlock(HDevNames);
    ::GlobalFree(HDevNames);
    HDevNames = 0;
    DevNames = 0;
  }
}

//
//
//
LPCTSTR
TPrintDialog::TData::GetDriverName() const
{
  return DevNames ? (LPTSTR)DevNames + DevNames->wDriverOffset : 0;
}

//
//
//
LPCTSTR
TPrintDialog::TData::GetDeviceName() const
{
  return DevNames ? (LPTSTR)DevNames + DevNames->wDeviceOffset : 0;
}

//
//
//
LPCTSTR
TPrintDialog::TData::GetOutputName() const
{
  return DevNames ? (LPTSTR)DevNames + DevNames->wOutputOffset : 0;
}

//
//
//
void
TPrintDialog::TData::SetDevNames(LPCTSTR driver,
                                 LPCTSTR device,
                                 LPCTSTR output)
{
  ClearDevNames();
  if (!driver || !device || !output)
    return;

  int size1 = ::_tcslen(driver)+1;
  int size2 = ::_tcslen(device)+1;
  int size3 = ::_tcslen(output)+1;
  HDevNames = ::GlobalAlloc(GHND, sizeof(DEVNAMES)+(size1+size2+size3) * sizeof(_TCHAR)); 
  DevNames = (DEVNAMES *)::GlobalLock(HDevNames);
  LPTSTR p = (LPTSTR)(DevNames + 1);

  DevNames->wDriverOffset = uint16((LPTSTR)p - (LPTSTR)DevNames);
  if (driver)
    while (*driver)
      *p++ = *driver++;
  *p++ = 0;

  DevNames->wDeviceOffset = uint16((LPTSTR)p - (LPTSTR)DevNames);
  if (device)
    while (*device)
      *p++ = *device++;
  *p++ = 0;

  DevNames->wOutputOffset = uint16((LPTSTR)p - (LPTSTR)DevNames);
  if (output)
    while (*output)
      *p++ = *output++;
  *p = 0;

  DevNames->wDefault = false;
}

//
// Pass ownership of our hDC to the caller thru a new's TPrintDC object
//
TPrintDC*
TPrintDialog::TData::TransferDC()
{
  if (!HDc)
    return 0;
  HDC hdc = HDc;
  HDc = 0;
  return new TPrintDC(hdc, AutoDelete);
}

#endif
#if !defined(SECTION) || SECTION == 2

//
// Read the persistent object from the stream.
//
#if !defined(__GNUC__) //JJH added pragma warn removal
#pragma warn -par
#endif

void*
TPrintDialog::TData::Read(ipstream& is, uint32 version)
{
  is >> Flags;
  is >> FromPage;
  is >> ToPage;
  is >> MinPage;
  is >> MaxPage;
  is >> Copies;
  char * driver = is.freadString();
  char * device = is.freadString();
  char * output = is.freadString();
  uint16 deflt;
  is >> deflt;
  _USES_CONVERSION;
  SetDevNames(_A2W(driver), _A2W(device), _A2W(output));
  if (DevNames)
    DevNames->wDefault = deflt;
  delete[] driver;
  delete[] device;
  delete[] output;

  int16 size;
  is >> size;
  if (size) {
    DEVMODE * devMode = (DEVMODE *)new  _TCHAR[size];
    is.freadBytes(devMode, size);
    SetDevMode(devMode);
    delete[] devMode;
  }
  else
    ClearDevMode();

  if (version > 1) {
    is >> PageSetupFlags;
    is >> PaperSize;
    is >> MinMargin;
    is >> Margin;
  }

  return this;
}
#if !defined(__GNUC__) //JJH added pragma warn removal
#pragma warn .par
#endif

//
// Write the object to a peristent stream.
//
void
TPrintDialog::TData::Write(opstream& os)
{
  os << Flags;
  os << FromPage;
  os << ToPage;
  os << MinPage;
  os << MaxPage;
  os << Copies;
  _USES_CONVERSION;
  os.fwriteString(_W2A(GetDriverName()));
  os.fwriteString(_W2A(GetDeviceName()));
  os.fwriteString(_W2A(GetOutputName()));
  os << (DevNames ? DevNames->wDefault : uint16(0));

  if (DevMode) {

  //!BB The DEVMODE structure now has additional members which
  //!BB are only relevant when running under Windows v4.0. Consequently,
  //!BB it's no longer safe to rely on the compile-time sizeof DEVMODE.
  //!BB The recommended/safe method is to use the dmSize member.
  //!BB
  //!BB Actually, using 'dmSize' is correct for determining the
  //!BB size at runtime. However, should we still stream the whole
  //!BB structure even if it contains members which are not relevant
  //!BB under the current environment???
  //!BB
#if 0 //!BB
    int16 size = int16(sizeof(DEVMODE) + DevMode->dmDriverExtra);
#else
    int16 size = int16(DevMode->dmSize + DevMode->dmDriverExtra);
#endif
    os << size;
    os.fwriteBytes(DevMode, size); //PROBLEM with UNICODE ????????????????????????????????????
  }
  else
    os << int16(0);

  os << PageSetupFlags;
  os << PaperSize;
  os << MinMargin;
  os << Margin;
}

#endif

__OWL_END_NAMESPACE
/* ========================================================================== */

