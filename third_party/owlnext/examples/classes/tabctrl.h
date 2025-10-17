#pragma once

#include <owl/mdichild.h>
#include <owl/notetab.h>
#include <owl/tabctrl.h>

const int TabCtlID = 0x100;        // ID of the TabControl

class TTabControlWindow :
  public owl::TMDIChild
{
public:
  TTabControlWindow(owl::TMDIClient&     parent,
    LPCTSTR         title = 0,
    owl::TWindow*        clientWnd = 0,
    bool            shrinkToClient = false,
    owl::TModule*        module = 0);

protected:

  void AdjustTabClient();

  // Message Handlers
  //
  void EvSize(owl::uint sizeType, const owl::TSize& size);
  void TabSelChange(owl::TNotify&);
  bool TabSelChanging(owl::TNotify&);
  void TabKeyDown(owl::TTabKeyDown&);

  void CmNewTab();
  void CmAddItem();
  void CmRemoveItem();
  void CeRemoveItem(owl::TCommandEnabler& ce);
  void CmDeleteAll();
  void CeDeleteAll(owl::TCommandEnabler& ce);
  void CmChangeItem();
  void CeChangeItem(owl::TCommandEnabler& ce);
  void CmInfo();
  void CmVetoSelChanging();
  void CeVetoSelChanging(owl::TCommandEnabler& ce);

private:

  owl::TTabControl*  Tab;
  bool VetoSelChanging;
  owl::TWindow* Client;

  DECLARE_RESPONSE_TABLE(TTabControlWindow);

};


class TNoteTabDlg : public owl::TDialog {
public:
  TNoteTabDlg(owl::TWindow* parent, owl::TResId id);

  void      SetupWindow();

protected:
  owl::TNoteTab* NoteTab;

  // Event handlers
  //
  void      TabSelChange(owl::TNotify &);

  DECLARE_RESPONSE_TABLE(TNoteTabDlg);
};


