#include "StreamGMock.h"

void StreamGMock::SetupDefaults() {
  ON_CALL(*this, available()).WillByDefault([this]() { return MockStream::available(); });

  ON_CALL(*this, read()).WillByDefault([this]() { return MockStream::read(); });

  ON_CALL(*this, write(testing::An<uint8_t>())).WillByDefault([this](uint8_t c) {
    return MockStream::write(c);
  });

  ON_CALL(*this, write(testing::_, testing::_))
      .WillByDefault(
          [this](const uint8_t* buffer, size_t size) { return MockStream::write(buffer, size); });

  ON_CALL(*this, flush()).WillByDefault([this]() { MockStream::flush(); });

  ON_CALL(*this, peek()).WillByDefault([this]() { return MockStream::peek(); });
}
