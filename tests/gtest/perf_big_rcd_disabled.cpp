#include <gtest/gtest.h>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

// Disabled by default; intended for manual/local perf checks.
TEST(PerfDISABLED, BuildAndParseLargeSyntheticRcd) {
  std::ostringstream o;
  o << "[GENERAL]\nStartTime=0700\nStopTime=2000\n";
  o << "[SECTIONS]\n";
  for (int i = 1; i <= 2000; ++i) o << i << '\n';
  o << "[OVERLAPPING]\n";
  for (int i = 1; i <= 500; ++i) o << (i*2) << ", " << (i*2+1) << '\n';
  o << "[PLATFORMS]\n";
  for (int i = 1; i <= 1000; ++i) o << i << ", 0,0,1,1,2,2,3,3\n";
  o << "[SELECTOR]\n";
  for (int i = 1; i <= 500; ++i) o << i << ", 1,1,1,1,1,1,1, UF\n";
  o << "[ROUTES]\n";
  for (int i = 1; i <= 1500; ++i) o << i << ", 0, 0, 0, 0, 0, 0, 0, 0\n";
  o << "[LOCOS]\n";
  for (int i = 1; i <= 200; ++i) o << i << ", 31, 999, 1\n";
  o << "[LOCOYARD]\nDISABLED\n";
  o << "[TIMETABLE]\n";
  for (int i = 1; i <= 2000; ++i) o << i << ", A, A, 1, 700, 0, 705, 1, 1, 0, 0, 0\n";

  std::string content = o.str();
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = std::filesystem::current_path() / "synthetic_big.rcd"; d.name = "big";
  { std::ofstream out(d.sourcePath, std::ios::binary); out << content; }
  auto s = engine->LoadLayout(d);
  EXPECT_EQ(s.code, StatusCode::Ok) << s.message;
}

