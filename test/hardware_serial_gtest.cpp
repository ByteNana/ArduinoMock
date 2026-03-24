#include <gtest/gtest.h>

#include "HardwareSerial.h"

// --- begin() overloads ---

TEST(HardwareSerialTest, BeginBaudOnly) {
  HardwareSerial s(0);
  s.begin(115200);
  EXPECT_EQ(s.baudRate(), 115200UL);
}

TEST(HardwareSerialTest, BeginBaudWithConfig) {
  HardwareSerial s(0);
  s.begin(9600, SERIAL_8N1);
  EXPECT_EQ(s.baudRate(), 9600UL);
}

TEST(HardwareSerialTest, BeginBaudConfigPins) {
  HardwareSerial s(1);
  s.begin(115200, SERIAL_8N1, 16, 17);
  EXPECT_EQ(s.baudRate(), 115200UL);
}

// --- Serial config constants ---

TEST(HardwareSerialTest, ConfigConstantsDefined) {
  EXPECT_EQ(SERIAL_8N1, 0x800001cUL);
  EXPECT_EQ(SERIAL_8N2, 0x800003cUL);
  EXPECT_EQ(SERIAL_8E1, 0x8000016UL);
  EXPECT_EQ(SERIAL_8O1, 0x8000017UL);
}

// --- write / getTxData ---

TEST(HardwareSerialTest, WriteSingleByte) {
  HardwareSerial s(0);
  EXPECT_EQ(s.write(static_cast<uint8_t>('A')), 1u);
  EXPECT_EQ(s.getTxData(), "A");
}

TEST(HardwareSerialTest, WriteBuffer) {
  HardwareSerial s(0);
  const uint8_t buf[] = {'h', 'i', '\n'};
  EXPECT_EQ(s.write(buf, 3), 3u);
  EXPECT_EQ(s.getTxData(), "hi\n");
}

TEST(HardwareSerialTest, GetTxDataDrainsBuffer) {
  HardwareSerial s(0);
  s.write(static_cast<uint8_t>('X'));
  s.getTxData();
  EXPECT_EQ(s.getTxData(), "");
}

// --- injectRxData / read / available / peek ---

TEST(HardwareSerialTest, AvailableReflectsInjectedBytes) {
  HardwareSerial s(0);
  EXPECT_EQ(s.available(), 0);
  s.injectRxData("abc");
  EXPECT_EQ(s.available(), 3);
}

TEST(HardwareSerialTest, ReadConsumesBytes) {
  HardwareSerial s(0);
  s.injectRxData("hi");
  EXPECT_EQ(s.read(), 'h');
  EXPECT_EQ(s.read(), 'i');
  EXPECT_EQ(s.read(), -1);
}

TEST(HardwareSerialTest, PeekDoesNotConsume) {
  HardwareSerial s(0);
  s.injectRxData("Z");
  EXPECT_EQ(s.peek(), 'Z');
  EXPECT_EQ(s.peek(), 'Z');
  EXPECT_EQ(s.available(), 1);
}

TEST(HardwareSerialTest, ReadEmptyReturnsMinusOne) {
  HardwareSerial s(0);
  EXPECT_EQ(s.read(), -1);
}

TEST(HardwareSerialTest, PeekEmptyReturnsMinusOne) {
  HardwareSerial s(0);
  EXPECT_EQ(s.peek(), -1);
}

// --- flush ---

TEST(HardwareSerialTest, FlushClearsTxBuffer) {
  HardwareSerial s(0);
  s.write(static_cast<uint8_t>('X'));
  s.flush();
  EXPECT_EQ(s.getTxData(), "");
}

// --- clearBuffers ---

TEST(HardwareSerialTest, ClearBuffersEmptiesBoth) {
  HardwareSerial s(0);
  s.injectRxData("data");
  s.write(static_cast<uint8_t>('X'));
  s.clearBuffers();
  EXPECT_EQ(s.available(), 0);
  EXPECT_EQ(s.getTxData(), "");
}

// --- reset ---

TEST(HardwareSerialTest, ResetClearsStateAndBuffers) {
  HardwareSerial s(0);
  s.begin(115200, SERIAL_8N1, 4, 5);
  s.injectRxData("hello");
  s.write(static_cast<uint8_t>('!'));
  s.reset();
  EXPECT_EQ(s.baudRate(), 0UL);
  EXPECT_EQ(s.available(), 0);
  EXPECT_EQ(s.getTxData(), "");
}

// --- end ---

TEST(HardwareSerialTest, EndResetsState) {
  HardwareSerial s(0);
  s.begin(9600);
  s.injectRxData("x");
  s.end();
  EXPECT_EQ(s.baudRate(), 0UL);
  EXPECT_EQ(s.available(), 0);
}

// --- multi-instance independence ---

TEST(HardwareSerialTest, InstancesAreIndependent) {
  HardwareSerial s0(0);
  HardwareSerial s1(1);
  s0.injectRxData("from-s0");
  s1.injectRxData("from-s1");
  EXPECT_EQ(s0.available(), 7);
  EXPECT_EQ(s1.available(), 7);
  s0.read();
  EXPECT_EQ(s1.available(), 7);
}

// --- global instances ---

TEST(HardwareSerialTest, GlobalSerialInstances) {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial3.begin(115200, SERIAL_8N1);
  EXPECT_EQ(Serial.baudRate(), 115200UL);
  EXPECT_EQ(Serial2.baudRate(), 9600UL);
  EXPECT_EQ(Serial3.baudRate(), 115200UL);
  Serial.reset();
  Serial2.reset();
  Serial3.reset();
}
