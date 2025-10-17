//
// Razee - Dice Challenge
// Copyright (c) 2019 Vidar Hasfjord
// All rights reserved.
//
// \file User interface and application entry point.
//
#include "pch.h"
#pragma hdrstop

#include "razee.h"
#include <owl/applicat.h>
#include <owl/framewin.h>
#include <owl/layoutwi.h>
#include <owl/glyphbtn.h>
#include <owl/listviewctrl.h>
#include <owl/gdiobjec.h>
#include <owl/profile.h>
#include <owl/inputdia.h>
#include <sapi.h>
#include <shlobj.h>
#include <functional>
#include <utility>
#include <memory>
#include <type_traits>
#include <filesystem>
#include <fstream>

#include "main.rh"

using namespace owl;
using namespace std;

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

class TRazeeWindow
  : public TLayoutWindow
{
public:

  class TDieButton
    : public TGlyphButton
  {
  public:

    static const auto GlyphSize = 48;
    static const auto Margin = 6;
    static const TSize Size;

    TDieButton(TWindow* parent, int id)
      : TGlyphButton{parent, id, nullptr, 0, 0, Size.cx, Size.cy}
    {}

  };

  TRazeeWindow(TWindow* parent, filesystem::path iniFileName)
    : TLayoutWindow{parent},
    DieButton1{this, DieButton1Id},
    DieButton2{this, DieButton2Id},
    DieButton3{this, DieButton3Id},
    DieButton4{this, DieButton4Id},
    DieButton5{this, DieButton5Id},
    DieButton6{this, DieButton6Id},
    RollButton{this, RollButtonId, LoadString(IDS_ROLL), 0, 0, (TDieButton::Size.cx - Margin) / 2, TDieButton::Size.cy, TGlyphButton::btRevert},
    LockButton{this, LockButtonId, LoadString(IDS_LOCK), 0, 0, (TDieButton::Size.cx - Margin) / 2, TDieButton::Size.cy, TGlyphButton::btOk},
    ScoreboardList{this, ScoreboardListId, 0, 0, 0, 0},
    Dice{},
    Game{razee::TGame::Easy},
    HighScores{},
    IniFileName{iniFileName},
    HighScoresFileName{iniFileName.parent_path() / "High Scores.txt"}
  {
    RollButton.SetLayoutStyle(TGlyphButton::lsV_SGST);
    LockButton.SetLayoutStyle(TGlyphButton::lsV_SGST);

    if (exists(HighScoresFileName))
    {
      auto is = tifstream{HighScoresFileName};
      auto h = razee::THighScores{is};
      WARN(is.fail(), _T("Reading high scores failed."));
      if (!is.fail())
        HighScores = move(h);
    }
  }

  auto GetGame() const -> const razee::TGame& {return Game;}

  void ResignGame()
  {
    Game.Resign();
    UnlockDice();
    EnableControls();
  }

  void ResetGame(razee::TGame::TDifficulty difficulty = razee::TGame::Easy)
  {
    Game = {difficulty};
    UpdateScoreboard();
    ScoreboardList.EnableWindow(false);
    UnlockDice();
    Roll();
    EnableControls();
    SetTimer(GameTimer, 1000);
  }

  void ShowHighScores() const
  {
    using namespace razee;
    const auto difficultyIdMap = map<TGame::TDifficulty, uint>
    {
      {TGame::Easy, IDS_EASY},
      {TGame::Hard, IDS_HARD}, 
      {TGame::Mean, IDS_MEAN}
    };
    auto s = tostringstream{};
    auto first = true;
    for (const auto& d: difficultyIdMap)
    {
      const auto& entries = HighScores.GetEntries(d.first);
      if (entries.empty()) continue;

      if (!first)
        s << _T("\n");
      first = false;

      s << LoadString(d.second) << _T(":\n\n");
      auto rank = 0;
      for (const auto& e: HighScores.GetEntries(d.first))
        s << ++rank << _T(". ") << e.Player << _T(" ... ") << e.Score << _T('\n');
    }
    if (first)
      s << LoadString(IDS_NOHIGHSCORESYET);
    MessageBoxIndirect(TResId{IDI_HIGHSCORES}, s.str(), LoadString(IDS_HIGHSCORES), MB_OK);
  }

protected:

  void SetupWindow() override
  {
    TLayoutWindow::SetupWindow();
    MoveWindow({{}, TSize{
      Margin + TDieButton::GlyphSize * 2 + Margin + ScoreboardWidth + Margin, 
      6 * (Margin + TDieButton::Size.cy) + 2 * Margin + TDieButton::Size.cy + Margin + ::GetSystemMetrics(SM_CYMENU)}});
    auto f = TDefaultGuiFont{TDefaultGuiFont::sfiMessage}.GetObject();
    f.lfHeight = 24;
    auto font = TFont{f};
    SetWindowFont(font);
    ArrangeControls();

    ScoreboardList.SetWindowFont(font);
    ScoreboardList.ModifyStyle(WS_BORDER, LVS_REPORT | LVS_SINGLESEL);
    ScoreboardList.ModifyExStyle(WS_EX_CLIENTEDGE, 0);
    ScoreboardList.SetExtStyle(0xFFFFFFF, LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);
    const auto padding = 24 + ::GetSystemMetrics(SM_CXVSCROLL); // Account for column padding and scrollbar.
    const auto scoreWidth = 60;
    const auto fieldWidth = ScoreboardWidth - scoreWidth - padding;
    ScoreboardList.InsertColumn(0, {LoadString(IDS_FIELD), fieldWidth});
    ScoreboardList.InsertColumn(1, {LoadString(IDS_SCORE), scoreWidth, TLvColumn::Right});
    ScoreboardList.AddItem(_T(""));
    const auto dots = tstring(80, _T('.'));
    const auto fields = 
    {
      IDS_PAIR,
      IDS_PAIRDUO,
      IDS_PAIRTRIO,
      IDS_TRIPLE,
      IDS_TRIPLEDUO,
      IDS_TRIPLEANDPAIR,
      IDS_QUADRUPLE,
      IDS_QUADRUPLEANDPAIR,
      IDS_QUINTUPLE,
      IDS_SEXTUPLE,
      IDS_LOWPARTIALSEQUENCE,
      IDS_HIGHPARTIALSEQUENCE,
      IDS_COMPLETESEQUENCE
    };
    for (const auto& f: fields)
      const int j = ScoreboardList.AddItem(LoadString(f) + dots);
    ScoreboardList.AddItem(_T(""));
    const auto iTime = ScoreboardList.AddItem(LoadString(IDS_TIMEBONUS) + dots);
    const auto iTotal = ScoreboardList.AddItem(LoadString(IDS_TOTALSCORE) + dots);

    ResetGame();
  }

  void CleanupWindow() override
  {
    TLayoutWindow::CleanupWindow();

    auto os = tofstream{HighScoresFileName};
    HighScores.Serialize(os);
  }

private:

  static const auto Margin = 10;
  static const auto ScoreboardWidth = 300;

  enum {DieButton1Id = 1, DieButton2Id, DieButton3Id, DieButton4Id, DieButton5Id, DieButton6Id, RollButtonId, LockButtonId, ScoreboardListId};

  TDieButton DieButton1;
  TDieButton DieButton2;
  TDieButton DieButton3;
  TDieButton DieButton4;
  TDieButton DieButton5;
  TDieButton DieButton6;
  array<TDieButton*, 6> DieButtons = {&DieButton1, &DieButton2, &DieButton3, &DieButton4, &DieButton5, &DieButton6};
  TGlyphButton RollButton;
  TGlyphButton LockButton;
  TListViewCtrl ScoreboardList;

  razee::TDice Dice;
  razee::TGame Game;
  razee::THighScores HighScores;
  filesystem::path IniFileName;
  filesystem::path HighScoresFileName;

  static const auto GameTimer = 1;
  static const auto ErrorTimer = 2;

  void ArrangeControls()
  {
    const auto margin = TSize{Margin, Margin};

    TLayoutMetrics m;
    m.SetMeasurementUnits(lmPixels);

    m.X.Absolute(lmLeft, margin.cx);
    m.Y.Absolute(lmTop, margin.cy);
    m.Width.AsIs(lmWidth);
    m.Height.AsIs(lmHeight);
    SetChildLayoutMetrics(DieButton1, m);

    for (auto i = 1; i < razee::TDice::Count; ++i)
    {
      m.Y.Below(DieButtons[i - 1], margin.cy);
      SetChildLayoutMetrics(*DieButtons[i], m);
    }

    m.Y.Below(&DieButton6, 2 * margin.cy);
    SetChildLayoutMetrics(RollButton, m);

    m.X.RightOf(&RollButton, margin.cx);
    SetChildLayoutMetrics(LockButton, m);

    m.X.RightOf(&LockButton, margin.cx);
    m.Y.Absolute(lmTop, margin.cy);
    m.Width.Set(lmRight, lmSameAs, lmParent, lmRight, -margin.cx);
    m.Height.Set(lmBottom, lmSameAs, lmParent, lmBottom, -margin.cy);
    SetChildLayoutMetrics(ScoreboardList, m);
  }

  void UpdateScoreboard()
  {
    for (auto i = 0; i != razee::TScoreboard::FieldCount; ++i)
    {
      const auto listIndex = i + 1; // Skip blank row.
      const auto fieldId = static_cast<razee::TScoreboard::TFieldId>(i);
      ScoreboardList.SetItemText(listIndex, 1, to_tstring(Game.GetScore(fieldId)));
    }
    const auto iTime = razee::TScoreboard::FieldCount + 2;
    ScoreboardList.SetItemText(iTime, 1, to_tstring(Game.GetTimeBonus()));
    const auto iTotal = razee::TScoreboard::FieldCount + 3;
    ScoreboardList.SetItemText(iTotal, 1, to_tstring(Game.GetTotalScore()));
  }

  void ToggleDie(int i)
  {
    if (Dice.IsLocked(i))
      Dice.Unlock(i);
    else
      Dice.Lock(i);
    auto d = DieButtons[i];
    d->SetLayoutStyle(TGlyphButton::lsNone);
    d->SetGlyphOrigin(Dice.IsLocked(i) ? d->GetClientRect().Width() - TDieButton::GlyphSize - TDieButton::Margin : TDieButton::Margin, TDieButton::Margin);
    d->Invalidate();
  }

  void LockDice()
  {
    for (int i = 0; i != razee::TDice::Count; ++i)
      if (!Dice.IsLocked(i))
        ToggleDie(i);
  }

  void UnlockDice()
  {
    for (int i = 0; i != razee::TDice::Count; ++i)
      if (Dice.IsLocked(i))
        ToggleDie(i);
  }

  void Roll()
  {
    const auto glyphIds = array<int, razee::TDice::Count>{IDB_DIE1, IDB_DIE2, IDB_DIE3, IDB_DIE4, IDB_DIE5, IDB_DIE6};
    Dice.Roll();
    for (int i = 0; i != razee::TDice::Count; ++i)
      if (!Dice.IsLocked(i))
      {
        auto& d = DieButtons[i];
        d->SetGlyph(TResId{glyphIds[Dice.GetDie(i).GetValue() - 1]});
        d->SetLayoutStyle(TGlyphButton::lsNone);
        d->SetGlyphOrigin(TDieButton::Margin, TDieButton::Margin);
        d->Invalidate();
      }
  }

  void Error()
  {
    SetBkgndColor(TColor::LtRed, true);
    SetTimer(ErrorTimer, 1000);
  }

  void EnableControls()
  {
    for (auto& w : GetChildren())
    {
      auto c = TControlEnabler{static_cast<uint>(w.GetId()), w.GetHandle()};
      TDispatch<WM_COMMAND_ENABLE>::Encode(&::SendMessage, w.GetParent()->GetHandle(), c);
    }
  }

  void EnterHighScore()
  {
    auto difficultyStrings = map<razee::TGame::TDifficulty, uint>
    {
      {razee::TGame::Easy, IDS_EASY},
      {razee::TGame::Hard, IDS_HARD},
      {razee::TGame::Mean, IDS_MEAN}
    };
    auto prompt = tostringstream{};
    prompt << LoadString(IDS_SCORE) << _T(": ") << Game.GetTotalScore() 
      << _T(". ") << LoadString(IDS_DIFFICULTY) << _T(": ") << LoadString(difficultyStrings[Game.GetDifficulty()]) << _T(". ")
      << LoadString(IDS_HIGHSCOREENTRYPROMPT);

    auto s = TProfile{_T("HighScores"), to_tstring(IniFileName)};
    const auto getUserName = []() -> tstring
    {
      auto buf = array<TCHAR, 256>{};
      auto n = static_cast<DWORD>(buf.size());
      auto r = GetUserName(buf.data(), &n); CHECK(r); InUse(r);
      return buf.data();
    };
    auto name = s.GetString(_T("LastPlayer"), getUserName());
    auto d = TInputDialog{this, LoadString(IDS_HIGHSCOREENTRYCAPTION), prompt.str(), name};
    if (d.Execute() != IDOK) return;
    name = d.GetBuffer();
    if (name.empty()) return;
    s.WriteString(_T("LastPlayer"), name);
    HighScores.Enter(name, Game);
    ShowHighScores();
  }

  void EvTimer(uint timerId)
  {
    switch (timerId)
    {
    case GameTimer:
      {
        const auto timeRow = razee::TScoreboard::FieldCount + 2; // Adjust for blank rows.
        ScoreboardList.SetItemText(timeRow, 1, to_tstring(Game.GetTimeBonus()));
        const auto totalRow = razee::TScoreboard::FieldCount + 3; // Adjust for blank rows and time row.
        ScoreboardList.SetItemText(totalRow, 1, to_tstring(Game.GetTotalScore()));
        if (Game.IsGameOver())
        {
          UnlockDice();
          KillTimer(GameTimer);
          if (HighScores.DoesQualify(Game))
            EnterHighScore();
        }
        EnableControls();
        break;
      }

    case ErrorTimer:
      {
        SetBkgndColor(TColor::None);
        KillTimer(ErrorTimer);
        break;
      }
    }
  }

  template <int DieButtonId>
  void EvDieButtonClicked()
  {
    ToggleDie(DieButtonId - 1);
    EnableControls();
  }

  void CeDieButton(TCommandEnabler& c)
  {
    c.Enable(!Game.IsGameOver());
  }

  void EvRollButtonClicked()
  {
    Roll();
  }

  void CeRollButton(TCommandEnabler& c)
  {
    c.Enable(!Game.IsGameOver() && !Dice.IsAllLocked());
  }

  void EvLockButtonClicked()
  {
    Dice.IsAllLocked() ? UnlockDice() : LockDice();
    EnableControls();
  }

  void CeLockButton(TCommandEnabler& c)
  {
    c.Enable(!Game.IsGameOver());
    c.SetText(LoadString(Dice.IsAllLocked() ? IDS_UNLOCK : IDS_LOCK));
    LockButton.Invalidate();
  }

  void EvFieldDblClk()
  {
    const auto clickedRow = ScoreboardList.GetSelIndex();
    const auto i = clickedRow - 1; // Adjust for the first blank row.
    if (i < 0 || i >= razee::TScoreboard::FieldCount) return;
    const auto fieldId = static_cast<razee::TScoreboard::TFieldId>(i);
    if (Game.SetScore(fieldId, Dice))
    {
      ScoreboardList.SetItemText(clickedRow, 1, to_tstring(Game.GetScore(fieldId)));
      const auto totalRow = razee::TScoreboard::FieldCount + 3; // Adjust for blank rows and time row.
      ScoreboardList.SetItemText(totalRow, 1, to_tstring(Game.GetTotalScore()));
    }
    else
      Error();
    UnlockDice();
    Roll();
    EnableControls();
  }

  void CeScoreboardList(TCommandEnabler& c)
  {
    c.Enable(Dice.IsAllLocked());
  }

  DECLARE_RESPONSE_TABLE(TRazeeWindow);
};

