#include <windows.h>
#include <shellapi.h>
#include <cstdio>
#include <cwchar>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include "cli_validate.h"
#include "railcore/persistence/rcd_repository.h"
#include "railcore/services.h"
#include "railcore/status.h"
#include "railcore/types.h"

using namespace RailCore;

namespace {

bool IsSwitchToken(const wchar_t* s) {
  if (!s) return false;
  return _wcsicmp(s, L"--rcd-validate") == 0 ||
         _wcsicmp(s, L"-rcd-validate") == 0 ||
         _wcsicmp(s, L"/rcd-validate") == 0;
}

bool IsPrintIdToken(const wchar_t* s) {
  if (!s) return false;
  return _wcsicmp(s, L"--print-id") == 0 ||
         _wcsicmp(s, L"-print-id") == 0 ||
         _wcsicmp(s, L"/print-id") == 0;
}

bool HasRcdExtension(const std::wstring& p) {
  size_t dot = p.find_last_of(L'.');
  if (dot == std::wstring::npos) return false;
  std::wstring ext = p.substr(dot);
  return _wcsicmp(ext.c_str(), L".rcd") == 0;
}

bool ContainsWildcard(const std::wstring& p) {
  return p.find(L'*') != std::wstring::npos || p.find(L'?') != std::wstring::npos;
}

bool FileExists(const std::wstring& p) {
  DWORD attrs = GetFileAttributesW(p.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool IsDirectory(const std::wstring& p) {
  DWORD attrs = GetFileAttributesW(p.c_str());
  return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

void AttachToConsoleIfPossible() {
  if (AttachConsole(ATTACH_PARENT_PROCESS)) {
    FILE* fDummy = nullptr;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    std::ios::sync_with_stdio(true);
  }
}

struct MirrorLog {
  FILE* file{nullptr};
  MirrorLog() {
    wchar_t buf[1024];
    DWORD n = GetEnvironmentVariableW(L"RCD_CLI_LOG", buf, 1024);
    if (n > 0 && n < 1024) {
      file = _wfopen(buf, L"wt, ccs=UTF-16LE");
    }
  }
  ~MirrorLog() { if (file) { fclose(file); file = nullptr; } }
  void Line(const wchar_t* s) {
    if (file) { std::fwprintf(file, L"%ls\n", s); std::fflush(file); }
  }
};

void PrintUsage(MirrorLog* mirror) {
  std::fwprintf(stderr, L"Usage: railc --rcd-validate [--print-id] <file-or-directory> [more paths]\n");
  std::fwprintf(stderr, L"Validates .RCD files without launching the UI.\n");
  if (mirror) {
    mirror->Line(L"Usage: railc --rcd-validate [--print-id] <file-or-directory> [more paths]");
    mirror->Line(L"Validates .RCD files without launching the UI.");
  }
  std::fflush(stderr);
}

std::wstring BuildDirectoryPattern(const std::wstring& dir, const wchar_t* suffix) {
  std::wstring pattern = dir;
  if (!pattern.empty()) {
    wchar_t back = pattern.back();
    if (back != L'\\' && back != L'/') pattern += L"\\";
  }
  pattern += suffix;
  return pattern;
}

struct Match {
  std::wstring path;
  bool isDirectory{false};
};

std::vector<Match> ExpandWildcardMatches(const std::wstring& pattern) {
  std::vector<Match> matches;
  WIN32_FIND_DATAW fd{};
  HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
  if (h == INVALID_HANDLE_VALUE) return matches;

  std::wstring dir = pattern;
  size_t slash = dir.find_last_of(L"/\\");
  dir = (slash == std::wstring::npos) ? L"." : dir.substr(0, slash);

  do {
    if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
    std::wstring full = dir + L"\\" + fd.cFileName;
    bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    matches.push_back({full, isDir});
  } while (FindNextFileW(h, &fd));
  FindClose(h);

  std::sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
    return _wcsicmp(a.path.c_str(), b.path.c_str()) < 0;
  });
  return matches;
}

std::vector<std::wstring> ExpandDirectoryFiles(const std::wstring& dir) {
  std::vector<std::wstring> files;
  std::wstring pattern = BuildDirectoryPattern(dir, L"*.RCD");
  WIN32_FIND_DATAW fd{};
  HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
  if (h == INVALID_HANDLE_VALUE) return files;

  std::wstring base = pattern;
  size_t slash = base.find_last_of(L"/\\");
  base = (slash == std::wstring::npos) ? L"." : base.substr(0, slash);

  do {
    if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) continue;
    std::wstring full = base + L"\\" + fd.cFileName;
    files.push_back(full);
  } while (FindNextFileW(h, &fd));
  FindClose(h);
  return files;
}

void SortDeterministic(std::vector<std::wstring>& v) {
  std::sort(v.begin(), v.end(), [](const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(a.c_str(), b.c_str()) < 0;
  });
}

void AppendLine(CliValidationSummary& summary, bool success, const std::wstring& text) {
  summary.lines.push_back({success, text});
  if (!success) summary.anyFailure = true;
}

std::vector<std::wstring> ExpandInputPath(const std::wstring& arg, CliValidationSummary& summary) {
  std::vector<std::wstring> files;
  if (ContainsWildcard(arg)) {
    auto matches = ExpandWildcardMatches(arg);
    for (const auto& m : matches) {
      if (m.isDirectory) {
        auto expanded = ExpandDirectoryFiles(m.path);
        if (expanded.empty()) {
          AppendLine(summary, false, L"Invalid: " + m.path + L": no .RCD files found");
        } else {
          files.insert(files.end(), expanded.begin(), expanded.end());
        }
      } else {
        files.push_back(m.path);
      }
    }
    return files;
  }

  if (IsDirectory(arg)) {
    auto expanded = ExpandDirectoryFiles(arg);
    if (expanded.empty()) {
      AppendLine(summary, false, L"Invalid: " + arg + L": no .RCD files found");
    } else {
      files.insert(files.end(), expanded.begin(), expanded.end());
    }
    return files;
  }

  files.push_back(arg);
  return files;
}

std::wstring ToWide(const std::string& s) {
  return std::wstring(s.begin(), s.end());
}

} // namespace

