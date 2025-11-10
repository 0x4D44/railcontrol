#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>
#include <exception>

#if defined(_MSC_VER)
#include <crtdbg.h>
#include <stdlib.h>
#include <windows.h>
#pragma warning(disable:4702)
#endif

#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"

using namespace RailCore;

static std::filesystem::path RepoRootFromOutDir() {
  std::filesystem::path cwd = std::filesystem::current_path();
  if (std::filesystem::exists(cwd / "FAST.RCD") || std::filesystem::exists(cwd / "Game files" / "FAST.RCD")) return cwd;
  return cwd / ".." / ".." / "..";
}

static std::filesystem::path DataFile(const char* name) {
  auto root = RepoRootFromOutDir();
  auto p1 = root / name; if (std::filesystem::exists(p1)) return p1;
  auto p2 = root / "Game files" / name; if (std::filesystem::exists(p2)) return p2;
  return p1;
}

static int TestMissingFile() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  if (!engine) { std::fprintf(stderr, "CreateEngine failed\n"); return 1; }
  LayoutDescriptor d; d.sourcePath = RepoRootFromOutDir() / "NON_EXISTENT.RCD"; d.name = "missing";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::NotFound) { std::fprintf(stderr, "Expected NotFound for missing file, got %d\n", (int)s.code); return 2; }
  return 0;
}

static int TestLoadFastRcd() {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) { std::fprintf(stderr, "Load FAST.RCD failed: %s\n", s.message.c_str()); return 3; }
  auto snap = engine->GetSnapshot();
  if (!snap.state) { std::fprintf(stderr, "Snapshot state null after load\n"); return 4; }
  if (snap.state->sections.size() < 60 || snap.state->routes.size() < 90 || snap.state->timetable.size() < 40) {
    std::fprintf(stderr, "Parsed counts too small: sections=%zu routes=%zu timetable=%zu\n",
                 snap.state->sections.size(), snap.state->routes.size(), snap.state->timetable.size());
    return 5;
  }
  return 0;
}

int main() {
#if defined(_MSC_VER)
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
  SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
#endif
  // Minimal smoke mode banner
#if defined(_MSC_VER)
  {
    char* mode = nullptr; size_t mlen = 0;
    if (_dupenv_s(&mode, &mlen, "RAILCORE_SMOKE_MODE") == 0 && mode) {
      bool minimal = (_stricmp(mode, "minimal") == 0);
      free(mode);
      if (minimal) {
        std::puts("RailCoreTests: minimal smoke mode active. For full coverage, build & run RailCoreGTest or check CI artifacts.");
        int rc = 0;
        std::puts("SmokeMinimal: TestMissingFile"); if ((rc = TestMissingFile()) != 0) return rc;
        std::puts("SmokeMinimal: TestLoadFastRcd"); if ((rc = TestLoadFastRcd()) != 0) return rc;
        return 0;
      }
    }
  }
#else
  if (const char* mode = std::getenv("RAILCORE_SMOKE_MODE")) {
    if (std::string(mode) == "minimal") {
      std::puts("RailCoreTests: minimal smoke mode active. For full coverage, build & run RailCoreGTest or check CI artifacts.");
      int rc = 0;
      std::puts("SmokeMinimal: TestMissingFile"); if ((rc = TestMissingFile()) != 0) return rc;
      std::puts("SmokeMinimal: TestLoadFastRcd"); if ((rc = TestLoadFastRcd()) != 0) return rc;
      return 0;
    }
  }
#endif
  int rc = 0;
  std::puts("TestMissingFile"); if ((rc = TestMissingFile()) != 0) return rc;
  std::puts("TestLoadFastRcd"); if ((rc = TestLoadFastRcd()) != 0) return rc;
  return 0;
}
