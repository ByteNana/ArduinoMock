#pragma once
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <initializer_list>
#include <map>
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

  /// Clears both the read queue and the written buffer. Attached devices stay
  /// attached (their state belongs to the test that owns them).
  void mockReset();

  // --- addressed device hook ---
  //
  // A test can attach a device model at a 7-bit address. Once attached:
  //   * endTransmission() hands the device every byte written since
  //     beginTransmission(addr) (`onReceive`, like an Arduino slave);
  //   * requestFrom(addr, n) asks the device for n bytes (`onRequest`) and
  //     queues what it returns for read()/available().
  // Buses/addresses without a device keep the plain FIFO behaviour, so
  // existing mockQueueRead() tests are unaffected. write() still records into
  // mockGetWritten() for inspection.
  using OnReceive = std::function<void(const std::vector<uint8_t>& bytes)>;
  using OnRequest = std::function<std::vector<uint8_t>(size_t count)>;

  void mockAttachDevice(uint8_t addr, OnReceive onReceive, OnRequest onRequest);
  void mockDetachDevice(uint8_t addr);
  void mockDetachAllDevices();
  bool mockHasDevice(uint8_t addr) const;

 private:
  struct Device {
    OnReceive onReceive;
    OnRequest onRequest;
  };

  uint8_t _bus;
  std::deque<uint8_t> _readQueue;
  std::vector<uint8_t> _written;
  std::map<uint8_t, Device> _devices;
  uint8_t _txAddr = 0;
  bool _txOpen = false;
  std::vector<uint8_t> _txBuf;
};

extern TwoWire Wire;
extern TwoWire Wire1;
