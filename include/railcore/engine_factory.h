#pragma once

#include <memory>

#include "railcore/engine.h"
#include "railcore/services.h"

namespace RailCore {

std::shared_ptr<IRailEngine> CreateEngine(
  const EngineConfig& config,
  std::shared_ptr<ILayoutRepository> repo,
  std::shared_ptr<ITelemetrySink> telemetry,
  std::shared_ptr<IRandomProvider> random,
  std::shared_ptr<IClockService> clock,
  std::shared_ptr<IPersistenceService> persistence);

} // namespace RailCore

