#include <gtest/gtest.h>

#include "Arduino.h"

class GpioTest : public ::testing::Test {
 protected:
  void SetUp() override { mockResetGpio(); }
};

// --- digitalWrite / digitalRead state persistence -------------------------

TEST_F(GpioTest, DigitalWriteRead_Roundtrip) {
  digitalWrite(5, HIGH);
  EXPECT_EQ(HIGH, digitalRead(5));
}

TEST_F(GpioTest, DigitalRead_DefaultLow) { EXPECT_EQ(LOW, digitalRead(42)); }

TEST_F(GpioTest, DigitalWrite_MultiplePinsIndependent) {
  digitalWrite(1, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH);
  EXPECT_EQ(HIGH, digitalRead(1));
  EXPECT_EQ(LOW, digitalRead(2));
  EXPECT_EQ(HIGH, digitalRead(3));
}

TEST_F(GpioTest, DigitalWrite_OverwritesPreviousValue) {
  digitalWrite(7, HIGH);
  digitalWrite(7, LOW);
  EXPECT_EQ(LOW, digitalRead(7));
}

// --- mockResetGpio --------------------------------------------------------

TEST_F(GpioTest, MockResetGpio_ClearsPinState) {
  digitalWrite(10, HIGH);
  mockResetGpio();
  EXPECT_EQ(LOW, digitalRead(10));
}

// --- attachInterrupt / detachInterrupt ------------------------------------

TEST_F(GpioTest, AttachInterrupt_RegistersCallback) {
  int fired = 0;
  attachInterrupt(4, [&fired]() { fired++; }, RISING);
  mockSetPinState(4, HIGH);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, DetachInterrupt_StopsFiring) {
  int fired = 0;
  attachInterrupt(4, [&fired]() { fired++; }, RISING);
  detachInterrupt(4);
  mockSetPinState(4, HIGH);
  EXPECT_EQ(0, fired);
}

TEST_F(GpioTest, DigitalPinToInterrupt_IdentityMapping) {
  EXPECT_EQ(13u, digitalPinToInterrupt(13));
}

// --- mockSetPinState / RISING ---------------------------------------------

TEST_F(GpioTest, Rising_FiresOnLowToHigh) {
  int fired = 0;
  attachInterrupt(2, [&fired]() { fired++; }, RISING);
  mockSetPinState(2, HIGH);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, Rising_DoesNotFireOnHighToLow) {
  int fired = 0;
  attachInterrupt(2, [&fired]() { fired++; }, RISING);
  mockSetPinState(2, HIGH);  // LOW→HIGH: fires
  fired = 0;
  mockSetPinState(2, LOW);  // HIGH→LOW: must not fire
  EXPECT_EQ(0, fired);
}

TEST_F(GpioTest, Rising_DoesNotFireWhenAlreadyHigh) {
  int fired = 0;
  digitalWrite(2, HIGH);
  attachInterrupt(2, [&fired]() { fired++; }, RISING);
  mockSetPinState(2, HIGH);  // HIGH→HIGH: no edge
  EXPECT_EQ(0, fired);
}

// --- mockSetPinState / FALLING --------------------------------------------

TEST_F(GpioTest, Falling_FiresOnHighToLow) {
  int fired = 0;
  digitalWrite(3, HIGH);
  attachInterrupt(3, [&fired]() { fired++; }, FALLING);
  mockSetPinState(3, LOW);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, Falling_DoesNotFireOnLowToHigh) {
  int fired = 0;
  attachInterrupt(3, [&fired]() { fired++; }, FALLING);
  mockSetPinState(3, HIGH);  // LOW→HIGH: must not fire
  EXPECT_EQ(0, fired);
}

// --- mockSetPinState / CHANGE ---------------------------------------------

TEST_F(GpioTest, Change_FiresOnLowToHigh) {
  int fired = 0;
  attachInterrupt(5, [&fired]() { fired++; }, CHANGE);
  mockSetPinState(5, HIGH);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, Change_FiresOnHighToLow) {
  int fired = 0;
  digitalWrite(5, HIGH);
  attachInterrupt(5, [&fired]() { fired++; }, CHANGE);
  mockSetPinState(5, LOW);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, Change_DoesNotFireWhenValueUnchanged) {
  int fired = 0;
  attachInterrupt(5, [&fired]() { fired++; }, CHANGE);
  mockSetPinState(5, LOW);  // LOW→LOW: no change
  EXPECT_EQ(0, fired);
}

// --- mockSetPinState / LOW ------------------------------------------------

TEST_F(GpioTest, LowMode_FiresWhenPinIsLow) {
  int fired = 0;
  attachInterrupt(6, [&fired]() { fired++; }, LOW);
  mockSetPinState(6, LOW);
  EXPECT_EQ(1, fired);
}

TEST_F(GpioTest, LowMode_DoesNotFireWhenPinIsHigh) {
  int fired = 0;
  attachInterrupt(6, [&fired]() { fired++; }, LOW);
  mockSetPinState(6, HIGH);
  EXPECT_EQ(0, fired);
}

// --- mockSetPinState updates pin state ------------------------------------

TEST_F(GpioTest, MockSetPinState_UpdatesReadableState) {
  mockSetPinState(9, HIGH);
  EXPECT_EQ(HIGH, digitalRead(9));
  mockSetPinState(9, LOW);
  EXPECT_EQ(LOW, digitalRead(9));
}

// --- multiple pins are independent ----------------------------------------

TEST_F(GpioTest, MultipleInterrupts_IndependentFiring) {
  int fired_a = 0, fired_b = 0;
  attachInterrupt(10, [&fired_a]() { fired_a++; }, RISING);
  attachInterrupt(11, [&fired_b]() { fired_b++; }, RISING);
  mockSetPinState(10, HIGH);
  EXPECT_EQ(1, fired_a);
  EXPECT_EQ(0, fired_b);
  mockSetPinState(11, HIGH);
  EXPECT_EQ(1, fired_a);
  EXPECT_EQ(1, fired_b);
}

// --- mockResetGpio clears isr_table ---------------------------------------

TEST_F(GpioTest, MockResetGpio_ClearsIsrTable) {
  int fired = 0;
  attachInterrupt(12, [&fired]() { fired++; }, RISING);
  mockResetGpio();
  mockSetPinState(12, HIGH);
  EXPECT_EQ(0, fired);
}
