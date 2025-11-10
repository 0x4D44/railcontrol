#pragma once

#include <map>

class TLayout;

struct StageTelemetryUpdate {
  int timetableId{0};
  int stage{-1};
  int stageIndex{-1};
  int stagePrimary{0};
  int stageSecondary{0};
  long progressMs{-1};
};

class StageTelemetryCache {
public:
  void Reset();
  void Queue(const StageTelemetryUpdate& update);
  void ApplyPending(TLayout& layout);

private:
  std::map<int, StageTelemetryUpdate> mPending;
};
