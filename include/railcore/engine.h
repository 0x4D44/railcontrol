#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "railcore/commands.h"
#include "railcore/observer.h"
#include "railcore/services.h"
#include "railcore/status.h"
#include "railcore/types.h"

namespace RailCore {

struct LayoutSnapshot {
  WorldStatePtr state; // immutable snapshot of core entities (never null)
  SimulationClock snapshotClock {SimulationClock{0}};
  std::chrono::system_clock::time_point capturedAt {std::chrono::system_clock::now()};
};

struct SimulationTickResult {
  std::shared_ptr<const WorldDelta> delta; // nullptr if no change
  std::vector<DomainEvent> events;
  SimulationClock clock {SimulationClock{0}};
  std::chrono::milliseconds wallTime {std::chrono::milliseconds{0}};
};

struct AdvanceOutcome {
  Status status;                 // Ok on success; non-OK otherwise
  SimulationTickResult result;   // valid only when status.code == Ok
};

class IRailEngine {
public:
  virtual ~IRailEngine() = default;

  virtual Status LoadLayout(const LayoutDescriptor& layout) = 0;
  virtual AdvanceOutcome Advance(std::chrono::milliseconds dt) = 0;
  virtual Status Reset() = 0;
  virtual Status Command(const CommandPayload& cmd) = 0;
  virtual LayoutSnapshot GetSnapshot() const = 0;
  // Returns the stable layout ID for the currently loaded layout, or empty if none loaded.
  virtual std::string GetLayoutId() const = 0;
  virtual void Subscribe(IObserver&) = 0;
  virtual void Unsubscribe(IObserver&) = 0;
};

} // namespace RailCore
