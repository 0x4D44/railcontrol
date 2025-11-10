#include "railcore/persistence/rcd_id.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <windows.h>
#include <bcrypt.h>

#ifndef BCRYPT_SUCCESS
#define BCRYPT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

namespace RailCore {

std::string CanonicalizeRcdContent(const std::string& raw) {
  std::string s;
  s.reserve(raw.size());
  // Drop CR characters
  for (char ch : raw) if (ch != '\r') s.push_back(ch);
  // Trim leading and trailing whitespace on each line
  std::vector<std::string> lines;
  std::string line;
  std::istringstream in(s);
  while (std::getline(in, line)) {
    size_t start = 0;
    size_t end = line.size();
    while (start < end && (line[start] == ' ' || line[start] == '\t')) ++start;
    while (end > start) {
      char c = line[end - 1];
      if (c == ' ' || c == '\t') --end; else break;
    }
    lines.emplace_back(end > start ? line.substr(start, end - start) : std::string());
  }
  while (!lines.empty() && lines.back().empty()) {
    lines.pop_back();
  }
  std::ostringstream out;
  for (const auto& ln : lines) {
    out.write(ln.data(), static_cast<std::streamsize>(ln.size()));
    out.put('\n');
  }
  return out.str();
}

std::string ComputeRcdIdFromContent(const std::string& canonicalContent, const char* schemaTag) {
  std::string input;
  input.reserve(canonicalContent.size() + 16);
  input.append(schemaTag).push_back('\n');
  input.append(canonicalContent);

  BCRYPT_ALG_HANDLE hAlg = nullptr;
  BCRYPT_HASH_HANDLE hHash = nullptr;
  NTSTATUS status;
  DWORD hashLen = 0, cbData = 0;
  std::array<unsigned char, 32> hashBuf{}; // SHA-256 is 32 bytes

  status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
  if (!BCRYPT_SUCCESS(status)) {
    return std::string();
  }

  status = BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0);
  if (!BCRYPT_SUCCESS(status)) {
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return std::string();
  }

  status = BCryptHashData(hHash,
                          reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())),
                          static_cast<ULONG>(input.size()),
                          0);
  if (!BCRYPT_SUCCESS(status)) {
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return std::string();
  }

  status = BCryptFinishHash(hHash, hashBuf.data(), static_cast<ULONG>(hashBuf.size()), 0);
  BCryptDestroyHash(hHash);
  BCryptCloseAlgorithmProvider(hAlg, 0);
  if (!BCRYPT_SUCCESS(status)) {
    return std::string();
  }

  static const char* hex = "0123456789abcdef";
  std::string out;
  out.resize(64);
  for (size_t i = 0; i < hashBuf.size(); ++i) {
    unsigned char b = hashBuf[i];
    out[i*2] = hex[(b >> 4) & 0xF];
    out[i*2 + 1] = hex[b & 0xF];
  }
  return out;
}

} // namespace RailCore
