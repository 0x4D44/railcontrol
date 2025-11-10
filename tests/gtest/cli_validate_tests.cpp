#include <gtest/gtest.h>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "cli_validate.h"
#include "railcore/persistence/rcd_repository.h"
#include "railcore/status.h"

namespace {

std::wstring GetExeDir() {
  wchar_t buf[MAX_PATH];
  DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
  std::wstring path(buf, (n ? n : 0));
  size_t pos = path.find_last_of(L"/\\");
  if (pos == std::wstring::npos) return L".";
  return path.substr(0, pos);
}

std::filesystem::path GetRepoRoot() {
  std::filesystem::path exeDir = GetExeDir();
  return exeDir.parent_path().parent_path();
}

CliValidationSummary RunHelper(std::initializer_list<std::wstring> inputs,
                               const CliValidationOptions& opts = CliValidationOptions{}) {
  return EvaluateRcdValidationForTesting(std::vector<std::wstring>(inputs), opts);
}

bool AnyLineExact(const CliValidationSummary& summary, const std::wstring& needle) {
  for (const auto& line : summary.lines) if (line.text == needle) return true;
  return false;
}

bool AnyLineStartsWith(const CliValidationSummary& summary, const std::wstring& prefix) {
  for (const auto& line : summary.lines) if (line.text.rfind(prefix, 0) == 0) return true;
  return false;
}

std::vector<std::wstring> CollectLinesWithPrefix(const CliValidationSummary& summary, const std::wstring& prefix) {
  std::vector<std::wstring> results;
  for (const auto& line : summary.lines) if (line.text.rfind(prefix, 0) == 0) results.push_back(line.text);
  return results;
}

size_t FindLineIndex(const CliValidationSummary& summary, const std::wstring& needle) {
  for (size_t i = 0; i < summary.lines.size(); ++i) {
    if (summary.lines[i].text == needle) return i;
  }
  return static_cast<size_t>(-1);
}

std::wstring ComputeLayoutId(const std::wstring& path) {
  RailCore::RcdLayoutRepository repo;
  RailCore::LayoutDescriptor desc;
  desc.sourcePath = std::filesystem::path(path);
  RailCore::WorldState ws;
  auto status = repo.Load(desc, ws);
  if (status.code != RailCore::StatusCode::Ok) {
    ADD_FAILURE() << "Load failed for " << std::string(path.begin(), path.end())
                  << " with status " << static_cast<int>(status.code) << ": " << status.message;
    return {};
  }
  return std::wstring(desc.id.begin(), desc.id.end());
}

TEST(CliValidate, ValidSingleFile) {
  auto repoRoot = GetRepoRoot();
  auto path = (repoRoot / L"Game files" / L"FAST.RCD").wstring();
  auto summary = RunHelper({path});
  EXPECT_FALSE(summary.anyFailure);
  EXPECT_TRUE(AnyLineExact(summary, L"Valid"));
}

TEST(CliValidate, PrintIdIncludesHashForSingle) {
  auto repoRoot = GetRepoRoot();
  auto path = (repoRoot / L"Game files" / L"FAST.RCD").wstring();
  CliValidationOptions opts; opts.printId = true;
  auto summary = RunHelper({path}, opts);
  EXPECT_FALSE(summary.anyFailure);
  auto id = ComputeLayoutId(path);
  ASSERT_FALSE(id.empty());
  EXPECT_TRUE(AnyLineExact(summary, L"Valid (id=" + id + L")"));
}

TEST(CliValidate, InvalidBadArrSelector) {
  auto repoRoot = GetRepoRoot();
  auto path = (repoRoot / L"bad_tt_arrsel.rcd").wstring();
  auto summary = RunHelper({path});
  EXPECT_TRUE(summary.anyFailure);
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Invalid: " + path + L":"));
}

TEST(CliValidate, WildcardUnderGameFiles) {
  auto repoRoot = GetRepoRoot();
  auto pattern = (repoRoot / L"Game files").wstring() + L"\\*.RCD";
  auto summary = RunHelper({pattern});
  EXPECT_FALSE(summary.anyFailure);
  auto valids = CollectLinesWithPrefix(summary, L"Valid:");
  EXPECT_FALSE(valids.empty());
  for (const auto& line : valids) {
    EXPECT_NE(line.find(L"Game files"), std::wstring::npos);
  }
}

TEST(CliValidate, NotFoundPath) {
  auto repoRoot = GetRepoRoot();
  auto path = (repoRoot / L"Game files" / L"NOPE.RCD").wstring();
  auto summary = RunHelper({path});
  EXPECT_TRUE(summary.anyFailure);
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Invalid: " + path + L": not found"));
}

TEST(CliValidate, EmptyDirectoryYieldsInvalid) {
  std::filesystem::path tmp = std::filesystem::path(GetExeDir()) / L"cli_empty_dir";
  std::error_code ec;
  std::filesystem::create_directories(tmp, ec);

  auto summary = RunHelper({tmp.wstring()});
  EXPECT_TRUE(summary.anyFailure);
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Invalid: " + tmp.wstring() + L": no .RCD files found"));

  std::filesystem::remove_all(tmp, ec);
}

TEST(CliValidate, MixedDirectoryValidAndInvalid) {
  std::filesystem::path exeDir = GetExeDir();
  std::filesystem::path tmp = exeDir / L"cli_mix_dir";
  std::error_code ec;
  std::filesystem::create_directories(tmp, ec);

  auto source = GetRepoRoot() / L"Game files" / L"FAST.RCD";
  std::filesystem::copy_file(source, tmp / L"FAST.RCD", std::filesystem::copy_options::overwrite_existing, ec);
  {
    std::ofstream out((tmp / L"bad.rcd").string(), std::ios::binary);
  }

  auto summary = RunHelper({tmp.wstring()});
  EXPECT_TRUE(summary.anyFailure);
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Valid: " + (tmp / L"FAST.RCD").wstring()));
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Invalid: " + (tmp / L"bad.rcd").wstring()));

  std::filesystem::remove_all(tmp, ec);
}

TEST(CliValidate, MultiPathsMixedValidInvalid) {
  auto repoRoot = GetRepoRoot();
  auto good = (repoRoot / L"Game files" / L"FAST.RCD").wstring();
  auto bad = (repoRoot / L"bad_tt_arrsel.rcd").wstring();

  auto summary = RunHelper({good, bad});
  EXPECT_TRUE(summary.anyFailure);
  EXPECT_TRUE(AnyLineExact(summary, L"Valid: " + good));
  EXPECT_TRUE(AnyLineStartsWith(summary, L"Invalid: " + bad + L":"));
}

TEST(CliValidate, PrintIdIncludesHashForMultiple) {
  auto repoRoot = GetRepoRoot();
  auto pathA = (repoRoot / L"Game files" / L"FAST.RCD").wstring();
  auto pathB = (repoRoot / L"Game files" / L"KINGSX.RCD").wstring();
  CliValidationOptions opts; opts.printId = true;
  auto summary = RunHelper({pathA, pathB}, opts);
  EXPECT_FALSE(summary.anyFailure);
  auto idA = ComputeLayoutId(pathA);
  auto idB = ComputeLayoutId(pathB);
  ASSERT_FALSE(idA.empty());
  ASSERT_FALSE(idB.empty());
  EXPECT_TRUE(AnyLineExact(summary, L"Valid: " + pathA + L" (id=" + idA + L")"));
  EXPECT_TRUE(AnyLineExact(summary, L"Valid: " + pathB + L" (id=" + idB + L")"));
}

TEST(CliValidate, DeterministicSortingOfWildcard) {
  std::filesystem::path exeDir = GetExeDir();
  std::filesystem::path tmp = exeDir / L"cli_sort_dir";
  std::error_code ec;
  std::filesystem::create_directories(tmp, ec);

  auto source = GetRepoRoot() / L"Game files" / L"FAST.RCD";
  std::filesystem::copy_file(source, tmp / L"b.rcd", std::filesystem::copy_options::overwrite_existing, ec);
  std::filesystem::copy_file(source, tmp / L"A.RCD", std::filesystem::copy_options::overwrite_existing, ec);

  auto pattern = tmp.wstring() + L"\\*.RCD";
  auto summary = RunHelper({pattern});
  EXPECT_FALSE(summary.anyFailure);

  std::wstring aLine = L"Valid: " + (tmp / L"A.RCD").wstring();
  std::wstring bLine = L"Valid: " + (tmp / L"b.rcd").wstring();
  size_t idxA = FindLineIndex(summary, aLine);
  size_t idxB = FindLineIndex(summary, bLine);
  ASSERT_NE(idxA, static_cast<size_t>(-1));
  ASSERT_NE(idxB, static_cast<size_t>(-1));
  EXPECT_LT(idxA, idxB);

  std::filesystem::remove_all(tmp, ec);
}

TEST(CliValidate, WildcardHandlesDirectoriesDeterministically) {
  std::filesystem::path exeDir = GetExeDir();
  std::filesystem::path base = exeDir / L"cli_wc_parent";
  std::filesystem::path nested = base / L"badNest";
  std::filesystem::path emptyDir = base / L"badEmpty";
  std::error_code ec;
  std::filesystem::remove_all(base, ec);
  std::filesystem::create_directories(nested, ec);
  std::filesystem::create_directories(emptyDir, ec);

  auto source = GetRepoRoot() / L"Game files" / L"FAST.RCD";
  std::filesystem::copy_file(source, base / L"badRoot.rcd", std::filesystem::copy_options::overwrite_existing, ec);
  std::filesystem::copy_file(source, nested / L"inner.rcd", std::filesystem::copy_options::overwrite_existing, ec);

  auto pattern = base.wstring() + L"\\bad*";
  auto summary = RunHelper({pattern});

  // Expect deterministic ordering: badEmpty directory message, then files sorted.
  std::wstring emptyLine = L"Invalid: " + emptyDir.wstring() + L": no .RCD files found";
  std::wstring nestedLine = L"Valid: " + (nested / L"inner.rcd").wstring();
  std::wstring rootLine = L"Valid: " + (base / L"badRoot.rcd").wstring();

  EXPECT_TRUE(AnyLineExact(summary, emptyLine));
  EXPECT_TRUE(AnyLineExact(summary, nestedLine));
  EXPECT_TRUE(AnyLineExact(summary, rootLine));

  size_t idxEmpty = FindLineIndex(summary, emptyLine);
  size_t idxNested = FindLineIndex(summary, nestedLine);
  size_t idxRoot = FindLineIndex(summary, rootLine);
  ASSERT_NE(idxEmpty, static_cast<size_t>(-1));
  ASSERT_NE(idxNested, static_cast<size_t>(-1));
  ASSERT_NE(idxRoot, static_cast<size_t>(-1));
  EXPECT_LT(idxEmpty, idxNested);
  EXPECT_LT(idxNested, idxRoot);

  std::filesystem::remove_all(base, ec);
}

} // namespace
