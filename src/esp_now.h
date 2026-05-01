#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <mutex>

#include "esp_wifi.h"

// ---- Constants ----

#define ESP_NOW_ETH_ALEN 6

#ifndef ESP_ERR_ESPNOW_NOT_INIT
#define ESP_ERR_ESPNOW_NOT_INIT 0x3066
#endif

// ---- Types ----

typedef void (*esp_now_recv_cb_t)(const uint8_t* mac, const uint8_t* data, int len);
typedef void (*esp_now_send_cb_t)(const uint8_t* mac, esp_err_t status);

struct esp_now_peer_info_t {
  uint8_t peer_addr[ESP_NOW_ETH_ALEN];
  uint8_t channel;
  int ifidx;
  bool encrypt;
  uint8_t lmk[16];
};

// ---- Internal mock state ----

namespace _esp_now_mock {
using recv_fn = std::function<void(const uint8_t*, const uint8_t*, int)>;
using send_fn = std::function<void(const uint8_t*, esp_err_t)>;

inline recv_fn& recv_cb() {
  static recv_fn cb;
  return cb;
}
inline send_fn& send_cb() {
  static send_fn cb;
  return cb;
}
inline bool& initialized() {
  static bool v = false;
  return v;
}
inline std::mutex& mu() {
  static std::mutex m;
  return m;
}
}  // namespace _esp_now_mock

// ---- API ----

inline esp_err_t esp_now_init() {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::initialized() = true;
  return ESP_OK;
}

inline esp_err_t esp_now_deinit() {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::initialized() = false;
  _esp_now_mock::recv_cb() = _esp_now_mock::recv_fn{};
  _esp_now_mock::send_cb() = _esp_now_mock::send_fn{};
  return ESP_OK;
}

// Accepts a raw function pointer or capturing lambda (std::function).
inline esp_err_t esp_now_register_recv_cb(_esp_now_mock::recv_fn cb) {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::recv_cb() = cb;
  return ESP_OK;
}

inline esp_err_t esp_now_unregister_recv_cb() {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::recv_cb() = nullptr;
  return ESP_OK;
}

// Accepts a raw function pointer or capturing lambda (std::function).
inline esp_err_t esp_now_register_send_cb(_esp_now_mock::send_fn cb) {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::send_cb() = cb;
  return ESP_OK;
}

inline esp_err_t esp_now_unregister_send_cb() {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::send_cb() = nullptr;
  return ESP_OK;
}

inline esp_err_t esp_now_add_peer(const esp_now_peer_info_t* peer) {
  (void)peer;
  return ESP_OK;
}

inline esp_err_t esp_now_del_peer(const uint8_t* peer_addr) {
  (void)peer_addr;
  return ESP_OK;
}

inline bool esp_now_is_peer_exist(const uint8_t* peer_addr) {
  (void)peer_addr;
  return true;
}

inline esp_err_t esp_now_send(const uint8_t* peer_addr, const uint8_t* data, size_t len) {
  (void)data;
  (void)len;
  _esp_now_mock::send_fn cb;
  {
    std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
    cb = _esp_now_mock::send_cb();
  }
  if (cb) cb(peer_addr, ESP_OK);
  return ESP_OK;
}

inline esp_err_t esp_now_get_version(uint32_t* version) {
  if (version) *version = 1;
  return ESP_OK;
}

inline const char* esp_err_to_name(esp_err_t err) {
  if (err == ESP_OK) return "ESP_OK";
  if (err == ESP_ERR_ESPNOW_NOT_INIT) return "ESP_ERR_ESPNOW_NOT_INIT";
  return "ESP_UNKNOWN";
}

// ---- Test helper ----

// Simulate an incoming ESP-NOW packet by invoking the registered receive callback.
// Thread-safe — can be called from any thread.
inline void esp_now_inject_recv(const uint8_t* mac, const uint8_t* data, int len) {
  _esp_now_mock::recv_fn cb;
  {
    std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
    cb = _esp_now_mock::recv_cb();
  }
  if (cb) cb(mac, data, len);
}

// Reset all mock state (call in test teardown).
inline void esp_now_mock_reset() {
  std::lock_guard<std::mutex> lock(_esp_now_mock::mu());
  _esp_now_mock::recv_cb() = _esp_now_mock::recv_fn{};
  _esp_now_mock::send_cb() = _esp_now_mock::send_fn{};
  _esp_now_mock::initialized() = false;
}
