#include "railcore/persistence/rcd_repository.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <set>
#include <map>
#include <sstream>
#include "railcore/persistence/rcd_id.h"

namespace RailCore {

namespace {
bool FileExists(const std::filesystem::path& p) {
  std::error_code ec;
  return std::filesystem::exists(p, ec) && std::filesystem::is_regular_file(p, ec);
}

inline std::string Trim(const std::string& s) {
  size_t a = 0, b = s.size();
  while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
  while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
  return s.substr(a, b - a);
}

inline std::string ToUpper(std::string s) {
  for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
  return s;
}

inline bool ParseFirstIntToken(const std::string& line, uint32_t& out) {
  auto pos = line.find(',');
  std::string tok = pos == std::string::npos ? line : line.substr(0, pos);
  tok = Trim(tok);
  if (tok.empty()) return false;
  int val = 0;
  // simple atoi-like
  bool neg = false; size_t i = 0;
  if (tok[0] == '+') i = 1; else if (tok[0] == '-') { neg = true; i = 1; }
  for (; i < tok.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(tok[i]))) break;
    val = val * 10 + (tok[i] - '0');
  }
  if (neg) return false; // IDs should be positive
  if (val <= 0) return false;
  out = static_cast<uint32_t>(val);
  return true;
}

inline bool TryParseInt(const std::string& token, int& out) {
  std::string t = Trim(token);
  if (t.empty()) return false;
  size_t i = 0; long long v = 0;
  if (t[0] == '+') i = 1; else if (t[0] == '-') { return false; }
  bool any = false;
  for (; i < t.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(t[i]))) break;
    any = true;
    long long next = v * 10 + (t[i] - '0');
    // Check for overflow BEFORE assignment to prevent undefined behavior
    if (next > static_cast<long long>(INT_MAX)) return false;
    v = next;
  }
  if (!any) return false;
  out = static_cast<int>(v);
  return true;
}

inline std::vector<std::string> SplitCSV(const std::string& line) {
  std::vector<std::string> out;
  size_t start = 0;
  while (start < line.size()) {
    size_t comma = line.find(',', start);
    if (comma == std::string::npos) comma = line.size();
    out.emplace_back(Trim(line.substr(start, comma - start)));
    start = comma + 1;
  }
  return out;
}

} // namespace

