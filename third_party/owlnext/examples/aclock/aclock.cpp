//
// AClock - Animated Clock Example
// Copyright (c) 1993, 1995 by Borland International, All Rights Reserved
// Rewritten in modern C++ and OWLNext style by Vidar Hasfjord, 2018.
// Distributed under the OWLNext License (see http://sourceforge.net/projects/owlnext).
//
#include <owl/pch.h>
#pragma hdrstop

#include <owl/framewin.h>
#include <owl/dialog.h>
#include <owl/dc.h>
#include <owl/configfl.h>
#include <owl/inputdia.h>

#define USE_GDIPLUS 1
#if USE_GDIPLUS
#include <owl/gdiplus.h>
#endif

#include <playsoundapi.h>
#include <sapi.h>
#include <shlobj.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <utility>
#include <vector>
#include <memory>
#include <cmath>
#include <sstream>
#include <filesystem>

#include "ledclock.h"

#include "aclock.rh"

using namespace owl;
using namespace std;

namespace {

//
// Utility function that breaks down a time into components (hour, minutes, etc).
// This function takes a time_point from the system clock and returns the constituent parts of the
// corresponding local time.
//
// Although C++11 introduces date and time functionality ("chrono"), it still relies on C date and
// time utilities, in particular "time_t" and "tm" for breaking a time down into constituent parts,
// and std::put_time (which takes a pointer to "tm") for formatting.
//
// See: http://en.cppreference.com/w/cpp/chrono
// 
auto to_tm(chrono::system_clock::time_point time) -> tm
{
  const auto t = chrono::system_clock::to_time_t(time);
  const auto p_tm = localtime(&t);
  if (!p_tm) throw runtime_error("to_tm: localtime failed!");
  return *p_tm;
}

//
// Utility type for formatting time points (see "format" below).
//
template <class TFmt>
using TTimeFormatter = pair<chrono::system_clock::time_point, TFmt>;

//
// Stream operator for formatting time points (see "format" below).
//
template <class TOStream, class TFmt>
auto operator <<(TOStream& os, const TTimeFormatter<TFmt>& tf) -> TOStream&
{
  const auto t = to_tm(tf.first);
  return os << std::put_time(&t, tf.second);
}

//
// Utility stream manipulator for formatting the given time point in the given format.
// Returns a formatter, which if passed to the output stream operator (<<) for a standard stream,
// will call "to_tm" to break the time down into its constituent parts, and then call std::put_time
// to format the parts according to the given format specification.
//
// Note: C++20 will get the new function "format" to directly format any streamable chrono object,
// at which point this function can be obsoleted.
//
template <class TFmt>
auto format(chrono::system_clock::time_point time, TFmt fmt) noexcept -> TTimeFormatter<TFmt>
{
  return {time, fmt};
}

} // anonymous namespace

//
// Encapsulates the ISpVoice interface from the Microsoft Speech API (SAPI).
//
class TVoice
{
public:

  TVoice()
  {
    const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) throw runtime_error{"Failed to initialize COM!"};
    const auto hr2 = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, reinterpret_cast<void**>(&VoicePtr));
    if (FAILED(hr2)) throw runtime_error{"Failed to create IID_ISpVoice instance!"};
  }

  ~TVoice()
  {
    if (VoicePtr)
      VoicePtr->Release();
    CoUninitialize();
  }

  TVoice(const TVoice&) = delete;
  TVoice(TVoice&&) = delete;
  auto operator =(const TVoice&) -> TVoice& = delete;
  auto operator =(TVoice&&) -> TVoice& = delete;

  auto IsAvailable() const -> bool
  {
    return VoicePtr != nullptr;
  }

  auto Speak(const wstring& s, DWORD flags = SPF_ASYNC) -> ULONG
  {
    PRECONDITION(VoicePtr);
    auto streamNumber = ULONG{};
    const auto hr = VoicePtr->Speak(s.c_str(), flags, &streamNumber);
    if (FAILED(hr)) throw runtime_error{"Attempt to speak failed!"};
    return streamNumber;
  }

