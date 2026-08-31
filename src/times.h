#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>

// --- mock clock state ---
//
// Mirrors the opt-in pattern already used by TimeLib.h: the default is real
// time, simulation is switched on explicitly, and there is a reset for SetUp().
//
// Everything is atomic because the FreeRTOS shim runs tasks on real
// std::threads: a task spinning on micros() reads the counter from a different
// thread than the one stepping it.
namespace mock {

/// false (the default) => millis()/micros() read the real steady_clock and
/// delay()/delayMicroseconds() really sleep. Nothing below is consulted.
inline std::atomic<bool>& clock_simulated() {
  static std::atomic<bool> simulated{false};
  return simulated;
}

/// Simulated microseconds since boot. Only read while clock_simulated().
inline std::atomic<uint64_t>& clock_us() {
  static std::atomic<uint64_t> us{0};
  return us;
}

/// When true, micros()/millis() truncate their result to 32 bits before
/// widening back to unsigned long, so the counters wrap the way they do on a
/// 32-bit target (micros() at 2^32 us ~= 71.6 min, millis() at 2^32 ms).
/// Native unsigned long is 64-bit, so without this the counter just counts on
/// past 2^32 and no rollover bug is reproducible.
inline std::atomic<bool>& clock_narrow32() {
  static std::atomic<bool> narrow{false};
  return narrow;
}

}  // namespace mock

inline std::chrono::steady_clock::time_point& getTimeStart() {
  static std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
  return time_start;
}

/// Back to the real steady_clock, narrowing off, counter zeroed. Call in SetUp().
inline void mockClockReset() {
  mock::clock_simulated().store(false);
  mock::clock_narrow32().store(false);
  mock::clock_us().store(0);
}

/// Switch millis()/micros()/delay() over to the simulated clock.
inline void mockClockUse(uint64_t start_us = 0) {
  mock::clock_us().store(start_us);
  mock::clock_simulated().store(true);
}

/// Enable/disable 32-bit truncation of the returned millis()/micros() values.
inline void mockClockNarrow32(bool on) { mock::clock_narrow32().store(on); }

/// Step the simulated clock. Safe to call from any thread.
inline void mockClockAdvanceUs(uint64_t us) { mock::clock_us().fetch_add(us); }
inline void mockClockAdvanceMs(uint64_t ms) { mock::clock_us().fetch_add(ms * 1000ULL); }

/// Jump the simulated clock to an absolute value. Not a read-modify-write, so
/// only call it while no task thread is stepping the clock.
inline void mockClockSetUs(uint64_t us) { mock::clock_us().store(us); }

/// The untruncated simulated counter, for assertions.
inline uint64_t mockClockNowUs() { return mock::clock_us().load(); }

// --- core clock ---

inline unsigned long millis() {
  if (mock::clock_simulated().load()) {
    uint64_t ms = mock::clock_us().load() / 1000ULL;
    return mock::clock_narrow32().load() ? static_cast<unsigned long>(static_cast<uint32_t>(ms))
                                         : static_cast<unsigned long>(ms);
  }
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - getTimeStart()).count();
}

inline unsigned long micros() {
  if (mock::clock_simulated().load()) {
    uint64_t us = mock::clock_us().load();
    return mock::clock_narrow32().load() ? static_cast<unsigned long>(static_cast<uint32_t>(us))
                                         : static_cast<unsigned long>(us);
  }
  auto now = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(now - getTimeStart()).count();
}

// While simulated, delaying advances the clock instead of sleeping. This is
// what keeps `while (!due(micros())) delayMicroseconds(5);` loops making
// progress -- against a frozen clock they would spin forever.
inline void delay(unsigned long ms) {
  if (mock::clock_simulated().load()) {
    mock::clock_us().fetch_add(static_cast<uint64_t>(ms) * 1000ULL);
    std::this_thread::yield();
    return;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void delayMicroseconds(unsigned int us) {
  if (mock::clock_simulated().load()) {
    mock::clock_us().fetch_add(static_cast<uint64_t>(us));
    std::this_thread::yield();
    return;
  }
  std::this_thread::sleep_for(std::chrono::microseconds(us));
}
