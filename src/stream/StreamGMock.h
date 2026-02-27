#pragma once

#include <gmock/gmock.h>

#include "MockStream.h"

class StreamGMock : public MockStream {
 public:
  MOCK_METHOD(int, available, (), (override));
  MOCK_METHOD(int, read, (), (override));
  MOCK_METHOD(size_t, write, (uint8_t), (override));
  MOCK_METHOD(size_t, write, (const uint8_t*, size_t), (override));
  MOCK_METHOD(void, flush, (), (override));
  MOCK_METHOD(int, peek, (), (override));

  void SetupDefaults();
};