private:

  ISpVoice* VoicePtr;

};

//----------------------------------------------------------------------------

//
// Animates a set of bitmaps.
//
class TAnimation 
{
public:

  TAnimation(HINSTANCE hInst, size_t numB, LPCTSTR name);

  void Begin(const TPoint& position, int startFrame = 0, int endFrame = MAXINT);
  void Progress(int frameCount = 1);
  void DisplayCurrentFrame(TDC& dc);
  auto GetPosition() const -> TPoint { return Position; }
  auto GetCurrentFrame() const -> int { return CurrentFrame; }
  auto GetFrameCount() const -> int { return static_cast<int>(Frames.size()); }
  auto IsRunning() const -> bool 
  { 
    return (StartFrame <= EndFrame) ?
      (CurrentFrame >= StartFrame && CurrentFrame <= EndFrame) :
      (CurrentFrame <= StartFrame && CurrentFrame >= EndFrame);
  }

private:

  vector<unique_ptr<TBitmap>> Frames; // Bitmaps
  TPoint Position; // Position of bitmap
  int CurrentFrame; // Currently displayed bitmap
  int StartFrame;
  int EndFrame;
};

//
// Constructs an animated sequence.
// hInst is the instance handle of the module in which the frame bitmaps are located.
// frameCount is the number of frames in the animation.
// name is the prefix of the resource names of the frame bitmaps.
// It is assumed that the bitmap names have a number suffix, starting at 1.
// For example, for prefix "frame", names are assume to be "frame1", "frame2", etc.
//
TAnimation::TAnimation(HINSTANCE hInst, size_t frameCount, LPCTSTR name)
  : Frames{frameCount}, Position{}, CurrentFrame{-1}, StartFrame{0}, EndFrame{0}
{
  // Load in bitmap resources.
  //
  auto i = 1;
  for (auto& b : Frames) 
  {
    auto s = tostringstream{};
    s << name << i++;
    b = make_unique<TBitmap>(hInst, TResId{s.str().c_str()});
  }
}

//
// Resets the animation.
//
void TAnimation::Begin(const TPoint& position, int startFrame, int endFrame)
{
  Position = position;
  CurrentFrame = startFrame;
  StartFrame = startFrame;
  EndFrame = (endFrame == MAXINT) ? GetFrameCount() - 1 : endFrame;
}

//
// Progresses the animation by the given number of frames.
//
void TAnimation::Progress(int frameCount)
{
  CurrentFrame += frameCount;
}

//
// Draws the current frame of the animation.
//
void TAnimation::DisplayCurrentFrame(TDC& dc)
{
  const auto n = GetFrameCount();
  const auto i = (CurrentFrame < 0) ? 0 :
    (CurrentFrame >= n) ? n - 1 :
    CurrentFrame;
  auto& b = *Frames[i];
  TMemoryDC mdc{dc};
  mdc.SelectObject(b);
  dc.BitBlt({Position, b.Size()}, mdc, {}, SRCCOPY);
}

//----------------------------------------------------------------------------

