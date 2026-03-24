#include <gtest/gtest.h>

#include "Preferences.h"
#include "SPI.h"

// ---------------------------------------------------------------------------
// SPI tests
// ---------------------------------------------------------------------------

TEST(SPITest, BeginEndCompile) {
  SPI.begin();
  SPI.end();
}

TEST(SPITest, BeginWithPinsCompile) {
  SPI.begin(18, 19, 23, 5);
  SPI.end();
}

TEST(SPITest, TransferReturnsZero) { EXPECT_EQ(SPI.transfer(0xAB), 0); }

TEST(SPITest, Transfer16ReturnsZero) { EXPECT_EQ(SPI.transfer16(0x1234), 0); }

TEST(SPITest, Transfer32ReturnsZero) { EXPECT_EQ(SPI.transfer32(0xDEADBEEF), 0u); }

TEST(SPITest, SPISettingsDefaultConstructorCompiles) { SPISettings s; }

TEST(SPITest, SPISettingsParamConstructorCompiles) { SPISettings s(1000000, MSBFIRST, SPI_MODE0); }

TEST(SPITest, BeginEndTransactionCompile) {
  SPISettings s(4000000, MSBFIRST, SPI_MODE3);
  SPI.beginTransaction(s);
  SPI.endTransaction();
}

// ---------------------------------------------------------------------------
// Preferences tests
// ---------------------------------------------------------------------------

class PreferencesTest : public ::testing::Test {
 protected:
  Preferences prefs;

  void SetUp() override { prefs.begin("test"); }
  void TearDown() override { prefs.end(); }
};

TEST_F(PreferencesTest, PutGetIntRoundTrip) {
  EXPECT_TRUE(prefs.putInt("count", 42));
  EXPECT_EQ(prefs.getInt("count"), 42);
}

TEST_F(PreferencesTest, PutGetStringRoundTrip) {
  EXPECT_TRUE(prefs.putString("name", "hello"));
  char buf[32] = {};
  size_t len = prefs.getString("name", buf, sizeof(buf));
  EXPECT_EQ(len, 5u);
  EXPECT_STREQ(buf, "hello");
}

TEST_F(PreferencesTest, PutGetBoolRoundTrip) {
  EXPECT_TRUE(prefs.putBool("flag", true));
  EXPECT_TRUE(prefs.getBool("flag"));
  EXPECT_TRUE(prefs.putBool("flag2", false));
  EXPECT_FALSE(prefs.getBool("flag2"));
}

TEST_F(PreferencesTest, GetIntReturnsDefaultWhenMissing) {
  EXPECT_EQ(prefs.getInt("missing", 99), 99);
}

TEST_F(PreferencesTest, ClearRemovesAllKeys) {
  prefs.putInt("a", 1);
  prefs.putInt("b", 2);
  prefs.clear();
  EXPECT_EQ(prefs.getInt("a", -1), -1);
  EXPECT_EQ(prefs.getInt("b", -1), -1);
}

TEST_F(PreferencesTest, RemoveSpecificKey) {
  prefs.putInt("x", 10);
  prefs.putInt("y", 20);
  EXPECT_TRUE(prefs.remove("x"));
  EXPECT_EQ(prefs.getInt("x", -1), -1);
  EXPECT_EQ(prefs.getInt("y"), 20);
}

TEST_F(PreferencesTest, RemoveNonExistentKeyReturnsFalse) { EXPECT_FALSE(prefs.remove("ghost")); }

TEST_F(PreferencesTest, IsKeyReturnsTrueAndFalse) {
  EXPECT_FALSE(prefs.isKey("k"));
  prefs.putInt("k", 7);
  EXPECT_TRUE(prefs.isKey("k"));
}

TEST_F(PreferencesTest, PutGetFloatRoundTrip) {
  EXPECT_TRUE(prefs.putFloat("pi", 3.14f));
  EXPECT_FLOAT_EQ(prefs.getFloat("pi"), 3.14f);
}

TEST_F(PreferencesTest, PutGetBytesRoundTrip) {
  const uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  EXPECT_TRUE(prefs.putBytes("raw", data, sizeof(data)));
  uint8_t out[4] = {};
  size_t read = prefs.getBytes("raw", out, sizeof(out));
  EXPECT_EQ(read, 4u);
  EXPECT_EQ(memcmp(data, out, 4), 0);
}

TEST_F(PreferencesTest, PutGetUIntRoundTrip) {
  EXPECT_TRUE(prefs.putUInt("u", 0xDEADBEEFu));
  EXPECT_EQ(prefs.getUInt("u"), 0xDEADBEEFu);
}

TEST_F(PreferencesTest, FreeEntriesReturns100) { EXPECT_EQ(prefs.freeEntries(), 100u); }
