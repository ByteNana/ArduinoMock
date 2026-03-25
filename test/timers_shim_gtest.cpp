#include <gtest/gtest.h>

// Include via the ESP32 path to verify the shim works
#include "freertos/timers.h"

TEST(TimersShim, CreateViaShimInclude) {
  mockClearTimers();
  TimerHandle_t t = xTimerCreate("shim", 1000, pdFALSE, nullptr, nullptr);
  EXPECT_NE(t, nullptr);
  EXPECT_EQ(mockGetTimerCount(), 1u);
  mockClearTimers();
}

TEST(TimersShim, IsTimerActiveViaShimInclude) {
  mockClearTimers();
  TimerHandle_t t = xTimerCreate("shim", 1000, pdFALSE, nullptr, nullptr);
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
  xTimerStart(t, 0);
  EXPECT_EQ(xTimerIsTimerActive(t), pdTRUE);
  mockClearTimers();
}
