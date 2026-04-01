#include <gtest/gtest.h>

#include "Arduino.h"

TEST(ToneTest, ToneDefaultDuration) { EXPECT_NO_THROW(tone(5, 440)); }

TEST(ToneTest, ToneWithDuration) { EXPECT_NO_THROW(tone(5, 440, 1000)); }

TEST(ToneTest, NoTone) { EXPECT_NO_THROW(noTone(5)); }

TEST(ToneTest, SetToneChannelWithArg) { EXPECT_NO_THROW(setToneChannel(2)); }

TEST(ToneTest, SetToneChannelDefaultArg) { EXPECT_NO_THROW(setToneChannel()); }
