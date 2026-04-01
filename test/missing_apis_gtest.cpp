#include <gtest/gtest.h>

#include <type_traits>

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

// --- Print class ---

namespace {
struct TestPrinter : Print {
  std::string buf;
  size_t write(uint8_t b) override {
    buf += static_cast<char>(b);
    return 1;
  }
};
}  // namespace

TEST(PrintClass, IsDistinctFromStream) {
  static_assert(!std::is_same<Print, Stream>::value, "Print and Stream must be distinct");
  static_assert(std::is_base_of<Print, Stream>::value, "Stream must derive from Print");
}

TEST(PrintClass, CanInheritWithoutStreamObligations) {
  // Print only requires write(uint8_t) — no available/read/peek needed
  TestPrinter p;
  p.print(String("hello"));
  EXPECT_EQ(p.buf, "hello");
}

TEST(PrintClass, PrintChar) {
  TestPrinter p;
  p.print('A');
  EXPECT_EQ(p.buf, "A");
}

TEST(PrintClass, PrintIntDecimal) {
  TestPrinter p;
  p.print(-42, 10);
  EXPECT_EQ(p.buf, "-42");
}

TEST(PrintClass, PrintUnsignedLongHex) {
  TestPrinter p;
  p.print(255UL, 16);
  EXPECT_EQ(p.buf, "FF");
}

TEST(PrintClass, PrintUnsignedLongBinary) {
  TestPrinter p;
  p.print(5UL, 2);
  EXPECT_EQ(p.buf, "101");
}

TEST(PrintClass, PrintDouble) {
  TestPrinter p;
  p.print(3.14, 2);
  EXPECT_EQ(p.buf, "3.14");
}

TEST(PrintClass, Println) {
  TestPrinter p;
  p.println("hi");
  EXPECT_EQ(p.buf, "hi\r\n");
}

TEST(PrintClass, Printf) {
  TestPrinter p;
  p.printf("val=%d", 7);
  EXPECT_EQ(p.buf, "val=7");
}

TEST(PrintClass, FlushIsNonPureAndNoOp) {
  TestPrinter p;
  EXPECT_NO_THROW(p.flush());
}

// --- yield ---

TEST(YieldTest, YieldCompiles) { EXPECT_NO_THROW(yield()); }
