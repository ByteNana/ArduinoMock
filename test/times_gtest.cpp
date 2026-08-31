#include <Arduino.h>
#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

// --- controllable mock clock ---

namespace {

constexpr uint64_t kMicrosRollover = 0x100000000ULL;
constexpr int kAdvanceThreads = 8;
constexpr int kAdvanceSteps = 1000;

/// Real elapsed wall-clock milliseconds since `start`.
long wallElapsedMs(std::chrono::steady_clock::time_point start) {
  auto elapsed = std::chrono::steady_clock::now() - start;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
  return static_cast<long>(ms.count());
}

}  // namespace

class MockClockTest : public ::testing::Test {
 protected:
  void SetUp() override { mockClockReset(); }
  void TearDown() override { mockClockReset(); }
};

TEST_F(MockClockTest, DefaultIsRealTime) {
  // Without mockClockUse() nothing below the seam is consulted: real clock,
  // real sleeps, and advances are ignored.
  mockClockAdvanceMs(10000);
  unsigned long t1 = micros();
  delayMicroseconds(2000);
  unsigned long t2 = micros();
  EXPECT_GE(t2 - t1, 2000ul);
  EXPECT_LT(t2, 10000000ul);
}

TEST_F(MockClockTest, UseFreezesTheClockAtTheGivenStart) {
  mockClockUse(1234567);
  EXPECT_EQ(micros(), 1234567ul);
  EXPECT_EQ(millis(), 1234ul);
  EXPECT_EQ(mockClockNowUs(), 1234567ULL);
  // Nothing steps it on its own.
  EXPECT_EQ(micros(), 1234567ul);
}

TEST_F(MockClockTest, AdvanceAndSetMoveTheCounter) {
  mockClockUse();
  EXPECT_EQ(micros(), 0ul);
  mockClockAdvanceUs(750);
  EXPECT_EQ(micros(), 750ul);
  EXPECT_EQ(millis(), 0ul);
  mockClockAdvanceMs(2);
  EXPECT_EQ(micros(), 2750ul);
  EXPECT_EQ(millis(), 2ul);
  mockClockSetUs(500);
  EXPECT_EQ(micros(), 500ul);
  EXPECT_EQ(mockClockNowUs(), 500ULL);
}

TEST_F(MockClockTest, DelayAdvancesTheClockInsteadOfSleeping) {
  mockClockUse();
  auto wallStart = std::chrono::steady_clock::now();
  delay(5000);
  delayMicroseconds(1500);
  EXPECT_EQ(mockClockNowUs(), 5001500ULL);
  EXPECT_EQ(millis(), 5001ul);
  // Five simulated seconds must not cost five real ones.
  EXPECT_LT(wallElapsedMs(wallStart), 1000L);
}

TEST_F(MockClockTest, SpinLoopOnMicrosTerminatesAgainstTheSimulatedClock) {
  // The pattern that hangs forever against a frozen clock.
  mockClockUse();
  unsigned long deadline = micros() + 10000;
  while (micros() < deadline) delayMicroseconds(5);
  EXPECT_GE(micros(), 10000ul);
  EXPECT_LT(micros(), 10010ul);
}

TEST_F(MockClockTest, Narrow32WrapsMicrosAtTwoToThe32) {
  mockClockUse(kMicrosRollover - 1);
  mockClockNarrow32(true);
  EXPECT_EQ(micros(), 0xFFFFFFFFul);
  mockClockAdvanceUs(1);
  EXPECT_EQ(micros(), 0ul);
  mockClockAdvanceUs(25);
  EXPECT_EQ(micros(), 25ul);
  // The untruncated counter keeps counting, so assertions stay unambiguous.
  EXPECT_EQ(mockClockNowUs(), kMicrosRollover + 25ULL);
}

