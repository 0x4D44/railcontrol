#pragma once
#include <owl\mdi.h>

class TClassesMDIClient :
  public owl::TMDIClient
{
public:
  TClassesMDIClient();
  virtual ~TClassesMDIClient();

protected:
  void CmControls();
  void CmTCheckList();
  void CmTMonthCalendar();
  void CmTSlider();
  void CmTUrlLink();
  void CmTGlyphButton();
  void CmTValidate();
  void CmTNoteTab();
  void CmTGridPicker();
  void CmTLedClock();
  void CmTStatic();
  void CmTProgressDlg();

  void CmTScroller();
  void CmTTreeView();
  void CmTListView();
  void CmTBitmapView();
  void CmTTabControl();
  void CmTTabbedWindow();
  void CmOpenTTabbedWindow();
  void CmTLayoutWindow();
  void CmTTreeList();
  void CmTDragList();

  void CmCopyData();
  void CmCpuInfo();
  void CmExceptionTransport();
  void CmOpenDialog();

  void CmDialogDataTransfer();
  void CmTransfer();
  void CmTransferBuffer();

  void CmToggleCaption();
  void CeToggleCaption(owl::TCommandEnabler& c);

  void CmResizableDialog1();
  void CmRegistry();

DECLARE_RESPONSE_TABLE(TClassesMDIClient);
};

