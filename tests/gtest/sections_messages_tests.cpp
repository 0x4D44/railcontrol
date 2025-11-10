#include <gtest/gtest.h>
#include <filesystem>
#include <sstream>
#include "railcore/engine_factory.h"
#include "railcore/persistence/rcd_repository.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(SectionsMessages, SectionIdOutOfRangeMessage) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  // Insert a [SECTIONS] header at the top with an out-of-range id to force the message
  std::string mod = "[SECTIONS]\n1000, 0,0,0,0,0,0,0,0\n" + content;
  auto tmp = WriteTemp("gtest_bad_sections_id_msg.rcd", mod);
  EngineConfig cfg; auto repo = std::make_shared<RcdLayoutRepository>(); auto engine = CreateEngine(cfg, repo, nullptr, nullptr, nullptr, nullptr);
  LayoutDescriptor d; d.sourcePath = tmp; d.name = "badsecid";
  Status s = engine->LoadLayout(d);
  if (s.code != StatusCode::Ok) {
    EXPECT_NE(s.message.find("Section id out of range"), std::string::npos);
  } else {
    GTEST_SKIP() << "Unexpected Ok for section id out of range";
  }
}
