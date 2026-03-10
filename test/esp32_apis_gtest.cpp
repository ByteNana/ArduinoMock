#include <gtest/gtest.h>

#include "EspClass.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "freertos/FreeRTOS.h"

// --- wl_status_t enum values match ESP32 SDK ---

TEST(WiFiEnumTest, StatusValuesMatchESP32) {
  EXPECT_EQ(WL_IDLE_STATUS, 0);
  EXPECT_EQ(WL_NO_SSID_AVAIL, 1);
  EXPECT_EQ(WL_SCAN_COMPLETED, 2);
  EXPECT_EQ(WL_CONNECTED, 3);
  EXPECT_EQ(WL_CONNECT_FAILED, 4);
  EXPECT_EQ(WL_CONNECTION_LOST, 5);
  EXPECT_EQ(WL_DISCONNECTED, 6);
}

// --- WiFiEventSysCb typedef exists ---

TEST(WiFiEnumTest, WiFiEventSysCbTypeExists) {
  WiFiEventSysCb cb = [](int) {};
  cb(0);  // should compile and run
}

// --- WiFiClient::setCanConnect is static ---

TEST(WiFiClientStaticTest, SetCanConnectIsStatic) {
  WiFiClient::setCanConnect(false);
  WiFiClient client;
  EXPECT_FALSE(client.connect(IPAddress(1, 2, 3, 4), 80));
  WiFiClient::setCanConnect(true);
  EXPECT_TRUE(client.connect(IPAddress(1, 2, 3, 4), 80));
}

// --- FreeRTOS Task Notifications ---

TEST(TaskNotificationTest, XTaskNotifyReturnsPdPASS) {
  TaskHandle_t th = nullptr;
  // xTaskNotify with nullptr handle — just test it compiles and returns pdPASS
  EXPECT_EQ(xTaskNotify(th, 1, eSetValueWithOverwrite), pdPASS);
}

TEST(TaskNotificationTest, UlTaskNotifyTakeReturnsZero) {
  EXPECT_EQ(ulTaskNotifyTake(pdTRUE, 0), 0u);
}

TEST(TaskNotificationTest, ENotifyActionEnumValues) {
  // Verify all enum values exist
  EXPECT_EQ(eNoAction, 0);
  EXPECT_EQ(eSetBits, 1);
  EXPECT_EQ(eIncrement, 2);
  EXPECT_EQ(eSetValueWithOverwrite, 3);
  EXPECT_EQ(eSetValueWithoutOverwrite, 4);
}

// --- ESP System ---

TEST(EspClassTest, GetFlashChipSpeed) {
  EXPECT_EQ(ESP.getFlashChipSpeed(), static_cast<uint32_t>(FLASH_CHIP_SPEED));
}

TEST(EspClassTest, RtcDataAttrDefined) {
  RTC_DATA_ATTR int x = 42;
  EXPECT_EQ(x, 42);
}
