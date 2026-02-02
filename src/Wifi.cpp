#include "Wifi.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>

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

WiFiClass WiFi;