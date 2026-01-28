#ifdef ARDUINOMOCK_USE_GMOCK

#include "Stream.h"

void MockStream::SetupDefaults() {
  ON_CALL(*this, available()).WillByDefault([this]() {
    std::lock_guard<std::mutex> lock(rxMutex);
    return static_cast<int>(rxBuffer.size());
  });

  ON_CALL(*this, read()).WillByDefault([this]() {
    std::lock_guard<std::mutex> lock(rxMutex);
    if (rxBuffer.empty()) return -1;
    int c = rxBuffer.front();
    rxBuffer.pop();
    return c;
  });

  ON_CALL(*this, write(testing::_)).WillByDefault([this](uint8_t c) {
    std::lock_guard<std::mutex> lock(txMutex);
    txBuffer.push(c);
    return 1;
  });

  ON_CALL(*this, write(testing::_, testing::_))
      .WillByDefault([this](const uint8_t* buffer, size_t size) {
        std::lock_guard<std::mutex> lock(txMutex);
        for (size_t i = 0; i < size; i++) { txBuffer.push(buffer[i]); }
        return size;
      });

  // Add default action for flush() if not already present
  ON_CALL(*this, flush()).WillByDefault([]() { /* No-op for mock */ });
}

void MockStream::InjectRxData(const std::string& data) {
  std::lock_guard<std::mutex> lock(rxMutex);
  for (char c : data) { rxBuffer.push(static_cast<uint8_t>(c)); }
  dataAvailable.notify_all();  // Notify any waiting readers
}

std::string MockStream::GetTxData() {
  std::lock_guard<std::mutex> lock(txMutex);
  std::string result;
  while (!txBuffer.empty()) {
    result += static_cast<char>(txBuffer.front());
    txBuffer.pop();
  }
  return result;
}

void MockStream::ClearTxData() {
  std::lock_guard<std::mutex> lock(txMutex);
  std::queue<uint8_t> emptyQueue;
  std::swap(txBuffer, emptyQueue);
}

void MockStream::ClearRxData() {
  std::lock_guard<std::mutex> lock(rxMutex);
  std::queue<uint8_t> emptyQueue;
  std::swap(rxBuffer, emptyQueue);
}

#endif
