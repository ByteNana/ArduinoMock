#include <cinttypes>
#include <cstring>

#include "IPAddress.h"

typedef enum {
  WL_IDLE_STATUS,
  WL_NO_SSID_AVAIL,
  WL_CONNECTED,
  WL_CONNECT_FAILED,
  WL_DISCONNECTED
} wl_status_t;

class WiFiClass {
 public:
  int32_t RSSI();
  wl_status_t status();
  bool isConnected();
  IPAddress localIP();
  String macAddress();
  bool begin(const char* ssid, const char* password) {
    _status = WL_CONNECTED;
    return true;
  }
  void disconnect() { _status = WL_DISCONNECTED; }

  // test helpers
  void setRSSI(int32_t rssi) { _rssi = rssi; }
  void setStatus(wl_status_t status) { _status = status; }

 private:
  int32_t _rssi = -50;
  wl_status_t _status = WL_CONNECTED;
};

extern WiFiClass WiFi;