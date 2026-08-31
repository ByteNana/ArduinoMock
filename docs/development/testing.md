Testing (GoogleTest/GoogleMock)
===============================

Layout
- Sources to test live in `src/`. Tests live in `test/`.
- We keep `src/Stream.h` free of gmock; test-only `MockStream.h` lives in `src/` for convenience.

Building tests
- Enable tests at configure time (FetchContent will download GoogleTest on first run):
  - `cmake -DENABLE_GTEST_TESTS=ON -B build && cmake --build build`
  - Or simply `make test` (runs configure + build + ctest).

Running tests
- `ctest --test-dir build -V` or `make test`.

Writing tests
- Include the mocks and headers normally:
  - `#include "MockStream.h"`
  - `#include "freertos/FreeRTOS.h"`
- Link against `ArduinoNativeMocks` and GoogleTest/GoogleMock. The repo’s `test/CMakeLists.txt` handles this wiring for the built-in tests.

Controllable clock
- By default `millis()`/`micros()` read the real `steady_clock` and `delay()`/`delayMicroseconds()` really sleep. Tests that need to place the clock opt in, the same way `TimeLib.h` does:
  - `mockClockUse(start_us = 0)`: switch `millis()`/`micros()`/`delay()` over to the simulated clock.
  - `mockClockReset()`: back to real time, narrowing off, counter zeroed — call it in `SetUp()`.
  - `mockClockAdvanceUs(us)` / `mockClockAdvanceMs(ms)`: step the clock. Safe from any thread.
  - `mockClockSetUs(us)`: jump to an absolute value. Not a read-modify-write, so only call it while no task thread is stepping the clock.
  - `mockClockNowUs()`: the untruncated counter, for assertions.
  - `mockClockNarrow32(on)`: truncate `micros()`/`millis()` to 32 bits so they wrap the way they do on a 32-bit target (`micros()` at ~71.6 min, `millis()` at ~49.7 days). Native `unsigned long` is 64-bit, so without this a counter parked near 2^32 just counts past it and no rollover bug is reproducible.
- While simulated, `delay()`/`delayMicroseconds()` advance the clock instead of sleeping. That is what keeps `while (!due(micros())) delayMicroseconds(5);` loops making progress — against a frozen clock they would spin forever — and it means simulated time only moves while the code under test is actually throttling.
- The state is atomic because the FreeRTOS shim runs tasks on real `std::thread`s: a task spinning on `micros()` reads the counter from a different thread than the one stepping it.
- See `test/times_gtest.cpp` (`MockClockTest`) for worked examples, including a throttle loop driven across the `micros()` rollover.

CI
- GitHub Actions builds and runs tests in `.github/workflows/ci.yml`.

