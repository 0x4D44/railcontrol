#include <gtest/gtest.h>
#include <string>
#include "railcore/persistence/rcd_id.h"
#include "tests/gtest/test_utils.h"

using namespace RailCore;

TEST(Ids, Sha256IsLowercaseHex64) {
  std::string content = ReadAll(DataFile("FAST.RCD"));
  std::string canon = CanonicalizeRcdContent(content);
  std::string id = ComputeRcdIdFromContent(canon);
  ASSERT_EQ(id.size(), 64u);
  for (char ch : id) {
    ASSERT_TRUE((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) << "non-hex char: " << ch;
  }
}

