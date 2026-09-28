#include <gtest/gtest.h>

#include "Wire.h"

class WireTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Wire.mockReset();
    Wire1.mockReset();
    Wire.mockDetachAllDevices();
    Wire1.mockDetachAllDevices();
  }
  // Device models are locals of the test body, so nothing may stay attached
  // past it -- the captured `this` would dangle.
  void TearDown() override {
    Wire.mockDetachAllDevices();
    Wire1.mockDetachAllDevices();
  }
};

// 1. begin returns true
TEST_F(WireTest, BeginReturnsTrue) { EXPECT_TRUE(Wire.begin()); }

// 2. write(byte) returns 1
TEST_F(WireTest, WriteByteReturnsOne) {
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

// --- addressed device hook ---

// A 256-byte "EEPROM" with a one-byte address pointer: the first byte of a
// transmission sets the pointer, the rest are data; reads start at the pointer.
struct TinyEeprom {
  uint8_t mem[256];
  uint8_t ptr = 0;
  TinyEeprom() {
    for (auto& b : mem) b = 0xFF;
  }
  void attach(TwoWire& bus, uint8_t addr) {
    bus.mockAttachDevice(
        addr,
        [this](const std::vector<uint8_t>& w) {
          if (w.empty()) return;
          ptr = w[0];
          for (size_t i = 1; i < w.size(); ++i) mem[ptr++] = w[i];
        },
        [this](size_t n) {
          std::vector<uint8_t> out;
          for (size_t i = 0; i < n; ++i) out.push_back(mem[ptr++]);
          return out;
        });
  }
};

// 13. requestFrom on an attached device serves bytes from the device
TEST_F(WireTest, AttachedDeviceServesRequestFrom) {
  TinyEeprom dev;
  dev.mem[0x10] = 0x42;
  dev.mem[0x11] = 0x43;
  dev.attach(Wire, 0x50);

  Wire.beginTransmission(0x50);
  Wire.write(0x10);  // set pointer
  Wire.endTransmission();
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)2), 2u);
  EXPECT_EQ(Wire.available(), 2);
  EXPECT_EQ(Wire.read(), 0x42);
  EXPECT_EQ(Wire.read(), 0x43);
}

// 14. endTransmission delivers the written bytes to the device
TEST_F(WireTest, AttachedDeviceReceivesWrites) {
  TinyEeprom dev;
  dev.attach(Wire, 0x50);

  Wire.beginTransmission(0x50);
  Wire.write(0x20);
  Wire.write(0xAA);
  Wire.write(0xBB);
  Wire.endTransmission();
  EXPECT_EQ(dev.mem[0x20], 0xAA);
  EXPECT_EQ(dev.mem[0x21], 0xBB);
  // mockGetWritten still records the raw stream
  EXPECT_EQ(Wire.mockGetWritten().size(), 3u);
}

// 15. a device answers only its own address; others keep the FIFO
TEST_F(WireTest, OtherAddressesKeepTheFifoBehaviour) {
  TinyEeprom dev;
  dev.attach(Wire, 0x50);
  Wire.mockQueueRead({0x99});
  EXPECT_EQ(Wire.requestFrom(0x21, (uint8_t)1), 1u);
  EXPECT_EQ(Wire.read(), 0x99);
}

// 16. a device may return fewer bytes than requested
TEST_F(WireTest, DeviceMayReturnFewerBytes) {
  Wire.mockAttachDevice(0x30, nullptr, [](size_t) { return std::vector<uint8_t>{0x01}; });
  EXPECT_EQ(Wire.requestFrom(0x30, (uint8_t)4), 1u);
  EXPECT_EQ(Wire.available(), 1);
}

// 17. detach restores the FIFO; mockReset keeps devices attached
TEST_F(WireTest, DetachRestoresFifoAndResetKeepsDevices) {
  TinyEeprom dev;
  dev.attach(Wire, 0x50);
  EXPECT_TRUE(Wire.mockHasDevice(0x50));
  Wire.mockReset();
  EXPECT_TRUE(Wire.mockHasDevice(0x50));
  Wire.mockDetachDevice(0x50);
  EXPECT_FALSE(Wire.mockHasDevice(0x50));
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)1), 0u);
}

// 18. Wire and Wire1 devices are independent
TEST_F(WireTest, DevicesArePerBus) {
  TinyEeprom dev;
  dev.attach(Wire1, 0x50);
  EXPECT_TRUE(Wire1.mockHasDevice(0x50));
  EXPECT_FALSE(Wire.mockHasDevice(0x50));
}

// 19. a device serves a whole transfer: bytes left from a read the code under
// test abandoned are not handed back as the answer to the next request
TEST_F(WireTest, RequestFromADeviceStartsAFreshTransfer) {
  TinyEeprom dev;
  dev.mem[0x00] = 0x11;
  dev.mem[0x01] = 0x22;
  dev.attach(Wire, 0x50);

  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)2), 2u);
  EXPECT_EQ(Wire.read(), 0x11);  // second byte deliberately left unread

  Wire.beginTransmission(0x50);
  Wire.write(0x00);
  Wire.endTransmission();
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)1), 1u);
  EXPECT_EQ(Wire.available(), 1);
  EXPECT_EQ(Wire.read(), 0x11);
}

