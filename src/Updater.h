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
  bool begin(size_t size = 0, int command = U_FLASH) {
    (void)size;
    (void)command;
    return false;
  }
  bool end(bool evenIfRemaining = false) {
    (void)evenIfRemaining;
    return false;
  }
  size_t write(uint8_t* data, size_t len) {
    (void)data;
    return 0;
  }
  size_t writeStream(Stream& stream) {
    (void)stream;
    return 0;
  }
  bool isRunning() { return false; }
  bool isFinished() { return false; }
  bool hasError() { return false; }
  uint8_t getError() { return 0; }
  void clearError() {}
  const char* errorString() { return ""; }
  size_t size() { return 0; }
  size_t progress() { return 0; }
  size_t remaining() { return 0; }
  void abort() {}
  void printError(void*) {}
};

extern UpdateClass Update;
