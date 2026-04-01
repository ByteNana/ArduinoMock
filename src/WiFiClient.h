#pragma once

#include "Client.h"
#include "IPAddress.h"

class WiFiClient : public Client {
 public:
  int connect(IPAddress ip, uint16_t port) override;
  int connect(const char *host, uint16_t port) override;
  bool connect(IPAddress ip, uint16_t port, int timeout);

  size_t write(uint8_t) override;
  size_t write(const uint8_t *buf, size_t size) override;
  int available() override;
  int read() override;
  int read(uint8_t *buf, size_t size) override;
  int peek() override;
  void flush() override;
  void stop() override;
  uint8_t connected() override;
  operator bool() override;

  // Test helper — static so tests can configure before instantiation
  static void setCanConnect(bool v) { _canConnect = v; }

 private:
  static bool _canConnect;
  bool _connected = false;
};
