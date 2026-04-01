#pragma once

#include <cinttypes>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "IPAddress.h"

// WiFi mode — matches ESP32 Arduino Core wifi_mode_t values
typedef enum {
  WIFI_OFF = 0,
  WIFI_STA = 1,
  WIFI_AP = 2,
  WIFI_AP_STA = 3,
} wifi_mode_t;

// Values match ESP32 Arduino Core wifi_sta_status_t
typedef enum {
  WL_IDLE_STATUS = 0,
  WL_NO_SSID_AVAIL = 1,
  WL_SCAN_COMPLETED = 2,
  WL_CONNECTED = 3,
  WL_CONNECT_FAILED = 4,
  WL_CONNECTION_LOST = 5,
  WL_DISCONNECTED = 6
} wl_status_t;

typedef std::function<void(int)> WiFiEventSysCb;

struct MockScanResult {
  std::string ssid;
  int32_t rssi;
};

class WiFiClass {
 public:
  int32_t RSSI();
  wl_status_t status();
  bool isConnected();
  IPAddress localIP();
  String macAddress();
  void macAddress(uint8_t* mac);
  bool begin(const char* ssid, const char* password);
  // No-arg begin — reconnect using saved credentials (stub: returns _beginConnects)
  bool begin() { return _beginConnects; }

  // Soft AP
  bool softAP(const char*, const char* = nullptr, int = 1, int = 0, int = 4) { return true; }
  bool softAPConfig(IPAddress, IPAddress, IPAddress) { return true; }
  bool softAPdisconnect(bool = false) { return true; }
  uint8_t softAPgetStationNum() { return 0; }
  IPAddress softAPIP() { return IPAddress(192, 168, 3, 3); }

  // AP/STA enable
  bool enableAP(bool) { return true; }
  bool enableSTA(bool) { return true; }

  // Credential/reconnect helpers
  String psk() { return String(""); }
  bool getAutoConnect() { return true; }
  bool getAutoReconnect() { return true; }
  void mode(wifi_mode_t m) { _mode = m; }
  wifi_mode_t getMode() const { return _mode; }
  void disconnect();
  void disconnect(bool wifiOff) { disconnect(wifiOff, false); }

  // Extended API
  void disconnect(bool wifiOff, bool eraseAP);
  wl_status_t waitForConnectResult(unsigned long timeout = 0);
  int scanNetworks(bool async = false, bool showHidden = false);
  String SSID(int index);
  String SSID();
  int32_t RSSI(int index);
  int hostByName(const char* hostname, IPAddress& result);
  IPAddress gatewayIP();
  IPAddress subnetMask();
  IPAddress dnsIP();
  void setAutoReconnect(bool);
  void setAutoConnect(bool);
  void onEvent(std::function<void(int)> cb);

  // Test helpers
  void setRSSI(int32_t rssi) { _rssi = rssi; }
  void setStatus(wl_status_t status) { _status = status; }
  void setScanResults(const std::vector<MockScanResult>& results) { _scanResults = results; }
  void setDnsResult(bool success) { _dnsSuccess = success; }
  void setBeginConnects(bool connects) { _beginConnects = connects; }
  void reset();

 private:
  int32_t _rssi = -50;
  wl_status_t _status = WL_CONNECTED;
  wifi_mode_t _mode = WIFI_STA;
  std::vector<MockScanResult> _scanResults;
  bool _dnsSuccess = true;
  bool _beginConnects = true;
  std::string _ssid;
};

extern WiFiClass WiFi;