class TClockWindow
  : public TWindow 
{
public:

  TClockWindow(TWindow* parent, bool chimeFlag, bool speechFlag, chrono::minutes pizzaCookingTime);

  auto IsChimeEnabled() const -> bool { return ChimeFlag; }
  auto IsSpeechEnabled() const -> bool { return SpeechFlag; }
  auto GetPizzaCookingTime() const -> chrono::minutes { return PizzaCookingTime; }

  auto IsDigitalClock() const -> bool { return ShowDigitalClock; }
  void SetDigitalClock(bool digital);

protected:

  void SetupWindow() override;
  void CleanupWindow() override;
  void Paint(TDC&, bool, TRect&) override;

  void EvTimer(uint);
  void EvSetFocus(HWND);
  void EvKillFocus(HWND);
  void EvLButtonDblClk(uint mod, const TPoint&);

  void CmEnableChime();
  void CeEnableChime(TCommandEnabler&);
  void CmEnableSpeech();
  void CeEnableSpeech(TCommandEnabler&);
  void CmStayOnTop();
  void CeStayOnTop(TCommandEnabler&);
  void CmDigitalClock();
  void CeDigitalClock(TCommandEnabler&);
  void CmAlarm();
  void CeAlarm(TCommandEnabler&);
  void CmTellTime();
  void CeTellTime(TCommandEnabler&);
  void CmChime();
  void CmMidnightChime();
  void CmAbout();

private:
  const TBitmap FaceBitmap; // Clock face bitmap
  TAnimation ChimeAnim; // Chime sequence
  int ChimeAnimStep;
  using TTime = chrono::system_clock::time_point;
  TTime LastPaintTime;
  TTime LastTimerTime;
  enum {TimerId = 1, TimerDelay = 100}; // ms
  bool ChimeFlag;
  TVoice Voice;
  bool SpeechFlag;
  unique_ptr<TTime> Alarm;
  chrono::minutes PizzaCookingTime;
  bool ShowDigitalClock;
  TLedClock DigitalClock;

  DECLARE_RESPONSE_TABLE(TClockWindow);
};

DEFINE_RESPONSE_TABLE1(TClockWindow, TWindow)
  EV_WM_TIMER,
  EV_WM_SETFOCUS,
  EV_WM_KILLFOCUS,
  EV_WM_LBUTTONDBLCLK,
  EV_COMMAND(CM_ENABLECHIME, CmEnableChime),
  EV_COMMAND_ENABLE(CM_ENABLECHIME, CeEnableChime),
  EV_COMMAND(CM_ENABLESPEECH, CmEnableSpeech),
  EV_COMMAND_ENABLE(CM_ENABLESPEECH, CeEnableSpeech),
  EV_COMMAND(CM_STAYONTOP, CmStayOnTop),
  EV_COMMAND_ENABLE(CM_STAYONTOP, CeStayOnTop),
  EV_COMMAND(CM_DIGITALCLOCK, CmDigitalClock),
  EV_COMMAND_ENABLE(CM_DIGITALCLOCK, CeDigitalClock),
  EV_COMMAND(CM_ALARM, CmAlarm),
  EV_COMMAND_ENABLE(CM_ALARM, CeAlarm),
  EV_COMMAND(CM_TELLTIME, CmTellTime),
  EV_COMMAND_ENABLE(CM_TELLTIME, CeTellTime),
  EV_COMMAND(CM_CHIME, CmChime),
  EV_COMMAND(CM_MIDNIGHTCHIME, CmMidnightChime),
  EV_COMMAND(CM_ABOUT, CmAbout),
  END_RESPONSE_TABLE;

TClockWindow::TClockWindow(TWindow* parent, bool chimeFlag, bool speechFlag, chrono::minutes pizzaCookingTime)
  : TWindow{parent},
  FaceBitmap{GetModule()->GetHandle(), _T("CLOCKFACE")},
  ChimeAnim{GetModule()->GetHandle(), 8, _T("MIDNIGHTOWL")},
  ChimeAnimStep{1},
  LastPaintTime{},
  LastTimerTime{},
  ChimeFlag{chimeFlag},
  Voice{},
  SpeechFlag{speechFlag},
  Alarm{},
  PizzaCookingTime{pizzaCookingTime},
  ShowDigitalClock{false},
  DigitalClock(this, 1, 0, 0, 0, 0)
{
  SetBkgndColor(TColor::None); // Avoid background erasure; reduces flicker.
  MoveWindow({{}, FaceBitmap.Size()});
  DigitalClock.SetBkgndColor(TColor::SysWindow);
  DigitalClock.SetTextColor(TColor::SysWindowText);
  DigitalClock.SetXMargin(10);
  DigitalClock.SetYMargin(10);
}

void TClockWindow::SetupWindow()
{
  TWindow::SetupWindow();
  SetTimer(TimerId, TimerDelay);
  SetDigitalClock(ShowDigitalClock);
}

