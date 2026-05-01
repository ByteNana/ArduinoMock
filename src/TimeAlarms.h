#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <functional>
#include <thread>

#include "times.h"

using OnTick_t = std::function<void()>;

using AlarmID_t = uint8_t;
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
    auto& a = _alarms[id];
    a.callback = callback;
    a.intervalMs = static_cast<unsigned long>(seconds) * 1000;
    a.nextFire = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    a.enabled = true;
    a.repeat = true;
    return id;
  }

  AlarmID_t timerOnce(time_t seconds, OnTick_t callback) {
    if (_nextId >= MAX_ALARMS) return dtINVALID_ALARM_ID;
    AlarmID_t id = static_cast<AlarmID_t>(_nextId++);
    auto& a = _alarms[id];
    a.callback = callback;
    a.intervalMs = 0;
    a.nextFire = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    a.enabled = true;
    a.repeat = false;
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
    _alarms[id].callback = nullptr;
    _alarms[id].enabled = false;
    return true;
  }

  void delay(unsigned long ms) {
    auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (std::chrono::steady_clock::now() < end) {
      tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  time_t getNextTrigger(AlarmID_t id) const {
    if (id >= MAX_ALARMS || !_alarms[id].enabled) return 0;
    auto remaining = _alarms[id].nextFire - std::chrono::steady_clock::now();
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(remaining).count();
    return static_cast<time_t>(std::time(nullptr) + (secs > 0 ? secs : 0));
  }

  void write(AlarmID_t id, time_t seconds) {
    if (id >= MAX_ALARMS) return;
    const time_t clampedSeconds = (seconds > 0) ? seconds : 0;
    _alarms[id].intervalMs = static_cast<unsigned long>(clampedSeconds) * 1000;
    _alarms[id].nextFire = std::chrono::steady_clock::now() + std::chrono::seconds(clampedSeconds);
  }

  void writeNextTrigger(AlarmID_t id, time_t timestamp) {
    if (id >= MAX_ALARMS) return;
    const time_t nowTime = std::time(nullptr);
    const auto steadyNow = std::chrono::steady_clock::now();
    using SecondsRep = std::chrono::seconds::rep;
    SecondsRep secs = 0;
    if (timestamp > nowTime) { secs = static_cast<SecondsRep>(timestamp - nowTime); }
    _alarms[id].nextFire = steadyNow + std::chrono::seconds(secs);
  }

  void reset() {
    _nextId = 0;
    for (auto& a : _alarms) {
      a.callback = nullptr;
      a.enabled = false;
    }
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
