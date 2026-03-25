#pragma once
#include <cstdint>
#include <functional>

#include "times.h"

typedef void (*OnTick_t)();

class AlarmClass {
 public:
  void delay(unsigned long ms) { ::delay(ms); }
  int timerRepeat(unsigned long seconds, OnTick_t callback) {
    (void)seconds;
    (void)callback;
    return _nextId++;
  }
  int timerOnce(unsigned long seconds, OnTick_t callback) {
    (void)seconds;
    (void)callback;
    return _nextId++;
  }
  bool free(int id) {
    (void)id;
    return true;
  }
  void reset() { _nextId = 0; }

 private:
  int _nextId = 0;
};

extern AlarmClass Alarm;
