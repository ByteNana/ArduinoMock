#pragma once

#include <cinttypes>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "IPAddress.h"

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
  bool begin(const char* ssid, const char* password);
  void disconnect();

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
  std::vector<MockScanResult> _scanResults;
  bool _dnsSuccess = true;
  bool _beginConnects = true;
  std::string _ssid;
};

extern WiFiClass WiFi;
