//
// Razee - Dice Challenge
// Copyright (c) 2019 Vidar Hasfjord
// All rights reserved.
//
// \file Game engine header.
//
#pragma once

#include <owl/defs.h> // For agnostic strings and streams (e.g. tstring, tostream).
#include <array>
#include <vector>
#include <map>
#include <chrono>
#include <optional>
#include <iosfwd>

namespace razee
{

class TDie 
{
public:

  static const auto SideCount = 6;

  void Roll() noexcept;
  auto GetValue() const noexcept -> int;

private:

  int Value = GetRandomSide();

  static auto GetRandomSide() noexcept -> int;

};

class TDice
{
public:

  static const auto Count = 6;
  enum class TState {Unlocked, Locked};

  auto GetDie(int i) const noexcept -> const TDie&;
  auto IsLocked(int i) const noexcept -> bool;
  auto IsAllLocked() const noexcept -> bool;
  void Lock(int i) noexcept;
  void LockAll() noexcept;
  void Unlock(int i) noexcept;
  void UnlockAll() noexcept;
  void Roll() noexcept;
  auto GetSum() const noexcept -> int;

  using TUniqueTuples = std::array<std::vector<int>, Count>;

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
  auto Analyse() const -> TUniqueTuples;

private:

  struct TDieState 
  {
    TDie Die;
    TState State;
  };
  using TDieStates = std::array<TDieState, Count>;
  TDieStates Dice{};

};

class TScoreboard
{
public:

  static auto GetInstance() -> const TScoreboard&;

  enum TFieldId
  {
    Pair,
    PairDuo,
    PairTrio,
    Triple,
    TripleDuo,
    TripleAndPair,
    Quadruple,
    QuadrupleAndPair,
    Quintuple,
    Sextuple,
    LowPartialSequence,
    HighPartialSequence,
    CompleteSequence
  };

  static const auto FieldCount = 13;

  class TField
  {
  public:

    using TValidateFun = auto (*)(const TDice&) -> bool;

    TField(TValidateFun f) noexcept
      : ValidateFun{f}
    {}

    auto CalculateScore(const TDice& d) const -> int {return Validate(d) ? d.GetSum() : 0;}
    auto Validate(const TDice& d) const -> bool {return (*ValidateFun)(d);}

  private:

    TValidateFun ValidateFun;

  };

  auto GetField(TFieldId i) const -> const TField &;

private:

  TScoreboard();

  using TFields = std::array<TField, FieldCount>;
  TFields Fields;

};

class TGame
{
public:

  enum TDifficulty {Easy = 300, Hard = 180, Mean = 120};
  static const auto DifficultyCount = 3;

  TGame(TDifficulty d);

  auto GetDifficulty() const -> TDifficulty {return Difficulty;}
  using TTime = std::chrono::system_clock::time_point;
  auto GetStartTime() const -> TTime {return StartTime;}
  auto GetEndTime() const -> std::optional<TTime> {return EndTime;}
  auto GetTimeBonus() const -> int;
  auto GetTotalScore() const -> int;
  auto GetScore(TScoreboard::TFieldId i) const -> int;
  auto SetScore(TScoreboard::TFieldId i, const TDice& d) -> bool;

  // The game is over if we have completed all the fields or the time has run out.
  //
  auto IsGameOver() const -> bool;

  void Resign();

private:

  TDifficulty Difficulty;
  TTime StartTime;
  std::optional<TTime> EndTime;
  std::array<int, TScoreboard::FieldCount> Scores;

};

class THighScores
{
public:

  static const auto MaxEntryCount = 9;

  THighScores();
  THighScores(owl::tistream&);

  using TPlayer = owl::tstring;
  struct TEntry
  {
    TPlayer Player;
    int Score;
  };
  using TEntries = std::vector<TEntry>;

  auto GetEntries(TGame::TDifficulty d) const -> const TEntries&;
  auto DoesQualify(const TGame& g) const -> bool;
  auto Enter(const TPlayer& player, const TGame& g) -> bool;

  void Serialize(owl::tostream&) const;

private:

  using TDictionary = std::map<TGame::TDifficulty, TEntries>;
  TDictionary Scores;

};

void TestGameEngine(owl::tostream&);

}; // namespace razee
