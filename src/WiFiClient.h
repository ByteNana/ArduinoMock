#pragma once

#include "IPAddress.h"

class WiFiClient {
 public:
  bool connect(IPAddress ip, uint16_t port, int timeout = 0);
  void stop();
  operator bool() const;

  // Test helper — static so tests can configure before instantiation
  static void setCanConnect(bool v) { _canConnect = v; }

 private:
  static bool _canConnect;
  bool _connected = false;
};