void TClockWindow::CleanupWindow()
{
  KillTimer(TimerId);
}

//
// Paints the clock face, hands and chime animation frame (if active).
//
void TClockWindow::Paint(TDC& dc, bool, TRect&)
{
  TMemoryDC mdc{dc};
  TBitmap bmp{FaceBitmap};
  mdc.SelectObject(bmp);
  if (ChimeAnim.IsRunning())
    ChimeAnim.DisplayCurrentFrame(mdc);

  const auto now = chrono::system_clock::now();

  if (ShowDigitalClock)
  {
    const auto h = 64;
    auto rect = GetClientRect();
    rect.top = rect.top + (rect.Height() - h) / 2;
    rect.bottom = rect.top + h;
    rect.left += 10;
    rect.right -= 10;
    DigitalClock.SetWindowPos(0, rect, SWP_SHOWWINDOW);
    DigitalClock.Invalidate(true);
  }
  else
  {
    const auto paintHands = [&](TTime time, TColor color)
    {
      // Compute the location of the hands.
      //
      const auto c = TPoint{ FaceBitmap.Width() / 2, FaceBitmap.Height() / 2 };
      const auto clockPos = [c](int r, double a) // center, radius, angle
      {
        return c + TPoint(static_cast<int>(r * sin(a)), static_cast<int>(-r * cos(a)));
      };
      const auto pi2 = 2 * 3.141592;
      const auto t = to_tm(time);
      const auto minAngle = t.tm_min * pi2 / 60;
      const auto hourAngle = ((t.tm_hour % 12) * pi2 + minAngle) / 12;
      const auto minPos = clockPos(3 * c.x / 4, minAngle);
      const auto hourPos = clockPos(c.x / 2, hourAngle);

#if USE_GDIPLUS

      // Note that, with Gdiplus, we can draw smoother and nicer clock hands.
      //
      using namespace Gdiplus;
      const auto ac = gdiplus_cast<Point>(c); // Anchor centre
      const auto as = Size{ 6, 6 }; // Anchor size
      const auto gp_color = gdiplus_cast<Color>(color);
      const Pen hPen{ gp_color, 5.0f }; // Hour hand
      const Pen mPen{ gp_color, 3.0f }; // Minute hand
      const SolidBrush brush{ gp_color };

      Graphics g{ mdc };
      g.SetSmoothingMode(SmoothingModeAntiAlias);
      g.DrawLine(&hPen, ac, gdiplus_cast<Point>(hourPos));
      g.DrawLine(&mPen, ac, gdiplus_cast<Point>(minPos));
      g.FillEllipse(&brush, Rect{ ac - as, as + as });

#else

      // Now draw the hands.
      //
      TPen mPen{ color, 3 };
      mdc.SelectObject(mPen);
      mdc.MoveTo(c);
      mdc.LineTo(minPos);

      TPen hPen{ color, 5 };
      mdc.SelectObject(hPen);
      mdc.MoveTo(c);
      mdc.LineTo(hourPos);

      TBrush aBrush{ color };
      mdc.SelectObject(aBrush);
      const auto as = TSize{ 9, 9 };
      mdc.Ellipse(c - TSize{ as.cx / 2, as.cy / 2 }, as);

      mdc.RestoreBrush();
      mdc.RestorePen();

#endif

    };

    if (Alarm)
      paintHands(*Alarm, TColor{ 255, 222, 240 });
    paintHands(now, TColor{ 100, 111, 120 });

    dc.BitBlt({ {0, 0}, bmp.Size() }, mdc, { 0, 0 }, SRCCOPY);
  }

  LastPaintTime = now;
}

