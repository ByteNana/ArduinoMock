# idf-mock

A pure-C native simulation layer for ESP-IDF firmware, focused on FreeRTOS.
Lets you compile and test ESP-IDF code on your development machine — no hardware, no toolchain, no flash cycle.

## What it mocks

| Header | API surface |
|---|---|
| `freertos/task.h` | `xTaskCreate`, `vTaskDelete`, `vTaskDelay`, `vTaskDelayUntil`, `xTaskGetTickCount` |
| `freertos/queue.h` | `xQueueCreate/Delete`, `xQueueSend/SendToFront/Receive`, `uxQueueMessagesWaiting`, `uxQueueSpacesAvailable`, `xQueueReset` |
| `freertos/semphr.h` | binary, mutex and counting semaphores; `xSemaphoreTake/Give/GiveFromISR`, `uxSemaphoreGetCount` |
| `freertos/timers.h` | `xTimerCreate/Start/Stop/Reset/Delete/IsTimerActive/ChangePeriod`, `pvTimerGetTimerID`, `pcTimerGetName` |
| `esp_err.h` | `esp_err_t`, `ESP_OK/FAIL/ERR_*`, `ESP_ERROR_CHECK`, `esp_err_to_name` |
| `esp_log.h` | `ESP_LOG{E,W,I,D,V}`, `esp_log_set_level` |

## How it works

Each mock is backed by real POSIX primitives so concurrency behaviour matches the target:

- **Tasks** → `pthread_create` per task; `vTaskDelay` blocks via `pthread_cond_timedwait` and exits the thread cleanly on cancellation
- **Queues** → circular buffer with two condition variables (`not_full` / `not_empty`); blocking send/receive with `portMAX_DELAY` support
- **Semaphores** → `pthread_mutex_t + pthread_cond_t`; deadline-based blocking for all three semaphore types
- **Timers** → `CLOCK_MONOTONIC` fire timestamp; `xTimerIsTimerActive` fires the callback and deactivates one-shots on expiry

### Test helpers (timers)

```c
mockTimerProcess();          // fire all active timers immediately
mockTimerFire(handle);       // fire one specific timer
size_t n = mockTimerCount(); // number of live timers
mockTimerClear();            // delete all timers (use in tearDown)
```

## Prerequisites

- CMake ≥ 3.15
- A C11 compiler (GCC or Clang)
- pthreads (standard on Linux and macOS)
- [`just`](https://just.systems) — task runner
- `clang-format` and `clang-tidy` (optional, for format/lint tasks)
- Internet access on first build (CMake fetches [Unity](https://github.com/ThrowTheSwitch/Unity) automatically)

## Quick start

```bash
git clone https://github.com/ByteNana/ArduinoMock.git
cd ArduinoMock
git checkout idf-mock

just test
```

## Available tasks

```
just build            # Configure and build the library + test binaries
just test             # Build and run all test suites
just test-suite <name># Run a single suite: just test-suite test_queue
just format           # Format all C source and header files in-place
just format-check     # Check formatting (exits non-zero on violations)
just lint             # Run clang-tidy and report issues
just lint-check       # Run clang-tidy, exit non-zero on any warning
just clean            # Remove the build directory
```

### Run without just

```bash
# Configure
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build --parallel

# Run all tests
ctest --test-dir build --output-on-failure

# Run a single suite (verbose Unity output)
./build/test/test_task
./build/test/test_queue
./build/test/test_semphr
./build/test/test_timers
```

Expected output:

```
Test project /path/to/build
    Start 1: test_task
1/4 Test #1: test_task ........................   Passed    0.10 sec
    Start 2: test_queue
2/4 Test #2: test_queue .......................   Passed    0.02 sec
    Start 3: test_semphr
3/4 Test #3: test_semphr ......................   Passed    0.03 sec
    Start 4: test_timers
4/4 Test #4: test_timers ......................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 4
```

## Using the mock in your own code

Add the `include/` directory to your include path and link against `idf_mock` and `pthread`:

```cmake
add_subdirectory(path/to/idf-mock)          # or FetchContent

add_executable(my_firmware_test test_main.c)
target_link_libraries(my_firmware_test PRIVATE idf_mock pthread)
```

Then include headers exactly as you would on the ESP32:

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
```

## Project layout

```
idf-mock/
├── .just/
│   ├── build.just
│   ├── test.just
│   ├── lint.just
│   └── format.just
├── .clang-format
├── .clang-tidy
├── justfile
├── include/
│   ├── freertos/
│   │   ├── FreeRTOS.h
│   │   ├── projdefs.h
│   │   ├── task.h
│   │   ├── queue.h
│   │   ├── semphr.h
│   │   └── timers.h
│   ├── esp_err.h
│   └── esp_log.h
├── src/
│   ├── task.c
│   ├── queue.c
│   ├── semphr.c
│   ├── timers.c
│   └── esp_log.c
└── test/
    ├── test_task.c    (9 tests)
    ├── test_queue.c   (12 tests)
    ├── test_semphr.c  (13 tests)
    └── test_timers.c  (17 tests)
```
