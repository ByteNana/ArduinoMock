// Verifies that FreeRTOS types are available via Arduino.h alone,
// without an explicit #include "freertos/FreeRTOS.h".
#include <gtest/gtest.h>

#include "Arduino.h"

TEST(FreeRTOSTypesTest, TaskHandleAvailableViaArduinoH) {
  TaskHandle_t handle = nullptr;
  EXPECT_EQ(handle, nullptr);
}

TEST(FreeRTOSTypesTest, QueueHandleAvailableViaArduinoH) {
  QueueHandle_t q = nullptr;
  EXPECT_EQ(q, nullptr);
}

TEST(FreeRTOSTypesTest, TickTypeAvailableViaArduinoH) {
  TickType_t t = 0;
  EXPECT_EQ(t, 0u);
}
