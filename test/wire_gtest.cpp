#include <gtest/gtest.h>

#include "Wire.h"

class WireTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Wire.mockReset();
    Wire1.mockReset();
  }
};

// 1. begin returns true
TEST_F(WireTest, BeginReturnsTrue) { EXPECT_TRUE(Wire.begin()); }

// 2. write(byte) returns 1
TEST_F(WireTest, WriteBytReturnsOne) {
  Wire.beginTransmission(0x20);
  EXPECT_EQ(Wire.write(0xAB), 1u);
}

// 3. write(buf, n) returns n
TEST_F(WireTest, WriteBufReturnsN) {
  const uint8_t buf[] = {0x01, 0x02, 0x03};
  Wire.beginTransmission(0x20);
  EXPECT_EQ(Wire.write(buf, 3), 3u);
}

// 4. mockGetWritten contains written bytes in order
TEST_F(WireTest, MockGetWrittenContainsBytesInOrder) {
  Wire.beginTransmission(0x20);
  Wire.write(0x11);
  Wire.write(0x22);
  Wire.write(0x33);
  const std::vector<uint8_t>& w = Wire.mockGetWritten();
  ASSERT_EQ(w.size(), 3u);
  EXPECT_EQ(w[0], 0x11u);
  EXPECT_EQ(w[1], 0x22u);
  EXPECT_EQ(w[2], 0x33u);
}

// 5. available returns queue size
TEST_F(WireTest, AvailableReturnsQueueSize) {
  Wire.mockQueueRead({0xAA, 0xBB});
  EXPECT_EQ(Wire.available(), 2);
}

// 6. read returns queued byte
TEST_F(WireTest, ReadReturnsQueuedByte) {
  Wire.mockQueueRead({0x42});
  EXPECT_EQ(Wire.read(), 0x42);
}

// 7. read returns -1 when queue empty
TEST_F(WireTest, ReadReturnsMinusOneWhenEmpty) { EXPECT_EQ(Wire.read(), -1); }

// 8. peek returns front without consuming
TEST_F(WireTest, PeekReturnsFrontWithoutConsuming) {
  Wire.mockQueueRead({0x55, 0x66});
  EXPECT_EQ(Wire.peek(), 0x55);
  EXPECT_EQ(Wire.peek(), 0x55);
  EXPECT_EQ(Wire.available(), 2);
}

// 9. mockQueueRead then read multiple bytes in order
TEST_F(WireTest, MockQueueReadMultipleBytesInOrder) {
  Wire.mockQueueRead({0x01, 0x02, 0x03});
  EXPECT_EQ(Wire.read(), 0x01);
  EXPECT_EQ(Wire.read(), 0x02);
  EXPECT_EQ(Wire.read(), 0x03);
  EXPECT_EQ(Wire.available(), 0);
}

// 10. mockReset clears written buffer
TEST_F(WireTest, MockResetClearsWrittenBuffer) {
  Wire.write(0xFF);
  Wire.mockReset();
  EXPECT_TRUE(Wire.mockGetWritten().empty());
}

// 11. mockReset clears read queue
TEST_F(WireTest, MockResetClearsReadQueue) {
  Wire.mockQueueRead({0xDE, 0xAD});
  Wire.mockReset();
  EXPECT_EQ(Wire.available(), 0);
  EXPECT_EQ(Wire.read(), -1);
}

// 12. Wire and Wire1 have independent state
TEST_F(WireTest, WireAndWire1HaveIndependentState) {
  Wire.beginTransmission(0x10);
  Wire.write(0xCA);
  Wire.write(0xFE);
  Wire.endTransmission();

  EXPECT_EQ(Wire.mockGetWritten().size(), 2u);
  EXPECT_TRUE(Wire1.mockGetWritten().empty());
}
