#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <initializer_list>
#include <vector>

class TwoWire {
 public:
  explicit TwoWire(uint8_t bus = 0);

  bool begin(int sda = -1, int scl = -1, uint32_t freq = 0);
  void end();
  void setClock(uint32_t freq);

  void beginTransmission(uint8_t addr);
  uint8_t endTransmission(bool stop = true);
  uint8_t requestFrom(uint8_t addr, uint8_t count);
  uint8_t requestFrom(uint8_t addr, uint8_t count, bool stop) {
    (void)stop;
    return requestFrom(addr, count);
  }
  uint8_t requestFrom(uint8_t addr, size_t count) {
    return requestFrom(addr, static_cast<uint8_t>(count));
  }
  uint8_t requestFrom(uint8_t addr, size_t count, bool stop) {
    (void)stop;
    return requestFrom(addr, static_cast<uint8_t>(count));
  }

  size_t write(uint8_t b);
  size_t write(const uint8_t* buf, size_t n);

  int available();
  int read();
  int peek() const;

  // --- mock helpers ---

  /// Push bytes that subsequent read() calls will return.
  void mockQueueRead(std::initializer_list<uint8_t> bytes);

  /// Returns all bytes written via write() since last mockReset().
  const std::vector<uint8_t>& mockGetWritten() const;

  /// Clears both the read queue and the written buffer.
  void mockReset();

 private:
  uint8_t _bus;
  std::deque<uint8_t> _readQueue;
  std::vector<uint8_t> _written;
};

extern TwoWire Wire;
extern TwoWire Wire1;
