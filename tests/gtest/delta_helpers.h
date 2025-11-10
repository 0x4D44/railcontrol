#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "railcore/types.h"

inline bool DeltaHasEntryField(const std::shared_ptr<const RailCore::WorldDelta>& delta,
                               uint32_t tt,
                               const char* key,
                               const char* expected = nullptr) {
  if (!delta) return false;
  for (const auto& ed : delta->timetableEntries) {
    if (ed.id == tt) {
      auto it = ed.changedFields.find(key);
      if (it == ed.changedFields.end()) continue;
      if (!expected) return true;
      return it->second == std::string(expected);
    }
  }
  return false;
}

inline bool DeltaGlobalsHave(const std::shared_ptr<const RailCore::WorldDelta>& delta,
                             const std::vector<std::string>& keys) {
  if (!delta) return false;
  for (const auto& k : keys) if (delta->globals.find(k) == delta->globals.end()) return false;
  return true;
}

