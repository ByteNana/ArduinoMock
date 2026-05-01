#include "timer.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

struct MockTimer {
  std::string name;
  uint32_t period;
  bool autoReload;
  bool active;
  void* timerID;
  TimerCallbackFunction_t callback;
  std::chrono::steady_clock::time_point fireAt;
};

static std::vector<MockTimer*> timers;

static std::chrono::steady_clock::time_point fireAtFromNow(uint32_t periodTicks) {
  return std::chrono::steady_clock::now() + std::chrono::milliseconds(periodTicks);
}

TimerHandle_t xTimerCreate(
    const char* name, uint32_t periodTicks, BaseType_t autoReload, void* timerID,
    TimerCallbackFunction_t callback) {
  MockTimer* t = new MockTimer();
  t->name = (name != nullptr) ? name : "";
  t->period = periodTicks;
  t->autoReload = (autoReload != 0);
  t->active = false;
  t->timerID = timerID;
  t->callback = callback;
  timers.push_back(t);
  return static_cast<TimerHandle_t>(t);
}

BaseType_t xTimerStart(TimerHandle_t timer, TickType_t) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  t->active = true;
  t->fireAt = fireAtFromNow(t->period);
  return pdPASS;
}

BaseType_t xTimerStop(TimerHandle_t timer, TickType_t) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  t->active = false;
  return pdPASS;
}

BaseType_t xTimerReset(TimerHandle_t timer, TickType_t) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  t->active = true;
  t->fireAt = fireAtFromNow(t->period);
  return pdPASS;
}

BaseType_t xTimerDelete(TimerHandle_t timer, TickType_t) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  timers.erase(std::remove(timers.begin(), timers.end(), t), timers.end());
  delete t;
  return pdPASS;
}

BaseType_t xTimerChangePeriod(TimerHandle_t timer, uint32_t newPeriod, TickType_t) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  t->period = newPeriod;
  return pdPASS;
}

BaseType_t xTimerIsTimerActive(TimerHandle_t timer) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  if (!t->active) return pdFALSE;

  if (std::chrono::steady_clock::now() >= t->fireAt) {
    if (t->callback != nullptr) t->callback(static_cast<TimerHandle_t>(t));
    if (t->autoReload) {
      t->fireAt = fireAtFromNow(t->period);
    } else {
      t->active = false;
    }
  }

  return t->active ? pdTRUE : pdFALSE;
}

void* pvTimerGetTimerID(TimerHandle_t timer) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  return t->timerID;
}

const char* pcTimerGetName(TimerHandle_t timer) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  return t->name.c_str();
}

void mockProcessTimers() {
  for (size_t i = 0; i < timers.size(); i++) {
    if (timers[i]->active && timers[i]->callback != nullptr) {
      timers[i]->callback(static_cast<TimerHandle_t>(timers[i]));
      if (!timers[i]->autoReload) timers[i]->active = false;
    }
  }
}

void mockFireTimer(TimerHandle_t timer) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  if (t->callback != nullptr) {
    t->callback(timer);
    if (!t->autoReload) t->active = false;
  }
}

size_t mockGetTimerCount() { return timers.size(); }

void mockClearTimers() {
  for (size_t i = 0; i < timers.size(); i++) { delete timers[i]; }
  timers.clear();
}
