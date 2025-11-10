#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include "railcore/engine_factory.h"
#include "railcore/commands.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

static bool FindDelayChanged(const std::vector<DomainEvent>& evs, std::unordered_map<std::string,std::string>& out) {
  for (const auto& e : evs) {
    if (e.id == DomainEventId::DelayChanged) {
      if (const auto* p = std::get_if<std::map<std::string,std::string>>(&e.payload)) {
        out.clear();
        out.insert(p->begin(), p->end());
        return true;
      }
    }
  }
  return false;
}

TEST(Events, DelayChangedPayloadHasExpectedKeysAndValues) {
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>();
  auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = DataFile("FAST.RCD"); d.name = "FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  DelaySettings ds; ds.mode = DelayMode::MaintenanceOnly; ds.threshold = std::chrono::minutes{7}; ds.maintenanceThrough = true;
  ASSERT_EQ(engine->Command(CommandPayload{CommandId::SetDelayMode, ds}).code, StatusCode::Ok);
  auto out = engine->Advance(std::chrono::milliseconds{0});
  std::unordered_map<std::string,std::string> payload;
  ASSERT_TRUE(FindDelayChanged(out.result.events, payload));
  ASSERT_EQ(payload["mode"], std::string("MaintenanceOnly"));
  ASSERT_EQ(payload["thresholdMinutes"], std::to_string(ds.threshold.count()));
  ASSERT_EQ(payload["maintenanceThrough"], std::string("true"));
}

