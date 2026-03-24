#include <gtest/gtest.h>

#include "Stream.h"

TEST(HardwareSerialTest, BeginBaudOnly) {
  HardwareSerial s(0);
  s.begin(115200);  // should compile and not crash
}

TEST(HardwareSerialTest, BeginBaudWithConfig) {
  HardwareSerial s(0);
  s.begin(115200, SERIAL_8N1);
}

TEST(HardwareSerialTest, BeginBaudConfigPins) {
  HardwareSerial s(1);
  s.begin(9600, SERIAL_8N1, 16, 17);
}

TEST(HardwareSerialTest, SerialConfigConstantsDefined) {
  EXPECT_EQ(SERIAL_8N1, 0x800001cUL);
}

TEST(HardwareSerialTest, GlobalSerialInstances) {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial3.begin(115200, SERIAL_8N1);
}
