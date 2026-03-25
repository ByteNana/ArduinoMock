#include <gtest/gtest.h>

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
