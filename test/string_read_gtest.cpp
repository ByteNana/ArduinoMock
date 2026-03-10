#include <gtest/gtest.h>

#include "WString.h"

TEST(StringReadTest, ReadReturnsCharsSequentially) {
  String s("ABC");
  EXPECT_EQ(s.read(), 'A');
  EXPECT_EQ(s.read(), 'B');
  EXPECT_EQ(s.read(), 'C');
  EXPECT_EQ(s.read(), -1);
}

TEST(StringReadTest, PeekDoesNotAdvance) {
  String s("XY");
  EXPECT_EQ(s.peek(), 'X');
  EXPECT_EQ(s.peek(), 'X');
  EXPECT_EQ(s.read(), 'X');
  EXPECT_EQ(s.peek(), 'Y');
}

TEST(StringReadTest, AvailableReturnsRemaining) {
  String s("Hello");
  EXPECT_EQ(s.available(), 5);
  s.read();
  s.read();
  EXPECT_EQ(s.available(), 3);
  s.read();
  s.read();
  s.read();
  EXPECT_EQ(s.available(), 0);
}

TEST(StringReadTest, EmptyStringReturnsNegativeOne) {
  String s("");
  EXPECT_EQ(s.read(), -1);
  EXPECT_EQ(s.peek(), -1);
  EXPECT_EQ(s.available(), 0);
}

TEST(StringReadTest, ReadWorksWithHighBytes) {
  // Test unsigned char handling (values > 127)
  std::string raw;
  raw.push_back(static_cast<char>(0xC0));
  raw.push_back(static_cast<char>(0xFF));
  String s(raw);
  EXPECT_EQ(s.read(), 0xC0);
  EXPECT_EQ(s.read(), 0xFF);
}
