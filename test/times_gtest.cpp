#include <Arduino.h>
#include <gtest/gtest.h>

TEST(TimesTest, MillisIncreases) {
  unsigned long t1 = millis();
  delay(10);
  unsigned long t2 = millis();
  EXPECT_GT(t2, t1);
  EXPECT_GE(t2 - t1, 10ul);
}

TEST(TimesTest, MicrosIncreases) {
  unsigned long t1 = micros();
  delayMicroseconds(1000);  // 1ms = 1000us
  unsigned long t2 = micros();
  EXPECT_GT(t2, t1);
  EXPECT_GE(t2 - t1, 1000ul);
}

TEST(TimesTest, MicrosPrecision) {
  unsigned long t1 = micros();
  delayMicroseconds(100);
  unsigned long t2 = micros();
  EXPECT_GT(t2, t1);
  // Verify we can measure time intervals less than 1ms
  EXPECT_GE(t2 - t1, 100ul);
}

TEST(TimesTest, DelayFunctionality) {
  unsigned long start = millis();
  delay(50);
  unsigned long elapsed = millis() - start;
  EXPECT_GE(elapsed, 50ul);
}

TEST(TimesTest, DelayMicrosecondsFunctionality) {
  unsigned long start = micros();
  delayMicroseconds(5000);  // 5ms = 5000us
  unsigned long elapsed = micros() - start;
  EXPECT_GE(elapsed, 5000ul);
}
