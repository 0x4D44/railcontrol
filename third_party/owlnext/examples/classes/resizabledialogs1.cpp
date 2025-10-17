#include "pch.h"
#include "resizabledialogs1.h"

#include "resizabledialogs.rh"

using namespace owl;

class TRepaintingControl : public TControl
{
public:
  TRepaintingControl(TWindow* parent, int id, LPCTSTR title, int x, int y, int w, int h, TModule* module = 0) : TControl(parent, id, title, x, y, w, h, module) {}
  TRepaintingControl(TWindow* parent, int id, const tstring& title, int x, int y, int w, int h, TModule* module = 0) : TControl(parent, id, title, x, y, w, h, module) {}
  TRepaintingControl(TWindow* parent, int resourceId, TModule* module = 0) : TControl(parent, resourceId, module) {}
  TRepaintingControl(TWindow* parent, int resourceId, const tstring& title, TModule* module = 0) : TControl(parent, resourceId, title, module) {}

protected:
  void EvWindowPosChanged(const WINDOWPOS& windowPos)
  {
    TControl::EvWindowPosChanged(windowPos);
    Invalidate(true);
  }

  DECLARE_RESPONSE_TABLE(TRepaintingControl);
};

DEFINE_RESPONSE_TABLE1(TRepaintingControl, TControl)
EV_WM_WINDOWPOSCHANGED,
END_RESPONSE_TABLE;


DEFINE_RESPONSE_TABLE1(TSampleResizableDialog1, TResizableDialog)
END_RESPONSE_TABLE;

TSampleResizableDialog1::TSampleResizableDialog1(TWindow *parent, TResId resId, TModule *module)
  : TResizableDialog(parent, resId == TResId(0) ? TResId(IDD_RESIZABLEDIALOG1) : resId, module), TLayoutWindow(parent)
{
  edit1 = new TEdit(this, IDC_EDIT1);
  combo1 = new TComboBox(this, IDC_COMBO1);

  listBox1 = new TListBox(this, IDC_LIST1);
  listView1 = new TListViewCtrl(this, IDC_LIST2);

  button1 = new TButton(this, IDC_BUTTON1);
  button2 = new TButton(this, IDC_BUTTON2);
  button3 = new TButton(this, IDC_BUTTON3);
  button4 = new TButton(this, IDC_BUTTON4);
  button5 = new TButton(this, IDC_BUTTON5);
  button6 = new TButton(this, IDC_BUTTON6);
  button7 = new TButton(this, IDC_BUTTON7);

  new TRepaintingControl(this, IDC_TEXT1);
  new TRepaintingControl(this, IDC_TEXT2);
}

TSampleResizableDialog1::~TSampleResizableDialog1()
{
}

void TSampleResizableDialog1::SetupWindow()
{
  TResizableDialog::SetupWindow();

  edit1->SetText(_T("Edit box"));

  combo1->SetText(_T("Combo box"));

  listView1->InsertColumn(0, TLvColumn(_T("Column 1"), 80));
  listView1->InsertColumn(1, TLvColumn(_T("Column 2"), 100));
  listView1->InsertColumn(2, TLvColumn(_T("Column 3"), 120, TLvColumn::Right));

  for (int index = 0; index < 10; ++index)
  {
    tostringstream sItem;
    sItem << _T("Item ") << index + 1;

    combo1->AddString(sItem.str());
    listBox1->AddString(sItem.str());

    TLvItem item(sItem.str());

    int row = listView1->AddItem(item);

    tostringstream sSubItem;
    sSubItem << _T("Subitem ") << index + 1;

    listView1->SetItem(TLvItem(sSubItem.str()), row, 1);
    listView1->SetItem(TLvItem(sSubItem.str()), row, 2);
  }
}

void TSampleResizableDialog1::SetupLayout()
{
  AnchorLeftRight(IDC_GROUPBOX1);
  AnchorLeftRight(*edit1);
  AnchorLeftRight(*combo1);

  AnchorTopBottom(IDC_GROUPBOX2);
  AnchorTopBottom(*listBox1);

  AnchorAll(IDC_GROUPBOX3);
  AnchorAll(*listView1);

  AnchorRight(IDC_GROUPBOX4);
  AnchorRight(*button1);
  AnchorRight(*button2);
  AnchorRight(*button3);

  AnchorBottom(IDC_GROUPBOX5);
  AnchorBottom(*button4);
  AnchorBottom(*button5);

  AnchorRightBottom(IDC_GROUPBOX6);
  AnchorRightBottom(*button6);
  AnchorRightBottom(*button7);

  AnchorLeftRight(IDC_TEXT1);
  AnchorLeftRight(IDC_TEXT2);
}
