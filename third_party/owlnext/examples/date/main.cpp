//
// OWLNext Application Entry Point
//
#include "pch.h"
#pragma hdrstop

#include <owl/date.h>
#include <cassert>

using namespace owl;

void TestConstructors()
{
  auto check = [](const TDate& d)
  {
    assert(d.DayOfMonth() == 23);
    assert(d.Month() == 12);
    assert(d.Year() == 2014);
  };

  check(TDate{_T("2014-12-23")}); // Default locale
  check(TDate{_T("2014-12-23"), _T("YYYY-MM-DD")}); // ISO format
  check(TDate{_T("23/12/2014"), _T("DD/MM/YYYY")}); // EU format
  check(TDate{_T("12/23/2014"), _T("MM/DD/YYYY")}); // USA format
}

void TestStreaming()
{
  enum { day = 3, month = 12, year = 2013 };
  const auto d1 = TDate{day, month, year};
  
  auto oss = std::ostringstream{};
  TDate::SetPrintOption(TDate::Numbers);
  oss << d1;
  const auto s1 = oss.str();

  auto iss = std::istringstream{s1};
  auto d2 = TDate{};
  iss >> d2;
  assert(!iss.fail());
  const auto s2 = d2.AsString();

  assert(d2.DayOfMonth() == day);
  assert(d2.Month() == month);
  assert(d2.Year() == year);
  assert(d2 == d1);
  assert(s2 == s1);
}

auto OwlMain(int, LPTSTR[]) -> int // argc, argv
{
  TestConstructors();
  TestStreaming();
  return 0;
}
