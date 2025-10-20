#pragma once

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>

#include "railcore/status.h"
#include "railcore/types.h"

namespace RailCore {

struct LayoutDescriptor {
  std::filesystem::path sourcePath;
  std::string name;
  std::string id; // stable identifier for regression baselines
};

class ILayoutRepository {
public:
  virtual ~ILayoutRepository() = default;
  // Loads the layout, populates outState and sets desc.id to a stable ID.
  virtual Status Load(LayoutDescriptor& desc, WorldState& outState) = 0;
};

class ITelemetrySink {
public:
  virtual ~ITelemetrySink() = default;
  virtual void Emit(const DiagnosticsEvent& event) = 0;
};

class IRandomProvider {
public:
  virtual ~IRandomProvider() = default;
  virtual uint32_t Next() = 0;
  virtual void Seed(uint32_t seed) = 0;
};

class IClockService {
public:
  virtual ~IClockService() = default;
  virtual std::chrono::milliseconds NowDelta() = 0; // elapsed since last query
};

class IPersistenceService {
public:
  virtual ~IPersistenceService() = default;
  // reserved for future save/export operations
};

struct EngineConfig {
  std::chrono::milliseconds tickInterval {std::chrono::milliseconds{100}};
  bool autoPauseOnError {true};
  size_t maxActiveTrains {512};
  size_t maxSections {2048};
  size_t maxRoutes {4096};
  size_t maxTimetableEntries {8192};
  std::filesystem::path telemetryDirectory;
  bool enableDeterministicSeeds {false};
  bool enablePersistence {false};
};

} // namespace RailCore
