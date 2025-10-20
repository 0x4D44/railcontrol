#pragma once

#include "railcore/types.h"

namespace RailCore {

struct LayoutSnapshot;
struct SimulationTickResult;

class IObserver {
public:
  virtual void OnSnapshot(const LayoutSnapshot& snapshot) = 0;
  virtual void OnEvents(const SimulationTickResult& tick) = 0;
  virtual void OnDiagnostics(const DiagnosticsEvent& diag) = 0;
  virtual ~IObserver() = default;
};

} // namespace RailCore

