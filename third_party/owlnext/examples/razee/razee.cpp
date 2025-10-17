//
// Razee - Dice Challenge
// Copyright (c) 2019 Vidar Hasfjord
// All rights reserved.
//
// \file Game engine implementation.
//
#include "pch.h"
#pragma hdrstop

#include "razee.h"
#include <random>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <ostream>

using namespace std;

namespace razee
{

void TDie::Roll() noexcept
{
  Value = GetRandomSide();
}

auto TDie::GetValue() const noexcept -> int
{
  assert(Value >= 1 && Value <= SideCount);
  return Value;
}

auto TDie::GetRandomSide() noexcept -> int
{
  static auto e = default_random_engine{random_device{}()};
  return uniform_int_distribution<int>{1, SideCount}(e);
}

auto TDice::GetDie(int i) const noexcept -> const TDie&
{
  assert(i >= 0 && i < Count);
  return Dice[i].Die;
}

auto TDice::IsLocked(int i) const noexcept -> bool
{
  assert(i >= 0 && i < Count);
  return Dice[i].State == TState::Locked;
}

auto TDice::IsAllLocked() const noexcept -> bool
{
  return all_of(cbegin(Dice), cend(Dice), [](const TDieState & e) { return e.State == TState::Locked; });
}

void TDice::Lock(int i) noexcept
{
  assert(i >= 0 and i < Count);
  Dice[i].State = TState::Locked;
}

void TDice::LockAll() noexcept
{
  for_each(begin(Dice), end(Dice), [](TDieState & e) { e.State = TState::Locked; });
}

void TDice::Unlock(int i) noexcept
{
  assert(i >= 0 and i < Count);
  Dice[i].State = TState::Unlocked;
}

void TDice::UnlockAll() noexcept
{
  for_each(begin(Dice), end(Dice), [](TDieState & e) { e.State = TState::Unlocked; });
}

void TDice::Roll() noexcept
{
  for (auto& d : Dice)
    if (d.State != TState::Locked)
      d.Die.Roll();
}

auto TDice::GetSum() const noexcept -> int
{
  return accumulate(cbegin(Dice), cend(Dice), 0, [](int a, const TDieState & e) { return a + e.Die.GetValue(); });
}

//
// Returns the unique tuples found for each die value.
//
// In the array of vectors returned, the first vector contains the 1-tuples found. For example,
// if the dice are all equal, this vector will contain one element equal to the die value. If the
// dice are all different, then this vector will contain elements for each die value.
//
// Similarly, the second vector contains the unique 2-tuples (pairs) found. If the dice contain
// a pair of 2s and a pair of 3s, then the vector will have two elements {2, 3}. Note that pairs
// of the same value are not counted. For example, if the dice contains four 3s, there is only
// one unique pair of 3s, and hence only one entry for 3 in the second vector.
//
// The third vector contains 3-tuples found, and so on. In total, n vectors are returned, where
// n is the number of dice.
//
auto TDice::Analyse() const -> TUniqueTuples
{
  auto counts = array<int, TDie::SideCount + 1>{}; // Add one for convenience (die value == index).
  for (const auto& d: Dice)
    ++counts[d.Die.GetValue()];
  auto r = TUniqueTuples{};
  for (auto v = 1; v != size(counts); ++v)
  {
    const auto n = counts[v];
    for_each(begin(r), next(begin(r), n), [&](auto& t) { t.push_back(v); });
  }
  return r;
}

auto TScoreboard::GetInstance() -> const TScoreboard&
{
  static auto i = TScoreboard{};
  return i;
}

auto TScoreboard::GetField(TFieldId i) const -> const TField &
{
  assert(i >= 0 && i < FieldCount);
  return Fields[i];
}

TScoreboard::TScoreboard()
  : Fields
{{
  TField{[](const TDice& d) {return d.Analyse()[1].size() >= 1;}},
  TField{[](const TDice& d) {return d.Analyse()[1].size() >= 2;}},
  TField{[](const TDice& d) {return d.Analyse()[1].size() >= 3;}},
  TField{[](const TDice& d) {return d.Analyse()[2].size() >= 1;}},
  TField{[](const TDice& d) {return d.Analyse()[2].size() >= 2;}},
  TField{[](const TDice& d) {const auto t = d.Analyse(); return t[2].size() >= 1 && t[1].size() >= 2;}},
  TField{[](const TDice& d) {return d.Analyse()[3].size() >= 1;}},
  TField{[](const TDice& d) {const auto t = d.Analyse(); return t[3].size() >= 1 && t[1].size() >= 2;}},
  TField{[](const TDice& d) {return d.Analyse()[4].size() >= 1;}},
  TField{[](const TDice& d) {return d.Analyse()[5].size() >= 1;}},
  TField{[](const TDice& d) {const auto t = d.Analyse(); return (t[0].size() == TDie::SideCount) || (t[0].size() == TDie::SideCount - 1 && t[0][TDie::SideCount - 2] != TDie::SideCount);}},
  TField{[](const TDice& d) {const auto t = d.Analyse(); return (t[0].size() == TDie::SideCount) || (t[0].size() == TDie::SideCount - 1 && t[0][0] != 1);}},
  TField{[](const TDice& d) {return d.Analyse()[0].size() == TDie::SideCount;}}
}}  
{}

TGame::TGame(TDifficulty d)
  : Difficulty(d),
  StartTime(chrono::system_clock::now()),
  EndTime{},
  Scores{}
{}

auto TGame::GetTimeBonus() const -> int
{
  const auto t = EndTime ? *EndTime : chrono::system_clock::now();
  const auto limit = StartTime + chrono::seconds{Difficulty};
  return (t < limit) ? static_cast<int>(chrono::duration_cast<chrono::seconds>(limit - t).count()) : 0;
}

auto TGame::GetTotalScore() const -> int
{
  const auto fieldScores = accumulate(cbegin(Scores), cend(Scores), 0);
  return fieldScores + GetTimeBonus();
}

auto TGame::GetScore(TScoreboard::TFieldId i) const -> int
{
  assert(i >= 0 && i < TScoreboard::FieldCount);
  return Scores[i];
}

auto TGame::SetScore(TScoreboard::TFieldId i, const TDice& d) -> bool
{
  assert(i >= 0 && i < TScoreboard::FieldCount);
  if (IsGameOver() || Scores[i] != 0) return false;

  const auto s = TScoreboard::GetInstance().GetField(i).CalculateScore(d);
  if (s != 0)
  {
    Scores[i] = s;
    if (count(begin(Scores), end(Scores), 0) == 0) // Scoreboard completed?
      EndTime = chrono::system_clock::now();
  }
  return s != 0;
}

// The game is over if we have completed all the fields or the time has run out.
//

auto TGame::IsGameOver() const -> bool
{
  return EndTime || chrono::system_clock::now() >= (StartTime + chrono::seconds{Difficulty});
}

void TGame::Resign()
{
  EndTime = StartTime + chrono::seconds{Difficulty};
}

THighScores::THighScores()
  : Scores{{TGame::Easy, {}}, {TGame::Hard, {}}, {TGame::Mean, {}}}
{}

THighScores::THighScores(owl::tistream& is)
  : Scores{}
{
  const auto difficulties = {TGame::Easy, TGame::Hard, TGame::Mean};
  for (int i = 0; i != TGame::DifficultyCount; ++i)
  {
    auto di = 0; is >> di; is.ignore();
    auto d = static_cast<TGame::TDifficulty>(di);
    if (find(begin(difficulties), end(difficulties), d) == end(difficulties))
    {
      is.setstate(ios::failbit);
      break;
    }
    auto s = TEntries{};
    int n; is >> n; is.ignore();
    for (; n != 0; --n)
    {
      auto player = TPlayer{};
      getline(is, player);
      auto score = 0;
      is >> score; is.ignore();
      s.push_back({player, score});
    }
    Scores.insert({d, s});
  }
}

auto THighScores::GetEntries(TGame::TDifficulty d) const -> const TEntries &
{
  const auto i = Scores.find(d);
  assert(i != end(Scores));
  return i->second;
}

auto THighScores::DoesQualify(const TGame& g) const -> bool
{
  if (!g.IsGameOver() || g.GetTotalScore() == 0) return false;
  const auto i = Scores.find(g.GetDifficulty());
  assert(i != end(Scores));
  const auto & entries = i->second;
  return entries.size() < MaxEntryCount || g.GetTotalScore() > entries.back().Score;
}

auto THighScores::Enter(const TPlayer& player, const TGame& g) -> bool
{
  if (!DoesQualify(g)) return false;

  const auto score = g.GetTotalScore();
  auto& v = Scores[g.GetDifficulty()];
  const auto i = lower_bound(begin(v), end(v), score, [](const TEntry& e, int s) { return e.Score >= s; });
  v.insert(i, TEntry{player, g.GetTotalScore()});
  if (v.size() > MaxEntryCount)
    v.pop_back();
  return true;
}

void THighScores::Serialize(owl::tostream& os) const
{
  const auto nl = _T('\n');
  for (const auto& s: Scores)
  {
    os << s.first << nl << s.second.size() << nl;
    for (const auto& entry: s.second)
    {
      os << entry.Player << nl
        << entry.Score << nl;
    }
  }
}

void TestGameEngine(owl::tostream& os)
{
  auto d = TDice{};
  auto sb = TScoreboard::GetInstance();
  auto g = TGame{TGame::Easy};

  static const auto fields = array<pair<TScoreboard::TFieldId, const owl::tchar*>, TScoreboard::FieldCount>
  {{
    {TScoreboard::Pair, _T("Pair")},
    {TScoreboard::PairDuo, _T("PairDuo")},
    {TScoreboard::PairTrio, _T("PairTrio")},
    {TScoreboard::Triple, _T("Triple")},
    {TScoreboard::TripleDuo, _T("TripleDuo")},
    {TScoreboard::TripleAndPair, _T("TripleAndPair")},
    {TScoreboard::Quadruple, _T("Quadruple")},
    {TScoreboard::QuadrupleAndPair, _T("QuadrupleAndPair")},
    {TScoreboard::Quintuple, _T("Quintuple")},
    {TScoreboard::Sextuple, _T("Sextuple")},
    {TScoreboard::LowPartialSequence, _T("LowPartialSequence")},
    {TScoreboard::HighPartialSequence, _T("HighPartialSequence")},
    {TScoreboard::CompleteSequence, _T("CompleteSequence")}
  }};

  os << _T("Scoreboard:\n\n");
  auto totalRollCount = 0;
  for (const auto [fieldId, fieldName]: fields)
  {
    const auto& field = sb.GetField(fieldId);
    auto rollCount = 0;
    do
    {
      d.Roll();
      ++rollCount;
    }
    while (!field.Validate(d));
    g.SetScore(fieldId, d);
    totalRollCount += rollCount;
    os << fieldName << _T(" = ") << g.GetScore(fieldId) << _T(" {");
    for (auto j = 0; j != TDice::Count; ++j)
      os << (j > 0 ? _T(", ") : _T("")) << d.GetDie(j).GetValue();
    os << _T("} #") << rollCount << '\n';
  }
  os << _T("\nTime bonus: ") << g.GetTimeBonus()
    << _T("\nTotal score: ") << g.GetTotalScore()
    << _T("\nNumber of rolls: ") << totalRollCount;
}

} // namespace