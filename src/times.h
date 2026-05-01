#pragma once

#include <chrono>
#include <thread>

inline std::chrono::steady_clock::time_point& getTimeStart() {
  static std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
  return time_start;
}

inline unsigned long millis() {
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - getTimeStart()).count();
}

inline unsigned long micros() {
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now - getTimeStart()).count();
}

inline void delay(unsigned long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

inline void delayMicroseconds(unsigned int us) {
  std::this_thread::sleep_for(std::chrono::microseconds(us));
}
