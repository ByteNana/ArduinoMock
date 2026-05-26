#include "HTTPClient.h"

int HTTPClient::_statusCode = HTTP_CODE_OK;
int HTTPClient::_contentLength = -1;
int HTTPClient::_endCallCount = 0;
int HTTPClient::_getCallCount = 0;
int HTTPClient::_postCallCount = 0;
String HTTPClient::_lastPostPayload = "";

bool HTTPClient::begin(WiFiClient& client, const char* url) {
  (void)client;
  (void)url;
  return true;
}

bool HTTPClient::begin(const String& url) {
  (void)url;
  return true;
}

int HTTPClient::GET() {
  _getCallCount++;
  return _statusCode;
}

int HTTPClient::POST(const String& payload) {
  _postCallCount++;
  _lastPostPayload = payload;
  return _statusCode;
}

int HTTPClient::getSize() { return _contentLength; }

void HTTPClient::end() { _endCallCount++; }

void HTTPClient::addHeader(const String& name, const String& value) {
  (void)name;
  (void)value;
}
