#include "railcore/persistence/rcd_id.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstdint>

namespace RailCore {

std::string CanonicalizeRcdContent(const std::string& raw) {
  std::string s;
  s.reserve(raw.size());
  // Drop CR characters
  for (char ch : raw) if (ch != '\r') s.push_back(ch);
  // Trim trailing whitespace on each line
  std::ostringstream out;
  std::string line;
  std::istringstream in(s);
  while (std::getline(in, line)) {
    size_t end = line.size();
    while (end > 0) {
      char c = line[end - 1];
      if (c == ' ' || c == '\t') --end; else break;
    }
    out.write(line.data(), static_cast<std::streamsize>(end));
    out.put('\n');
  }
  return out.str();
}

static inline uint64_t fnv1a64(const unsigned char* data, size_t len) {
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = FNV_OFFSET;
  for (size_t i = 0; i < len; ++i) {
    hash ^= data[i];
    hash *= FNV_PRIME;
  }
  return hash;
}

std::string ComputeRcdIdFromContent(const std::string& canonicalContent, const char* schemaTag) {
  std::string input;
  input.reserve(canonicalContent.size() + 16);
  input.append(schemaTag).push_back('\n');
  input.append(canonicalContent);
  uint64_t h = fnv1a64(reinterpret_cast<const unsigned char*>(input.data()), input.size());
  char buf[17] = {0};
  static const char* hex = "0123456789abcdef";
  for (int i = 15; i >= 0; --i) {
    buf[i] = hex[h & 0xF];
    h >>= 4;
  }
  return std::string(buf, 16);
}

} // namespace RailCore
