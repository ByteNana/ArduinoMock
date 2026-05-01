#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <thread>

#ifndef ARDUINO
#define ARDUINO 10813
#endif

// Arduino type definitions
using boolean = bool;
using byte = uint8_t;
using word = uint16_t;

#ifndef PROGMEM
#define PROGMEM
#endif

#include <cstring>

static inline uint8_t pgm_read_byte_impl(const void* addr) {
  uint8_t v;
  std::memcpy(&v, addr, sizeof(v));
  return v;
}
static inline uint16_t pgm_read_word_impl(const void* addr) {
  uint16_t v;
  std::memcpy(&v, addr, sizeof(v));
  return v;
}
#ifndef pgm_read_byte
#define pgm_read_byte(addr) ::pgm_read_byte_impl(reinterpret_cast<const void*>(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) ::pgm_read_word_impl(reinterpret_cast<const void*>(addr))
#endif

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

// GPIO pin modes and levels
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif

#ifndef LOW
#define LOW 0
#endif
#ifndef HIGH
#define HIGH 1
#endif

// Interrupt trigger modes
#ifndef CHANGE
#define CHANGE 2
#endif
#ifndef RISING
#define RISING 3
#endif
#ifndef FALLING
#define FALLING 4
#endif

#include <functional>
#include <unordered_map>

namespace mock {

struct IsrEntry {
  std::function<void()> callback;
  uint8_t mode;
};

inline uint8_t* pin_state() {
  static uint8_t s[256] = {};
  return s;
}

inline std::unordered_map<uint8_t, IsrEntry>& isr_table() {
  static std::unordered_map<uint8_t, IsrEntry> t;
  return t;
}

}  // namespace mock

inline void pinMode(uint8_t /*pin*/, uint8_t /*mode*/) {}
inline void digitalWrite(uint8_t pin, uint8_t val) { mock::pin_state()[pin] = val; }
inline int digitalRead(uint8_t pin) { return mock::pin_state()[pin]; }
inline int analogRead(uint8_t /*pin*/) { return 0; }
inline void analogWrite(uint8_t /*pin*/, int /*value*/) {}

inline void attachInterrupt(uint8_t pin, std::function<void()> isr, uint8_t mode) {
  mock::isr_table()[pin] = {std::move(isr), mode};
}
inline void detachInterrupt(uint8_t pin) { mock::isr_table().erase(pin); }
inline uint8_t digitalPinToInterrupt(uint8_t pin) { return pin; }

/// Set a pin's logic level and fire any registered ISR according to its mode.
/// Use this from tests or a simulator to inject physical pin events.
inline void mockSetPinState(uint8_t pin, uint8_t value) {
  uint8_t prev = mock::pin_state()[pin];
  mock::pin_state()[pin] = value;

  auto it = mock::isr_table().find(pin);
  if (it == mock::isr_table().end()) return;

  const auto& entry = it->second;
  bool fire = false;
  switch (entry.mode) {
    case CHANGE:
      fire = (prev != value);
      break;
    case RISING:
      fire = (prev == LOW && value == HIGH);
      break;
    case FALLING:
      fire = (prev == HIGH && value == LOW);
      break;
    case LOW:
      fire = (value == LOW);
      break;
    default:
      break;
  }
  if (fire && entry.callback) entry.callback();
}

/// Reset all pin states and clear the interrupt table. Call in SetUp().
inline void mockResetGpio() {
  std::fill(mock::pin_state(), mock::pin_state() + 256, 0);
  mock::isr_table().clear();
}

inline void tone(uint8_t /*pin*/, unsigned int /*frequency*/, unsigned long /*duration*/ = 0) {}
inline void noTone(uint8_t /*pin*/) {}
inline void setToneChannel(uint8_t /*channel*/ = 0) {}
inline void yield() { std::this_thread::yield(); }

#include "Esp.h"
#include "HardwareSerial.h"
#include "esp32-hal-log.h"
// Arduino math macros
#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef TWO_PI
#define TWO_PI 6.28318530717958647692
#endif
#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.29577951308232
#endif

#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)
#define sq(x) ((x) * (x))
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Flash-string helpers — no-ops on native
#ifndef PSTR
#define PSTR(s) (s)
#endif
#ifndef F
#define F(s) (reinterpret_cast<const __FlashStringHelper*>(PSTR(s)))
#endif

#include "Print.h"
#include "Stream.h"
#include "TimeLib.h"
#include "WString.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "times.h"

inline bool isSpace(char c) { return isspace(static_cast<unsigned char>(c)) != 0; }

inline bool isHexadecimalDigit(char c) {
  return isdigit(static_cast<unsigned char>(c)) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

template <typename T, typename U>
inline constexpr auto min(T a, U b) -> typename std::common_type<T, U>::type {
  using R = typename std::common_type<T, U>::type;
  return (a < b) ? R(a) : R(b);
}

template <typename T, typename U>
inline constexpr auto max(T a, U b) -> typename std::common_type<T, U>::type {
  using R = typename std::common_type<T, U>::type;
  return (a > b) ? R(a) : R(b);
}

inline bool isDigit(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }

inline bool isAlpha(char c) { return isalpha(static_cast<unsigned char>(c)) != 0; }

inline bool isAlphaNumeric(char c) { return isalnum(static_cast<unsigned char>(c)) != 0; }

inline char* dtostrf(double val, signed char width, unsigned char prec, char* buf) {
  // 64 bytes matches the Arduino reference implementation and is sufficient
  // for any double with reasonable width/precision values.
  constexpr size_t kBufMax = 64;
  std::snprintf(buf, kBufMax, "%*.*f", static_cast<int>(width), static_cast<int>(prec), val);
  return buf;
}

inline void randomSeed(unsigned long seed) {
  static std::mt19937 generator;
  generator.seed(seed);
}

inline long random(long max) {
  static std::mt19937 generator(millis());  // Seed with current time
  std::uniform_int_distribution<long> distribution(0, max - 1);
  return distribution(generator);
}

inline long random(long min, long max) {
  static std::mt19937 generator(millis());  // Seed with current time
  std::uniform_int_distribution<long> distribution(min, max - 1);
  return distribution(generator);
}
