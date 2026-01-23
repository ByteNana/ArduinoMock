#include <gtest/gtest.h>

#include "freertos/FreeRTOS.h"

// Test that configASSERT fails when condition is false
TEST(AssertTest, ConfigAssertFailsOnFalseCondition) {
  EXPECT_DEATH(
      {
        configASSERT(0);  // This should trigger vAssertCalled()
      },
      ".*");  // Match any output pattern
}

// Test that configASSERT doesn't fail when condition is true
TEST(AssertTest, ConfigAssertPassesOnTrueCondition) {
  // This should NOT cause death
  configASSERT(1);
  configASSERT(true);
  configASSERT(42);

  // If we reach here, the test passes
  SUCCEED();
}

// Test with actual conditions
TEST(AssertTest, ConfigAssertWithRealConditions) {
  int value = 10;

  // These should pass
  configASSERT(value > 0);
  configASSERT(value == 10);

  // This should cause death
  EXPECT_DEATH({ configASSERT(value < 0); }, ".*");
}

// Test in the context of your FreeRTOS code
TEST(TaskTest, AssertOnInvalidTaskCreation) {
  EXPECT_DEATH(
      {
        TaskHandle_t th = nullptr;
        // Assuming this triggers an assert internally
        configASSERT(th != nullptr);  // This should fail
      },
      ".*");
}
