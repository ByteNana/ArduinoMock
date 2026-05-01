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

TEST(StringClearTest, ClearEmptiesString) {
  String s("hello");
  s.clear();
  EXPECT_EQ(s.length(), 0u);
  EXPECT_STREQ(s.c_str(), "");
}

TEST(StringClearTest, ClearOnEmptyStringIsNoOp) {
  String s;
  EXPECT_NO_THROW(s.clear());
  EXPECT_EQ(s.length(), 0u);
}

TEST(StringEqualsTest, EqualsString) {
  String a("hello");
  String b("hello");
  String c("world");
  EXPECT_TRUE(a.equals(b));
  EXPECT_FALSE(a.equals(c));
}

TEST(StringEqualsTest, EqualsCStr) {
  String a("hello");
  EXPECT_TRUE(a.equals("hello"));
  EXPECT_FALSE(a.equals("world"));
}

TEST(StringEqualsTest, EqualsIgnoreCaseMatch) {
  String a("Hello");
  String b("HELLO");
  EXPECT_TRUE(a.equalsIgnoreCase(b));
}

TEST(StringEqualsTest, EqualsIgnoreCaseMismatch) {
  String a("Hello");
  String b("World");
  EXPECT_FALSE(a.equalsIgnoreCase(b));
}

TEST(StringEqualsTest, EqualsIgnoreCaseDifferentLength) {
  String a("Hello");
  String b("Hello!");
  EXPECT_FALSE(a.equalsIgnoreCase(b));
}

TEST(StringWriteTest, WriteByteAppendsChar) {
  String s;
  EXPECT_EQ(s.write(static_cast<uint8_t>('A')), 1u);
  EXPECT_STREQ(s.c_str(), "A");
}

TEST(StringWriteTest, WriteBufferAppendsBytes) {
  String s;
  const uint8_t buf[] = {'h', 'i'};
  EXPECT_EQ(s.write(buf, 2), 2u);
  EXPECT_STREQ(s.c_str(), "hi");
}
