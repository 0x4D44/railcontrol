#pragma once

#include <chrono>
#include <string>
#include <variant>

#include "railcore/types.h"

namespace RailCore {

// DelaySettings is defined in types.h to allow WorldState to reference it.

enum class AssignmentAction { Assign, Release, Refuel };

struct LocoAssignment {
  uint32_t timetableId {0};
  uint32_t locoId {0};
  AssignmentAction action {AssignmentAction::Assign};
};

enum class CommandId {
  Pause,
  Resume,
  Stop,
  SetDelayMode,
  AssignLoco,
  ReleaseLoco,
};

struct CommandPayload {
  CommandId id {CommandId::Pause};
  std::variant<std::monostate, DelaySettings, LocoAssignment> data {};
};

} // namespace RailCore
