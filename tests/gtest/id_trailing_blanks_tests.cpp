#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/persistence/rcd_id.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Ids, TrailingBlankLinesAreIgnoredInCanonicalization) {
  auto fast = DataFile("FAST.RCD");
  std::string content = ReadAll(fast.string().c_str());
  auto id1 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content));

  std::string with_blanks = content;
  with_blanks.append("\n\n\n");
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(with_blanks));

  EXPECT_EQ(id1, id2);
  EXPECT_EQ(id1.size(), 64u);
}

