#include <gtest/gtest.h>

#include <type_traits>

#include "WString.h"

TEST(StringSumHelperTest, ConstructFromString) {
  String s("hello");
  StringSumHelper h(s);
  EXPECT_STREQ(h.c_str(), "hello");
}

TEST(StringSumHelperTest, ConstructFromCStr) {
  StringSumHelper h("world");
  EXPECT_STREQ(h.c_str(), "world");
}

TEST(StringSumHelperTest, IsSubclassOfString) {
  EXPECT_TRUE((std::is_base_of<String, StringSumHelper>::value));
}

TEST(StringSumHelperTest, OperatorPlusStringString) {
  String a("foo");
  String b("bar");
  StringSumHelper result = a + b;
  EXPECT_STREQ(result.c_str(), "foobar");
}

TEST(StringSumHelperTest, OperatorPlusStringCStr) {
  String a("foo");
  StringSumHelper result = a + "bar";
  EXPECT_STREQ(result.c_str(), "foobar");
}

TEST(StringSumHelperTest, OperatorPlusCStrString) {
  String b("bar");
  StringSumHelper result = "foo" + b;
  EXPECT_STREQ(result.c_str(), "foobar");
}

TEST(StringSumHelperTest, ChainedConcatenation) {
  String a("a");
  String b("b");
  String c("c");
  String result = a + b + c;
  EXPECT_STREQ(result.c_str(), "abc");
}

TEST(StringSumHelperTest, ArduinoJsonTraitCompiles) {
  // Reproduces the ArduinoJson IsString<::StringSumHelper> specialisation —
  // this test will fail to compile if StringSumHelper is not in scope.
  bool is_str = std::is_base_of<String, ::StringSumHelper>::value;
  EXPECT_TRUE(is_str);
}
