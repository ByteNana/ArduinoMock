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

  // Every namespace is forgotten first: the store is static now, so without
  // this a key written by one case would still be there for the next.
  void SetUp() override {
    Preferences::mockResetAll();
    prefs.begin("test");
  }
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

// Two instances that begin() the same namespace share one store, like NVS.
TEST_F(PreferencesTest, InstancesShareANamespace) {
  Preferences a, b;
  a.begin("shared", false);
  b.begin("shared", false);
  a.putUInt("v", 7u);
  EXPECT_EQ(b.getUInt("v", 0u), 7u);
  EXPECT_TRUE(b.remove("v"));
  EXPECT_EQ(a.getUInt("v", 0u), 0u);
  Preferences c;
  c.begin("other", false);
  EXPECT_FALSE(c.isKey("v"));
}

// The motivating case: a test presets and removes keys through its own object,
// and the (file-static) object the code under test owns sees the result when it
// begin()s the namespace afterwards.
TEST_F(PreferencesTest, PresetIsVisibleToAnInstanceThatBeginsLater) {
  Preferences fixture;
  fixture.begin("stats", false);
  fixture.putUInt("stats_v", 3u);
  EXPECT_TRUE(fixture.remove("stats_v"));

  Preferences firmware;
  firmware.begin("stats", false);
  EXPECT_FALSE(firmware.isKey("stats_v"));
  firmware.putUInt("stats_v", 1u);
  EXPECT_EQ(fixture.getUInt("stats_v", 0u), 1u);
}

TEST_F(PreferencesTest, NamespacesIsolateTheSameKey) {
  Preferences a, b;
  a.begin("ns_a", false);
  b.begin("ns_b", false);
  a.putInt("k", 1);
  b.putInt("k", 2);
  EXPECT_EQ(a.getInt("k", 0), 1);
  EXPECT_EQ(b.getInt("k", 0), 2);
}

TEST_F(PreferencesTest, ClearOnlyAffectsTheCurrentNamespace) {
  Preferences a, b;
  a.begin("ns_a", false);
  b.begin("ns_b", false);
  a.putInt("k", 1);
  b.putInt("k", 2);
  a.clear();
  EXPECT_FALSE(a.isKey("k"));
  EXPECT_EQ(b.getInt("k", 0), 2);
}

TEST_F(PreferencesTest, ReBeginSwitchesNamespace) {
  Preferences a;
  a.begin("ns_a", false);
  a.putInt("k", 1);
  a.begin("ns_b", false);
  EXPECT_FALSE(a.isKey("k"));
  a.begin("ns_a", false);
  EXPECT_EQ(a.getInt("k", 0), 1);
}

TEST_F(PreferencesTest, StringAndBytesCrossInstances) {
  Preferences a, b;
  a.begin("shared", false);
  b.begin("shared", false);

  a.putString("name", "kiln");
  char buf[16] = {};
  EXPECT_EQ(b.getString("name", buf, sizeof(buf)), 4u);
  EXPECT_STREQ(buf, "kiln");

  const uint8_t blob[3] = {0xDE, 0xAD, 0xBE};
  a.putBytes("blob", blob, sizeof(blob));
  uint8_t out[3] = {};
  EXPECT_EQ(b.getBytes("blob", out, sizeof(out)), 3u);
  EXPECT_EQ(out[0], 0xDE);
  EXPECT_EQ(out[2], 0xBE);
}

TEST_F(PreferencesTest, MockResetAllForgetsEveryNamespace) {
  Preferences a, b;
  a.begin("ns_a", false);
  b.begin("ns_b", false);
  a.putInt("k", 1);
  b.putInt("k", 2);

  Preferences::mockResetAll();
  EXPECT_FALSE(a.isKey("k"));
  EXPECT_FALSE(b.isKey("k"));
  EXPECT_FALSE(prefs.isKey("k"));
}
