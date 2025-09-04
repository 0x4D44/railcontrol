//----------------------------------------------------------------------------
// ObjectWindows
// Copyright (c) 1992, 1996 by Borland International, All Rights Reserved
//
//$Revision: 1.28 $
//
// Definition of Print and PrintSetup common Dialogs classes
//----------------------------------------------------------------------------

#if !defined(OWL_PRINTDIA_H)
#define OWL_PRINTDIA_H

#if !defined(OWL_COMMDIAL_H)
# include <owl/commdial.h>
#endif
#if !defined(OWL_DC_H)
# include <owl/dc.h>
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
// class TPrintDialog
// ~~~~~ ~~~~~~~~~~~~
class _OWLCLASS TPrintDialog : public TCommonDialog {
  public:
    class _OWLCLASS TData {
      public:
        uint32          Flags;
        uint32          Error;
        int             FromPage;
        int             ToPage;
        int             MinPage;
        int             MaxPage;
        int             Copies;

        uint32          PageSetupFlags;    // Additional page setup dialog flags
        TPoint          PaperSize;         // Size of the paper user has chosen
        TRect           MinMargin;         // Minimum allowable margins of the paper
        TRect           Margin;            // Initial margins for the paper
        bool            DoPageSetup;       // Flag to do page setup?
        bool						UseOldDialog;			 // Flag to force use of the old print dialog under Win2K/XP/

        TData();
       ~TData();
        void                Lock();
        void                Unlock();
        void                ClearDevMode();
        const DEVMODE *  GetDevMode() const;
        DEVMODE *        GetDevMode();
        void                SetDevMode(const DEVMODE * devMode);
        void                ClearDevNames();
        const DEVNAMES * GetDevNames() const;
        LPCTSTR     GetDriverName() const;
        LPCTSTR     GetDeviceName() const;
        LPCTSTR     GetOutputName() const;
        void                SetDevNames(LPCTSTR driver,
                                        LPCTSTR device,
                                        LPCTSTR output);
        TPrintDC*           TransferDC();

        void*               Read(ipstream& is, uint32 version);
        void                Write(opstream& os);

      private:
        HGLOBAL       HDevMode;
        HGLOBAL       HDevNames;
        HDC           HDc;

        DEVMODE *  DevMode;
        DEVNAMES * DevNames;

        TData(const TData&){}         // hidden to prevent copying & assignment
        TData& operator =(const TData&){ return *this;}

      friend class TPrintDialog;
    };

    TPrintDialog(TWindow*        parent,
                 TData&          data,
                 LPCTSTR printTemplateName = 0,
                 LPCTSTR setupTemplateName = 0,
                 LPCTSTR title = 0,
                 TModule*        module = 0);

    bool       GetDefaultPrinter();

    int        DoExecute();

  protected:
    bool    DialogFunction(uint message, TParam1, TParam2);

    // Default behavior inline for message response functions
    //
    void       CmSetup(); //EV_COMMAND(psh1,

    PRINTDLG&  GetPD();
    void       SetPD(const PRINTDLG& _pd);

    TData&     GetData();
    void       SetData(const TData& data);

  protected_data:
    union {
      PRINTDLG     pd;  // old name
      PRINTDLG     Pd;  // new name
    };
    PAGESETUPDLG   Psd;
  #if(WINVER >= 0x0500)
    PRINTDLGEX     Pde;  // new structure
    PRINTPAGERANGE PdeRange;
  #endif
    TData&         Data;

  private:
    TPrintDialog(const TPrintDialog&);
    TPrintDialog& operator=(const TPrintDialog&);

  DECLARE_RESPONSE_TABLE(TPrintDialog);
  DECLARE_CASTABLE;
};

// Generic definitions/compiler options (eg. alignment) following the
// definition of classes
#include <owl/posclass.h>

// ---------------------------------------------------------------------------
// Inline implementation
//
//

//
// Return the DEVMODE structure used by the dialog.
//
inline const DEVMODE *  TPrintDialog::TData::GetDevMode() const {
  return DevMode;
}

//
// Return the non-const DEVMODE structure used by the dialog.
//
inline DEVMODE *  TPrintDialog::TData::GetDevMode() {
  return DevMode;
}

//
// Return the DEVNAMES structure used by the dialog.
//
inline const DEVNAMES * TPrintDialog::TData::GetDevNames() const {
  return DevNames;
}

//
// Default processing.
//
inline void TPrintDialog::CmSetup() {
  DefaultProcessing();
}

//
// Return the PRINTDLG structure used by the dialog.
//
inline PRINTDLG& TPrintDialog::GetPD() {
  return pd;
}

//
// Set the PRINTDLG structure used by the dialog.
//
inline void TPrintDialog::SetPD(const PRINTDLG& _pd) {
  pd = _pd;
}

//
// Return the transfer data of the dialog.
//
inline TPrintDialog::TData& TPrintDialog::GetData() {
  return Data;
}

//
// Set the transfer data of the dialog.
//
inline void TPrintDialog::SetData(const TData& data) {
  Data = data;
}

__OWL_END_NAMESPACE


#endif  // OWL_PRINTDIA_H
