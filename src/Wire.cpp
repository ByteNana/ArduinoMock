#include "Wire.h"

TwoWire::TwoWire(uint8_t bus) : _bus(bus) {}

bool TwoWire::begin(int /*sda*/, int /*scl*/, uint32_t freq) {
  if (freq > 0) setClock(freq);
  return true;
}

void TwoWire::end() {}

void TwoWire::setClock(uint32_t /*freq*/) {}

void TwoWire::beginTransmission(uint8_t /*addr*/) {}

uint8_t TwoWire::endTransmission(bool /*stop*/) { return 0; }

uint8_t TwoWire::requestFrom(uint8_t /*addr*/, uint8_t count) {
  uint8_t avail = static_cast<uint8_t>(available());
  return avail < count ? avail : count;
}

size_t TwoWire::write(uint8_t b) {
  _written.push_back(b);
  return 1;
}

size_t TwoWire::write(const uint8_t* buf, size_t n) {
  for (size_t i = 0; i < n; ++i) { _written.push_back(buf[i]); }
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
}

TwoWire Wire(0);
TwoWire Wire1(1);
