#include <gtest/gtest.h>

#include "Arduino.h"
#include "esp32-hal-log.h"
#include "esp_wifi.h"
#include "nvs.h"

// --- esp_wifi_get_mac ---

TEST(EspWifiTest, GetMacReturnsOkAndFillsBuffer) {
  uint8_t mac[6] = {};
  EXPECT_EQ(esp_wifi_get_mac(WIFI_IF_STA, mac), ESP_OK);
  EXPECT_EQ(mac[0], 0xDE);
  EXPECT_EQ(mac[1], 0xAD);
  EXPECT_EQ(mac[2], 0xBE);
  EXPECT_EQ(mac[3], 0xEF);
  EXPECT_EQ(mac[4], 0x00);
  EXPECT_EQ(mac[5], 0x01);
}

// --- esp_wifi_get_channel ---

TEST(EspWifiTest, GetChannelReturnsOkAndSetsPrimaryAndSecond) {
  uint8_t primary = 0;
  wifi_second_chan_t second = WIFI_SECOND_CHAN_ABOVE;
  EXPECT_EQ(esp_wifi_get_channel(&primary, &second), ESP_OK);
  EXPECT_EQ(primary, 1);
  EXPECT_EQ(second, WIFI_SECOND_CHAN_NONE);
}

// --- WIFI_IF_STA / WIFI_IF_AP ---

TEST(EspWifiTest, InterfaceConstants) {
  EXPECT_EQ(WIFI_IF_STA, 0);
  EXPECT_EQ(WIFI_IF_AP, 1);
}

// --- NVS_KEY_NAME_MAX_SIZE ---

TEST(NvsTest, KeyNameMaxSize) { EXPECT_EQ(NVS_KEY_NAME_MAX_SIZE, 16); }

// --- Arduino math macros ---

TEST(ArduinoMathTest, PiApproximation) { EXPECT_NEAR(PI, 3.14159, 1e-4); }

TEST(ArduinoMathTest, RadiansOf180EqualsPi) { EXPECT_NEAR(radians(180.0), PI, 1e-9); }

TEST(ArduinoMathTest, DegreesOfPiEquals180) { EXPECT_NEAR(degrees(PI), 180.0, 1e-6); }

TEST(ArduinoMathTest, SqOf4Is16) { EXPECT_EQ(sq(4), 16); }

TEST(ArduinoMathTest, ConstrainClampsHigh) { EXPECT_EQ(constrain(15, 0, 10), 10); }

TEST(ArduinoMathTest, ConstrainClampsLow) { EXPECT_EQ(constrain(-1, 0, 10), 0); }

TEST(ArduinoMathTest, ConstrainPassthrough) { EXPECT_EQ(constrain(5, 0, 10), 5); }

TEST(ArduinoMathTest, MapScalesValue) { EXPECT_EQ(map(50, 0, 100, 0, 200), 100); }

// --- esp32-hal-log.h compiles (inclusion verified at top of file) ---

TEST(Esp32HalLogTest, HeaderCompiles) {
  // Including esp32-hal-log.h at the top of this file is sufficient to
  // verify that it compiles correctly.
  SUCCEED();
}
