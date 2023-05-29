#include <gtest/gtest.h>

// Demonstrate some basic assertions.
TEST(Test1, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(Test2, testcase0) {
  EXPECT_EQ(2, 2);
}
