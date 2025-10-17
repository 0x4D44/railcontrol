//----------------------------------------------------------------------------
//  Project Themes
//  
//  (C) 2007 Sebastian Ledesma
//
//  SUBSYSTEM:    Themes Application
//  FILE:         thmsdlgc.h
//  AUTHOR:       Sebastian Ledesma
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TThemesDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(thmsdlgc_h)              // Sentry, use file only if it's not already included.
#define thmsdlgc_h

#include "themsapp.rh"                  // Definition of all resources.


#include <owl/commctrl.h>

#include <owl/tabctrl.h>
#include <owl/gauge.h>
#include <owl/progressbar.h>
#include <owl/listbox.h>
#include <owl/combobox.h>

#include <pshpack1.h>
struct TThemesDialogXfer {
  owl::TComboBoxData  pComboBox1;
  owl::TListBoxData  pListBox1;
};
#include <poppack.h>

class TThemesDialog : public owl::TDialog {
  public:
    TThemesDialog(owl::TWindow* parent, owl::TResId resId = IDD_CLIENT, owl::TModule* module = 0);
    virtual ~TThemesDialog();


  public:
    virtual void SetupWindow();
    virtual void CleanupWindow();

  protected:
    void EvTimer(owl::uint timerId);
    void OnButton1();
DECLARE_RESPONSE_TABLE(TThemesDialog);

  protected:
    owl::TComboBox* pComboBox1;
    owl::TListBox* pListBox1;
    owl::TGauge* pGauge;
    owl::TProgressBar* pProgress;
    owl::TTabControl* pTabControl;

};


#endif  // thmsdlgc_h sentry.
