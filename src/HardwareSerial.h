#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <queue>
#include <string>

#include "Stream.h"

// ESP32 serial config constants
#ifndef SERIAL_8N1
#define SERIAL_8N1 0x800001cUL
#endif
#ifndef SERIAL_8N2
#define SERIAL_8N2 0x800003cUL
#endif
#ifndef SERIAL_8E1
#define SERIAL_8E1 0x8000016UL
#endif
#ifndef SERIAL_8O1
#define SERIAL_8O1 0x8000017UL
#endif

class HardwareSerial : public Stream {
 public:
  explicit HardwareSerial(int port) : _port(port) {}

  // --- Arduino / ESP32 API ---

  void begin(unsigned long baud) { _baud = baud; }

  void begin(unsigned long baud, uint32_t config) {
    _baud = baud;
    _config = config;
  }

  void begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin) {
    _baud = baud;
    _config = config;
    _rxPin = rxPin;
    _txPin = txPin;
  }

  void end() { reset(); }

  size_t printf(const char* format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    if (len <= 0) return 0;
    return write(reinterpret_cast<const uint8_t*>(buf), static_cast<size_t>(len));
  }

  unsigned long baudRate() const { return _baud; }

  // --- Stream interface ---

  int available() override {
    std::lock_guard<std::mutex> lock(_rxMu);
    return static_cast<int>(_rxBuf.size());
  }

  int read() override {
    std::lock_guard<std::mutex> lock(_rxMu);
    if (_rxBuf.empty()) return -1;
    int c = _rxBuf.front();
    _rxBuf.pop();
    return c;
  }

  int peek() override {
    std::lock_guard<std::mutex> lock(_rxMu);
    if (_rxBuf.empty()) return -1;
    return _rxBuf.front();
  }

  size_t write(uint8_t c) override {
    std::lock_guard<std::mutex> lock(_txMu);
    _txBuf.push(c);
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    std::lock_guard<std::mutex> lock(_txMu);
    for (size_t i = 0; i < size; i++) _txBuf.push(buffer[i]);
    return size;
  }

  void flush() override {
    std::lock_guard<std::mutex> lock(_txMu);
    while (!_txBuf.empty()) _txBuf.pop();
  }

  // --- Test helpers ---

  void injectRxData(const std::string& data) {
    std::lock_guard<std::mutex> lock(_rxMu);
    for (unsigned char c : data) _rxBuf.push(c);
  }

  std::string getTxData() {
    std::lock_guard<std::mutex> lock(_txMu);
    std::string result;
    while (!_txBuf.empty()) {
      result += static_cast<char>(_txBuf.front());
      _txBuf.pop();
    }
    return result;
  }

  void clearBuffers() {
    std::lock_guard<std::mutex> rxLock(_rxMu);
    std::lock_guard<std::mutex> txLock(_txMu);
    std::queue<uint8_t>().swap(_rxBuf);
    std::queue<uint8_t>().swap(_txBuf);
  }

  void reset() {
    clearBuffers();
    _baud = 0;
    _config = SERIAL_8N1;
    _rxPin = -1;
    _txPin = -1;
  }

 private:
  int _port;
  unsigned long _baud = 0;
  uint32_t _config = SERIAL_8N1;
  int8_t _rxPin = -1;
  int8_t _txPin = -1;

  std::queue<uint8_t> _rxBuf;
  std::queue<uint8_t> _txBuf;
  std::mutex _rxMu;
  std::mutex _txMu;
};

extern HardwareSerial Serial;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;
