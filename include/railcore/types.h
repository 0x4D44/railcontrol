#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace RailCore {

using StructuredPayload = std::variant<std::monostate, std::map<std::string, std::string>>;

enum class DelayMode { None, Randomized, MaintenanceOnly };
enum class DiagnosticsLevel { Trace, Info, Warning, Error };
enum class DomainEventId { TrainArrived, LocoAssigned, LocoReleased, DelayChanged };

struct DelaySettings {
  DelayMode mode {DelayMode::None};
  std::chrono::minutes threshold {std::chrono::minutes{0}};
  bool maintenanceThrough {false};
};

struct Section { uint32_t id{}; std::string name; };
struct Route   { uint32_t id{}; std::string name; };
struct Loco    { uint32_t id{}; std::string name; };

struct TimetableEntry {
  uint32_t id{};
  std::string name;
};

// Simplified logical simulation clock placeholder
using SimulationClock = std::chrono::milliseconds;

struct DiagnosticsEvent {
  DiagnosticsLevel level {DiagnosticsLevel::Info};
  std::string message;
  std::chrono::system_clock::time_point timestamp {std::chrono::system_clock::now()};
  std::variant<std::monostate, StructuredPayload> payload;
};

struct DomainEvent {
  DomainEventId id;
  StructuredPayload payload{};
};

struct EntityDelta {
  uint32_t id{};
  std::map<std::string, std::string> changedFields;
};

struct WorldDelta {
  std::vector<EntityDelta> sections;
  std::vector<EntityDelta> routes;
  std::vector<EntityDelta> locos;
  std::vector<EntityDelta> timetableEntries;
  SimulationClock clock {SimulationClock{0}};
  std::map<std::string, std::string> globals; // miscellaneous key-value changes (e.g., delay settings)
};

struct WorldState {
  std::vector<Section> sections;
  std::vector<Route> routes;
  std::vector<Loco> locos;
  std::vector<TimetableEntry> timetable;
  struct Assignment { uint32_t timetableId{0}; uint32_t locoId{0}; };
  std::vector<Assignment> assignments;
  DelaySettings currentDelay{};
  SimulationClock clock {SimulationClock{0}};
  bool simulationActive {false};
  uint32_t tickId {0};
};

using WorldStatePtr = std::shared_ptr<const WorldState>;

} // namespace RailCore