const TSize TRazeeWindow::TDieButton::Size = {2 * GlyphSize + 4 * Margin, GlyphSize + 2 * Margin};

DEFINE_RESPONSE_TABLE1(TRazeeWindow, TLayoutWindow)
  EV_WM_TIMER,
  EV_BN_CLICKED(DieButton1Id, EvDieButtonClicked<DieButton1Id>),
  EV_COMMAND_ENABLE(DieButton1Id, CeDieButton),
  EV_BN_CLICKED(DieButton2Id, EvDieButtonClicked<DieButton2Id>),
  EV_COMMAND_ENABLE(DieButton2Id, CeDieButton),
  EV_BN_CLICKED(DieButton3Id, EvDieButtonClicked<DieButton3Id>),
  EV_COMMAND_ENABLE(DieButton3Id, CeDieButton),
  EV_BN_CLICKED(DieButton4Id, EvDieButtonClicked<DieButton4Id>),
  EV_COMMAND_ENABLE(DieButton4Id, CeDieButton),
  EV_BN_CLICKED(DieButton5Id, EvDieButtonClicked<DieButton5Id>),
  EV_COMMAND_ENABLE(DieButton5Id, CeDieButton),
  EV_BN_CLICKED(DieButton6Id, EvDieButtonClicked<DieButton6Id>),
  EV_COMMAND_ENABLE(DieButton6Id, CeDieButton),
  EV_BN_CLICKED(RollButtonId, EvRollButtonClicked),
  EV_COMMAND_ENABLE(RollButtonId, CeRollButton),
  EV_BN_CLICKED(LockButtonId, EvLockButtonClicked),
  EV_COMMAND_ENABLE(LockButtonId, CeLockButton),
  EV_LVN_DBLCLK(ScoreboardListId, EvFieldDblClk),
  EV_COMMAND_ENABLE(ScoreboardListId, CeScoreboardList),
