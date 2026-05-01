#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

#include "WString.h"

class __FlashStringHelper;

class Print {
 public:
  virtual ~Print() = default;

  // Core write — subclasses must implement write(uint8_t).
  // write(const uint8_t*, size_t) has a default loop implementation.
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t* buffer, size_t size);
  size_t write(const char* str);
  size_t write(const char* buffer, size_t size);

  virtual int availableForWrite() { return 0; }
  virtual void flush() {}

  // print overloads
  size_t print(const char* str);
  size_t print(char c);
  size_t print(unsigned char val, int base = 10);
  size_t print(int val, int base = 10);
  size_t print(unsigned int val, int base = 10);
  size_t print(long val, int base = 10);
  size_t print(unsigned long val, int base = 10);
  size_t print(long long val, int base = 10);
  size_t print(unsigned long long val, int base = 10);
  size_t print(double val, int digits = 2);
  size_t print(const String& str);
  size_t print(const __FlashStringHelper* str);

  // println overloads
  size_t println();
  size_t println(const char* str);
  size_t println(char c);
  size_t println(unsigned char val, int base = 10);
  size_t println(int val, int base = 10);
  size_t println(unsigned int val, int base = 10);
  size_t println(long val, int base = 10);
  size_t println(unsigned long val, int base = 10);
  size_t println(long long val, int base = 10);
  size_t println(unsigned long long val, int base = 10);
  size_t println(double val, int digits = 2);
  size_t println(const String& str);
  size_t println(const __FlashStringHelper* str);

#ifdef __GNUC__
  size_t printf(const char* format, ...) __attribute__((format(printf, 2, 3)));
#else
  size_t printf(const char* format, ...);
#endif

 private:
  size_t printNumber(unsigned long val, int base);
  size_t printNumber(unsigned long long val, int base);
  size_t printFloat(double val, int digits);
};
