#include "Print.h"

#include <array>
#include <cstdio>
#include <cstring>

// --- write ---

size_t Print::write(const uint8_t* buffer, size_t size) {
  size_t n = 0;
  while ((size--) != 0u) n += write(*buffer++);
  return n;
}

size_t Print::write(const char* str) {
  if (str == nullptr) return 0;
  return write(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

size_t Print::write(const char* buffer, size_t size) {
  return write(reinterpret_cast<const uint8_t*>(buffer), size);
}

// --- private helpers ---

size_t Print::printNumber(unsigned long val, int base) {
  std::array<char, 66> buf{};
  int n = 0;
  if (base == 2) {
    std::array<char, 64> tmp{};
    int idx = 0;
    unsigned long v = val;
    if (v == 0) {
      tmp[idx++] = '0';
    } else {
      while (v != 0 && idx < static_cast<int>(tmp.size())) {
        tmp[idx++] = ((v & 1UL) != 0u) ? '1' : '0';
        v >>= 1;
      }
    }
    for (int i = 0; i < idx; ++i) buf[i] = tmp[idx - 1 - i];
    n = idx;
  } else {
    n = (base == 16)  ? snprintf(buf.data(), buf.size(), "%lX", val)
        : (base == 8) ? snprintf(buf.data(), buf.size(), "%lo", val)
                      : snprintf(buf.data(), buf.size(), "%lu", val);
  }
  return write(reinterpret_cast<const uint8_t*>(buf.data()), n > 0 ? static_cast<size_t>(n) : 0);
}

size_t Print::printNumber(unsigned long long val, int base) {
  std::array<char, 66> buf{};
  int n = 0;
  if (base == 2) {
    std::array<char, 64> tmp{};
    int idx = 0;
    unsigned long long v = val;
    if (v == 0) {
      tmp[idx++] = '0';
    } else {
      while (v != 0 && idx < static_cast<int>(tmp.size())) {
        tmp[idx++] = ((v & 1ULL) != 0u) ? '1' : '0';
        v >>= 1;
      }
    }
    for (int i = 0; i < idx; ++i) buf[i] = tmp[idx - 1 - i];
    n = idx;
  } else {
    n = (base == 16)  ? snprintf(buf.data(), buf.size(), "%llX", val)
        : (base == 8) ? snprintf(buf.data(), buf.size(), "%llo", val)
                      : snprintf(buf.data(), buf.size(), "%llu", val);
  }
  return write(reinterpret_cast<const uint8_t*>(buf.data()), n > 0 ? static_cast<size_t>(n) : 0);
}

size_t Print::printFloat(double val, int digits) {
  std::array<char, 64> buf{};
  int n = snprintf(buf.data(), buf.size(), "%.*f", digits, val);
  return write(reinterpret_cast<const uint8_t*>(buf.data()), n > 0 ? static_cast<size_t>(n) : 0);
}

// --- print ---

size_t Print::print(const char* str) { return write(str); }

size_t Print::print(char c) { return write(static_cast<uint8_t>(c)); }

size_t Print::print(unsigned char val, int base) {
  return printNumber(static_cast<unsigned long>(val), base);
}

size_t Print::print(int val, int base) {
  if (val < 0 && base == 10) {
    size_t n = write('-');
    unsigned long magnitude = static_cast<unsigned long>(-(val + 1)) + 1UL;
    return n + printNumber(magnitude, base);
  }
  return printNumber(static_cast<unsigned long>(val), base);
}

size_t Print::print(unsigned int val, int base) {
  return printNumber(static_cast<unsigned long>(val), base);
}

size_t Print::print(long val, int base) {
  if (val < 0 && base == 10) {
    size_t n = write('-');
    unsigned long magnitude = static_cast<unsigned long>(-(val + 1)) + 1UL;
    return n + printNumber(magnitude, base);
  }
  return printNumber(static_cast<unsigned long>(val), base);
}

size_t Print::print(unsigned long val, int base) { return printNumber(val, base); }

size_t Print::print(long long val, int base) {
  if (val < 0 && base == 10) {
    size_t n = write('-');
    unsigned long long magnitude = static_cast<unsigned long long>(-(val + 1)) + 1ULL;
    return n + printNumber(magnitude, base);
  }
  return printNumber(static_cast<unsigned long long>(val), base);
}

size_t Print::print(unsigned long long val, int base) { return printNumber(val, base); }

size_t Print::print(double val, int digits) { return printFloat(val, digits); }

size_t Print::print(const String& str) {
  return write(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
}

size_t Print::print(const __FlashStringHelper* str) {
  return write(reinterpret_cast<const char*>(str));
}

// --- println ---

size_t Print::println() {
  size_t n = write('\r');
  n += write('\n');
  return n;
}

size_t Print::println(const char* str) { return print(str) + println(); }
size_t Print::println(char c) { return print(c) + println(); }
size_t Print::println(unsigned char val, int base) { return print(val, base) + println(); }
size_t Print::println(int val, int base) { return print(val, base) + println(); }
size_t Print::println(unsigned int val, int base) { return print(val, base) + println(); }
size_t Print::println(long val, int base) { return print(val, base) + println(); }
size_t Print::println(unsigned long val, int base) { return print(val, base) + println(); }
size_t Print::println(long long val, int base) { return print(val, base) + println(); }
size_t Print::println(unsigned long long val, int base) { return print(val, base) + println(); }
size_t Print::println(double val, int digits) { return print(val, digits) + println(); }
size_t Print::println(const String& str) { return print(str) + println(); }
size_t Print::println(const __FlashStringHelper* str) { return print(str) + println(); }

// --- printf ---

size_t Print::printf(const char* format, ...) {
  std::array<char, 256> buf{};
  va_list args;
  va_start(args, format);
  int n = vsnprintf(buf.data(), buf.size(), format, args);
  va_end(args);
  if (n <= 0) return 0;
  size_t writeLen = (n < static_cast<int>(buf.size())) ? static_cast<size_t>(n)
                                                       : static_cast<size_t>(buf.size() - 1);
  write(reinterpret_cast<const uint8_t*>(buf.data()), writeLen);
  return static_cast<size_t>(n);
}
