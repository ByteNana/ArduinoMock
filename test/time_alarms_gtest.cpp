#include <gtest/gtest.h>

#include "TimeAlarms.h"

// --- existing API ---

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
  AlarmID_t id0 = Alarm.timerRepeat(10, nullptr);
  AlarmID_t id1 = Alarm.timerOnce(5, nullptr);
  AlarmID_t id2 = Alarm.timerRepeat(15, nullptr);
  EXPECT_EQ(id0, 0);
  EXPECT_EQ(id1, 1);
  EXPECT_EQ(id2, 2);
  EXPECT_LT(id0, id1);
  EXPECT_LT(id1, id2);
}

TEST(TimeAlarmsTest, FreeReturnsTrue) {
  Alarm.reset();
  AlarmID_t id = Alarm.timerRepeat(10, nullptr);
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

// --- AlarmID_t and dtINVALID_ALARM_ID ---

TEST(TimeAlarmsTest, AlarmIdTypeExists) {
  AlarmID_t id = 0;
  EXPECT_EQ(id, 0);
}

TEST(TimeAlarmsTest, InvalidAlarmIdConstant) { EXPECT_EQ(dtINVALID_ALARM_ID, 255); }

TEST(TimeAlarmsTest, MaxAlarmsReturnsInvalidId) {
  Alarm.reset();
  for (int i = 0; i < MAX_ALARMS; ++i) Alarm.timerRepeat(1, nullptr);
  EXPECT_EQ(Alarm.timerRepeat(1, nullptr), dtINVALID_ALARM_ID);
}

// --- enable / disable ---

TEST(TimeAlarmsTest, EnableDisableDoNotThrow) {
  Alarm.reset();
  AlarmID_t id = Alarm.timerRepeat(100, nullptr);
  EXPECT_NO_THROW(Alarm.disable(id));
  EXPECT_NO_THROW(Alarm.enable(id));
}

TEST(TimeAlarmsTest, DisabledAlarmDoesNotFire) {
  Alarm.reset();
  int count = 0;
  AlarmID_t id = Alarm.timerRepeat(0, [&]() { count++; });
  Alarm.disable(id);
  Alarm.delay(5);
  EXPECT_EQ(count, 0);
}

// --- callback firing ---

TEST(TimeAlarmsTest, TimerOnceFiresCallback) {
  Alarm.reset();
  int count = 0;
  Alarm.timerOnce(0, [&]() { count++; });
  Alarm.delay(10);
  EXPECT_EQ(count, 1);
}

TEST(TimeAlarmsTest, TimerRepeatFiresMultipleTimes) {
  Alarm.reset();
  int count = 0;
  Alarm.timerRepeat(0, [&]() { count++; });
  Alarm.delay(10);
  EXPECT_GT(count, 1);
}

TEST(TimeAlarmsTest, TimerOnceFiresOnlyOnce) {
  Alarm.reset();
  int count = 0;
  Alarm.timerOnce(0, [&]() { count++; });
  Alarm.delay(20);
  EXPECT_EQ(count, 1);
}
