#include <gtest/gtest.h>

#include "TimeAlarms.h"

TEST(TimeAlarmsTest, TimerRepeatReturnsIncrementingIds) {
  Alarm.reset();
  EXPECT_EQ(Alarm.timerRepeat(10, nullptr), 0);
  EXPECT_EQ(Alarm.timerRepeat(20, nullptr), 1);
  EXPECT_EQ(Alarm.timerRepeat(30, nullptr), 2);
}

TEST(TimeAlarmsTest, TimerOnceReturnsIncrementingIds) {
  Alarm.reset();
  EXPECT_EQ(Alarm.timerOnce(10, nullptr), 0);
  EXPECT_EQ(Alarm.timerOnce(20, nullptr), 1);
  EXPECT_EQ(Alarm.timerOnce(30, nullptr), 2);
}

TEST(TimeAlarmsTest, RepeatAndOnceIdsAreDistinctAndMonotonic) {
  Alarm.reset();
  int id0 = Alarm.timerRepeat(10, nullptr);
  int id1 = Alarm.timerOnce(5, nullptr);
  int id2 = Alarm.timerRepeat(15, nullptr);
  EXPECT_EQ(id0, 0);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(id2, 2);
  EXPECT_LT(id0, id1);
  EXPECT_LT(id1, id2);
}

TEST(TimeAlarmsTest, FreeReturnsTrue) {
  Alarm.reset();
  int id = Alarm.timerRepeat(10, nullptr);
  EXPECT_TRUE(Alarm.free(id));
}

TEST(TimeAlarmsTest, DelayZeroCompiles) {
  Alarm.delay(0);
  SUCCEED();
}

TEST(TimeAlarmsTest, ResetResetsIdCounterToZero) {
  Alarm.reset();
  Alarm.timerRepeat(1, nullptr);
  Alarm.timerRepeat(2, nullptr);
  Alarm.reset();
  EXPECT_EQ(Alarm.timerRepeat(3, nullptr), 0);
}

TEST(TimeAlarmsTest, GlobalAlarmInstanceIsAccessible) {
  Alarm.reset();
  EXPECT_EQ(Alarm.timerRepeat(1, nullptr), 0);
}
