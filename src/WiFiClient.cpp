#include "WiFiClient.h"

bool WiFiClient::_canConnect = true;

int WiFiClient::connect(IPAddress ip, uint16_t port) { return connect(ip, port, 0) ? 1 : 0; }

int WiFiClient::connect(const char *, uint16_t) {
  _connected = _canConnect;
  return _connected ? 1 : 0;
}

bool WiFiClient::connect(IPAddress, uint16_t, int) {
  _connected = _canConnect;
  return _connected;
}

size_t WiFiClient::write(uint8_t) { return 0; }

size_t WiFiClient::write(const uint8_t *, size_t) { return 0; }

int WiFiClient::available() { return 0; }

int WiFiClient::read() { return -1; }

int WiFiClient::read(uint8_t *, size_t) { return 0; }

int WiFiClient::peek() { return -1; }

void WiFiClient::flush() {}

void WiFiClient::stop() { _connected = false; }

uint8_t WiFiClient::connected() { return _connected ? 1 : 0; }

WiFiClient::operator bool() { return _connected; }
