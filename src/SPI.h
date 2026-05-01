#pragma once
#include <cstdint>

#ifndef SPI_MODE0
#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3
#endif

#ifndef MSBFIRST
#define MSBFIRST 1
#define LSBFIRST 0
#endif

class SPISettings {
 public:
  SPISettings() {}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {}
};

class SPIClass {
 public:
  void begin() {}
  void begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss = -1) {}
  void end() {}

  void beginTransaction(SPISettings settings) {}
  void endTransaction() {}

  uint8_t transfer(uint8_t data) { return 0; }
  uint16_t transfer16(uint16_t data) { return 0; }
  uint32_t transfer32(uint32_t data) { return 0; }
  void transferBytes(const uint8_t* txbuf, uint8_t* rxbuf, uint32_t count) {}

  void setClockDivider(uint8_t) {}
  void setBitOrder(uint8_t) {}
  void setDataMode(uint8_t) {}
  void setFrequency(uint32_t) {}
  void setHwCs(bool) {}
};

extern SPIClass SPI;