TEST_F(MockClockTest, Narrow32WrapsMillisAtTwoToThe32) {
  mockClockUse((kMicrosRollover - 1) * 1000ULL);
  mockClockNarrow32(true);
  EXPECT_EQ(millis(), 0xFFFFFFFFul);
  mockClockAdvanceMs(1);
  EXPECT_EQ(millis(), 0ul);
}

TEST_F(MockClockTest, WithoutNarrow32TheCounterRunsPastTwoToThe32) {
  mockClockUse(kMicrosRollover - 1);
  mockClockAdvanceUs(1);
  if (sizeof(unsigned long) >= 8) { EXPECT_EQ(static_cast<uint64_t>(micros()), kMicrosRollover); }
  EXPECT_EQ(mockClockNowUs(), kMicrosRollover);
}

TEST_F(MockClockTest, ThrottleLoopSurvivesTheMicrosRollover) {
  // A 1 kHz throttle re-anchored in 32-bit arithmetic, driven across the 71.6
  // minute boundary in a few milliseconds of real time.
  const uint32_t interval = 1000;
  const int wanted = 20;
  mockClockUse(kMicrosRollover - 5 * interval);
  mockClockNarrow32(true);

  uint64_t startUs = mockClockNowUs();
  uint32_t anchor = static_cast<uint32_t>(micros());
  int samples = 0;
  while (samples < wanted) {
    while (static_cast<uint32_t>(micros()) - anchor < interval) delayMicroseconds(5);
    anchor += interval;
    ++samples;
  }

  EXPECT_EQ(samples, wanted);
  EXPECT_LT(startUs, kMicrosRollover);
  EXPECT_GT(mockClockNowUs(), kMicrosRollover);
  uint64_t consumed = mockClockNowUs() - startUs;
  EXPECT_GE(consumed, static_cast<uint64_t>(wanted) * interval);
  EXPECT_LT(consumed, static_cast<uint64_t>(wanted) * interval + 100ULL);
}

TEST_F(MockClockTest, AdvancesFromManyThreadsAreNotLost) {
  mockClockUse();
  std::vector<std::thread> workers;
  for (int i = 0; i < kAdvanceThreads; ++i) {
    workers.emplace_back([]() {
      for (int j = 0; j < kAdvanceSteps; ++j) mockClockAdvanceUs(1);
    });
  }
  for (auto& w : workers) w.join();
  EXPECT_EQ(mockClockNowUs(), static_cast<uint64_t>(kAdvanceThreads) * kAdvanceSteps);
}

namespace {

struct PacedTaskArgs {
  std::atomic<int> samples{0};
  std::atomic<bool> done{false};
};

/// Paces itself on micros() the way firmware does, on a real task thread.
void paced_task(void* arg) {
  PacedTaskArgs* s = static_cast<PacedTaskArgs*>(arg);
  uint32_t anchor = static_cast<uint32_t>(micros());
  while (s->samples.load() < 50) {
    while (static_cast<uint32_t>(micros()) - anchor < 1000) delayMicroseconds(5);
    anchor += 1000;
    s->samples.fetch_add(1);
  }
  s->done.store(true);
  vTaskDelete(nullptr);
}

}  // namespace

TEST_F(MockClockTest, TaskThreadPacesItselfOnTheSimulatedClock) {
  mockClockUse();
  vTaskStartScheduler();

  PacedTaskArgs args;
  TaskHandle_t th = nullptr;
  ASSERT_EQ(xTaskCreate(paced_task, "paced", 2048, &args, tskIDLE_PRIORITY + 1, &th), pdPASS);

  auto wallStart = std::chrono::steady_clock::now();
  while (!args.done.load() && wallElapsedMs(wallStart) < 5000L) std::this_thread::yield();

  EXPECT_TRUE(args.done.load());
  EXPECT_EQ(args.samples.load(), 50);
  // 50 ms of simulated time, spent from a thread that is not the one that
  // called mockClockUse().
  EXPECT_GE(mockClockNowUs(), 50000ULL);
  vTaskEndScheduler();
}
