#pragma once
#include <cstddef>
#include <cstdint>

#include "Stream.h"

#ifndef U_FLASH
#define U_FLASH 0
#endif
#ifndef U_SPIFFS
#define U_SPIFFS 100
#endif

class UpdateClass {
 public:
  bool begin(size_t size = 0, int command = U_FLASH);
  bool end(bool evenIfRemaining = false);
  size_t write(uint8_t* data, size_t len) {
    (void)data;
    return 0;
  }
  size_t writeStream(Stream& stream);
  bool isRunning() { return false; }
  bool isFinished() { return false; }
  bool hasError() { return false; }
  uint8_t getError() { return _error; }
  void clearError() { _error = 0; }
  const char* errorString() { return ""; }
  size_t size() { return 0; }
  size_t progress() { return 0; }
  size_t remaining() { return 0; }
  void abort() {}
  void printError(void*) {}

  // Test configuration
  static void setBeginResult(bool result) { _beginResult = result; }
  static void setWriteStreamResult(size_t n) { _writeResult = n; }
  static void setError(uint8_t e) { _error = e; }
  static void reset() {
    _beginResult = false;
    _writeResult = 0;
    _error = 0;
    _endCallCount = 0;
    _lastEndArg = false;
  }

  // Test observability
  static int endCallCount() { return _endCallCount; }
  static bool lastEndArg() { return _lastEndArg; }

 private:
  static bool _beginResult;
  static size_t _writeResult;
  static uint8_t _error;
  static int _endCallCount;
  static bool _lastEndArg;
};

extern UpdateClass Update;
