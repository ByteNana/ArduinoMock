#include <gtest/gtest.h>

#include "freertos/FreeRTOS.h"

static void simple_task(void*) {
  vTaskDelay(pdMS_TO_TICKS(30));
  vTaskDelete(nullptr);
}

TEST(TaskTest, TickAndCreate) {
  vTaskStartScheduler();
  TickType_t t0 = xTaskGetTickCount();
  vTaskDelay(pdMS_TO_TICKS(20));
  TickType_t t1 = xTaskGetTickCount();
  EXPECT_GE(t1, t0);

  TaskHandle_t th = nullptr;
  EXPECT_EQ(xTaskCreate(simple_task, "t", 2048, nullptr, tskIDLE_PRIORITY + 1, &th), pdPASS);
  vTaskDelay(pdMS_TO_TICKS(80));
  vTaskEndScheduler();
}

TEST(TaskTest, TaskYieldCompiles) { taskYIELD(); }
