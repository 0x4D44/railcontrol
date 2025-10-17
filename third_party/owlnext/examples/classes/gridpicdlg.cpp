#include "pch.h"
#include "gridpicdlg.h"

#include "resource.h"

#include <owl/glyphbtn.h>

using namespace owl;

static COLORREF ColorTable16[16] = {
  { RGB(0x00,0x00,0x00) },  // Black
  { RGB(0x80,0x00,0x00) },  // Dark Red
  { RGB(0x00,0x80,0x00) },  // Green
  { RGB(0x80,0x80,0x00) },  // Dark yellow
  { RGB(0x00,0x00,0x80) },  // Dark Blue
  { RGB(0x80,0x00,0x80) },  // Violet
  { RGB(0x00,0x80,0x80) },  // Dark Olive Green
  { RGB(0x80,0x80,0x80) },  // Grey - 50
  { RGB(0xC0,0xC0,0xC0) },
  { RGB(0xFF,0x00,0x00) },
  { RGB(0x00,0xFF,0x00) },
  { RGB(0xFF,0xFF,0x00) },
  { RGB(0x00,0x00,0xFF) },
  { RGB(0xFF,0x00,0xFF) },
  { RGB(0x00,0xFF,0xFF) },
  { RGB(0xFF,0xFF,0xFF) }
};

static COLORREF ColorTable8[8] =
{
  RGB(0x00, 0x00, 0x00),
  RGB(0x7F, 0x7F, 0x7F),
  RGB(0x00, 0x00, 0xFF),
  RGB(0xFF, 0x00, 0x00),
  RGB(0xFF, 0xFF, 0x00),
  RGB(0x00, 0xFF, 0x00),
  RGB(0xC0, 0xC0, 0xC0),
  RGB(0xFF, 0xFF, 0xFF)
};

static COLORREF ColorTable40[40] =
{
  RGB(0x00, 0x00, 0x00),
  RGB(0xA5, 0x2A, 0x00),
  RGB(0x00, 0x40, 0x40),
  RGB(0x00, 0x55, 0x00),
  RGB(0x00, 0x00, 0x5E),
  RGB(0x00, 0x00, 0x8B),
  RGB(0x4B, 0x00, 0x82),
  RGB(0x28, 0x28, 0x28),
  RGB(0x8B, 0x00, 0x00),
  RGB(0xFF, 0x68, 0x20),
  RGB(0x8B, 0x8B, 0x00),
  RGB(0x00, 0x93, 0x00),
  RGB(0x38, 0x8E, 0x8E),
  RGB(0x00, 0x00, 0xFF),
  RGB(0x7B, 0x7B, 0xC0),
  RGB(0x66, 0x66, 0x66),
  RGB(0xFF, 0x00, 0x00),
  RGB(0xFF, 0xAD, 0x5B),
  RGB(0x32, 0xCD, 0x32),
  RGB(0x3C, 0xB3, 0x71),
  RGB(0x7F, 0xFF, 0xD4),
  RGB(0x7D, 0x9E, 0xC0),
  RGB(0x80, 0x00, 0x80),
  RGB(0x7F, 0x7F, 0x7F),
  RGB(0xFF, 0xC0, 0xCB),
  RGB(0xFF, 0xD7, 0x00),
  RGB(0xFF, 0xFF, 0x00),
  RGB(0x00, 0xFF, 0x00),
  RGB(0x40, 0xE0, 0xD0),
  RGB(0xC0, 0xFF, 0xFF),
  RGB(0x48, 0x00, 0x48),
  RGB(0xC0, 0xC0, 0xC0),
  RGB(0xFF, 0xE4, 0xE1),
  RGB(0xD2, 0xB4, 0x8C),
  RGB(0xFF, 0xFF, 0xE0),
  RGB(0x98, 0xFB, 0x98),
  RGB(0xAF, 0xEE, 0xEE),
  RGB(0x68, 0x83, 0x8B),
  RGB(0xE6, 0xE6, 0xFA),
  RGB(0xFF, 0xFF, 0xFF)
};



const int IDC_CONTROL1 = 10001;
const int IDC_CONTROL2 = 10002;
const int IDC_CONTROL3 = 10003;
const int IDC_CONTROL4 = 10004;
const int IDC_CONTROL5 = 10005;
const int IDC_CONTROL6 = 10006;
const int IDC_CONTROL7 = 10007;
const int IDC_CONTROL8 = 10008;
const int IDC_CONTROL9 = 10009;


