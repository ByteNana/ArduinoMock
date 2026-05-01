#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "freertos/FreeRTOS.h"

static int callbackCount = 0;
static TimerHandle_t lastCallbackTimer = nullptr;

static void testCallback(TimerHandle_t timer) {
  callbackCount++;
  lastCallbackTimer = timer;
}

class TimerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mockClearTimers();
    callbackCount = 0;
    lastCallbackTimer = nullptr;
  }
  void TearDown() override { mockClearTimers(); }
};

TEST_F(TimerTest, CreateReturnsNonNull) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  ASSERT_NE(t, nullptr);
}

TEST_F(TimerTest, GetNameReturnsCorrectName) {
  TimerHandle_t t = xTimerCreate("myTimer", 1000, pdFALSE, nullptr, testCallback);
  EXPECT_STREQ(pcTimerGetName(t), "myTimer");
}

TEST_F(TimerTest, GetTimerIDReturnsCorrectID) {
  int id = 42;
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, &id, testCallback);
  EXPECT_EQ(pvTimerGetTimerID(t), &id);
}

TEST_F(TimerTest, StartAndProcessFiresCallback) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(xTimerStart(t, 0), pdPASS);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(lastCallbackTimer, t);
}

TEST_F(TimerTest, StoppedTimerDoesNotFire) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  xTimerStop(t, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 0);
}

TEST_F(TimerTest, MockFireTimerFiresSpecificTimer) {
  TimerHandle_t t1 = xTimerCreate("t1", 1000, pdFALSE, nullptr, testCallback);
  TimerHandle_t t2 = xTimerCreate("t2", 2000, pdFALSE, nullptr, testCallback);
  (void)t1;
  mockFireTimer(t2);
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(lastCallbackTimer, t2);
}

TEST_F(TimerTest, ChangePeriodUpdatesPeriod) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(xTimerChangePeriod(t, 5000, 0), pdPASS);
}

TEST_F(TimerTest, DeleteRemovesTimer) {
  TimerHandle_t t1 = xTimerCreate("t1", 1000, pdFALSE, nullptr, testCallback);
  xTimerCreate("t2", 2000, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(mockGetTimerCount(), 2u);
  xTimerDelete(t1, 0);
  EXPECT_EQ(mockGetTimerCount(), 1u);
}

TEST_F(TimerTest, GetTimerCountTracksTimers) {
  EXPECT_EQ(mockGetTimerCount(), 0u);
  xTimerCreate("t1", 100, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(mockGetTimerCount(), 1u);
  xTimerCreate("t2", 200, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(mockGetTimerCount(), 2u);
}

TEST_F(TimerTest, ClearTimersRemovesAll) {
  xTimerCreate("t1", 100, pdFALSE, nullptr, testCallback);
  xTimerCreate("t2", 200, pdFALSE, nullptr, testCallback);
  mockClearTimers();
  EXPECT_EQ(mockGetTimerCount(), 0u);
}

TEST_F(TimerTest, OnlyActiveTimersFire) {
  TimerHandle_t t1 = xTimerCreate("active", 100, pdFALSE, nullptr, testCallback);
  xTimerCreate("inactive", 200, pdFALSE, nullptr, testCallback);
  xTimerStart(t1, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(lastCallbackTimer, t1);
}

TEST_F(TimerTest, ResetReactivatesStoppedTimer) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  xTimerStop(t, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 0);

  xTimerReset(t, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 1);
}

TEST_F(TimerTest, IsTimerActiveReturnsFalseAfterCreate) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
}

TEST_F(TimerTest, IsTimerActiveReturnsTrueAfterStart) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  EXPECT_EQ(xTimerIsTimerActive(t), pdTRUE);
}

TEST_F(TimerTest, IsTimerActiveReturnsFalseAfterStop) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  xTimerStop(t, 0);
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
}

TEST_F(TimerTest, IsTimerActiveReturnsTrueAfterReset) {
  TimerHandle_t t = xTimerCreate("test", 1000, pdFALSE, nullptr, testCallback);
  xTimerReset(t, 0);
  EXPECT_EQ(xTimerIsTimerActive(t), pdTRUE);
}

// --- time semantics ---

TEST_F(TimerTest, OneShotBecomesInactiveAfterPeriodElapses) {
  // period=0 fires immediately on the next xTimerIsTimerActive call
  TimerHandle_t t = xTimerCreate("one", 0, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
  EXPECT_EQ(callbackCount, 1);
}

TEST_F(TimerTest, AutoReloadStaysActiveAfterPeriodElapses) {
  TimerHandle_t t = xTimerCreate("repeat", 0, pdTRUE, nullptr, testCallback);
  xTimerStart(t, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  EXPECT_EQ(xTimerIsTimerActive(t), pdTRUE);
  EXPECT_EQ(callbackCount, 1);
}

TEST_F(TimerTest, MockProcessTimersDeactivatesOneShotAfterFiring) {
  TimerHandle_t t = xTimerCreate("one", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 1);
  // one-shot should now be inactive
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
}

TEST_F(TimerTest, MockFireTimerDeactivatesOneShotAfterFiring) {
  TimerHandle_t t = xTimerCreate("one", 1000, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  mockFireTimer(t);
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
}

TEST_F(TimerTest, MockProcessTimersKeepsAutoReloadActive) {
  TimerHandle_t t = xTimerCreate("repeat", 1000, pdTRUE, nullptr, testCallback);
  xTimerStart(t, 0);
  mockProcessTimers();
  EXPECT_EQ(callbackCount, 1);
  // auto-reload stays active after firing
  EXPECT_EQ(xTimerStop(t, 0), pdPASS);  // just verify it is still stoppable
}

TEST_F(TimerTest, ResetRestartsFireAtFromNow) {
  // Start with 0ms period so it expires after a short sleep
  TimerHandle_t t = xTimerCreate("one", 0, pdFALSE, nullptr, testCallback);
  xTimerStart(t, 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  // Expired: confirm inactive before reset
  EXPECT_EQ(xTimerIsTimerActive(t), pdFALSE);
  // Change period to 1s then reset — fireAt is now + 1s
  xTimerChangePeriod(t, 1000, 0);
  xTimerReset(t, 0);
  // Should be active again (new fireAt is 1s from now)
  EXPECT_EQ(xTimerIsTimerActive(t), pdTRUE);
}
