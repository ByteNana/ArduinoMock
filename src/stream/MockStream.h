#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>

#include "../Stream.h"

class MockStream : public Stream {
 private:
  std::queue<uint8_t> rxBuffer;
  std::queue<uint8_t> txBuffer;
  mutable std::mutex rxMutex;
  mutable std::mutex txMutex;
  std::condition_variable dataAvailable;

 public:
  int available() override;
  int read() override;
  size_t write(uint8_t) override;
  size_t write(const uint8_t* buffer, size_t size) override;
  void flush() override;
  int peek() override;

  void InjectRxData(const std::string& data);
  std::string GetTxData();
  void ClearTxData();
  void ClearRxData();
  operator bool() const { return true; }
};
