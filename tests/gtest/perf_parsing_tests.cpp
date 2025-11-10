#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static std::string MakeLargeRcd(size_t sections, size_t routes, size_t selectors) {
  std::ostringstream o;
  o << "[GENERAL]\nStartTime, 0700\nStopTime, 2300\n";
  o << "[SECTIONS]\n";
  for (size_t i=1;i<=sections;++i) o << i << ", 0,0,0,0,0,0,0,0\n";
  o << "[OVERLAPPING]\n";
  o << "[PLATFORMS]\n1, 0,0,0,0,0,0,0,0\n";
  o << "[SELECTOR]\n";
  for (size_t i=1;i<=selectors;++i) o << i << ", 1,1,1,1,1,1, UF\n";
  o << "[ROUTES]\n";
  for (size_t i=1;i<=routes;++i) o << i << ", 0, 0, 0, 0, 0, 0, 0, 0\n";
  o << "[LOCOS]\n1, X\n";
  o << "[LOCOYARD]\nDisabled\n";
  o << "[TIMETABLE]\n1, T, T, 1, 0700, 0, 0705, 1, 1, 0, 0, 0\n";
  return o.str();
}

TEST(DISABLED_Perf, RcdParsingLargeSynth) {
  std::string data = MakeLargeRcd(1000, 1000, 1000);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.name = "synth";
  // Write to temp file
  std::filesystem::path tmp = std::filesystem::current_path() / "gtest_perf_large.rcd";
  std::ofstream out(tmp, std::ios::binary); out << data; out.close();
  d.sourcePath = tmp;
  Status s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}
