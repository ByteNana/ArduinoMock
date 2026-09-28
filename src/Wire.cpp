#include "Wire.h"

TwoWire::TwoWire(uint8_t bus) : _bus(bus) {}

bool TwoWire::begin(int /*sda*/, int /*scl*/, uint32_t freq) {
  if (freq > 0) setClock(freq);
  return true;
}

void TwoWire::end() {}

void TwoWire::setClock(uint32_t /*freq*/) {}

void TwoWire::beginTransmission(uint8_t addr) {
  _txAddr = addr;
  _txOpen = true;
  _txBuf.clear();
}

uint8_t TwoWire::endTransmission(bool /*stop*/) {
  if (_txOpen) {
    auto it = _devices.find(_txAddr);
    if (it != _devices.end() && it->second.onReceive) { it->second.onReceive(_txBuf); }
  }
  _txOpen = false;
  _txBuf.clear();
  return 0;
}

uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count) {
  auto it = _devices.find(addr);
  if (it != _devices.end() && it->second.onRequest) {
    // A device serves a whole transfer, so bytes left over from an earlier one
    // are gone -- otherwise a read the code under test abandoned would be
    // handed back as the answer to the next request.
    _readQueue.clear();
    std::vector<uint8_t> bytes = it->second.onRequest(count);
    if (bytes.size() > count) { bytes.resize(count); }
    for (uint8_t b : bytes) { _readQueue.push_back(b); }
  }
  uint8_t avail = static_cast<uint8_t>(available());
  return avail < count ? avail : count;
}

size_t TwoWire::write(uint8_t b) {
  _written.push_back(b);
  if (_txOpen) { _txBuf.push_back(b); }
  return 1;
}

size_t TwoWire::write(const uint8_t* buf, size_t n) {
  for (size_t i = 0; i < n; ++i) { write(buf[i]); }
  return n;
}

int TwoWire::available() { return static_cast<int>(_readQueue.size()); }

int TwoWire::read() {
  if (_readQueue.empty()) return -1;
  uint8_t b = _readQueue.front();
  _readQueue.pop_front();
  return b;
}

int TwoWire::peek() const {
  if (_readQueue.empty()) return -1;
  return _readQueue.front();
}

void TwoWire::mockQueueRead(std::initializer_list<uint8_t> bytes) {
  for (uint8_t b : bytes) { _readQueue.push_back(b); }
}

const std::vector<uint8_t>& TwoWire::mockGetWritten() const { return _written; }

void TwoWire::mockReset() {
  _readQueue.clear();
  _written.clear();
  _txBuf.clear();
  _txOpen = false;
}

void TwoWire::mockAttachDevice(uint8_t addr, OnReceive onReceive, OnRequest onRequest) {
  _devices[addr] = Device{std::move(onReceive), std::move(onRequest)};
}

void TwoWire::mockDetachDevice(uint8_t addr) { _devices.erase(addr); }

void TwoWire::mockDetachAllDevices() { _devices.clear(); }

bool TwoWire::mockHasDevice(uint8_t addr) const { return _devices.count(addr) != 0; }

TwoWire Wire(0);
TwoWire Wire1(1);
