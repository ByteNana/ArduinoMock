#pragma once

#include <cctype>

#include "Print.h"
#include "times.h"

class Stream : public Print {
 protected:
  unsigned long _startMillis;
  unsigned long _timeout;

 public:
  Stream(unsigned long timeout = 1000) : _startMillis(0), _timeout(timeout) {}
  ~Stream() override = default;

  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;

  int timedRead() {
    int c;
    _startMillis = millis();
    do {
      c = read();
      if (c >= 0) { return c; }
    } while (millis() - _startMillis < _timeout);
    return -1;
  }

  String readString() {
    String result;
    int c;
    while ((c = timedRead()) >= 0) result += static_cast<char>(c);
    return result;
  }

  String readStringUntil(char terminator) {
    String result;
    int c;
    while ((c = timedRead()) >= 0) {
      if (static_cast<char>(c) == terminator) break;
      result += static_cast<char>(c);
    }
    return result;
  }

  void setTimeout(unsigned long timeout) { _timeout = timeout; }
  unsigned long getTimeout() const { return _timeout; }

  size_t readBytes(char* buffer, size_t length) {
    size_t count = 0;
    while (count < length) {
      int c = timedRead();
      if (c < 0) break;
      buffer[count++] = static_cast<char>(c);
    }
    return count;
  }

  size_t readBytesUntil(char terminator, char* buffer, size_t length) {
    size_t count = 0;
    while (count < length) {
      int c = timedRead();
      if (c < 0 || static_cast<char>(c) == terminator) break;
      buffer[count++] = static_cast<char>(c);
    }
    return count;
  }

  long parseInt() {
    bool negative = false;
    long value = 0;
    int c;
    while ((c = timedRead()) >= 0 && (isdigit(c) == 0) && c != '-') {}
    if (c == '-') {
      negative = true;
      c = timedRead();
    }
    while (c >= 0 && (isdigit(c) != 0)) {
      value = value * 10 + (c - '0');
      c = timedRead();
    }
    return negative ? -value : value;
  }

  float parseFloat() {
    bool negative = false;
    float value = 0.0f;
    float fraction = 1.0f;
    bool inFraction = false;
    int c;
    while ((c = timedRead()) >= 0 && (isdigit(c) == 0) && c != '-' && c != '.') {}
    if (c == '-') {
      negative = true;
      c = timedRead();
    }
    while (c >= 0 && ((isdigit(c) != 0) || c == '.')) {
      if (c == '.') {
        inFraction = true;
      } else if (!inFraction) {
        value = value * 10.0f + static_cast<float>(c - '0');
      } else {
        fraction *= 0.1f;
        value += static_cast<float>(c - '0') * fraction;
      }
      c = timedRead();
    }
    return negative ? -value : value;
  }
};
