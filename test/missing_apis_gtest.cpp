#include <gtest/gtest.h>

#include "Arduino.h"
#include "HardwareSerial.h"
#include "freertos/task/task.h"

// --- vTaskDelayUntil ---

TEST(VTaskDelayUntil, UpdatesPreviousWakeTime) {
  TickType_t prev = 100;
  vTaskDelayUntil(&prev, 16);
  EXPECT_EQ(prev, 116u);
}

TEST(VTaskDelayUntil, NullPointerSafe) { EXPECT_NO_THROW(vTaskDelayUntil(nullptr, 10)); }

TEST(VTaskDelayUntil, AccumulatesCorrectly) {
  TickType_t prev = 0;
  for (int i = 0; i < 5; ++i) vTaskDelayUntil(&prev, 20);
  EXPECT_EQ(prev, 100u);
}

// --- HardwareSerial::printf ---

TEST(HardwareSerialPrintf, FormatsString) {
  HardwareSerial s(0);
  s.begin(115200);
  s.printf("Hello %s", "world");
  EXPECT_EQ(s.getTxData(), "Hello world");
}

TEST(HardwareSerialPrintf, FormatsInteger) {
  HardwareSerial s(0);
  s.begin(115200);
  s.printf("val=%d", 42);
  EXPECT_EQ(s.getTxData(), "val=42");
}

TEST(HardwareSerialPrintf, ReturnsCharCount) {
  HardwareSerial s(0);
  s.begin(115200);
  size_t n = s.printf("abc");
  EXPECT_EQ(n, 3u);
}

// --- OUTPUT / INPUT / INPUT_PULLUP ---

TEST(GpioConstants, OutputIsDefined) { EXPECT_EQ(OUTPUT, 0x1); }

TEST(GpioConstants, InputIsDefined) { EXPECT_EQ(INPUT, 0x0); }

TEST(GpioConstants, InputPullupIsDefined) { EXPECT_EQ(INPUT_PULLUP, 0x2); }

TEST(GpioConstants, PinModeDoesNotThrow) { EXPECT_NO_THROW(pinMode(4, OUTPUT)); }

TEST(GpioConstants, DigitalWriteDoesNotThrow) { EXPECT_NO_THROW(digitalWrite(4, 1)); }

TEST(GpioConstants, DigitalReadReturnsZero) {
  mockResetGpio();
  EXPECT_EQ(0, digitalRead(4));
}

// --- Print alias ---

namespace {
struct TestPrinter : Print {
  std::string buf;
  int available() override { return 0; }
  int read() override { return -1; }
  int peek() override { return -1; }
  void flush() override {}
  size_t write(uint8_t b) override {
    buf += static_cast<char>(b);
    return 1;
  }
  size_t write(const uint8_t* data, size_t size) override {
    buf.append(reinterpret_cast<const char*>(data), size);
    return size;
  }
};
}  // namespace

TEST(PrintAlias, InheritsFromStream) {
  TestPrinter p;
  p.print(String("hello"));
  EXPECT_EQ(p.buf, "hello");
}

TEST(PrintAlias, PrintAliasIsSameAsStream) {
  static_assert(std::is_same<Print, Stream>::value, "Print must be an alias for Stream");
}
