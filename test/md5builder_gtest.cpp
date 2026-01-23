// Tests for MD5Builder
#include <gtest/gtest.h>

#include "Arduino.h"
#include "MD5Builder.h"

static String md5_of(const String& s) {
  MD5Builder md5;
  md5.begin();
  md5.add((uint8_t*)s.c_str(), s.size());
  md5.calculate();
  return md5.toString();
}

TEST(MD5BuilderTest, EmptyString) {
  // MD5("") = d41d8cd98f00b204e9800998ecf8427e
  EXPECT_EQ(md5_of(""), String("d41d8cd98f00b204e9800998ecf8427e"));
}

TEST(MD5BuilderTest, SimpleAscii) {
  // MD5("abc") = 900150983cd24fb0d6963f7d28e17f72
  EXPECT_EQ(md5_of("abc"), String("900150983cd24fb0d6963f7d28e17f72"));
}

TEST(MD5BuilderTest, QuickBrownFox) {
  // MD5("The quick brown fox jumps over the lazy dog")
  // = 9e107d9d372bb6826bd81d3542a419d6
  EXPECT_EQ(
      md5_of("The quick brown fox jumps over the lazy dog"),
      String("9e107d9d372bb6826bd81d3542a419d6"));
}

TEST(MD5BuilderTest, MultiChunkAdd) {
  const char* part1 = "The quick brown fox ";
  const char* part2 = "jumps over the lazy ";
  const char* part3 = "dog";

  MD5Builder md5;
  md5.begin();
  md5.add((uint8_t*)part1, strlen(part1));
  md5.add((uint8_t*)part2, strlen(part2));
  md5.add((uint8_t*)part3, strlen(part3));
  // No explicit calculate to ensure implicit finalization works in toString
  String hex = md5.toString();
  EXPECT_EQ(hex, String("9e107d9d372bb6826bd81d3542a419d6"));
}

TEST(MD5BuilderTest, AddAfterFinalizeIsIgnored) {
  MD5Builder md5;
  md5.begin();
  const char* msg = "abc";
  md5.add((uint8_t*)msg, strlen(msg));
  String first = md5.toString();  // finalize occurs here if needed

  // Try to change digest after finalization; should have no effect
  const char* extra = "more";
  md5.add((uint8_t*)extra, strlen(extra));
  String second = md5.toString();

  EXPECT_EQ(first, String("900150983cd24fb0d6963f7d28e17f72"));
  EXPECT_EQ(second, first);
}

TEST(MD5BuilderTest, HandlesNullAndZeroLength) {
  MD5Builder md5;
  md5.begin();
  // Passing nullptr or zero length should be safe and a no-op
  md5.add(nullptr, 0);
  md5.add((uint8_t*)"abc", 0);
  md5.add(nullptr, 10);
  md5.add((uint8_t*)"abc", 3);

  EXPECT_EQ(md5.toString(), String("900150983cd24fb0d6963f7d28e17f72"));
}
