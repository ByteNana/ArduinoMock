#include <gtest/gtest.h>

#include "EEPROM.h"

class EEPROMTest : public ::testing::Test {
 protected:
  void SetUp() override { EEPROM.reset(); }
};

TEST_F(EEPROMTest, DefaultBufferIsAllFF) {
  for (int i = 0; i < 16; i++) { EXPECT_EQ(EEPROM.read(i), 0xFF); }
}

TEST_F(EEPROMTest, WriteReadRoundTrip) {
  EEPROM.write(0, 0x42);
  EEPROM.write(1, 0x00);
  EEPROM.write(2, 0xAB);
  EXPECT_EQ(EEPROM.read(0), 0x42);
  EXPECT_EQ(EEPROM.read(1), 0x00);
  EXPECT_EQ(EEPROM.read(2), 0xAB);
}

TEST_F(EEPROMTest, PutGetWithStruct) {
  struct Config {
    uint32_t magic;
    uint16_t version;
    uint8_t flags;
  };

  Config written = {0xDEADBEEF, 42, 0x01};
  EEPROM.put(0, written);

  Config readBack = {};
  EEPROM.get(0, readBack);
  EXPECT_EQ(readBack.magic, 0xDEADBEEF);
  EXPECT_EQ(readBack.version, 42);
  EXPECT_EQ(readBack.flags, 0x01);
}

TEST_F(EEPROMTest, PutGetReturnsReference) {
  int val = 12345;
  EEPROM.put(0, val);

  int result = 0;
  int& ref = EEPROM.get(0, result);
  EXPECT_EQ(result, 12345);
  EXPECT_EQ(&ref, &result);
}

TEST_F(EEPROMTest, LengthReturnsConfiguredSize) {
  EXPECT_EQ(EEPROM.length(), MOCK_EEPROM_SIZE);
  EEPROM.begin(512);
  EXPECT_EQ(EEPROM.length(), 512u);
}

TEST_F(EEPROMTest, CommitReturnsTrue) { EXPECT_TRUE(EEPROM.commit()); }

TEST_F(EEPROMTest, ResetClearsBuffer) {
  EEPROM.write(0, 0x42);
  EEPROM.write(100, 0xAB);
  EEPROM.begin(256);
  EEPROM.reset();
  EXPECT_EQ(EEPROM.read(0), 0xFF);
  EXPECT_EQ(EEPROM.read(100), 0xFF);
  EXPECT_EQ(EEPROM.size(), static_cast<size_t>(MOCK_EEPROM_SIZE));
}

TEST_F(EEPROMTest, RawGivesDirectAccess) {
  uint8_t* buf = EEPROM.raw();
  ASSERT_NE(buf, nullptr);
  buf[10] = 0x77;
  EXPECT_EQ(EEPROM.read(10), 0x77);
}

TEST_F(EEPROMTest, SizeReturnsCurrentSize) {
  EXPECT_EQ(EEPROM.size(), static_cast<size_t>(MOCK_EEPROM_SIZE));
  EEPROM.begin(1024);
  EXPECT_EQ(EEPROM.size(), 1024u);
}
