#pragma once

#include "WiFiClient.h"

class WiFiClientSecure : public WiFiClient {
 public:
  void setInsecure() {}
  void setCACert(const char *) {}
  void setCertificate(const char *) {}
  void setPrivateKey(const char *) {}
};
