#include "WiFi.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

#include <cstring>

int32_t WiFiClass::RSSI() { return _rssi; }

int32_t WiFiClass::RSSI(int index) {
  if (index >= 0 && static_cast<size_t>(index) < _scanResults.size()) {
    return _scanResults[index].rssi;
  }
  return 0;
}

wl_status_t WiFiClass::status() { return _status; }

bool WiFiClass::isConnected() { return _status == WL_CONNECTED; }

IPAddress WiFiClass::localIP() {
  struct ifaddrs* ifaddr = nullptr;

  if (getifaddrs(&ifaddr) == -1) return IPAddress();  // 0.0.0.0

  IPAddress result;

  for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) continue;

    if (ifa->ifa_addr->sa_family != AF_INET) continue;

    if (!(ifa->ifa_flags & IFF_UP)) continue;

    if (ifa->ifa_flags & IFF_LOOPBACK) continue;

    auto* sa = reinterpret_cast<sockaddr_in*>(ifa->ifa_addr);
    uint32_t addr = ntohl(sa->sin_addr.s_addr);

    result = IPAddress((addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, addr & 0xFF);
    break;
  }

  freeifaddrs(ifaddr);
  return result;
}

String WiFiClass::macAddress() {
  FILE* pipe = nullptr;
#if defined(__APPLE__)
  pipe = popen("ifconfig en0 | awk '/ether/{print toupper($2)}'", "r");
#elif defined(__linux__)
  pipe = popen(
      "cat /sys/class/net/$(ip route show default | awk '/default/{print $5}')/address | tr 'a-f' "
      "'A-F'",
      "r");
#endif
  if (!pipe) return String("00:00:00:00:00:00");

  char buf[18] = {0};
  fgets(buf, sizeof(buf), pipe);
  pclose(pipe);

  // Trim newline
  buf[strcspn(buf, "\n")] = 0;
  return String(buf);
}

bool WiFiClass::begin(const char* ssid, const char* password) {
  _ssid = ssid ? ssid : "";
  if (_beginConnects) {
    _status = WL_CONNECTED;
    return true;
  }
  _status = WL_CONNECT_FAILED;
  return false;
}

void WiFiClass::disconnect() { _status = WL_DISCONNECTED; }

void WiFiClass::disconnect(bool, bool) { disconnect(); }

wl_status_t WiFiClass::waitForConnectResult(unsigned long) { return _status; }

int WiFiClass::scanNetworks(bool, bool) { return static_cast<int>(_scanResults.size()); }

String WiFiClass::SSID(int index) {
  if (index >= 0 && static_cast<size_t>(index) < _scanResults.size()) {
    return String(_scanResults[index].ssid.c_str());
  }
  return String("");
}

String WiFiClass::SSID() { return String(_ssid.c_str()); }

int WiFiClass::hostByName(const char*, IPAddress& result) {
  if (_dnsSuccess) {
    result = IPAddress(8, 8, 8, 8);
    return 1;
  }
  return 0;
}

IPAddress WiFiClass::gatewayIP() { return IPAddress(192, 168, 1, 1); }
IPAddress WiFiClass::subnetMask() { return IPAddress(255, 255, 255, 0); }
IPAddress WiFiClass::dnsIP() { return IPAddress(8, 8, 8, 8); }

void WiFiClass::setAutoReconnect(bool) {}
void WiFiClass::setAutoConnect(bool) {}
void WiFiClass::onEvent(std::function<void(int)>) {}

void WiFiClass::reset() {
  _rssi = -50;
  _status = WL_CONNECTED;
  _scanResults.clear();
  _dnsSuccess = true;
  _beginConnects = true;
  _ssid.clear();
}

WiFiClass WiFi;
