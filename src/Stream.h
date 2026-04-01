#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

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

  size_t print(unsigned long val, int base) {
    char buf[64];
    int n = (base == 16)  ? snprintf(buf, sizeof(buf), "%lX", val)
            : (base == 8) ? snprintf(buf, sizeof(buf), "%lo", val)
                          : snprintf(buf, sizeof(buf), "%lu", val);
    return write(reinterpret_cast<const uint8_t*>(buf), n > 0 ? static_cast<size_t>(n) : 0);
  }
  size_t print(long val, int base) {
    if (val < 0 && base == 10) {
      size_t n = print("-");
      return n + print(static_cast<unsigned long>(-val), base);
    }
    return print(static_cast<unsigned long>(val), base);
  }
  size_t print(int val, int base) { return print(static_cast<long>(val), base); }
  size_t print(unsigned int val, int base) { return print(static_cast<unsigned long>(val), base); }

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
