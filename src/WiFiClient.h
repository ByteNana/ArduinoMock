#pragma once

#include "IPAddress.h"

class WiFiClient {
 public:
  bool connect(IPAddress ip, uint16_t port, int timeout = 0);
  void stop();
  operator bool() const;

  // Test helper
  void setCanConnect(bool v) { _canConnect = v; }

 private:
  bool _canConnect = true;
  bool _connected = false;
};