TGridPickerDialog::TGridPickerDialog(TWindow* parent)
  : TDialog(parent, IDD_GRIDPICDIALOG)
{
  int i;
  //////////////////////////////////////////////////////////////////////////////
  CelArray = new TCelArray(new TBtnBitmap(*GetModule(), IDB_BMPARRAY), 14);

  //////////////////////////////////////////////////////////////////////////////
  Picker1 = new TGridPicker(this, IDC_CONTROL1, 0, 0, 80, 80);
  for (i = 0; i < 16; i++)
    Picker1->AddCell(new TColorCell(IDS_STRING0 + i, TColor(ColorTable16[i])));

  Picker2 = new TGridPicker(this, IDC_CONTROL2, 0, 100, 80, 100);
  Picker2->ModifyPopStyle(0, TGridPicker::psTrackMouse);
  Picker2->EnableDefault(_T("Default"), IDS_DEFFIELD,
    new TColorCell(IDS_STRING0 + 5, TColor(ColorTable16[5])));
  for (i = 0; i < 14; i++)
    Picker2->AddCell(new TColorCell(IDS_STRING0 + i, TColor(ColorTable16[i])));

  //////////////////////////////////////////////////////////////////////////////
  Picker3 = new TGridPicker(this, IDC_CONTROL3, 100, 0, 80, 80);
  Picker3->ModifyPopStyle(0, TGridPicker::psFlatSyle);
  for (i = 0; i < 16; i++)
    Picker3->AddCell(new TColorCell(IDS_STRING0 + i, TColor(ColorTable16[i])));

  Picker4 = new TGridPicker(this, IDC_CONTROL4, 100, 100, 80, 130);
  Picker4->ModifyPopStyle(0, TGridPicker::psTrackMouse | TGridPicker::psFlatSyle |
    TGridPicker::psMessageTips | TGridPicker::psLocalTips);
  Picker4->EnableDefault(_T("Default"), IDS_DEFFIELD,
    new TColorCell(IDS_STRING0 + 5, TColor(ColorTable16[5])));
  Picker4->EnableTooltip();
  for (i = 0; i < 14; i++)
    Picker4->AddCell(new TColorCell(IDS_STRING0 + i, TColor(ColorTable16[i])));

  //////////////////////////////////////////////////////////////////////////////
  Picker5 = new TGridPicker(this, IDC_CONTROL5, 200, 0);
  for (i = 0; i < 14; i++)
    Picker5->AddCell(new TBitmapCell(IDS_CHART0 + i, CelArray, i));

  Picker6 = new TGridPicker(this, IDC_CONTROL6, 200, 100);
  Picker6->ModifyPopStyle(0, TGridPicker::psTrackMouse | TGridPicker::psLocalTips);
  for (i = 0; i < 14; i++)
    Picker6->AddCell(new TBitmapCell(IDS_CHART0 + i, CelArray, i));

  //////////////////////////////////////////////////////////////////////////////
  Picker7 = new TGridPicker(this, IDC_CONTROL7, 300, 0);
  Picker7->SetColumns(8);
  Picker7->ModifyPopStyle(0, TGridPicker::psFlatSyle);
  for (i = 0; i < 14; i++)
    Picker7->AddCell(new TBitmapCell(IDS_CHART0 + i, CelArray, i));

  //////////////////////////////////////////////////////////////////////////////
  Picker8 = new TGridPicker(this, IDC_CONTROL8, 300, 80);
  Picker8->ModifyStyle(0, BS_NOTIFY);
  Picker8->SetColumns(4);
  Picker8->ModifyPopStyle(0, TGridPicker::psTrackMouse | TGridPicker::psFlatSyle |
    TGridPicker::psLocalTips | TGridPicker::psShrinkToCells);
  Picker8->EnableDefault(_T("Default Item"), IDS_DEFFIELD,
    new TBitmapCell(IDS_CHART0 + 5, CelArray, 5));
  Picker8->EnableButton(_T("Custom field"), IDS_CUSTFIELD);
  Picker8->EnableTooltip();
  TMargins margins(TMargins::Pixels, 4, 4, 4, 4);
  Picker8->SetMargins(margins);

  for (i = 0; i < 14; i++)
    Picker8->AddCell(new TBitmapCell(IDS_CHART0 + i, CelArray, i));


  // This class is intended to be used in a control bar. Putting it as a child of a dialog is a bit of a hack, and not everything functions properly.
  ColorPicker = new TColorPicker(this, IDC_CONTROL9, 500, 10, 240, 200);
  ColorPicker->ModifyStyle(WS_POPUP, WS_VISIBLE | WS_CHILD); //Important - reverse the styles set in TColorPicker constructor, as the controll will not be able to be created
  ColorPicker->SetColumns(8);
  ColorPicker->ModifyPopStyle(0, TGridPicker::psTrackMouse | TGridPicker::psFlatSyle |
    TGridPicker::psMessageTips);
  ColorPicker->EnableDefault(_T("Default"), IDS_DEFFIELD,
    new TColorCell(IDS_STRING1 + 5, TColor(ColorTable40[5])));
  ColorPicker->EnableButton(_T("More Colors..."), IDS_CUSTFIELD);
  ColorPicker->EnableTooltip();

  for (int i = 0; i < 39; i++)
    ColorPicker->AddCell(new TColorCell(IDS_STRING1 + i, TColor(ColorTable40[i])));

  ColorPicker->SetSelection();
}


TGridPickerDialog::~TGridPickerDialog()
{
}

void TGridPickerDialog::SetupWindow()
{
  TDialog::SetupWindow();
}