#pragma once

#include "Print.h"
#include "times.h"

class Stream : public Print {
 protected:
  unsigned long _startMillis;
  unsigned long _timeout;

 public:
  Stream(unsigned long timeout = 1000) : _startMillis(0), _timeout(timeout) {}
  virtual ~Stream() = default;

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
};
