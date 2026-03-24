#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <random>
#include <thread>

// Arduino type definitions
typedef bool boolean;
typedef uint8_t byte;
typedef uint16_t word;

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#include "HardwareSerial.h"
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

#include "Stream.h"
#include "WString.h"
#include "times.h"

inline bool isSpace(char c) { return isspace(static_cast<unsigned char>(c)); }

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

inline bool isDigit(char c) { return isdigit(static_cast<unsigned char>(c)); }

inline bool isAlpha(char c) { return isalpha(static_cast<unsigned char>(c)); }

inline bool isAlphaNumeric(char c) { return isalnum(static_cast<unsigned char>(c)); }

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
