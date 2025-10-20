#pragma once

#include <string>

namespace RailCore {

enum class StatusCode {
  Ok,
  InvalidCommand,
  LayoutError,
  ValidationError,
  NotFound,
  Busy,
  InternalError,
};

struct Status {
  StatusCode code {StatusCode::Ok};
  std::string message; // empty on success
};

inline Status Ok() { return Status{StatusCode::Ok, {}}; }

} // namespace RailCore

