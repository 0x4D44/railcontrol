#include <gtest/gtest.h>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace {

std::wstring GetExeDir() {
  wchar_t buf[MAX_PATH];
  DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
  std::wstring path(buf, (n ? n : 0));
  size_t pos = path.find_last_of(L"/\\");
  if (pos == std::wstring::npos) return L".";
  return path.substr(0, pos);
}

struct RunResult {
  DWORD exitCode{0xFFFFFFFF};
  std::vector<std::wstring> logLines;
};

RunResult RunRailc(const std::wstring& args, const std::wstring& logName) {
  RunResult rr{};
  std::wstring exe = GetExeDir() + L"\\railc.exe";
  std::wstring cmd = L"\"" + exe + L"\" " + args;
  std::wstring logPath = GetExeDir() + L"\\" + logName;
  _wputenv_s(L"RCD_CLI_LOG", logPath.c_str());
  STARTUPINFOW si{}; si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  std::wstring cmdMutable = cmd;
  BOOL ok = CreateProcessW(exe.c_str(), cmdMutable.data(), nullptr, nullptr, FALSE, 0, nullptr, GetExeDir().c_str(), &si, &pi);
  if (!ok) { rr.exitCode = GetLastError(); return rr; }
  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD ec = 0; GetExitCodeProcess(pi.hProcess, &ec);
  rr.exitCode = ec;
  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  std::wifstream fin(logPath);
  std::wstring line;
  while (std::getline(fin, line)) { rr.logLines.push_back(line); }
  return rr;
}

bool AnyLineStartsWith(const std::vector<std::wstring>& v, const std::wstring& prefix) {
  for (const auto& s : v) if (s.rfind(prefix, 0) == 0) return true; return false;
}

TEST(CliUsage, MissingPathsReturnsUsageAndExit2) {
  auto rr = RunRailc(L"--rcd-validate", L"cli_log_usage.txt");
  EXPECT_EQ(rr.exitCode, 2u);
  EXPECT_TRUE(AnyLineStartsWith(rr.logLines, L"Usage: railc --rcd-validate"));
}

} // namespace

