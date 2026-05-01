#pragma once

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <string>
#include <type_traits>

#ifndef DEC
#define DEC 10
#endif
#ifndef HEX
#define HEX 16
#endif
#ifndef OCT
#define OCT 8
#endif
#ifndef BIN
#define BIN 2
#endif

class String {
 private:
  std::string _data;
  mutable size_t _readPos = 0;

 public:
  String() {}
  String(const char* str) : _data(str) {}
  String(const std::string& str) : _data(str) {}
  explicit String(char c) : _data(1, c) {}

  template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
  explicit String(T value) : _data(std::to_string(value)) {}

  String(unsigned long value, unsigned char base) {
    if (base == HEX) {
      char buf[17];
      snprintf(buf, sizeof(buf), "%lx", value);
      _data = buf;
    } else if (base == OCT) {
      char buf[23];
      snprintf(buf, sizeof(buf), "%lo", value);
      _data = buf;
    } else if (base == BIN) {
      if (value == 0) {
        _data = "0";
        return;
      }
      std::string bits;
      unsigned long v = value;
      while (v > 0) {
        bits = static_cast<char>('0' + (v & 1)) + bits;
        v >>= 1;
      }
      _data = bits;
    } else {
      _data = std::to_string(value);
    }
  }

  String& operator=(const char* str) {
    _data = str;
    return *this;
  }

  String& operator+=(const String& str) {
    _data += str._data;
    return *this;
  }

  String& operator+=(const char* str) {
    _data += str;
    return *this;
  }

  String& operator+=(char c) {
    _data += c;
    return *this;
  }

  char& operator[](size_t index) { return _data[index]; }
  char operator[](size_t index) const { return _data[index]; }

  bool operator==(const String& rhs) const { return _data == rhs._data; }
  bool operator!=(const String& rhs) const { return _data != rhs._data; }
  bool operator<(const String& rhs) const { return _data < rhs._data; }
  bool operator>(const String& rhs) const { return _data > rhs._data; }
  bool operator<=(const String& rhs) const { return _data <= rhs._data; }
  bool operator>=(const String& rhs) const { return _data >= rhs._data; }

  bool operator==(const char* rhs) const { return _data == rhs; }
  bool operator!=(const char* rhs) const { return _data != rhs; }

  bool equals(const String& rhs) const { return _data == rhs._data; }
  bool equals(const char* rhs) const { return _data == rhs; }

  bool equalsIgnoreCase(const String& rhs) const {
    if (_data.size() != rhs._data.size()) return false;
    for (size_t i = 0; i < _data.size(); i++) {
      if (tolower(static_cast<unsigned char>(_data[i])) !=
          tolower(static_cast<unsigned char>(rhs._data[i])))
        return false;
    }
    return true;
  }

  const char* c_str() const { return _data.c_str(); }
  size_t length() const { return _data.length(); }
  void clear() {
    _data.clear();
    _readPos = 0;
  }

  char charAt(int index) const {
    if (index >= 0 && static_cast<size_t>(index) < _data.size()) { return _data[index]; }
    return '\0';
  }

  bool startsWith(const String& prefix) const { return _data.find(prefix._data) == 0; }

  bool endsWith(const String& suffix) const {
    return _data.size() >= suffix._data.size() &&
           _data.compare(_data.size() - suffix._data.size(), suffix._data.size(), suffix._data) ==
               0;
  }

  void trim() {
    auto start = _data.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
      _data.clear();
      return;
    }
    auto end = _data.find_last_not_of(" \t\r\n");
    _data = _data.substr(start, end - start + 1);
  }

  int indexOf(char c) const {
    size_t pos = _data.find(c);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int indexOf(const String& str) const {
    size_t pos = _data.find(str._data);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int indexOf(char c, int fromIndex) const {
    if (fromIndex < 0) fromIndex = 0;
    size_t pos = _data.find(c, static_cast<size_t>(fromIndex));
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int indexOf(const String& str, int fromIndex) const {
    if (fromIndex < 0) fromIndex = 0;
    size_t pos = _data.find(str._data, static_cast<size_t>(fromIndex));
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int lastIndexOf(char c) const {
    size_t pos = _data.rfind(c);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  int lastIndexOf(const String& str) const {
    size_t pos = _data.rfind(str._data);
    return (pos == std::string::npos) ? -1 : static_cast<int>(pos);
  }

  String substring(int from) const {
    if (from < 0) from = 0;
    return String(_data.substr(from));
  }

  String substring(int from, int to) const {
    if (from < 0) from = 0;
    if (to < 0) to = 0;
    if (to < from) return String("");
    return String(_data.substr(from, to - from));
  }

  int toInt() const {
    try {
      return std::stoi(_data);
    } catch (...) { return 0; }
  }

  bool reserve(size_t size) {
    try {
      _data.reserve(size);
      return true;
    } catch (...) { return false; }
  }

  bool concat(char c) {
    try {
      _data += c;
      return true;
    } catch (...) { return false; }
  }

  bool concat(const String& str) {
    try {
      _data += str._data;
      return true;
    } catch (...) { return false; }
  }

  bool concat(const char* str) {
    try {
      _data += str;
      return true;
    } catch (...) { return false; }
  }

  void replace(char find, char replace) {
    for (size_t i = 0; i < _data.length(); i++) {
      if (_data[i] == find) { _data[i] = replace; }
    }
  }

  void replace(const String& find, const String& replace) {
    if (find._data.empty()) return;
    size_t pos = 0;
    while ((pos = _data.find(find._data, pos)) != std::string::npos) {
      _data.replace(pos, find._data.length(), replace._data);
      pos += replace._data.length();
    }
  }

  void replace(const char* find, const String& replace) { this->replace(String(find), replace); }

  void replace(const char* find, const char* replace) {
    this->replace(String(find), String(replace));
  }

  void remove(unsigned int index) {
    if (index < _data.length()) { _data.erase(index, 1); }
  }

  void remove(unsigned int index, unsigned int count) {
    if (index < _data.length()) {
      size_t available = _data.length() - index;
      size_t to_remove = (count > available) ? available : count;
      _data.erase(index, to_remove);
    }
  }

  bool isEmpty() const { return _data.empty(); }

  size_t write(uint8_t c) {
    _data += static_cast<char>(c);
    return 1;
  }

  size_t write(const uint8_t* buf, size_t size) {
    _data.append(reinterpret_cast<const char*>(buf), size);
    return size;
  }

  // Stream-like read interface (used by ArduinoJson Reader<String>)
  int read() const {
    if (_readPos >= _data.size()) return -1;
    return static_cast<unsigned char>(_data[_readPos++]);
  }

  int peek() const {
    if (_readPos >= _data.size()) return -1;
    return static_cast<unsigned char>(_data[_readPos]);
  }

  int available() const {
    return (_readPos < _data.size()) ? static_cast<int>(_data.size() - _readPos) : 0;
  }
};

inline bool operator==(const char* lhs, const String& rhs) { return rhs == lhs; }
inline bool operator!=(const char* lhs, const String& rhs) { return rhs != lhs; }

inline String operator+(const String& lhs, const String& rhs) {
  String result(lhs);
  result += rhs;
  return result;
}

inline String operator+(const String& lhs, const char* rhs) {
  String result(lhs);
  result += rhs;
  return result;
}

inline String operator+(const char* lhs, const String& rhs) {
  String result(lhs);
  result += rhs;
  return result;
}
