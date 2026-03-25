#include "timer.h"

#include <algorithm>
#include <string>
#include <vector>

struct MockTimer {
  std::string name;
  uint32_t period;
  bool autoReload;
  bool active;
  void* timerID;
  TimerCallbackFunction_t callback;
};

static std::vector<MockTimer*> timers;

TimerHandle_t xTimerCreate(
    const char* name, uint32_t periodTicks, BaseType_t autoReload, void* timerID,
    TimerCallbackFunction_t callback) {
  MockTimer* t = new MockTimer();
  t->name = name ? name : "";
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
    if (timers[i]->active && timers[i]->callback) {
      timers[i]->callback(static_cast<TimerHandle_t>(timers[i]));
    }
  }
}

void mockFireTimer(TimerHandle_t timer) {
  MockTimer* t = static_cast<MockTimer*>(timer);
  if (t->callback) { t->callback(timer); }
}

size_t mockGetTimerCount() { return timers.size(); }

void mockClearTimers() {
  for (size_t i = 0; i < timers.size(); i++) { delete timers[i]; }
  timers.clear();
}