END_RESPONSE_TABLE;

class TRazee
  : public TApplication
{
public:

  TRazee()
    : TApplication{}
  {
    SetName(LoadString(IDS_RAZEE));
  }

  auto GetIniFileName() const -> filesystem::path
  {
    using namespace filesystem;
    const auto getRoamingAppDataFolderPath = []
    {
      auto buf = LPWSTR{};
      const auto r = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &buf);
      CHECK(r == S_OK); InUse(r);
      auto p = unique_ptr<WCHAR, decltype(&CoTaskMemFree)>{buf, &CoTaskMemFree};
      return std::filesystem::path{p.get()};
    };
    const auto app = tstring{GetName()};
    const auto appdata = getRoamingAppDataFolderPath() / app;
    if (!exists(appdata))
      create_directory(appdata);
    return appdata / (app + _T(".ini"));
  }

protected:

  void InitMainWindow() override
  {
    const auto shrinkToClient = true;
    auto c = make_unique<TRazeeWindow>(nullptr, GetIniFileName());
    auto f = make_unique<TFrameWindow>(nullptr, GetName(), move(c), shrinkToClient);
    f->EnableKBHandler(); // Enable navigation between controls in the client window.
    f->SetMenuDescr(TMenuDescr{TResId{IDM_MAIN}});
    f->SetAcceleratorTable(TResId{IDM_MAIN});
    f->SetIcon(this, TResId{IDI_RAZEE});
    f->SetIconSm(this, TResId{IDI_RAZEE});

    // Restore the window position since last run.
    //
    auto s = TProfile{_T("MainWindowPosition"), to_tstring(GetIniFileName())};
    const auto r = f->GetWindowRect();
    if (nCmdShow == SW_SHOWDEFAULT)
      nCmdShow = s.GetInt(_T("showCmd"), nCmdShow);
    f->MoveWindow(TRect{
      s.GetInt(_T("left"), r.left),
      s.GetInt(_T("top"), r.top),
      s.GetInt(_T("right"), r.right),
      s.GetInt(_T("bottom"), r.bottom)});

    SetMainWindow(move(f));
  }

  auto CanClose() -> bool override
  {
    const auto r = TApplication::CanClose();
    if (!r) return false;

    // Save the window position until next run.
    //
    auto s = TProfile{_T("MainWindowPosition"), to_tstring(GetIniFileName())};
    const auto wp = GetMainWindow()->GetWindowPlacement();
    const auto showCmd = (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) ?
      SW_SHOWDEFAULT : wp.showCmd;
    s.WriteInt(_T("showCmd"), showCmd);
    s.WriteInt(_T("left"), wp.rcNormalPosition.left);
    s.WriteInt(_T("top"), wp.rcNormalPosition.top);
    s.WriteInt(_T("right"), wp.rcNormalPosition.right);
    s.WriteInt(_T("bottom"), wp.rcNormalPosition.bottom);
    return true;
  }

