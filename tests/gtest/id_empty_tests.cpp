#include <gtest/gtest.h>
#include <string>
#include "railcore/persistence/rcd_id.h"

using namespace RailCore;

TEST(Id, EmptyContentProducesDeterministicSha256Hex) {
  std::string canon = CanonicalizeRcdContent("");
  EXPECT_TRUE(canon.empty());
  auto id1 = ComputeRcdIdFromContent(canon);
  auto id2 = ComputeRcdIdFromContent(canon);
  EXPECT_EQ(id1, id2);
  EXPECT_EQ(id1.size(), 64u);
}

