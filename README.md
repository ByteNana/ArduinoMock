ArduinoNativeMocks
===================

[![CI](https://github.com/ByteNana/Ardunio-Mock/actions/workflows/ci.yml/badge.svg)](https://github.com/ByteNana/Ardunio-Mock/actions/workflows/ci.yml)

Host-native Arduino + FreeRTOS mocks to build and test Arduino-style code on your desktop using CMake. It provides light shims for common Arduino headers and a minimal FreeRTOS API implemented with C++ threads/condition_variables.

**Highlights**
- **Arduino shims:** `Arduino.h`, `WString.h`, `Stream.h` (plus a minimal `HardwareSerial`), and `times.h` (`millis`, `micros`, `delay`, plus an opt-in controllable clock for tests).
- **FreeRTOS subset:** `freertos/FreeRTOS.h` (+ `FreeRTOSConfig.h`, `projdefs.h`) and modules: `queue`, `semaphore`, `task`, with hooks in `freertos_hooks.c`.
- **Logging:** `esp_log.h` macros (`log_e`, `log_w`, `log_i`, etc.) with colorized console output; configurable `LOG_LEVEL`.
- **Host-native build:** Single static library `ArduinoNativeMocks` that you can link into unit tests or example apps.

**Repo Layout**
- `src/`: All public headers and implementations (see `docs/` for details).
- `examples/`: Small, runnable programs showing queue/semaphore/task usage (no gtest/gmock).
- `test/`: GoogleTest/GoogleMock-based unit tests.
- `makefiles/`: Dev targets for build/test/format.
- `CMakeLists.txt`: Library + examples + test wiring.

**Quickstart**
- Prereqs: CMake 3.15+, a C++11 compiler (Clang/GCC/MSVC), make.
- Build library and examples:
  - `make build`
  - Run examples: `make examples` (or execute the binaries under `build/examples/`).
- Run tests (uses GoogleTest/GoogleMock via FetchContent):
  - `make test`  (first run will fetch googletest; internet required)
  - Or explicitly: `cmake -DENABLE_GTEST_TESTS=ON -B build && cmake --build build && ctest --test-dir build -V`

**Using In Your Project**
- CMake (consume as a subdir):
  - `add_subdirectory(arduino)`
  - `target_link_libraries(your_target PRIVATE ArduinoNativeMocks)`
  - The library exposes `src/` as a PUBLIC include dir, so you can `#include <Arduino.h>` and `<freertos/FreeRTOS.h>`.

**Configuration**
- `LOG_LEVEL` (0..5): sets verbosity for `esp_log.h`.
  - With make: `make build LOG_LEVEL=4`
  - With CMake: `cmake -DLOG_LEVEL=4 -B build`

**Editor Integration (clangd)**
- Run `make setup` to generate `build/compile_commands.json` and copy it to the repo root as `compile_commands.json`.
- If clangd still needs help when opening headers standalone, we can add a `.clangd` fallback with `-Isrc` on request.

**Limitations**
- FreeRTOS API is a small, pragmatic subset implemented with `std::thread` and `std::condition_variable`; it’s designed for host-side testing, not real-time guarantees.
- `HardwareSerial` is a minimal stub for examples; use `MockStream` (in tests) for behavior verification.

For a detailed API/module tour, see `docs/README.md`.