//
// Handles clock repainting.
//
void TClockWindow::EvTimer(uint)
{
  if (ChimeAnim.IsRunning())
  {
    ChimeAnim.Progress(ChimeAnimStep);
    if (!ChimeAnim.IsRunning())
    {
      // Run the animation in reverse, if it just ended forward.
      //
      const auto i = ChimeAnim.GetCurrentFrame();
      if (i > 0)
        ChimeAnim.Begin(ChimeAnim.GetPosition(), i - 1, 0);
      ChimeAnimStep = -ChimeAnimStep;
    }
    Invalidate(false);
  }

  // Redraw the clock, if arms move. Also check alarm, if set.
  // Every hour play a chime. If midnight, run the chime animation.
  //
  const auto now = chrono::system_clock::now();
  const auto n = to_tm(now);
  const auto lpt = to_tm(LastPaintTime);
  if (n.tm_hour != lpt.tm_hour || n.tm_min != lpt.tm_min || (ShowDigitalClock && n.tm_sec != lpt.tm_sec))
    Invalidate(false);
  if (Alarm && *Alarm <= now)
  {
    CmChime();
    Alarm.reset();
  }
  else if (LastTimerTime != TTime{})
  {
    const auto ltt = to_tm(LastTimerTime);
    if (ChimeFlag && n.tm_min == 0 && n.tm_min != ltt.tm_min)
      n.tm_hour == 0 ? CmMidnightChime() : CmChime();
  }
  LastTimerTime = now;
}

//
// If we get focus, we draw the full program window, including frame and menu.
//
void TClockWindow::EvSetFocus(HWND wndLostFocus)
{
  TWindow::EvSetFocus(wndLostFocus);
  GetParent()->ResetWindowRgn();
}

//
// If we lose focus, we draw only the clock face, without program window frame and menu.
//
void TClockWindow::EvKillFocus(HWND wndGetFocus)
{
  TWindow::EvKillFocus(wndGetFocus);
  auto& p = *GetParent();
  if (ShowDigitalClock)
  {
    const auto r = DigitalClock.GetClientRect() + (DigitalClock.GetWindowRect().TopLeft() - p.GetWindowRect().TopLeft());
    p.SetWindowRgn(TRegion{ r });
  }
  else
  {
    const auto r = GetClientRect().InflatedBy(-2, -2) + (GetWindowRect().TopLeft() - p.GetWindowRect().TopLeft());
    p.SetWindowRgn(TRegion{ r, TRegion::Ellipse });
  }
}

void TClockWindow::EvLButtonDblClk(uint, const TPoint&)
{
  if (SpeechFlag)
    CmTellTime();
}

void TClockWindow::CmEnableChime()
{
  ChimeFlag = !ChimeFlag;
}

void TClockWindow::CeEnableChime(TCommandEnabler& c)
{
  c.SetCheck(ChimeFlag);
}

void TClockWindow::CmEnableSpeech()
{
  SpeechFlag = !SpeechFlag;
}

void TClockWindow::CeEnableSpeech(TCommandEnabler& c)
{
  c.SetCheck(Voice.IsAvailable() && SpeechFlag);
  c.Enable(Voice.IsAvailable());
}

void TClockWindow::CmStayOnTop()
{
  auto f = GetApplication()->GetMainWindow(); CHECK(f);
  f->SetWindowPos(
    (f->GetExStyle() & WS_EX_TOPMOST) ? HWND_NOTOPMOST : HWND_TOPMOST,
    f->GetWindowRect(),
    SWP_NOMOVE | SWP_NOSIZE);
}

void TClockWindow::CeStayOnTop(TCommandEnabler& c)
{
  c.SetCheck((GetParent()->GetExStyle() & WS_EX_TOPMOST) != 0);
}

void TClockWindow::SetDigitalClock(bool digital)
{
  ShowDigitalClock = digital;
  if (GetHandle() != 0)
  {
    if (ShowDigitalClock)
    { 
      SetBkgndColor(TColor::None); // Avoid background erasure; reduces flicker.
      DigitalClock.ShowWindow(SW_SHOW);
    }
    else
    {
      SetBkgndColor(TColor::SysWindow);
      DigitalClock.ShowWindow(SW_HIDE);
    }
    Invalidate();
  }
}

