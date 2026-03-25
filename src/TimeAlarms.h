#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

#include "times.h"

typedef std::function<void()> OnTick_t;

typedef uint8_t AlarmID_t;
constexpr AlarmID_t dtINVALID_ALARM_ID = 255;

static constexpr int MAX_ALARMS = 8;

struct AlarmEntry {
  OnTick_t callback;
  unsigned long intervalMs = 0;
  std::chrono::steady_clock::time_point nextFire;
  bool enabled = false;
  bool repeat = false;
};

class AlarmClass {
 public:
  AlarmID_t timerRepeat(time_t seconds, OnTick_t callback) {
    if (_nextId >= MAX_ALARMS) return dtINVALID_ALARM_ID;
    AlarmID_t id = static_cast<AlarmID_t>(_nextId++);
    _alarms[id] = {
        callback, static_cast<unsigned long>(seconds) * 1000,
        std::chrono::steady_clock::now() + std::chrono::seconds(seconds), true, true};
    return id;
  }

  AlarmID_t timerOnce(time_t seconds, OnTick_t callback) {
    if (_nextId >= MAX_ALARMS) return dtINVALID_ALARM_ID;
    AlarmID_t id = static_cast<AlarmID_t>(_nextId++);
    _alarms[id] = {
        callback, 0, std::chrono::steady_clock::now() + std::chrono::seconds(seconds), true, false};
    return id;
  }

  void enable(AlarmID_t id) {
    if (id < MAX_ALARMS) _alarms[id].enabled = true;
  }

  void disable(AlarmID_t id) {
    if (id < MAX_ALARMS) _alarms[id].enabled = false;
  }

  bool free(AlarmID_t id) {
    if (id >= MAX_ALARMS) return false;
    _alarms[id] = {};
    return true;
  }

  void delay(unsigned long ms) {
    auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (std::chrono::steady_clock::now() < end) {
      tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  void reset() {
    _nextId = 0;
    for (auto& a : _alarms) a = {};
  }

 private:
  std::array<AlarmEntry, MAX_ALARMS> _alarms{};
  int _nextId = 0;

  void tick() {
    auto now = std::chrono::steady_clock::now();
    for (auto& a : _alarms) {
      if (!a.enabled || !a.callback) continue;
      if (now >= a.nextFire) {
        a.callback();
        if (a.repeat)
          a.nextFire = now + std::chrono::milliseconds(a.intervalMs);
        else
          a.enabled = false;
      }
    }
  }
};

extern AlarmClass Alarm;
