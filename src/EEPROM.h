#pragma once

#include <array>
#include <cstdint>
#include <cstring>

#ifndef MOCK_EEPROM_SIZE
#define MOCK_EEPROM_SIZE 4096
#endif

class EEPROMClass {
 public:
  EEPROMClass() { _buffer.fill(0xFF); }

  void begin(size_t size) { _size = size; }
  uint8_t read(int address) { return _buffer[address]; }
  void write(int address, uint8_t value) { _buffer[address] = value; }
  bool commit() { return true; }

  template <typename T>
  T& get(int address, T& data) {
    memcpy(&data, _buffer.data() + address, sizeof(T));
    return data;
  }

  template <typename T>
  void put(int address, const T& data) {
    memcpy(_buffer.data() + address, &data, sizeof(T));
  }

  size_t length() { return _size; }

  // Test helpers
  void reset() {
    _buffer.fill(0xFF);
    _size = MOCK_EEPROM_SIZE;
  }
  uint8_t* raw() { return _buffer.data(); }
  size_t size() { return _size; }

 private:
  std::array<uint8_t, MOCK_EEPROM_SIZE> _buffer;
  size_t _size = MOCK_EEPROM_SIZE;
};

extern EEPROMClass EEPROM;
