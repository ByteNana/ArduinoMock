#pragma once

#include <cstddef>
#include <cstdint>

#include "WString.h"
#include "times.h"

class Stream {
 protected:
  unsigned long _startMillis;
  unsigned long _timeout;

 public:
  Stream(unsigned long timeout = 1000) : _startMillis(0), _timeout(timeout) {}
  virtual ~Stream() = default;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t* buffer, size_t size) = 0;
  virtual void flush() = 0;
  virtual int peek() = 0;
  virtual size_t print(int value) { return print(String(value)); }
  virtual size_t print(uint16_t value) { return print(String(value)); }

  virtual size_t println() { return println(""); }  // Empty line
  virtual size_t println(int value) { return println(String(value)); }

  int timedRead() {
    int c;
    _startMillis = millis();
    do {
      c = read();
      if (c >= 0) { return c; }
    } while (millis() - _startMillis < _timeout);
    return -1;
  }

  size_t print(const String& str) {
    return write(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
  }

  size_t println(const String& str) {
    size_t n = print(str);
    n += write('\r');
    n += write('\n');
    return n;
  }
};

class HardwareSerial : public Stream {
 public:
  HardwareSerial(int indexA) {}
  void begin(unsigned long baud) {}
  void println(const String& str) {}
  void print(const String& str) {}
  size_t write(uint8_t c) { return 1; }
  size_t write(const uint8_t* buffer, size_t size) { return size; }
  void flush() {}
  int available() { return 0; }
  int read() { return -1; }
  int peek() { return -1; }
};

extern HardwareSerial Serial;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;