CliValidationSummary EvaluateRcdValidationForTesting(const std::vector<std::wstring>& rawInputs,
                                                     const CliValidationOptions& options) {
  CliValidationSummary summary;
  std::vector<std::wstring> files;
  files.reserve(rawInputs.size());

  for (const auto& arg : rawInputs) {
    auto expanded = ExpandInputPath(arg, summary);
    files.insert(files.end(), expanded.begin(), expanded.end());
  }

  SortDeterministic(files);
  const bool multiple = files.size() > 1;

  RcdLayoutRepository repo;
  for (const auto& path : files) {
    if (!FileExists(path)) {
      AppendLine(summary, false, L"Invalid: " + path + L": not found");
      continue;
    }
    LayoutDescriptor desc;
    desc.sourcePath = std::filesystem::path(path);
    WorldState ws;
    Status st = repo.Load(desc, ws);
    if (st.code == StatusCode::Ok) {
      std::wstring idSuffix;
      if (options.printId && !desc.id.empty()) {
        idSuffix = L" (id=" + ToWide(desc.id) + L")";
      }
      if (multiple) {
        AppendLine(summary, true, L"Valid: " + path + idSuffix);
      } else {
        AppendLine(summary, true, idSuffix.empty() ? std::wstring(L"Valid")
                                                   : (std::wstring(L"Valid") + idSuffix));
      }
    } else {
      std::wstring wmsg = ToWide(st.message);
      AppendLine(summary, false, L"Invalid: " + path + L": " + wmsg);
    }
  }

  return summary;
}

extern "C" int RunRcdValidationCli(int argc, wchar_t** argv) {
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
  #if defined(_DEBUG)
  _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
  #endif
  AttachToConsoleIfPossible();
  MirrorLog mirror;

  auto handleException = [&](const std::wstring& msg) -> int {
    std::fwprintf(stderr, L"Internal error: %ls\n", msg.c_str());
    mirror.Line((L"Internal error: " + msg).c_str());
    std::fflush(stderr);
    return 3;
  };

  try {
    int switchIndex = -1;
    for (int i = 1; i < argc; ++i) {
      if (IsSwitchToken(argv[i])) { switchIndex = i; break; }
    }
    if (switchIndex < 0) {
      PrintUsage(&mirror);
      return 2;
    }
    if (switchIndex + 1 >= argc) {
      PrintUsage(&mirror);
      return 2;
    }

    CliValidationOptions options;
    std::vector<std::wstring> rawInputs;
    for (int i = switchIndex + 1; i < argc; ++i) {
      std::wstring arg = argv[i];
      if (IsPrintIdToken(arg.c_str())) {
        options.printId = true;
        continue;
      }
      if (!HasRcdExtension(arg) && !ContainsWildcard(arg) && !IsDirectory(arg) && !FileExists(arg) && (i + 1) < argc) {
        std::wstring merged = arg + L" " + argv[i + 1];
        if (HasRcdExtension(merged) || IsDirectory(merged) || FileExists(merged) || ContainsWildcard(merged)) {
          arg.swap(merged);
          ++i;
        }
      }
      rawInputs.push_back(arg);
    }

    if (rawInputs.empty()) {
      PrintUsage(&mirror);
      return 2;
    }

    CliValidationSummary summary = EvaluateRcdValidationForTesting(rawInputs, options);
    for (const auto& line : summary.lines) {
      std::wprintf(L"%ls\n", line.text.c_str());
      mirror.Line(line.text.c_str());
    }

    std::fflush(stdout);
    std::fflush(stderr);
    return summary.anyFailure ? 1 : 0;
  } catch (const std::exception& ex) {
    return handleException(ToWide(std::string(ex.what())));
  } catch (...) {
    return handleException(L"Unknown exception");
  }
}