void TClockWindow::CmDigitalClock()
{
  SetDigitalClock(!ShowDigitalClock);
}

void TClockWindow::CeDigitalClock(TCommandEnabler& c)
{
  c.SetCheck(ShowDigitalClock);
}

void TClockWindow::CmAlarm()
{
  if (Alarm)
  {
    Alarm.reset();
    if (SpeechFlag && Voice.IsAvailable())
      Voice.Speak(L"The alarm has been cancelled.");
  }
  else
  {
    TInputDialog d{this, _T("Pizza alarm"), _T("Cooking time in minutes"), to_tstring(PizzaCookingTime.count())};
    if (d.Execute() != IDOK) return;
    const auto m = chrono::minutes{_ttoi(d.GetBuffer())};
    if (m <= chrono::minutes{0} || m >= chrono::hours{24})
    {
      MessageBox(_T("Sorry, the cooking time must be within a day."), _T("Pizza alarm error"), MB_OK | MB_ICONEXCLAMATION);
      return;
    }
    PizzaCookingTime = m;
    Alarm = make_unique<TTime>(chrono::system_clock::now() + PizzaCookingTime);
    if (SpeechFlag && Voice.IsAvailable())
    {
      auto s = wostringstream{};
      s << L"The alarm has been set to " << format(*Alarm, L"%R") << L'.';
      Voice.Speak(s.str());
    }
  }
  Invalidate(false);
}

void TClockWindow::CeAlarm(TCommandEnabler& c)
{
  const auto active = static_cast<bool>(Alarm);
  auto s = tostringstream{};
  if (active)
    s << _T("Pizza &alarm at ") << format(*Alarm, _T("%R"));
  else
    s << _T("Set pizza &alarm...");
  s << _T("\tF7");
  c.SetText(s.str());
  c.SetCheck(active);
}

void TClockWindow::CmTellTime()
{
  if (Voice.IsAvailable())
  {
    auto s = wostringstream{};
    const auto now = chrono::system_clock::now();
    s << L"The time is now " << format(now, L"%R") << L'.';
    if (Alarm)
    {
      s << L' ';
      if (*Alarm <= now)
        s << L"Check your pizza!";
      else
      {
        s << L"Your pizza should be ready in ";
        const auto m = chrono::duration_cast<chrono::minutes>(*Alarm - now);
        if (m == chrono::minutes{0})
          s << L"a few seconds";
        else
        {
          const auto u = (m == chrono::minutes{1}) ? L"minute" : L"minutes";
          s << m.count() << L' ' << u;
        }
        s << L'.';
      }
    }
    Voice.Speak(s.str());
  }
}

void TClockWindow::CeTellTime(TCommandEnabler& c)
{
  c.Enable(Voice.IsAvailable() && SpeechFlag);
}

void TClockWindow::CmChime()
{
  sndPlaySound(nullptr, 0);
  sndPlaySound(_T("SystemExclamation"), SND_ASYNC);
  if (SpeechFlag)
    CmTellTime();
}

void TClockWindow::CmMidnightChime()
{
  CmChime();
  ChimeAnim.Begin({0, 0}, 0, 24);
  Invalidate(false);
  SetTimer(TimerId, TimerDelay); // Reset to give the first frame correct timing.
}

void TClockWindow::CmAbout()
{
  TDialog{this, IDD_ABOUT}.Execute();
}

//----------------------------------------------------------------------------

class TAClockApp
  : public TApplication
{
public:

  TAClockApp();

protected:

  void InitMainWindow() override;
  auto CanClose() -> bool override;

private:

  static const tstring AppName;
  static const tstring IniFileName;

};

const tstring TAClockApp::AppName = _T("AClock");
const tstring TAClockApp::IniFileName = []
{
  auto getRoamingAppDataFolderPath = []
  {
    auto buf = LPWSTR{};
    const auto r = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &buf);
    CHECK(r == S_OK); InUse(r);
    auto p = unique_ptr<WCHAR, decltype(&CoTaskMemFree)>(buf, &CoTaskMemFree);
    return std::filesystem::path{p.get()};
  };
  const auto appdata = getRoamingAppDataFolderPath() / AppName;
  if (!exists(appdata))
    create_directory(appdata);
  return to_tstring(appdata / (AppName + _T(".ini")));
}();

