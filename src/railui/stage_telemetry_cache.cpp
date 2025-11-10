#include "railui/stage_telemetry_cache.h"

#include "classdef.h"

void StageTelemetryCache::Reset()
{
  mPending.clear();
}

void StageTelemetryCache::Queue(const StageTelemetryUpdate& update)
{
  if (update.timetableId <= 0)
  {
    return;
  }
  mPending[update.timetableId] = update;
}

void StageTelemetryCache::ApplyPending(TLayout& layout)
{
  for (const auto& kv : mPending)
  {
    const StageTelemetryUpdate& update = kv.second;
    layout.ApplyStageTelemetry(update.timetableId,
                               update.stage,
                               update.stageIndex,
                               update.stagePrimary,
                               update.stageSecondary,
                               update.progressMs);
  }
  mPending.clear();
}