private:

  auto GetClientWindow() -> TRazeeWindow&
  {
    const auto c = GetMainWindow()->GetClientWindow(); CHECK(c);
    return dynamic_cast<TRazeeWindow&>(*c);
  }

  template <razee::TGame::TDifficulty D>
  void CmGameNew()
  {
    GetClientWindow().ResetGame(D);
  }

  void CeGameNew(TCommandEnabler& c)
  {
    c.Enable(GetClientWindow().GetGame().IsGameOver());
  }

  void CmGameResign()
  {
    GetClientWindow().ResignGame();
  }

  void CeGameResign(TCommandEnabler& c)
  {
    c.Enable(!GetClientWindow().GetGame().IsGameOver());
  }

  void CmGameHighScores()
  {
    GetClientWindow().ShowHighScores();
  }

#if defined(_DEBUG)

  void CmTestGameEngine()
  {
    // Note that this debug part of the program does not use resource strings.
    // Hence this part will remain in English, even in translated editions.
    //
    auto s = tostringstream{};
    razee::TestGameEngine(s);
    GetMainWindow()->MessageBox(s.str(), _T("Razee Game Engine Test"), MB_OK);
  }

#endif

  void CmHelpInstructions()
  {
    _USES_CONVERSION_A;
    const auto caption = LoadString(IDS_INSTRUCTIONSCAPTION);
    const auto s = LoadString(IDS_INSTRUCTIONS);
    auto v = TVoice{};
    auto w = wstring{_A2W_A((caption + _T(" - ") + s).c_str())};
    w.erase(remove(begin(w), end(w), L'\n'), end(w));
    v.Speak(w);
    GetMainWindow()->MessageBox(s, caption, MB_OK | MB_ICONINFORMATION);
  }

  void CmHelpAbout()
  {
    TModuleVersionInfo v{GetHandle()};
    const auto caption = LoadString(IDS_ABOUTCAPTION);
    auto s = tostringstream{};
    const auto nl = _T('\n');
    const auto spc = _T(' ');
    s << v.GetProductName() << nl
      << LoadString(IDS_VERSION) << spc << v.GetProductVersion();
    if (v.IsPreRelease())
      s << spc << LoadString(IDS_PRERELEASE);
    if (v.IsSpecialBuild())
      s << spc << v.GetSpecialBuild();
    if (v.IsPrivateBuild())
      s << nl << LoadString(IDS_PRIVATEBUILD);
    s << nl << nl
      << v.GetLegalCopyright() << nl << nl
      << LoadString(IDS_ABOUT);
    GetMainWindow()->MessageBoxIndirect(TResId{IDI_RAZEE}, s.str(), caption);
  }

  DECLARE_RESPONSE_TABLE(TRazee);
};