TAClockApp::TAClockApp()
  : TApplication{AppName}
{}

void TAClockApp::InitMainWindow()
{
  auto ini = TIniConfigFile{IniFileName};
  const auto s = TConfigFileSection{ini, _T("Settings")};
  const auto chimeFlag = s.ReadBool(_T("chime"), true);
  const auto speechFlag = s.ReadBool(_T("speech"), true);
  const auto pizzaCookingTime = chrono::minutes{s.ReadInteger(_T("pizzaCookingTime"), 15)};

  const auto shrinkFlag = true;
  auto c = make_unique<TClockWindow>(nullptr, chimeFlag, speechFlag, pizzaCookingTime);
  c->SetDigitalClock(s.ReadBool(_T("digitalClock"), false));

  auto f = make_unique<TFrameWindow>(nullptr, GetName(), move(c), shrinkFlag);
  f->AssignMenu(IDM_MENU);
  f->SetAcceleratorTable(IDM_MENU);
  f->ModifyStyle(WS_SIZEBOX | WS_MAXIMIZEBOX, 0); // Disallow window sizing.
  f->SetIcon(this, IDI_APP);

  // Restore the window position since last run.
  //
  const auto p = TConfigFileSection{ini, _T("MainWindowPosition")};
  const auto r = f->GetWindowRect();
  if (nCmdShow == SW_SHOWDEFAULT)
    nCmdShow = p.ReadInteger(_T("showCmd"), nCmdShow);
  f->MoveWindow(TRect{
    p.ReadInteger(_T("left"), r.left),
    p.ReadInteger(_T("top"), r.top),
    p.ReadInteger(_T("right"), r.right),
    p.ReadInteger(_T("bottom"), r.bottom)});
  if (p.ReadBool(_T("topMost"), false))
    f->SetExStyle(WS_EX_TOPMOST);

  SetMainWindow(move(f));
}

auto TAClockApp::CanClose() -> bool
{
  const auto r = TApplication::CanClose();
  if (!r) return false;

  auto ini = TIniConfigFile{IniFileName};
  auto s = TConfigFileSection{ini, _T("Settings")};
  const auto c = dynamic_cast<TClockWindow*>(GetMainWindow()->GetClientWindow()); CHECK(c);
  s.WriteBool(_T("chime"), c->IsChimeEnabled());
  s.WriteBool(_T("speech"), c->IsSpeechEnabled());
  s.WriteInteger(_T("pizzaCookingTime"), c->GetPizzaCookingTime().count());
  s.WriteBool(_T("digitalClock"), c->IsDigitalClock());

  // Save the window position until next run.
  //
  auto p = TConfigFileSection{ini, _T("MainWindowPosition")};
  const auto wp = GetMainWindow()->GetWindowPlacement();
  const auto showCmd = (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) ?
    SW_SHOWDEFAULT : wp.showCmd;
  p.WriteInteger(_T("showCmd"), showCmd);
  p.WriteInteger(_T("left"), wp.rcNormalPosition.left);
  p.WriteInteger(_T("top"), wp.rcNormalPosition.top);
  p.WriteInteger(_T("right"), wp.rcNormalPosition.right);
  p.WriteInteger(_T("bottom"), wp.rcNormalPosition.bottom);
  p.WriteBool(_T("topMost"), GetMainWindow()->GetExStyle() & WS_EX_TOPMOST);
  return true;
}

auto OwlMain(int /*argc*/, tchar* /*argv*/[]) -> int
{
  try
  {
    return TAClockApp{}.Run();
  }
  catch (const exception& x)
  {
    throw TXBase{to_tstring(x.what())}; // TXBase is handled in WinMain.
  }
}