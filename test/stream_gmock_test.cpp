#include <Arduino.h>

#ifdef ARDUINOMOCK_USE_GMOCK
#include <gmock/gmock.h>
#endif

#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

TEST(StreamGMockTest, PrintAndPrintlnInvokeWrites) {
  MockStream s;
  s.SetupDefaults();

  EXPECT_CALL(s, write(_, _)).Times(1).WillOnce(Invoke([](const uint8_t* buf, size_t size) {
    return size;
  }));
  EXPECT_EQ(s.print(String("hi")), 2u);

  EXPECT_CALL(s, write(_, _)).Times(1).WillOnce(Invoke([](const uint8_t* buf, size_t size) {
    return size;
  }));
  EXPECT_CALL(s, write(::testing::A<uint8_t>())).Times(2).WillRepeatedly(Return(1));
  EXPECT_EQ(s.println(String("ok")), 4u);
}
