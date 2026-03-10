#include "WiFiClient.h"

bool WiFiClient::connect(IPAddress, uint16_t, int) {
  _connected = _canConnect;
  return _connected;
}

void WiFiClient::stop() { _connected = false; }

WiFiClient::operator bool() const { return _connected; }
