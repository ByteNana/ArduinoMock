#pragma once

#include <cstdint>

#ifndef ESP_OK
#define ESP_OK 0
#endif

#ifndef WIFI_IF_STA
#define WIFI_IF_STA 0
#endif
#ifndef WIFI_IF_AP
#define WIFI_IF_AP 1
#endif

typedef int wifi_interface_t;

typedef enum {
  WIFI_SECOND_CHAN_NONE = 0,
  WIFI_SECOND_CHAN_ABOVE,
  WIFI_SECOND_CHAN_BELOW,
} wifi_second_chan_t;

typedef int esp_err_t;

inline esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t* mac) {
  (void)ifx;
  if (mac) {
    mac[0] = 0xDE;
    mac[1] = 0xAD;
    mac[2] = 0xBE;
    mac[3] = 0xEF;
    mac[4] = 0x00;
    mac[5] = 0x01;
  }
  return ESP_OK;
}

inline esp_err_t esp_wifi_get_channel(uint8_t* primary, wifi_second_chan_t* second) {
  if (primary) *primary = 1;
  if (second) *second = WIFI_SECOND_CHAN_NONE;
  return ESP_OK;
}

inline esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t* mac) {
  (void)ifx;
  (void)mac;
  return ESP_OK;
}
