#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

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

// --- xTaskNotify / ulTaskNotifyTake tests ---

struct NotifyArgs {
  std::atomic<uint32_t> received{0};
  std::atomic<bool> done{false};
};

static void notify_take_task(void* arg) {
  NotifyArgs* s = static_cast<NotifyArgs*>(arg);
  s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)));
  s->done.store(true);
  vTaskDelete(nullptr);
}

TEST(TaskNotifyTest, ValueDeliveredSetValueWithOverwrite) {
  NotifyArgs state;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(xTaskCreate(notify_take_task, "nt", 2048, &state, tskIDLE_PRIORITY + 1, &th), pdPASS);

  // Give task time to start and block inside ulTaskNotifyTake
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  EXPECT_EQ(xTaskNotify(th, 42u, eSetValueWithOverwrite), pdPASS);

  // Wait for task to finish
  for (int i = 0; i < 100 && !state.done.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(state.done.load());
  EXPECT_EQ(state.received.load(), 42u);
}

TEST(TaskNotifyTest, TimeoutReturnsZero) {
  NotifyArgs state;
  TaskHandle_t th = nullptr;
  // Very short wait — no notification will arrive
  struct ShortWaitArgs {
    std::atomic<uint32_t> received{0};
    std::atomic<bool> done{false};
  };
  static auto short_task = [](void* arg) {
    ShortWaitArgs* s = static_cast<ShortWaitArgs*>(arg);
    s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(30)));
    s->done.store(true);
    vTaskDelete(nullptr);
  };
  ShortWaitArgs s2;
  TaskHandle_t th2 = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* a) {
            auto* s = static_cast<ShortWaitArgs*>(a);
            s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(30)));
            s->done.store(true);
            vTaskDelete(nullptr);
          },
          "tw", 2048, &s2, tskIDLE_PRIORITY + 1, &th2),
      pdPASS);

  for (int i = 0; i < 100 && !s2.done.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(s2.done.load());
  EXPECT_EQ(s2.received.load(), 0u);
  (void)short_task;
  (void)state;
  (void)th;
}

TEST(TaskNotifyTest, SetBitsAccumulates) {
  struct BitsArgs {
    std::atomic<uint32_t> received{0};
    std::atomic<bool> done{false};
  };
  BitsArgs s;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* arg) {
            auto* s = static_cast<BitsArgs*>(arg);
            s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)));
            s->done.store(true);
            vTaskDelete(nullptr);
          },
          "bits", 2048, &s, tskIDLE_PRIORITY + 1, &th),
      pdPASS);

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(xTaskNotify(th, 0x01u, eSetBits), pdPASS);

  for (int i = 0; i < 100 && !s.done.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(s.done.load());
  EXPECT_EQ(s.received.load(), 0x01u);
}

TEST(TaskNotifyTest, IncrementAction) {
  struct IncArgs {
    std::atomic<uint32_t> received{0};
    std::atomic<bool> done{false};
  };
  IncArgs s;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* arg) {
            auto* s = static_cast<IncArgs*>(arg);
            s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)));
            s->done.store(true);
            vTaskDelete(nullptr);
          },
          "inc", 2048, &s, tskIDLE_PRIORITY + 1, &th),
      pdPASS);

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  EXPECT_EQ(xTaskNotify(th, 0u, eIncrement), pdPASS);

  for (int i = 0; i < 100 && !s.done.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(s.done.load());
  EXPECT_GE(s.received.load(), 1u);
}

TEST(TaskNotifyTest, SetValueWithoutOverwriteDoesNotReplaceIfPending) {
  // Notify before the task starts waiting — first value should win
  struct OvwArgs {
    std::atomic<uint32_t> received{0};
    std::atomic<bool> done{false};
  };
  OvwArgs s;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* arg) {
            auto* s = static_cast<OvwArgs*>(arg);
            // Delay slightly so the second xTaskNotify fires while we're asleep
            // and first has already been delivered via eSetValueWithoutOverwrite
            s->received.store(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500)));
            s->done.store(true);
            vTaskDelete(nullptr);
          },
          "ovw", 2048, &s, tskIDLE_PRIORITY + 1, &th),
      pdPASS);

  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  // First call: sets value to 10
  EXPECT_EQ(xTaskNotify(th, 10u, eSetValueWithoutOverwrite), pdPASS);
  // Second call: notification already pending — must return pdFALSE and not overwrite
  EXPECT_EQ(xTaskNotify(th, 99u, eSetValueWithoutOverwrite), pdFALSE);

  for (int i = 0; i < 100 && !s.done.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(s.done.load());
  // Task should have received 10, not 99
  EXPECT_EQ(s.received.load(), 10u);
}

TEST(TaskNotifyTest, NullHandleReturnsFail) {
  EXPECT_EQ(xTaskNotify(nullptr, 0u, eSetValueWithOverwrite), pdFALSE);
}

// --- vTaskSuspend / vTaskResume tests ---

TEST(TaskSuspendTest, SuspendSelf_ResumesFromOtherThread) {
  struct State {
    std::atomic<bool> reached_after_resume{false};
  };
  State s;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* arg) {
            auto* s = static_cast<State*>(arg);
            vTaskSuspend(nullptr);  // suspend self — unblocked by vTaskResume
            s->reached_after_resume.store(true);
            vTaskDelete(nullptr);
          },
          "sr", 2048, &s, tskIDLE_PRIORITY + 1, &th),
      pdPASS);

  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  EXPECT_FALSE(s.reached_after_resume.load());

  vTaskResume(th);
  for (int i = 0; i < 50 && !s.reached_after_resume.load(); ++i)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  EXPECT_TRUE(s.reached_after_resume.load());
}

TEST(TaskSuspendTest, SuspendFromOutside_FreezesAtDelay) {
  struct State {
    std::atomic<int> count{0};
    std::atomic<bool> stop{false};
  };
  State s;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(
      xTaskCreate(
          [](void* arg) {
            auto* s = static_cast<State*>(arg);
            while (!s->stop.load()) {
              s->count.fetch_add(1);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            vTaskDelete(nullptr);
          },
          "ext", 2048, &s, tskIDLE_PRIORITY + 1, &th),
      pdPASS);

  std::this_thread::sleep_for(std::chrono::milliseconds(40));
  vTaskSuspend(th);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));  // let it reach the gate
  int snap = s.count.load();
  std::this_thread::sleep_for(std::chrono::milliseconds(80));
  EXPECT_EQ(s.count.load(), snap);  // frozen while suspended

  vTaskResume(th);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_GT(s.count.load(), snap);  // progressing again after resume

  s.stop.store(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

TEST(TaskSuspendTest, ResumeNullIsNoOp) { EXPECT_NO_FATAL_FAILURE(vTaskResume(nullptr)); }
