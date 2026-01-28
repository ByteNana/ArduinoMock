#include "Wifi.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>

#include <cstring>

int32_t WiFiClass::RSSI() {
  const char* iface = "wlan0";
  char cmd[128];
  std::snprintf(cmd, sizeof(cmd), "iw dev %s link | grep signal:", iface);

  FILE* pipe = popen(cmd, "r");
  if (!pipe) return -100;

  char buffer[128];
  int rssi = -100;

  if (fgets(buffer, sizeof(buffer), pipe)) {
    char* p = std::strstr(buffer, "signal:");
    if (p) {
      rssi = std::atoi(p + strlen("signal:"));  // skips string "signal:"
    }
  }

  pclose(pipe);
  return rssi;
}

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
  struct ifaddrs* ifaddr = nullptr;

  if (getifaddrs(&ifaddr) == -1) return String();

  String result;

  for (auto* ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr) continue;

    if (!(ifa->ifa_flags & IFF_UP)) continue;

    if (ifa->ifa_flags & IFF_LOOPBACK) continue;

    if (ifa->ifa_addr->sa_family != AF_PACKET) continue;

    auto* sa = reinterpret_cast<sockaddr_ll*>(ifa->ifa_addr);

    if (sa->sll_halen != 6) continue;

    char buf[18];
    std::snprintf(
        buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", sa->sll_addr[0], sa->sll_addr[1],
        sa->sll_addr[2], sa->sll_addr[3], sa->sll_addr[4], sa->sll_addr[5]);

    result = buf;
    break;
  }

  freeifaddrs(ifaddr);
  return result;
}

WiFiClass WiFi;