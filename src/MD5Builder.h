#pragma once

#include <Arduino.h>

#if defined(__APPLE__)
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <CommonCrypto/CommonDigest.h>
class MD5Builder {
 public:
  void begin() {
    CC_MD5_Init(&ctx);
    finalized = false;
  }
  void add(uint8_t *data, size_t len) {
    if (!finalized && data && len) CC_MD5_Update(&ctx, data, (CC_LONG)len);
  }
  void calculate() {
    if (!finalized) {
      CC_MD5_Final(digest, &ctx);
      finalized = true;
    }
  }
  String toString() {
    if (!finalized) calculate();
    static const char *hex = "0123456789abcdef";
    String out;
    out.reserve(32);
    for (int i = 0; i < 16; ++i) {
      out += hex[digest[i] >> 4];
      out += hex[digest[i] & 0xF];
    }
    return out;
  }

 private:
  CC_MD5_CTX ctx{};
  uint8_t digest[CC_MD5_DIGEST_LENGTH]{};
  bool finalized = false;
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#elif defined(HAVE_OPENSSL_MD5)
// Suppress deprecation warnings for OpenSSL 3.x MD5_* APIs; usage is intentional here.
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <openssl/md5.h>
class MD5Builder {
 public:
  void begin() {
    MD5_Init(&ctx);
    finalized = false;
  }
  void add(uint8_t *data, size_t len) {
    if (!finalized && data && len) MD5_Update(&ctx, data, len);
  }
  void calculate() {
    if (!finalized) {
      MD5_Final(digest, &ctx);
      finalized = true;
    }
  }
  String toString() {
    if (!finalized) calculate();
    static const char *hex = "0123456789abcdef";
    String out;
    out.reserve(32);
    for (int i = 0; i < 16; ++i) {
      out += hex[digest[i] >> 4];
      out += hex[digest[i] & 0xF];
    }
    return out;
  }

 private:
  MD5_CTX ctx{};
  unsigned char digest[MD5_DIGEST_LENGTH]{};
  bool finalized = false;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#else
// Fallback deterministic hash (not MD5). Used only if no native MD5 is available.
class MD5Builder {
 public:
  MD5Builder() { reset(); }
  void begin() { reset(); }
  void add(uint8_t *data, size_t len) {
    if (!data || len == 0) return;
    for (size_t i = 0; i < len; ++i) {
      uint8_t b = data[i];
      s0 = (s0 + b) * 2654435761u;
      s1 ^= (uint32_t(b) << (i % 24));
      s2 = (s2 ^ (b * 97u)) + 0x9e3779b9u;
      s3 = (s3 + ((b << 5) | (b >> 3))) ^ 0xa5a5a5a5u;
    }
  }
  void calculate() { calculated = true; }
  String toString() {
    if (!calculated) calculate();
    uint8_t digest[16];
    pack32(s0, &digest[0]);
    pack32(s1, &digest[4]);
    pack32(s2, &digest[8]);
    pack32(s3, &digest[12]);
    static const char *hex = "0123456789abcdef";
    String out;
    out.reserve(32);
    for (int i = 0; i < 16; ++i) {
      out += hex[digest[i] >> 4];
      out += hex[digest[i] & 0xF];
    }
    return out;
  }

 private:
  uint32_t s0, s1, s2, s3;
  bool calculated = false;
  void reset() {
    s0 = 0x67452301u;
    s1 = 0xefcdab89u;
    s2 = 0x98badcfeu;
    s3 = 0x10325476u;
    calculated = false;
  }
  static void pack32(uint32_t v, uint8_t *out) {
    out[0] = v & 0xFF;
    out[1] = (v >> 8) & 0xFF;
    out[2] = (v >> 16) & 0xFF;
    out[3] = (v >> 24) & 0xFF;
  }
};
#endif
