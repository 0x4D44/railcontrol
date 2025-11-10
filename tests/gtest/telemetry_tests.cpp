#include <gtest/gtest.h>
#include <filesystem>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "railcore/services.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

struct Sink : ITelemetrySink {
  int count{0};
  DiagnosticsEvent last;
  void Emit(const DiagnosticsEvent& ev) override { ++count; last = ev; }
};

 

TEST(Telemetry, DtClampEmitsWarning){
  EngineConfig cfg; auto repo=std::make_shared<RcdLayoutRepository>(); auto sink=std::make_shared<Sink>();
  auto engine=CreateEngine(cfg, repo, sink, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath=DataFile("FAST.RCD"); d.name="FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto out=engine->Advance(std::chrono::milliseconds{5000});
  (void)out;
  EXPECT_GE(sink->count,1);
  EXPECT_EQ(sink->last.level, DiagnosticsLevel::Warning);
}

TEST(Telemetry, DiagnosticsMessageContentClamp){
  // Verify the diagnostic message contains "clamped" when dt exceeds maxStep
  EngineConfig cfg; auto repo=std::make_shared<RcdLayoutRepository>(); auto sink=std::make_shared<Sink>();
  auto engine=CreateEngine(cfg, repo, sink, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath=DataFile("FAST.RCD"); d.name="FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  engine->Advance(std::chrono::milliseconds{2000}); // Exceeds 1000ms maxStep
  EXPECT_GE(sink->count, 1);
  EXPECT_NE(sink->last.message.find("clamped"), std::string::npos);
  EXPECT_NE(sink->last.message.find("maxStep"), std::string::npos);
}

TEST(Telemetry, NoEmissionWhenNullSink){
  // Verify engine doesn't crash when telemetry sink is null
  EngineConfig cfg; auto repo=std::make_shared<RcdLayoutRepository>();
  auto engine=CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr); // null telemetry
  LayoutDescriptor d; d.sourcePath=DataFile("FAST.RCD"); d.name="FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);
  auto out=engine->Advance(std::chrono::milliseconds{5000}); // Would trigger diagnostics
  EXPECT_EQ(out.status.code, StatusCode::Ok); // Should not crash
}

TEST(Telemetry, MultipleClampEventsEmitted){
  // Verify multiple diagnostic events are emitted when clamping happens repeatedly
  EngineConfig cfg; auto repo=std::make_shared<RcdLayoutRepository>(); auto sink=std::make_shared<Sink>();
  auto engine=CreateEngine(cfg, repo, sink, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath=DataFile("FAST.RCD"); d.name="FAST";
  ASSERT_EQ(engine->LoadLayout(d).code, StatusCode::Ok);

  engine->Advance(std::chrono::milliseconds{3000}); // First clamp
  int firstCount = sink->count;
  EXPECT_GE(firstCount, 1);

  engine->Advance(std::chrono::milliseconds{3000}); // Second clamp
  EXPECT_GE(sink->count, firstCount + 1); // Should have emitted again
}
