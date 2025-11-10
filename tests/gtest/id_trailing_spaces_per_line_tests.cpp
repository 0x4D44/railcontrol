#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "railcore/persistence/rcd_id.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Ids, TrailingSpacesPerLineDoNotChangeId) {
  auto fast = DataFile("FAST.RCD");
  std::string content = ReadAll(fast.string().c_str());
  auto id1 = ComputeRcdIdFromContent(CanonicalizeRcdContent(content));

  std::string modified;
  modified.reserve(content.size() + content.size() / 8);
  for (char c : content) {
    modified.push_back(c);
    if (c == '\n') {
      modified.push_back(' ');
      modified.push_back(' ');
    }
  }
  auto id2 = ComputeRcdIdFromContent(CanonicalizeRcdContent(modified));
  EXPECT_EQ(id1, id2);
}

