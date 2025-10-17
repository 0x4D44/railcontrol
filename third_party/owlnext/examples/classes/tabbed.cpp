//
// Tabbed - test-case for TTabbedBase and TTabbedWindow, and the underlying TNoteTab
//
#include "pch.h"
#include "client.h"
#include "tabbed.rh"
#include <owl/tabbed.h>
#include <owl/celarray.h>
#include <owl/transfer.h>
#include <owl/updown.h>
#include <owl/inputdia.h>
#include <owl/choosefo.h>
#include <owl/chooseco.h>
#include <owl/mdi.h>
#include <owl/mdichild.h>
#include <owl/applicat.h>
#include <owl/uimetric.h>
#include <owl/opensave.h>
#include <owl/configfl.h>
#include <memory>

using namespace owl;

static int ChildNum;
static tchar ConfigurationFileFilter[] = _T("Configuration files (*.ini)|*.ini");

// 
// TTabbedWindow test case
//
class TTestWindow
  : public TTabbedWindow
{
public:

  TTestWindow(TWindow* parent = 0)
    : TTabbedWindow(parent),
    ResizingState(rsInactive)
  {
    // Load tab icons.
    //
    TNoteTab* t = GetTabControl();
    std::unique_ptr<TBitmap> b(new TBitmap(GetModule()->GetHandle(), IDB_IMAGES));
    const int iconCount = 7;
    std::unique_ptr<TCelArray> c(new TCelArray(b.get(), iconCount));
    b.release(); // The cel array has taken ownership of the bitmap.
    t->SetCelArray(c.get());
    c.release(); // The tab control has taken ownership of the cel array.
    t->SetCelArrayTransparentColor(TColor::LtMagenta);

    // Create a default page for every icon.
    //
    for (int i = 0; i != iconCount; ++i)
    {
      TWindow* w = new TWindow(this, GetNameForWindow(i));
      w->SetBkgndColor(GetColorForWindow(i));
      AddPage(*w);
      TNoteTabItem tab = t->GetItem(i);
      tab.ImageIdx = i % iconCount;
      t->SetItem(i, tab);
    }
    t->SetSel(0);
  }

  void OpenWindow(TConfigFile& f)
  {
    TConfigFileSection s(f, _T("Window0"));
    if (!f.SectionExists(s.GetSection()))
      throw std::runtime_error("The file does not contain the required Window0 section.");

    TTestWindow* w = this;

    TNoteTab* t = w->GetTabControl(); CHECK(t);

    // Restore window state.
    //
    int h = s.ReadInteger(_T("TabControlHeight"), t->GetClientRect().Height());
    int marginX = s.ReadInteger(_T("Margin.X"), t->GetMargin().cx);
    int marginY = s.ReadInteger(_T("Margin.Y"), t->GetMargin().cy);
    t->SetMargin(TSize(marginX, marginY));

    int labelMarginX = s.ReadInteger(_T("LabelMargin.X"), t->GetLabelMargin().cx);
    int labelMarginY = s.ReadInteger(_T("LabelMargin.Y"), t->GetLabelMargin().cy);
    t->SetLabelMargin(TSize(labelMarginX, labelMarginY));

    int focusMarginX = s.ReadInteger(_T("FocusMargin.X"), t->GetFocusMargin().cx);
    int focusMarginY = s.ReadInteger(_T("FocusMargin.Y"), t->GetFocusMargin().cy);
    t->SetFocusMargin(TSize(focusMarginX, focusMarginY));

    t->SetTabSpacing(s.ReadInteger(_T("TabSpacing"), t->GetTabSpacing()));
    t->SetTabTapering(s.ReadInteger(_T("TabTapering"), t->GetTabTapering()));
    t->SetSelectedTabProtrusion(s.ReadInteger(_T("SelectedTabProtrusion"), t->GetSelectedTabProtrusion()));

    LOGFONT font = t->GetTabFont().GetObject();
    s.ReadString(_T("TabFont.FaceName"), font.lfFaceName, LF_FACESIZE, font.lfFaceName);
    font.lfHeight = s.ReadInteger(_T("TabFont.Height"), font.lfHeight);
    font.lfWeight = s.ReadInteger(_T("TabFont.Weight"), FW_NORMAL);
    font.lfItalic = s.ReadInteger(_T("TabFont.Italic"), FALSE);
    t->SetTabFont(TFont(font));

    LOGFONT selectedFont = t->GetSelectedTabFont().GetObject();
    s.ReadString(_T("SelectedTabFont.FaceName"), selectedFont.lfFaceName, LF_FACESIZE, font.lfFaceName);
    selectedFont.lfHeight = s.ReadInteger(_T("SelectedTabFont.Height"), font.lfHeight);
    selectedFont.lfWeight = s.ReadInteger(_T("SelectedTabFont.Weight"), font.lfWeight);
    selectedFont.lfItalic = s.ReadInteger(_T("SelectedTabFont.Italic"), font.lfItalic);
    t->SetSelectedTabFont(TFont(selectedFont));

    t->SetEdgeColor(static_cast<COLORREF>(s.ReadInteger(_T("EdgeColor"), t->GetEdgeColor().GetValue())));
    t->SetTabColor(static_cast<COLORREF>(s.ReadInteger(_T("TabColor"), t->GetTabColor().GetValue())));
    t->SetSelectedTabColor(static_cast<COLORREF>(s.ReadInteger(_T("SelectedTabColor"), t->GetSelectedTabColor().GetValue())));
    t->SetBkgndColor(static_cast<COLORREF>(s.ReadInteger(_T("BkgndColor"), t->GetBkgndColor().GetValue())));

    t->SetWindowFace(s.ReadInteger(_T("WindowFace"), t->GetWindowFace() ? 1 : 0) == 1);;
    t->SetStyle3d(s.ReadInteger(_T("Style3d"), t->GetStyle3d() ? 1 : 0) == 1);
    t->EnableThemes(s.ReadInteger(_T("Themes"), t->AreThemesEnabled() ? 1 : 0) == 1);;
    t->SetScrollLocation(static_cast<TAbsLocation>(s.ReadInteger(_T("ScrollLocation"), static_cast<int>(t->GetScrollLocation()))));

    int tabCount = s.ReadInteger(_T("TabCount"), 0);
    for (int i = 0; i != tabCount; ++i)
    {
      tostringstream tabSectionName; tabSectionName << s.GetSection() << _T(".Tab") << i;
      TConfigFileSection ts(f, tabSectionName.str());
      tstring label; ts.ReadString(_T("Label"), label);
      int imageIdx = ts.ReadInteger(_T("ImageIdx"));
      TAbsLocation imageLoc = static_cast<TAbsLocation>(ts.ReadInteger(_T("ImageLoc")));
      TColor pageBkgndColor = static_cast<COLORREF>(ts.ReadInteger(_T("ClientData.BkgndColor")));

      TWindow* page = new TWindow(w, label);
      page->SetBkgndColor(pageBkgndColor);
      int pageIndex = w->AddPage(*page);
      TNoteTabItem tab = t->GetItem(pageIndex);
      tab.ImageIdx = imageIdx;
      tab.ImageLoc = imageLoc;
      t->SetItem(pageIndex, tab);
    }

    // Remove any old default pages we had before loading the new.
    //
    while (t->GetCount() > tabCount)
    {
      std::unique_ptr<TWindow> defaultPage(w->GetPage(0));
      w->DeletePage(*defaultPage);
    }

    // Initialize the window.
    //
    w->ResizeTabControl(h);
    t->SetFirstVisibleTab(0);
  }

  void SaveWindow(TConfigFile& f, TTestWindow* w)
  {
    TConfigFileSection s(f, _T("Window0"));
    TNoteTab* t = w->GetTabControl(); CHECK(t);

    s.WriteInteger(_T("TabControlHeight"), t->GetClientRect().Height());
    s.WriteInteger(_T("Margin.X"), t->GetMargin().cx);
    s.WriteInteger(_T("Margin.Y"), t->GetMargin().cy);
    s.WriteInteger(_T("LabelMargin.X"), t->GetLabelMargin().cx);
    s.WriteInteger(_T("LabelMargin.Y"), t->GetLabelMargin().cy);
    s.WriteInteger(_T("FocusMargin.X"), t->GetFocusMargin().cx);
    s.WriteInteger(_T("FocusMargin.Y"), t->GetFocusMargin().cy);
    s.WriteInteger(_T("TabSpacing"), t->GetTabSpacing());
    s.WriteInteger(_T("TabTapering"), t->GetTabTapering());
    s.WriteInteger(_T("SelectedTabProtrusion"), t->GetSelectedTabProtrusion());

    LOGFONT font = t->GetTabFont().GetObject();
    s.WriteString(_T("TabFont.FaceName"), font.lfFaceName);
    s.WriteInteger(_T("TabFont.Height"), font.lfHeight);
    s.WriteInteger(_T("TabFont.Weight"), font.lfWeight);
    s.WriteInteger(_T("TabFont.Italic"), font.lfItalic);

    LOGFONT selectedFont = t->GetSelectedTabFont().GetObject();
    s.WriteString(_T("SelectedTabFont.FaceName"), selectedFont.lfFaceName);
    s.WriteInteger(_T("SelectedTabFont.Height"), selectedFont.lfHeight);
    s.WriteInteger(_T("SelectedTabFont.Weight"), selectedFont.lfWeight);
    s.WriteInteger(_T("SelectedTabFont.Italic"), selectedFont.lfItalic);

    s.WriteInteger(_T("EdgeColor"), t->GetEdgeColor().GetValue());
    s.WriteInteger(_T("TabColor"), t->GetTabColor().GetValue());
    s.WriteInteger(_T("SelectedTabColor"), t->GetSelectedTabColor().GetValue());
    s.WriteInteger(_T("BkgndColor"), t->GetBkgndColor().GetValue());

    s.WriteInteger(_T("WindowFace"), t->GetWindowFace() ? 1 : 0);
    s.WriteInteger(_T("Style3d"), t->GetStyle3d() ? 1 : 0);
    s.WriteInteger(_T("Themes"), t->AreThemesEnabled() ? 1 : 0);
    s.WriteInteger(_T("ScrollLocation"), static_cast<int>(t->GetScrollLocation()));

    int tabCount = t->GetCount();
    s.WriteInteger(_T("TabCount"), tabCount);
    for (int i = 0; i != tabCount; ++i)
    {
      const TNoteTabItem& n = t->GetItem(i);
      tostringstream tabSectionName; tabSectionName << s.GetSection() << _T(".Tab") << i;
      TConfigFileSection ts(f, tabSectionName.str());

      ts.WriteString(_T("Label"), n.Label);
      ts.WriteInteger(_T("ImageIdx"), n.ImageIdx);
      ts.WriteInteger(_T("ImageLoc"), static_cast<int>(n.ImageLoc));

      TWindow* page = reinterpret_cast<TWindow*>(n.ClientData); CHECK(page);
      ts.WriteInteger(_T("ClientData.BkgndColor"), page->GetBkgndColor().GetValue());
    }
  }

private:

  const UINT_PTR ResizingTimerId = 1;
  enum TResizingState
  {
    rsInactive,
    rsStart,
    rsShrinkToHalf,
    rsGrowTowardsRightEdge,
    rsShrinkTowardsRightEdge,
    rsGrowTowardsLeftEdge,
    rsShrinkTowardsLeftEdge,
    rsGrowTowardsOriginalPosition,
    rsShrinkTowardsOriginalPosition,
    rsGrowToMax,
    rsRestoreToOriginalPosition,
    rsDone
  };
  TResizingState ResizingState;
  TRect ResizingOriginalRect;
  enum TTabControlParameter
  {
    tcpTabControlHeight,
    tcpMarginX,
    tcpMarginY,
    tcpLabelMarginX,
    tcpLabelMarginY,
    tcpFocusMarginX,
    tcpFocusMarginY,
    tcpTabSpacing,
    tcpTabTapering,
    tcpSelectedTabProtrusion
  };
  static TTabControlParameter TabControlParameter;
  static TColor CustomColors[16];

  typedef TTransferDialog<TNoteTabItem> TPageDialogBase;

  class TPageDialog
    : public TPageDialogBase
  {
  public:

    TPageDialog(TWindow* parent, TNoteTabItem& d)
      : TPageDialogBase(parent, IDD_PAGE, d),
      ImageIndexSpin(this, IDC_IMAGEINDEXSPIN)
    {}

  protected:

    TUpDown ImageIndexSpin;

    virtual void SetupWindow() // override
    {
      TPageDialogBase::SetupWindow();
      ImageIndexSpin.SetRange(-100, 100); // Allow out-of-bounds for testing.
    }

    virtual void DoTransferData(const TTransferInfo& i, TNoteTabItem& d) // override
    {
      TransferEditData(i, IDC_LABEL, d.Label);
      TransferEditData(i, IDC_IMAGEINDEX, d.ImageIdx);
      struct TAlignmentProxy
      {
        TNoteTabItem& d;
        int Get() const { return d.ImageLoc == alLeft ? 0 : 1; }
        void Set(int v) { d.ImageLoc = (v == 0) ? alLeft : alRight; }
      };
      TAlignmentProxy ap = { d };
      TransferRadioButtonData(i, IDC_IMAGELEFT, &ap, &TAlignmentProxy::Get, &TAlignmentProxy::Set);
    }

  };

  //TODO: Maybe move to main window, as it can apply to all types of child windows, just like Toggle Caption
  void CmResizing()
  {
    if (ResizingState == rsInactive)
    {
      ResizingState = rsStart;
      SetTimer(ResizingTimerId, 17); // ~60 Hz
    }
    else
    {
      ResizingState = rsInactive;
      GetParent()->MoveWindow(ResizingOriginalRect, true);
    }
  }

  void CeResizing(TCommandEnabler& c)
  {
    return c.SetCheck(ResizingState != rsInactive);
  }

  void CmAddPage()
  {
    TNoteTab* t = GetTabControl();
    int i = t->GetCount();
    tostringstream s;
    s << _T("Page #") << i;
    TNoteTabItem dlgData(s.str(), 0, -1, owl::alLeft);
    TPageDialog dlg(this, dlgData);
    dlg.SetCaption(_T("Add page"));
    if (dlg.Execute() != IDOK) return;

    TWindow* w = new TWindow(this, dlgData.Label);
    w->SetBkgndColor(GetColorForWindow(i));
    int pageIndex = AddPage(*w, dlgData.Label);
    TNoteTabItem n = t->GetItem(pageIndex);
    n.ImageIdx = dlgData.ImageIdx;
    n.ImageLoc = dlgData.ImageLoc;
    bool r = t->SetItem(pageIndex, n); CHECK(r);
  }

  void CmEditPage()
  {
    TNoteTab* t = GetTabControl();
    int i = t->GetSel();
    TNoteTabItem n = t->GetItem(i);
    TPageDialog dlg(this, n);
    dlg.SetCaption(_T("Edit page"));
    if (dlg.Execute() != IDOK) return;

    bool r = t->SetItem(i, n); CHECK(r);
  }

  void CmDeletePage()
  {
    PRECONDITION(GetTabControl()->GetCount() > 1);
    TInputDialog dlg(this, _T("Delete page"), _T("Page index:"));
    if (dlg.Execute() != IDOK) return;

    int i = -1; tistringstream(dlg.GetBuffer()) >> i;
    if (i < 0) return;
    TNoteTab* t = GetTabControl();
    TNoteTabItem n = t->GetItem(i);
    TWindow* w = reinterpret_cast<TWindow*>(n.ClientData);
    DeletePage(*w);
  }

  void CeDeletePage(TCommandEnabler& ce)
  {
    ce.Enable(GetTabControl()->GetCount() > 1);
  }

  void CmDeleteSelectedPage()
  {
    PRECONDITION(GetTabControl()->GetCount() > 1);
    TNoteTab* t = GetTabControl();
    TNoteTabItem n = t->GetItem(t->GetSel());
    TWindow* w = reinterpret_cast<TWindow*>(n.ClientData);
    DeletePage(*w);
  }

  void CmSelectPage()
  {
    TInputDialog dlg(this, _T("Select page"), _T("Page index:"));
    if (dlg.Execute() != IDOK) return;

    int i = -1; tistringstream(dlg.GetBuffer()) >> i;
    if (i < 0) return;
    GetTabControl()->SetSel(i);
  }

  void CmSetFirstVisibleTab()
  {
    TInputDialog dlg(this, _T("Set first visible tab"), _T("Tab index:"));
    if (dlg.Execute() != IDOK) return;

    int i = -1; tistringstream(dlg.GetBuffer()) >> i;
    if (i < 0) return;
    GetTabControl()->SetFirstVisibleTab(i);
  }

  void CmEnsureTabIsVisible()
  {
    TInputDialog dlg(this, _T("Ensure tab is visible"), _T("Tab index:"));
    if (dlg.Execute() != IDOK) return;

    int i = -1; tistringstream(dlg.GetBuffer()) >> i;
    if (i < 0) return;
    GetTabControl()->EnsureVisible(i);
  }

  template <TTabControlParameter p>
  void CmSelectParameter()
  {
    TabControlParameter = p;
  }

  template <TTabControlParameter p>
  void CeSelectParameter(TCommandEnabler& c)
  {
    c.SetCheck(TabControlParameter == p);
  }

  void CmIncreaseParameter()
  {
    ChangeTabControlParameter(+1);
  }

  void CmDecreaseParameter()
  {
    ChangeTabControlParameter(-1);
  }

  void CmSetTabFont()
  {
    TNoteTab* t = GetTabControl();
    TChooseFontDialog::TData d;
    d.LogFont = t->GetTabFont().GetObject();
    d.Flags = CF_INITTOLOGFONTSTRUCT;
    TChooseFontDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetTabFont(TFont(d.LogFont));
  }

  void CmSetSelectedTabFont()
  {
    TNoteTab* t = GetTabControl();
    TChooseFontDialog::TData d;
    d.LogFont = t->GetSelectedTabFont().GetObject();
    d.Flags = CF_INITTOLOGFONTSTRUCT;
    TChooseFontDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetSelectedTabFont(TFont(d.LogFont));
  }

  void CmSetTabEdgeColor()
  {
    TNoteTab* t = GetTabControl();
    TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, t->GetEdgeColor());
    d.CustColors = &CustomColors[0];
    TChooseColorDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetEdgeColor(d.Color);
    t->Invalidate();
  }

  void CmSetTabColor()
  {
    TNoteTab* t = GetTabControl();
    TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, t->GetTabColor());
    d.CustColors = &CustomColors[0];
    TChooseColorDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetTabColor(d.Color);
    t->Invalidate();
  }

  void CmSetSelectedTabColor()
  {
    TNoteTab* t = GetTabControl();
    TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, t->GetSelectedTabColor());
    d.CustColors = &CustomColors[0];
    TChooseColorDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetSelectedTabColor(d.Color);
    t->Invalidate();
  }

  void CmSetTabBackgroundColor()
  {
    TNoteTab* t = GetTabControl();
    TChooseColorDialog::TData d(CC_RGBINIT | CC_FULLOPEN, t->GetBkgndColor());
    d.CustColors = &CustomColors[0];
    TChooseColorDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;

    t->SetBkgndColor(d.Color);
    t->Invalidate();
  }

  void Cm3dStyle()
  {
    TNoteTab* t = GetTabControl();
    if (!t->AreThemesEnabled() && t->GetStyle3d() && !t->GetWindowFace()) return;
    t->EnableThemes(false);
    t->SetStyle3d(true);
    t->SetWindowFace(false);
  }

  void Ce3dStyle(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(!t->AreThemesEnabled() && t->GetStyle3d());
  }

  void CmFlatStyle()
  {
    TNoteTab* t = GetTabControl();
    if (!t->AreThemesEnabled() && !t->GetStyle3d() && !t->GetWindowFace()) return;
    t->EnableThemes(false);
    t->SetStyle3d(false);
    t->SetWindowFace(false);
  }

  void CeFlatStyle(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(!t->AreThemesEnabled() && !t->GetStyle3d() && !t->GetWindowFace());
  }

  void CmFlatWindowFaceStyle()
  {
    TNoteTab* t = GetTabControl();
    if (!t->AreThemesEnabled() && !t->GetStyle3d() && t->GetWindowFace()) return;
    t->EnableThemes(false);
    t->SetStyle3d(false);
    t->SetWindowFace(true);
  }

  void CeFlatWindowFaceStyle(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(!t->AreThemesEnabled() && !t->GetStyle3d() && t->GetWindowFace());
  }

  void CmThemedStyle()
  {
    TNoteTab* t = GetTabControl();
    if (t->AreThemesEnabled()) return;
    t->EnableThemes(true);
    t->SetStyle3d(false);
    t->SetWindowFace(false);
  }

  void CeThemedStyle(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(t->AreThemesEnabled());
  }

  void CmScrollButtonsLeft()
  {
    TNoteTab* t = GetTabControl();
    t->SetScrollLocation(alLeft);
  }

  void CeScrollButtonsLeft(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(t->GetScrollLocation() == alLeft);
  }

  void CmScrollButtonsRight()
  {
    TNoteTab* t = GetTabControl();
    t->SetScrollLocation(alRight);
  }

  void CeScrollButtonsRight(TCommandEnabler& c)
  {
    TNoteTab* t = GetTabControl();
    c.SetCheck(t->GetScrollLocation() == alRight);
  }

  void CmSaveWindow()
  {
    TOpenSaveDialog::TData d(OFN_NOREADONLYRETURN, ConfigurationFileFilter, NULL, NULL, _T("ini"));
    TFileSaveDialog dlg(this, d);
    if (dlg.Execute() != IDOK) return;
    try
    {
      TTestWindow* w = this;
      TIniConfigFile f(d.FileName);
      SaveWindow(f, w);
    }
    catch (const std::runtime_error& x)
    {
      ::MessageBoxA(GetHandle(), x.what(), "Save error", MB_OK | MB_ICONERROR);
    }
  }

  void EvTimer(uint timerId)
  {
    if (timerId == ResizingTimerId)
      UpdateResizingState();
  }

  void UpdateResizingState()
  {
    TWindow* w = GetParent();
    TWindow* m = w->GetParent();
    CHECK(dynamic_cast<TMDIClient*>(m));
    TRect a = m->GetClientRect();
    TRect r = m->GetChildRect(*w);
    switch (ResizingState)
    {
    case rsStart: // Save original state.
      ResizingOriginalRect = r;
      AdvanceResizingState(ResizingState);
      break;

    case rsShrinkToHalf:
      r.right -= Delta(r);
      if (r.Width() <= ResizingOriginalRect.Width() / 2)
        AdvanceResizingState(ResizingState);
      break;

    case rsGrowTowardsRightEdge:
      r.right += Delta(r);
      if (r.right >= a.right)
      {
        r.right = a.right;
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsShrinkTowardsRightEdge:
      r.left += Delta(r);
      if (r.Width() <= ResizingOriginalRect.Width())
      {
        r.left = r.right - ResizingOriginalRect.Width();
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsGrowTowardsLeftEdge:
      r.left -= Delta(r);
      if (r.left <= a.left) // > 0
      {
        r.left = a.left;
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsShrinkTowardsLeftEdge:
      r.right -= Delta(r);
      if (r.Width() <= ResizingOriginalRect.Width())
      {
        r.right = r.left + ResizingOriginalRect.Width();
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsGrowTowardsOriginalPosition:
      r.right += Delta(r);
      if (r.right >= ResizingOriginalRect.right)
      {
        r.right = ResizingOriginalRect.right;
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsShrinkTowardsOriginalPosition:
      r.left += Delta(r);
      if (r.Width() <= ResizingOriginalRect.Width())
      {
        r.left = ResizingOriginalRect.left;
        AdvanceResizingState(ResizingState);
      }
      break;

    case rsGrowToMax:
    {
      int d = Delta(r) / 2;
      r.Inflate(d, d);
      if (!a.Contains(r))
      {
        if (r.left < a.left) r.left = a.left;
        if (r.top < a.top) r.top = a.top;
        if (r.right > a.right) r.right = a.right;
        if (r.bottom > a.bottom) r.bottom = a.bottom;
        if (r == a)
          AdvanceResizingState(ResizingState);
      }
      break;
    }

    case rsRestoreToOriginalPosition:
    {
      int d = -Delta(r) / 3;
      r.Inflate(d, d);
      if (r.left > ResizingOriginalRect.left) r.left = ResizingOriginalRect.left;
      if (r.top > ResizingOriginalRect.top) r.top = ResizingOriginalRect.top;
      if (r.right < ResizingOriginalRect.right) r.right = ResizingOriginalRect.right;
      if (r.bottom < ResizingOriginalRect.bottom) r.bottom = ResizingOriginalRect.bottom;
      if (r == ResizingOriginalRect)
        AdvanceResizingState(ResizingState);
      break;
    }

    case rsDone:
      KillTimer(ResizingTimerId);
      AdvanceResizingState(ResizingState);
      return;

    case rsInactive:
      KillTimer(ResizingTimerId);
      return;

    default:
      CHECKX(false, _T("Unhandled ResizingState"));
    }

    w->MoveWindow(r, true);
  }

  void AdvanceResizingState(TResizingState& s)
  {
    TWindow* w = GetParent();
    TWindow* m = w->GetParent();
    CHECK(dynamic_cast<TMDIClient*>(m));
    TRect a = m->GetClientRect();
    TRect r = m->GetChildRect(*w);
    switch (s)
    {
    case rsInactive:
      break;

    case rsDone:
      s = rsInactive;
      break;

    case rsShrinkTowardsOriginalPosition:
      s = (r.Height() >= a.Height()) ? rsDone : rsGrowToMax;
      break;

    default:
      s = static_cast<TResizingState>(s + 1);
      break;
    }
  }

  static TColor GetColorForWindow(int windowNumber)
  {
    static TColor colors[] =
    {
      TColor::White,
      TColor::Sys3dFace,
      TColor::Sys3dDkShadow,
      TColor::Gray,
      TColor::LtBlue,
      TColor::LtCyan,
      TColor::LtGray,
      TColor::LtGreen,
      TColor::LtMagenta,
      TColor::LtRed,
      TColor::LtYellow,
      TColor::Black
    };
    return colors[windowNumber % COUNTOF(colors)];
  }

  static tstring GetNameForWindow(int windowNumber)
  {
    static tchar* names[] =
    {
      _T("Alfa"),
      _T("Bravo"),
      _T("Charlie"),
      _T("Delta"),
      _T("Echo"),
      _T("Foxtrot"),
      _T("Golf"),
      _T("Hotel"),
      _T("India"),
      _T("Juliett"),
      _T("Kilo"),
      _T("Lima"),
      _T("Mike"),
      _T("November"),
      _T("Oscar"),
      _T("Quebec"),
      _T("Romeo"),
      _T("Sierra"),
      _T("Tango"),
      _T("Uniform"),
      _T("Victor"),
      _T("X-ray"),
      _T("Yankee"),
      _T("Zulu")
    };
    return names[windowNumber % COUNTOF(names)];
  }

  static int Delta(const TRect& r)
  {
    return std::max(r.Width() / 50, 1);
  };

  void UpdateTabControlHeight(int d)
  {
    PRECONDITION(GetHandle());

    TNoteTab* t = GetTabControl(); CHECK(t);
    int h = t->GetClientRect().Height() + d;
    ResizeTabControl(h);
  }

  void ChangeTabControlParameter(int d)
  {
    TNoteTab* t = GetTabControl();
    switch (TabControlParameter)
    {
    case tcpTabControlHeight: UpdateTabControlHeight(d); break;
    case tcpMarginX: t->SetMargin(t->GetMargin() + TSize(d, 0)); break;
    case tcpMarginY: t->SetMargin(t->GetMargin() + TSize(0, d)); break;
    case tcpLabelMarginX: t->SetLabelMargin(t->GetLabelMargin() + TSize(d, 0)); break;
    case tcpLabelMarginY: t->SetLabelMargin(t->GetLabelMargin() + TSize(0, d)); break;
    case tcpFocusMarginX: t->SetFocusMargin(t->GetFocusMargin() + TSize(d, 0)); break;
    case tcpFocusMarginY: t->SetFocusMargin(t->GetFocusMargin() + TSize(0, d)); break;
    case tcpTabSpacing: t->SetTabSpacing(t->GetTabSpacing() + d); break;
    case tcpTabTapering: t->SetTabTapering(t->GetTabTapering() + d); break;
    case tcpSelectedTabProtrusion: t->SetSelectedTabProtrusion(t->GetSelectedTabProtrusion() + d); break;
    }
  }

  DECLARE_RESPONSE_TABLE(TTestWindow);
};

TTestWindow::TTabControlParameter TTestWindow::TabControlParameter = TTestWindow::tcpTabControlHeight;
TColor TTestWindow::CustomColors[16] = {};

DEFINE_RESPONSE_TABLE1(TTestWindow, TTabbedWindow)
  EV_COMMAND(CM_ADDPAGE, CmAddPage),
  EV_COMMAND(CM_EDITPAGE, CmEditPage),
  EV_COMMAND(CM_DELETEPAGE, CmDeletePage),
  EV_COMMAND_ENABLE(CM_DELETEPAGE, CeDeletePage),
  EV_COMMAND(CM_DELETESELECTEDPAGE, CmDeleteSelectedPage),
  EV_COMMAND_ENABLE(CM_DELETESELECTEDPAGE, CeDeletePage),
  EV_COMMAND(CM_SELECTPAGE, CmSelectPage),
  EV_COMMAND(CM_SETFIRSTVISIBLETAB, CmSetFirstVisibleTab),
  EV_COMMAND(CM_ENSURETABISVISIBLE, CmEnsureTabIsVisible),

  EV_COMMAND(CM_TABCONTROLHEIGHT, CmSelectParameter<tcpTabControlHeight>),
  EV_COMMAND_ENABLE(CM_TABCONTROLHEIGHT, CeSelectParameter<tcpTabControlHeight>),
  EV_COMMAND(CM_MARGINX, CmSelectParameter<tcpMarginX>),
  EV_COMMAND_ENABLE(CM_MARGINX, CeSelectParameter<tcpMarginX>),
  EV_COMMAND(CM_MARGINY, CmSelectParameter<tcpMarginY>),
  EV_COMMAND_ENABLE(CM_MARGINY, CeSelectParameter<tcpMarginY>),
  EV_COMMAND(CM_LABELMARGINX, CmSelectParameter<tcpLabelMarginX>),
  EV_COMMAND_ENABLE(CM_LABELMARGINX, CeSelectParameter<tcpLabelMarginX>),
  EV_COMMAND(CM_LABELMARGINY, CmSelectParameter<tcpLabelMarginY>),
  EV_COMMAND_ENABLE(CM_LABELMARGINY, CeSelectParameter<tcpLabelMarginY>),
  EV_COMMAND(CM_FOCUSMARGINX, CmSelectParameter<tcpFocusMarginX>),
  EV_COMMAND_ENABLE(CM_FOCUSMARGINX, CeSelectParameter<tcpFocusMarginX>),
  EV_COMMAND(CM_FOCUSMARGINY, CmSelectParameter<tcpFocusMarginY>),
  EV_COMMAND_ENABLE(CM_FOCUSMARGINY, CeSelectParameter<tcpFocusMarginY>),
  EV_COMMAND(CM_TABSPACING, CmSelectParameter<tcpTabSpacing>),
  EV_COMMAND_ENABLE(CM_TABSPACING, CeSelectParameter<tcpTabSpacing>),
  EV_COMMAND(CM_TABTAPERING, CmSelectParameter<tcpTabTapering>),
  EV_COMMAND_ENABLE(CM_TABTAPERING, CeSelectParameter<tcpTabTapering>),
  EV_COMMAND(CM_SELECTEDTABPROTRUSION, CmSelectParameter<tcpSelectedTabProtrusion>),
  EV_COMMAND_ENABLE(CM_SELECTEDTABPROTRUSION, CeSelectParameter<tcpSelectedTabProtrusion>),
  EV_COMMAND(CM_INCREASEPARAMETERVALUE, CmIncreaseParameter),
  EV_COMMAND(CM_DECREASEPARAMETERVALUE, CmDecreaseParameter),

  EV_COMMAND(CM_SETTABFONT, CmSetTabFont),
  EV_COMMAND(CM_SETSELECTEDTABFONT, CmSetSelectedTabFont),
  EV_COMMAND(CM_SETTABEDGECOLOR, CmSetTabEdgeColor),
  EV_COMMAND(CM_SETTABCOLOR, CmSetTabColor),
  EV_COMMAND(CM_SETSELECTEDTABCOLOR, CmSetSelectedTabColor),
  EV_COMMAND(CM_SETTABBACKGROUNDCOLOR, CmSetTabBackgroundColor),
  EV_COMMAND(CM_3DSTYLE, Cm3dStyle),
  EV_COMMAND_ENABLE(CM_3DSTYLE, Ce3dStyle),
  EV_COMMAND(CM_FLATSTYLE, CmFlatStyle),
  EV_COMMAND_ENABLE(CM_FLATSTYLE, CeFlatStyle),
  EV_COMMAND(CM_FLATWINDOWFACESTYLE, CmFlatWindowFaceStyle),
  EV_COMMAND_ENABLE(CM_FLATWINDOWFACESTYLE, CeFlatWindowFaceStyle),
  EV_COMMAND(CM_THEMEDSTYLE, CmThemedStyle),
  EV_COMMAND_ENABLE(CM_THEMEDSTYLE, CeThemedStyle),
  EV_COMMAND(CM_SCROLLBUTTONSLEFT, CmScrollButtonsLeft),
  EV_COMMAND_ENABLE(CM_SCROLLBUTTONSLEFT, CeScrollButtonsLeft),
  EV_COMMAND(CM_SCROLLBUTTONSRIGHT, CmScrollButtonsRight),
  EV_COMMAND_ENABLE(CM_SCROLLBUTTONSRIGHT, CeScrollButtonsRight),

  EV_COMMAND(CM_RESIZING, CmResizing),
  EV_COMMAND_ENABLE(CM_RESIZING, CeResizing),

  EV_COMMAND(CM_SAVEWINDOW, CmSaveWindow),

  EV_WM_TIMER,
END_RESPONSE_TABLE;

void TClassesMDIClient::CmTTabbedWindow()
{
  tostringstream title;
  title << _T("Child #") << ChildNum++;

  TMDIChild* child = new TMDIChild(*this, title.str(), new TTestWindow());
  child->SetMenuDescr(TMenuDescr(IDM_MAIN, 1, 0, 0, 1, 0, 0));
  child->SetAcceleratorTable(TResId{IDA_MAIN});
  child->Create();
  child->ShowWindow(SW_SHOW);
}

void TClassesMDIClient::CmOpenTTabbedWindow()
{
  TOpenSaveDialog::TData d(OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ENABLESIZING, ConfigurationFileFilter);
  TFileOpenDialog dlg(this, d);
  if (dlg.Execute() != IDOK) return;
  try
  {
    TIniConfigFile f(d.FileName);
    TConfigFileSection s(f, _T("Window0"));
    if (!f.SectionExists(s.GetSection()))
      throw std::runtime_error("The file does not contain the required Window0 section.");

    CmTTabbedWindow();

    TMDIChild* c = GetActiveMDIChild(); CHECK(c);
    TTestWindow* w = dynamic_cast<TTestWindow*>(c->GetClientWindow()); CHECK(w);

    w->OpenWindow(f);
  }
  catch (const std::runtime_error& x)
  {
    ::MessageBoxA(GetHandle(), x.what(), "Open error", MB_OK | MB_ICONERROR);
  }
}
