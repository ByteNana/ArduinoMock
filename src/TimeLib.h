#pragma once
#include <ctime>

// --- mock clock state ---
namespace mock {

inline time_t& time_offset() {
  static time_t offset = 0;
  return offset;
}

}  // namespace mock

/// Reset the mock clock offset to zero. Call in SetUp().
inline void mockTimeReset() { mock::time_offset() = 0; }

// --- core clock ---

/// Returns the current (possibly mocked) wall-clock time.
inline time_t now() { return std::time(nullptr) + mock::time_offset(); }

/// Set the mock clock to a specific wall-clock time without mutating the system clock.
inline void setTime(time_t t) { mock::time_offset() = t - std::time(nullptr); }

// --- time_t decomposition (single-argument) ---

inline int hour(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_hour;
}
inline int minute(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_min;
}
inline int second(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_sec;
}
inline int day(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_mday;
}
inline int month(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_mon + 1;
}
inline int year(time_t t) {
  struct tm r{};
  gmtime_r(&t, &r);
  return r.tm_year + 1900;
}

// --- no-argument variants operate on now() ---

inline int hour() { return hour(now()); }
inline int minute() { return minute(now()); }
inline int second() { return second(now()); }
inline int day() { return day(now()); }
inline int month() { return month(now()); }
inline int year() { return year(now()); }
