#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "railcore/services.h"

namespace RailCore {

// Minimal on-disk .RCD repository. Stage 2: parse is a stub; it only validates file presence
// and returns an empty WorldState. Later stages will implement real parsing/validation.
class RcdLayoutRepository : public ILayoutRepository {
public:
  Status Load(LayoutDescriptor& desc, WorldState& outState) override;
};

} // namespace RailCore