Status RcdLayoutRepository::Load(LayoutDescriptor& desc, WorldState& outState) {
  if (desc.sourcePath.empty()) {
    return Status{StatusCode::ValidationError, "Empty layout path"};
  }
  std::filesystem::path srcPath = desc.sourcePath;
  if (!FileExists(srcPath)) {
    // Fallback: look under sibling "Game files" directory next to the provided folder
    std::filesystem::path alt = srcPath.parent_path() / "Game files" / srcPath.filename();
    if (FileExists(alt)) {
      srcPath = alt;
    } else {
      return Status{StatusCode::NotFound, "Layout file not found: " + desc.sourcePath.string()};
    }
  }

  // Check file size before reading to prevent DoS via large files
  constexpr std::uintmax_t MAX_RCD_FILE_SIZE = 100 * 1024 * 1024;  // 100 MB
  {
    std::error_code ec;
    auto fileSize = std::filesystem::file_size(srcPath, ec);
    if (ec) {
      return Status{StatusCode::LayoutError, "Unable to determine file size"};
    }
    if (fileSize > MAX_RCD_FILE_SIZE) {
      return Status{StatusCode::ValidationError, "Layout file exceeds maximum size (100 MB)"};
    }
  }

  std::ifstream in(srcPath, std::ios::binary);
  if (!in) {
    return Status{StatusCode::LayoutError, "Unable to open layout file"};
  }
  std::ostringstream oss;
  oss << in.rdbuf();
  std::string contents = oss.str();
  if (contents.empty()) {
    return Status{StatusCode::LayoutError, "Layout file is empty"};
  }

  // Normalize CRLF to LF for scanning
  contents.erase(std::remove(contents.begin(), contents.end(), '\r'), contents.end());
  // Stable layout ID is computed via SHA-256 over canonicalized content (see rcd_id helpers).
  // Note: We do not store it here; the ID is exposed to callers via the Load() out descriptor.

  // Scan sections, collect IDs for key entities.
  enum class Sec { None, General, Sections, Overlapping, Platforms, Selector, Routes, Locos, Locoyard, Timetable };
  Sec cur = Sec::None;
  std::set<std::string> present;
  std::map<std::string,int> headerCounts;
  std::set<uint32_t> sectionIds, routeIds, locoIds, ttIds, selectorIds, platformIds;
  struct RouteParsed { uint32_t id{0}; uint32_t fromSel{0}; uint32_t toSel{0}; uint32_t stages[6][2]{}; };
  std::map<uint32_t, RouteParsed> routeParsed;
  struct TTRef { uint32_t id; int arrSel{0}; int arrTime{-1}; int depTime{-1}; int nextId{0}; };
  std::vector<TTRef> ttRefs;
  struct OverlapRef { uint32_t id; int a; int b; };
  std::vector<OverlapRef> overlaps;
  // [GENERAL]
  bool haveStart = false, haveStop = false;
  int startTime = 0, stopTime = 0;
  bool locoyardDisabled = false;

  std::istringstream iss(contents);
  std::string line;
  while (std::getline(iss, line)) {
    std::string t = Trim(line);
    if (t.empty()) continue;
    if (t.size() >= 3 && t.front() == '[' && t.back() == ']') {
      std::string name = ToUpper(Trim(t.substr(1, t.size() - 2)));
      if (name == "GENERAL") { cur = Sec::General; present.insert("GENERAL"); headerCounts["GENERAL"]++; }
      else if (name == "SECTIONS") { cur = Sec::Sections; present.insert("SECTIONS"); headerCounts["SECTIONS"]++; }
      else if (name == "OVERLAPPING") { cur = Sec::Overlapping; present.insert("OVERLAPPING"); headerCounts["OVERLAPPING"]++; }
      else if (name == "PLATFORMS") { cur = Sec::Platforms; present.insert("PLATFORMS"); headerCounts["PLATFORMS"]++; }
      else if (name == "SELECTOR") { cur = Sec::Selector; present.insert("SELECTOR"); headerCounts["SELECTOR"]++; }
      else if (name == "ROUTES") { cur = Sec::Routes; present.insert("ROUTES"); headerCounts["ROUTES"]++; }
      else if (name == "LOCOS") { cur = Sec::Locos; present.insert("LOCOS"); headerCounts["LOCOS"]++; }
      else if (name == "LOCOYARD") { cur = Sec::Locoyard; present.insert("LOCOYARD"); headerCounts["LOCOYARD"]++; }
      else if (name == "TIMETABLE") { cur = Sec::Timetable; present.insert("TIMETABLE"); headerCounts["TIMETABLE"]++; }
      else { cur = Sec::None; }
      continue;
    }

    // Data line
    uint32_t id = 0;
    switch (cur) {
      case Sec::General: {
        // Accept either Key=Value or comma-separated "Key, Value"
        std::string key;
        std::string val;
        size_t eq = t.find('=');
        if (eq != std::string::npos) {
          key = Trim(t.substr(0, eq));
          val = Trim(t.substr(eq + 1));
        } else {
          auto toks = SplitCSV(t);
          if (toks.size() >= 2) {
            key = toks[0];
            val = toks[1];
          }
        }
        if (!key.empty()) {
          if (key == "StartTime") {
            int v = 0; if (!TryParseInt(val, v)) return Status{StatusCode::ValidationError, "Invalid StartTime"};
            if (v < 0 || (v % 100) >= 60) return Status{StatusCode::ValidationError, "StartTime minutes out of range"};
            int hours = v / 100;
            if (hours < 0 || hours > 23) return Status{StatusCode::ValidationError, "StartTime hours out of range"};
            if (!haveStart) { haveStart = true; startTime = v; }
          } else if (key == "StopTime") {
            int v = 0; if (!TryParseInt(val, v)) return Status{StatusCode::ValidationError, "Invalid StopTime"};
            if (v < 0 || (v % 100) >= 60) return Status{StatusCode::ValidationError, "StopTime minutes out of range"};
            int hours = v / 100;
            if (hours < 0 || hours > 23) return Status{StatusCode::ValidationError, "StopTime hours out of range"};
            if (!haveStop) { haveStop = true; stopTime = v; }
          }
        }
        break; }
      case Sec::Sections:
        if (ParseFirstIntToken(t, id)) {
          if (id < 1 || id > 999)
            return Status{StatusCode::ValidationError, "Section id out of range: " + std::to_string(id)};
          if (!sectionIds.insert(id).second)
            return Status{StatusCode::ValidationError, "Duplicate section id: " + std::to_string(id)};
        }
        break;
      case Sec::Overlapping: {
        auto toks = SplitCSV(t);
        if (toks.size() >= 3) {
          int pid=0, s1=0, s2=0;
          if (!TryParseInt(toks[0], pid)) break;
          if (!TryParseInt(toks[1], s1)) break;
          if (!TryParseInt(toks[2], s2)) break;
          overlaps.push_back(OverlapRef{static_cast<uint32_t>(pid), s1, s2});
        }
        break; }
      case Sec::Platforms: {
        auto toks = SplitCSV(t);
        if (toks.empty()) break;
        int pid=0;
        if (!TryParseInt(toks[0], pid)) break;
        if (pid < 1 || pid > 999)
          return Status{StatusCode::ValidationError, "Platform id out of range: " + std::to_string(pid)};
        if (!platformIds.insert(static_cast<uint32_t>(pid)).second)
          return Status{StatusCode::ValidationError, "Duplicate platform id: " + std::to_string(pid)};
        if (toks.size() != 9)
          return Status{StatusCode::ValidationError, "Platform " + std::to_string(pid) + ": expected 8 coordinate tokens"};
        for (size_t i = 1; i < toks.size(); ++i) { int v=0; if (!TryParseInt(toks[i], v)) return Status{StatusCode::ValidationError, "Platform " + std::to_string(pid) + ": non-numeric coordinate"}; }
        break; }
      case Sec::Selector: {
        auto toks = SplitCSV(t);
        if (toks.empty()) break;
        int sid=0; if (!TryParseInt(toks[0], sid)) break;
        if (sid < 1 || sid > 999)
          return Status{StatusCode::ValidationError, "Selector id out of range: " + std::to_string(sid)};
        if (!selectorIds.insert(static_cast<uint32_t>(sid)).second)
          return Status{StatusCode::ValidationError, "Duplicate selector id: " + std::to_string(sid)};
        // Expect at least 8 tokens per selector line based on samples (coords, dims, types)
        if (toks.size() < 8)
          return Status{StatusCode::ValidationError, "Selector " + std::to_string(sid) + ": too few fields"};
        // Ensure first 7 tokens are numeric
        for (size_t i = 1; i < 7 && i < toks.size(); ++i) { int v=0; if (!TryParseInt(toks[i], v)) return Status{StatusCode::ValidationError, "Selector " + std::to_string(sid) + ": non-numeric field"}; }
        break; }
      case Sec::Routes: {
        if (ParseFirstIntToken(t, id)) {
          if (id < 1 || id > 999)
            return Status{StatusCode::ValidationError, "Route id out of range: " + std::to_string(id)};
          if (!routeIds.insert(id).second)
            return Status{StatusCode::ValidationError, "Duplicate route id: " + std::to_string(id)};
          // Cross-validate stage tokens that reference sections (tokens 4+)
          auto toks = SplitCSV(t);
          // Require exactly 6 stage tokens (total tokens == 9). Some legacy files may omit a comma
          // between stage tokens; attempt a conservative repair by splitting whitespace within stage area.
          if (toks.size() != 9) {
            if (toks.size() >= 3) {
              std::vector<std::string> repaired;
              repaired.reserve(12);
              // Keep first three tokens as-is (id, from, to)
              repaired.push_back(toks[0]);
              if (toks.size() >= 2) repaired.push_back(toks[1]);
              if (toks.size() >= 3) repaired.push_back(toks[2]);
              // Split remaining tokens on whitespace and append
              for (size_t i = 3; i < toks.size(); ++i) {
                const std::string& seg = toks[i];
                size_t p = 0, n = seg.size();
                while (p < n) {
                  while (p < n && std::isspace(static_cast<unsigned char>(seg[p]))) ++p;
                  if (p >= n) break;
                  size_t q = p;
                  while (q < n && !std::isspace(static_cast<unsigned char>(seg[q]))) ++q;
                  repaired.emplace_back(seg.substr(p, q - p));
                  p = q;
                }
              }
              if (repaired.size() == 9) {
                toks.swap(repaired);
              } else {
                return Status{StatusCode::ValidationError, "Route " + std::to_string(id) + ": expected exactly 6 stage tokens"};
              }
            } else {
              return Status{StatusCode::ValidationError, "Route " + std::to_string(id) + ": expected exactly 6 stage tokens"};
            }
          }
          // Validate FromSelector/ToSelector exist when provided
          if (toks.size() >= 3) {
            int fromSel=0, toSel=0;
            if (TryParseInt(toks[1], fromSel) && fromSel>0 && !selectorIds.empty()) {
              if (!selectorIds.count(static_cast<uint32_t>(fromSel)))
                return Status{StatusCode::ValidationError, "Route " + std::to_string(id) + ": unknown FromSelector " + std::to_string(fromSel)};
            }
            if (TryParseInt(toks[2], toSel) && toSel>0 && !selectorIds.empty()) {
              if (!selectorIds.count(static_cast<uint32_t>(toSel)))
                return Status{StatusCode::ValidationError, "Route " + std::to_string(id) + ": unknown ToSelector " + std::to_string(toSel)};
            }
          }
          // 0=id, 1=fromSelector, 2=toSelector, 3+=stage tokens (may be direct or encoded primary+1000*secondary)
          // Capture parsed fields for later world state population
          RouteParsed rp; rp.id = id;
          if (toks.size() >= 2) { int v=0; if (TryParseInt(toks[1], v) && v>0) rp.fromSel = static_cast<uint32_t>(v); }
          if (toks.size() >= 3) { int v=0; if (TryParseInt(toks[2], v) && v>0) rp.toSel = static_cast<uint32_t>(v); }
          size_t stageIndex = 0;
        for (size_t iTok = 3; iTok < toks.size(); ++iTok) {
          int val = 0;
          if (!TryParseInt(toks[iTok], val)) {
            return Status{StatusCode::ValidationError,
                          "Route " + std::to_string(id) + ": non-numeric stage token"};
          }
          if (val <= 0) continue;
          int primary = val % 1000;
          int secondary = val / 1000;
          if (primary != 0 && sectionIds.find(static_cast<uint32_t>(primary)) == sectionIds.end()) {
            return Status{StatusCode::ValidationError,
                             "Route " + std::to_string(id) + ": unknown section id " + std::to_string(primary)};
            }
            if (secondary != 0 && sectionIds.find(static_cast<uint32_t>(secondary)) == sectionIds.end()) {
              return Status{StatusCode::ValidationError,
                             "Route " + std::to_string(id) + ": unknown secondary section id " + std::to_string(secondary)};
            }
            // Return error if route has more than 6 stages instead of silently truncating
            if (stageIndex >= 6) {
              return Status{StatusCode::ValidationError,
                             "Route " + std::to_string(id) + ": exceeds maximum 6 stages"};
            }
            rp.stages[stageIndex][0] = static_cast<uint32_t>(primary);
            rp.stages[stageIndex][1] = static_cast<uint32_t>(secondary);
            ++stageIndex;
          }
          routeParsed[id] = rp;
        }
        break; }
      case Sec::Locos:
        if (ParseFirstIntToken(t, id)) {
          if (id < 1 || id > 499)
            return Status{StatusCode::ValidationError, "Loco id out of range: " + std::to_string(id)};
          // Legacy files may repeat LOCOS lines; keep first occurrence and ignore duplicates.
          (void)locoIds.insert(id);
        }
        break;
      case Sec::Locoyard: {
        auto toks = SplitCSV(t);
        if (toks.empty()) break;
        std::string t0u = ToUpper(toks[0]);
        if (t0u == "DISABLED") { locoyardDisabled = true; break; }
        // Expect StockCode, RefuelOffsetMinutes
        if (toks.size() < 2) return Status{StatusCode::ValidationError, "LOCOYARD: too few fields"};
        int stock=0, off=0; if (!TryParseInt(toks[0], stock) || !TryParseInt(toks[1], off))
          return Status{StatusCode::ValidationError, "LOCOYARD: non-numeric field"};
        // Allowed stock codes: 1-6 and 10-12 per guide
        bool allowed = (stock >= 1 && stock <= 6) || (stock >= 10 && stock <= 12);
        if (!allowed) return Status{StatusCode::ValidationError, "LOCOYARD: invalid stock code"};
        if (off < 0 || off > 59) return Status{StatusCode::ValidationError, "LOCOYARD: refuel offset out of range"};
        break; }
      case Sec::Timetable: {
        if (ParseFirstIntToken(t, id)) {
          if (id < 1 || id > 499)
            return Status{StatusCode::ValidationError, "Timetable id out of range: " + std::to_string(id)};
          // Allow duplicate timetable IDs in some legacy files; keep first occurrence
          bool firstSeen = ttIds.insert(id).second;
          auto toks = SplitCSV(t);
          if (toks.size() < 11) {
            return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(id) + ": too few fields"};
          }
          TTRef r; r.id = id;
          if (toks.size() >= 4) { int v=0; if (TryParseInt(toks[3], v)) r.arrSel = v; }
          if (toks.size() >= 5) { int v=0; if (TryParseInt(toks[4], v)) r.arrTime = v; }
          if (toks.size() >= 7) { int v=0; if (TryParseInt(toks[6], v)) r.depTime = v; }
          if (toks.size() >= 12) { int v=0; if (TryParseInt(toks[11], v)) r.nextId = v; }
          if (firstSeen) ttRefs.push_back(r);
        }
        break; }
      default:
        break;
    }
  }

  // Validate required sections presence
  const char* required[] = {"SECTIONS","GENERAL","OVERLAPPING","PLATFORMS","SELECTOR","ROUTES","LOCOS","LOCOYARD","TIMETABLE"};
  std::vector<std::string> missing;
  for (auto* r : required) if (!present.count(r)) missing.emplace_back(r);
  if (!missing.empty()) {
    std::ostringstream em;
    em << "Missing required sections: ";
    for (size_t i = 0; i < missing.size(); ++i) { if (i) em << ", "; em << missing[i]; }
    return Status{StatusCode::ValidationError, em.str()};
  }
  // Ensure each required section appears exactly once
  std::vector<std::string> dup;
  for (auto* r : required) {
    auto it = headerCounts.find(r);
    if (it != headerCounts.end() && it->second > 1) dup.emplace_back(r);
  }
  if (!dup.empty()) {
    std::ostringstream em;
    em << "Duplicate sections: ";
    for (size_t i = 0; i < dup.size(); ++i) { if (i) em << ", "; em << dup[i]; }
    return Status{StatusCode::ValidationError, em.str()};
  }
  // [GENERAL] Start/Stop required and ordered
  if (!(haveStart && haveStop)) {
    return Status{StatusCode::ValidationError, "GENERAL: StartTime/StopTime missing"};
  }
  if (stopTime <= startTime) {
    return Status{StatusCode::ValidationError, "GENERAL: StopTime must be greater than StartTime"};
  }
  // Validate overlaps sections exist
  for (const auto& o : overlaps) {
    if (o.a <= 0 || o.b <= 0 ||
        sectionIds.find(static_cast<uint32_t>(o.a)) == sectionIds.end() ||
        sectionIds.find(static_cast<uint32_t>(o.b)) == sectionIds.end()) {
      return Status{StatusCode::ValidationError, "OVERLAPPING pair references unknown section"};
    }
  }

  // Validate timetable references now that we have selectorIds and ttIds
  for (const auto& r : ttRefs) {
    if (r.arrSel <= 0 || selectorIds.find(static_cast<uint32_t>(r.arrSel)) == selectorIds.end()) {
      return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(r.id) + ": unknown ArrSelector " + std::to_string(r.arrSel)};
    }
    // ArrSelector must be within 1..49 (input selectors)
    if (r.arrSel < 1 || r.arrSel > 49) {
      return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(r.id) + ": ArrSelector out of allowed range (1..49)"};
    }
    if (r.arrTime >= 0 && (r.arrTime % 100) >= 60) {
      return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(r.id) + ": ArrTime minutes out of range"};
    }
    if (r.depTime >= 0 && (r.depTime % 100) >= 60) {
      return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(r.id) + ": DepTime minutes out of range"};
    }
    if (r.nextId != 0 && ttIds.find(static_cast<uint32_t>(r.nextId)) == ttIds.end()) {
      return Status{StatusCode::ValidationError, "TIMETABLE " + std::to_string(r.id) + ": NextEntry unknown " + std::to_string(r.nextId)};
    }
  }

  // Detect cycles in timetable NextEntry chains to prevent infinite loops
  {
    std::map<uint32_t, uint32_t> nextMap;
    for (const auto& r : ttRefs) {
      if (r.nextId != 0) {
        nextMap[r.id] = static_cast<uint32_t>(r.nextId);
      }
    }
    for (const auto& startEntry : nextMap) {
      std::set<uint32_t> visited;
      uint32_t current = startEntry.first;
      while (current != 0) {
        if (visited.count(current)) {
          return Status{StatusCode::ValidationError,
                         "TIMETABLE: cycle detected in NextEntry chain starting at entry " + std::to_string(startEntry.first)};
        }
        visited.insert(current);
        auto it = nextMap.find(current);
        if (it == nextMap.end()) break;
        current = it->second;
      }
    }
  }

  // Populate minimal world state with placeholder entities
  WorldState ws;
  ws.sections.reserve(sectionIds.size());
  for (auto sid : sectionIds) ws.sections.push_back(Section{sid, {}});
  ws.routes.reserve(routeIds.size());
  for (auto rid : routeIds) {
    Route r{}; r.id = rid;
    auto it = routeParsed.find(rid);
    if (it != routeParsed.end()) {
      r.fromSelector = it->second.fromSel;
      r.toSelector = it->second.toSel;
      for (size_t i = 0; i < 6; ++i) { r.stages[i].primary = it->second.stages[i][0]; r.stages[i].secondary = it->second.stages[i][1]; }
    }
    ws.routes.push_back(std::move(r));
  }
  ws.locos.reserve(locoIds.size());
  for (auto lid : locoIds) ws.locos.push_back(Loco{lid, {}});
  // Build a map for first-seen TTRef (legacy allows dups; we kept first)
  std::map<uint32_t,int> ttArrSel;
  for (const auto& r : ttRefs) { if (ttArrSel.find(r.id) == ttArrSel.end()) ttArrSel[r.id] = r.arrSel; }
  ws.timetable.reserve(ttIds.size());
  for (auto tid : ttIds) {
    TimetableEntry te{}; te.id = tid; te.arrSelector = static_cast<uint32_t>(ttArrSel[tid]);
    ws.timetable.push_back(std::move(te));
  }

  outState = std::move(ws);
  // Compute and assign stable layout ID
  {
    std::string canon = CanonicalizeRcdContent(contents);
    desc.id = ComputeRcdIdFromContent(canon, "rcd:v1");
    if (desc.id.empty()) {
      return Status{StatusCode::LayoutError, "Failed to compute layout id"};
    }
  }
  return Ok();
}

} // namespace RailCore
