#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>

inline std::filesystem::path RepoRoot() {
  auto cwd = std::filesystem::current_path();
  if (std::filesystem::exists(cwd / "Game files" / "FAST.RCD")) return cwd;
  return cwd / ".." / ".." / "..";
}

inline std::filesystem::path DataFile(const char* name) {
  auto root = RepoRoot();
  if (std::filesystem::exists(root / name)) return root / name;
  return root / "Game files" / name;
}

inline std::string ReadAll(const std::filesystem::path& p) {
  std::ifstream in(p, std::ios::binary);
  std::ostringstream oss; oss << in.rdbuf();
  return oss.str();
}

inline std::filesystem::path WriteTemp(const std::string& filename, const std::string& contents) {
  auto tmp = std::filesystem::current_path() / filename;
  std::ofstream out(tmp, std::ios::binary); out << contents; out.close();
  return tmp;
}

