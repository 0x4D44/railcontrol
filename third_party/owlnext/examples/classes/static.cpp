//
// OWLNext TStatic Demo
//
#include "pch.h"
#pragma hdrstop

#include "static.h"
#include <owl/applicat.h>
#include <owl/static.h>
#include <owl/gdiobjec.h>
#include <owl/time.h>
#include <memory>
#include <map>

#include "static.rh"

using namespace owl;
using namespace std;

#if OWLVersion < 0x0700
#define USE_OWLNEXT_7 0
#else
#define USE_OWLNEXT_7 1
#endif

#if USE_OWLNEXT_7

//
// This implementation uses OWLNext 7 features. In particular, in version 7 the dialog and controls
// can now simply use the new TWindow::SetTextColor to set the text color for controls. The text
// color can be set by the parent (dialog) or overridden individually by the child (control). In
// addition, the control font can now be set simply by passing a TFont instance to SetWindowFont.
// The TWindow class will manage the lifetime of the underlying font handle automatically. With
// these additions we no longer have to do font lifetime management and we no longer have to
// implement a handler for WM_CTLCOLORSTATIC etc. to set control colors.
//
class TStaticDemoDialog : public TDialog
{
public:

  TStaticDemoDialog(TWindow* parent = nullptr) : TDialog{parent, IDD_STATICDIALOG},
    Title{this, IDC_TITLE},
    Clock{this, IDC_CLOCK},
    Greeting{this, IDC_GREETING}
  {}

protected:

  void SetupWindow() override
  {
    TDialog::SetupWindow();

    Title.SetTextColor({128, 128, 255});
    Title.SetWindowFont({_T("Arial"), 36, 0, 0, 0, FW_BOLD});
    Title.SetBkgndColor({0, 10, 120});

    Clock.SetTextColor(TColor::White);
    Clock.SetWindowFont({_T("Arial"), 30});
    Clock.SetBkgndColor(Title.GetBkgndColor());

    Greeting.SetTextColor({128, 128, 196});
    Greeting.SetWindowFont({_T("Verdana"), 24, 0, 0, 0, FW_BOLD});

    SetTimer(TimerId, 1000);
  }

private:

  TStatic Title;
  TStatic Clock;
  TStatic Greeting;

  enum { TimerId = 1 };

  void EvTimer(uint id);

  DECLARE_RESPONSE_TABLE(TStaticDemoDialog);
};

DEFINE_RESPONSE_TABLE1(TStaticDemoDialog, TDialog)
  EV_WM_TIMER,
END_RESPONSE_TABLE;

#else

//
// This implementation uses only pre-7 features. In particular, the dialog must implement a handler
// for the WM_CTLCOLORSTATIC message, in which control colors are set, and control fonts have to be
// managed by the dialog.
//
class TStaticDemoDialog : public TDialog
{
public:

  TStaticDemoDialog(TWindow* parent = nullptr) : TDialog{parent, IDD_STATICDIALOG},
    Title{this, IDC_TITLE},
    TitleFont{_T("Arial"), 36, 0, 0, 0, FW_BOLD},
    Clock{this, IDC_CLOCK},
    ClockFont{_T("Arial"), 30},
    Greeting{this, IDC_GREETING},
    GreetingFont{_T("Verdana"), 24, 0, 0, 0, FW_BOLD}
  {
    SetBkgndColor(TColor::White);
  }

protected:

  void SetupWindow() override
  {
    TDialog::SetupWindow();
    Title.SetWindowFont(TitleFont, false);
    const auto titleBackground = TColor{0, 10, 120};
    Title.SetBkgndColor(titleBackground);

    Clock.SetWindowFont(ClockFont, false);
    Clock.SetBkgndColor(titleBackground);

    Greeting.SetWindowFont(GreetingFont, false);
    Greeting.SetBkgndColor(TColor::Transparent);

    SetTimer(TimerId, 1000);
  }

private:

  TStatic Title;
  TFont TitleFont;
  TStatic Clock;
  TFont ClockFont;
  TStatic Greeting;
  TFont GreetingFont;

  enum { TimerId = 1 };

  auto EvCtlColorStatic(HDC hdc, HWND ctl, uint ctlType) -> HBRUSH
  {
    const auto w = GetWindowPtr(ctl);
    if (!w) return TDialog::EvCtlColor(hdc, ctl, ctlType);

    // We forward the message to the control first, to allow it to set the
    // background color, background mode and erasure brush. We need to do this
    // first, since calling it later may override our text color setting.
    //
    const auto erasureBrush = reinterpret_cast<HBRUSH>(w->ForwardMessage());

    // Now look up the text color for the control.
    //
    static const auto colorTable = map<int, TColor>{
      {IDC_TITLE, TColor{128, 128, 255}},
      {IDC_CLOCK, TColor::White},
      {IDC_GREETING, TColor{128, 128, 196}}
    };
    const auto i = colorTable.find(w->GetId());
    if (i != colorTable.end())
      ::SetTextColor(hdc, i->second);
    return erasureBrush;
  }

  void EvTimer(uint id);

  DECLARE_RESPONSE_TABLE(TStaticDemoDialog);
};

DEFINE_RESPONSE_TABLE1(TStaticDemoDialog, TDialog)
  EV_WM_CTLCOLORSTATIC(EvCtlColorStatic),
  EV_WM_TIMER,
END_RESPONSE_TABLE;

#endif

void TStaticDemoDialog::EvTimer(uint id)
{
  PRECONDITION(id == TimerId);
  TTime now{};
  auto s = tostringstream{};
  s << setfill(_T('0'))
    << setw(2) << now.Hour() << _T(':')
    << setw(2) << now.Minute() << _T(':')
    << setw(2) << now.Second();
  Clock.SetWindowText(s.str());

  if (now.Second() % 2 == 0)
  {
    static const LPCTSTR greetings[] = {
      _T("Hello world"), _T("Hola mundo"), _T("Bonjour le monde"), _T("Ciao mondo"), _T("Hei maailma"),
      _T("Hallo verden"), _T("Hallo Wereld"), _T("Salut Lume"), _T("Hallo Welt"), _T("Olá Mundo"),

#if defined(UNICODE)

      _T("مرحبا بالعالم"), _T("שלום עולם"), _T("Привет мир"), _T("Γειά σου Κόσμε")

#endif

    };
    static auto i = 0;
    Greeting.SetWindowText(greetings[i]);
    i = (i + 1) % size(greetings);

    SetBkgndColor(TColor::White);
  }
  else
    SetBkgndColor(TColor::None);
}

auto CreateStaticDemoDialog(TWindow* parent) -> unique_ptr<TDialog>
{
  return make_unique<TStaticDemoDialog>(parent);
}
