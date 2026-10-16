Arduino Shims
=============

Overview of Arduino-style headers implemented for host-native builds.

Headers
- `Arduino.h`: Arduino types (`boolean`, `byte`, `word`) and helpers.
  - Includes: `Stream.h`, `WString.h`, `times.h`.
  - Utilities: `randomSeed`, `random(min,max)`, character helpers (`isDigit`, `isAlpha`, `isSpace`, ...).
- `WString.h`: Lightweight `String` class atop `std::string` with Arduino-like helpers.
  - Methods: `length`, `substring`, `indexOf/lastIndexOf`, `startsWith/endsWith`, `replace`, `concat`.
- `times.h`: Wall-clock timing helpers, with an opt-in simulated clock.
  - `millis()`/`micros()`: elapsed time since first call; `delay(ms)`/`delayMicroseconds(us)` sleep the current thread.
  - Simulation is off by default. See [Testing](testing.md#controllable-clock) for the mock clock API.
- `Stream.h`: Pure interface and convenience helpers for printing.
  - Virtuals: `available`, `read`, `peek`, `write(uint8_t)`, `write(const uint8_t*, size_t)`, `flush`.
  - Helpers: `print`, `println`, `timedRead`.
  - `HardwareSerial`: minimal stub for examples (no real I/O).

Notes
- `Stream` is designed to be mocked in tests; see `src/MockStream.h`.
- These shims are pragmatic for host testing, not full replacements for core Arduino.