DEFINE_RESPONSE_TABLE1(TRazee, TApplication)
  EV_COMMAND(CM_GAMEEASY, CmGameNew<razee::TGame::Easy>),
  EV_COMMAND_ENABLE(CM_GAMEEASY, CeGameNew),
  EV_COMMAND(CM_GAMEHARD, CmGameNew<razee::TGame::Hard>),
  EV_COMMAND_ENABLE(CM_GAMEHARD, CeGameNew),
  EV_COMMAND(CM_GAMEMEAN, CmGameNew<razee::TGame::Mean>),
  EV_COMMAND_ENABLE(CM_GAMEMEAN, CeGameNew),
  EV_COMMAND(CM_GAMERESIGN, CmGameResign),
  EV_COMMAND_ENABLE(CM_GAMERESIGN, CeGameResign),
  EV_COMMAND(CM_GAMEHIGHSCORES, CmGameHighScores),

#if defined(_DEBUG)

  EV_COMMAND(CM_TESTGAMEENGINE, CmTestGameEngine),

#endif

  EV_COMMAND(CM_HELPINSTRUCTIONS, CmHelpInstructions),
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
END_RESPONSE_TABLE;

auto OwlMain(int, LPTSTR []) -> int // argc, argv
{
  return TRazee{}.Run();
}