// 20. the order-independent round trip the hook exists for: write a block,
// read it back later without scripting the firmware's read order
TEST_F(WireTest, WriteThenReadBackThroughTheDevice) {
  TinyEeprom dev;
  dev.attach(Wire, 0x50);

  const uint8_t page[4] = {0x80, 0xDE, 0xAD, 0xBE};  // [ptr, data...]
  Wire.beginTransmission(0x50);
  EXPECT_EQ(Wire.write(page, sizeof(page)), 4u);
  Wire.endTransmission();

  Wire.beginTransmission(0x50);
  Wire.write(0x81);
  Wire.endTransmission();
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)2), 2u);
  EXPECT_EQ(Wire.read(), 0xAD);
  EXPECT_EQ(Wire.read(), 0xBE);
}

// 21. a device with no onRequest serves nothing
TEST_F(WireTest, DeviceWithoutOnRequestServesNothing) {
  std::vector<uint8_t> seen;
  Wire.mockAttachDevice(0x40, [&seen](const std::vector<uint8_t>& w) { seen = w; }, nullptr);
  EXPECT_EQ(Wire.requestFrom(0x40, (uint8_t)2), 0u);
  EXPECT_EQ(Wire.available(), 0);

  Wire.beginTransmission(0x40);
  Wire.write(0x07);
  Wire.endTransmission();
  ASSERT_EQ(seen.size(), 1u);
  EXPECT_EQ(seen[0], 0x07);
}

// 22. onReceive gets exactly the bytes of its own transmission, each time
TEST_F(WireTest, EachTransmissionIsDeliveredSeparately) {
  std::vector<std::vector<uint8_t>> frames;
  Wire.mockAttachDevice(
      0x40, [&frames](const std::vector<uint8_t>& w) { frames.push_back(w); }, nullptr);

  Wire.beginTransmission(0x40);
  Wire.write(0x01);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.beginTransmission(0x40);
  Wire.write(0x03);
  Wire.endTransmission();

  ASSERT_EQ(frames.size(), 2u);
  EXPECT_EQ(frames[0], (std::vector<uint8_t>{0x01, 0x02}));
  EXPECT_EQ(frames[1], (std::vector<uint8_t>{0x03}));
  // the raw stream is still there for inspection
  EXPECT_EQ(Wire.mockGetWritten().size(), 3u);
}

// 23. a transmission addressed elsewhere is not delivered
TEST_F(WireTest, TransmissionToAnotherAddressIsNotDelivered) {
  int calls = 0;
  Wire.mockAttachDevice(0x40, [&calls](const std::vector<uint8_t>&) { ++calls; }, nullptr);

  Wire.beginTransmission(0x41);
  Wire.write(0x01);
  Wire.endTransmission();
  EXPECT_EQ(calls, 0);

  Wire.beginTransmission(0x40);
  Wire.write(0x01);
  Wire.endTransmission();
  EXPECT_EQ(calls, 1);
}

// 24. endTransmission without a matching beginTransmission delivers nothing
TEST_F(WireTest, EndTransmissionWithoutBeginDeliversNothing) {
  int calls = 0;
  Wire.mockAttachDevice(0x40, [&calls](const std::vector<uint8_t>&) { ++calls; }, nullptr);
  EXPECT_EQ(Wire.endTransmission(), 0u);
  EXPECT_EQ(calls, 0);
}

// 25. attaching again at the same address replaces the model
TEST_F(WireTest, AttachingAgainReplacesTheDevice) {
  Wire.mockAttachDevice(0x30, nullptr, [](size_t) { return std::vector<uint8_t>{0x01}; });
  Wire.mockAttachDevice(0x30, nullptr, [](size_t) { return std::vector<uint8_t>{0x02, 0x03}; });
  EXPECT_EQ(Wire.requestFrom(0x30, (uint8_t)2), 2u);
  EXPECT_EQ(Wire.read(), 0x02);
  EXPECT_EQ(Wire.read(), 0x03);
}

// 26. a device returning more than asked is truncated to count
TEST_F(WireTest, DeviceReturningTooManyBytesIsTruncated) {
  Wire.mockAttachDevice(
      0x30, nullptr, [](size_t) { return std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04}; });
  EXPECT_EQ(Wire.requestFrom(0x30, (uint8_t)2), 2u);
  EXPECT_EQ(Wire.available(), 2);
  EXPECT_EQ(Wire.read(), 0x01);
  EXPECT_EQ(Wire.read(), 0x02);
  EXPECT_EQ(Wire.read(), -1);
}

// 27. peek() works on device-served bytes without consuming them
TEST_F(WireTest, PeekOnDeviceServedBytes) {
  TinyEeprom dev;
  dev.mem[0x00] = 0x5A;
  dev.attach(Wire, 0x50);
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)1), 1u);
  EXPECT_EQ(Wire.peek(), 0x5A);
  EXPECT_EQ(Wire.available(), 1);
  EXPECT_EQ(Wire.read(), 0x5A);
}

// 28. the size_t and stop overloads reach the device too
TEST_F(WireTest, RequestFromOverloadsReachTheDevice) {
  TinyEeprom dev;
  dev.mem[0x00] = 0x7E;
  dev.attach(Wire, 0x50);

  EXPECT_EQ(Wire.requestFrom(0x50, (size_t)1), 1u);
  EXPECT_EQ(Wire.read(), 0x7E);

  Wire.beginTransmission(0x50);
  Wire.write(0x00);
  Wire.endTransmission();
  EXPECT_EQ(Wire.requestFrom(0x50, (uint8_t)1, true), 1u);
  EXPECT_EQ(Wire.read(), 0x7E);
}
