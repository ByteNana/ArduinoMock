#include <gtest/gtest.h>

#include "TimeLib.h"

class TimeLibTest : public ::testing::Test {
 protected:
  void SetUp() override { mockTimeReset(); }
};

// --- single-argument decomposition ---

TEST_F(TimeLibTest, HourDecomposition) {
  // 2024-01-15 13:25:30 UTC  →  epoch 1705325130
  time_t t = 1705325130;
  EXPECT_EQ(13, hour(t));
}

TEST_F(TimeLibTest, MinuteDecomposition) {
  time_t t = 1705325130;
  EXPECT_EQ(25, minute(t));
}

TEST_F(TimeLibTest, SecondDecomposition) {
  time_t t = 1705325130;
  EXPECT_EQ(30, second(t));
}

TEST_F(TimeLibTest, DayDecomposition) {
  time_t t = 1705325130;
  EXPECT_EQ(15, day(t));
}

TEST_F(TimeLibTest, MonthDecomposition) {
  time_t t = 1705325130;
  EXPECT_EQ(1, month(t));
}

TEST_F(TimeLibTest, YearDecomposition) {
  time_t t = 1705325130;
  EXPECT_EQ(2024, year(t));
}

// --- setTime / now ---

TEST_F(TimeLibTest, NowReturnsApproximatelySystemTime) {
  time_t before = std::time(nullptr);
  time_t result = now();
  time_t after = std::time(nullptr);
  EXPECT_GE(result, before);
  EXPECT_LE(result, after + 1);
}

TEST_F(TimeLibTest, SetTimeAdjustsNow) {
  time_t target = 1700000000;
  setTime(target);
  EXPECT_NEAR(now(), target, 2);
}

TEST_F(TimeLibTest, SetTimeThenDecompose) {
  // 2023-11-14 22:13:20 UTC
  time_t target = 1700000000;
  setTime(target);
  EXPECT_EQ(2023, year(now()));
  EXPECT_EQ(11, month(now()));
  EXPECT_EQ(14, day(now()));
}

TEST_F(TimeLibTest, MockTimeResetRestoresSystemTime) {
  setTime(1700000000);
  mockTimeReset();
  time_t before = std::time(nullptr);
  EXPECT_NEAR(now(), before, 2);
}

// --- no-argument variants use now() ---

TEST_F(TimeLibTest, NoArgVariantsConsistentWithNow) {
  time_t target = 1705325130;
  setTime(target);
  EXPECT_EQ(hour(now()), hour());
  EXPECT_EQ(minute(now()), minute());
  EXPECT_EQ(second(now()), second());
  EXPECT_EQ(day(now()), day());
  EXPECT_EQ(month(now()), month());
  EXPECT_EQ(year(now()), year());
}

// --- mockTimeReset isolation ---

TEST_F(TimeLibTest, MockTimeResetDoesNotAffectSingleArgVariants) {
  time_t t = 1705325130;
  mockTimeReset();
  EXPECT_EQ(13, hour(t));  // pure decomposition, unaffected by mock state
}
