#pragma once
#include "WString.h"
#include "WiFiClient.h"

#define HTTP_CODE_OK 200
#define HTTP_CODE_NOT_FOUND 404
#define HTTP_CODE_INTERNAL_SERVER_ERROR 500

class HTTPClient {
 public:
  bool begin(WiFiClient& client, const char* url);
  bool begin(const String& url);
  int GET();
  int POST(const String& payload);
  int getSize();
  void end();
  void addHeader(const String& name, const String& value);

  // Test configuration
  static void setNextStatusCode(int code) { _statusCode = code; }
  static void setNextContentLength(int len) { _contentLength = len; }
  static void reset() {
    _statusCode = HTTP_CODE_OK;
    _contentLength = -1;
    _endCallCount = 0;
    _getCallCount = 0;
    _postCallCount = 0;
    _lastPostPayload = "";
  }

  // Test observability
  static int endCallCount() { return _endCallCount; }
  static int getCallCount() { return _getCallCount; }
  static int postCallCount() { return _postCallCount; }
  static String lastPostPayload() { return _lastPostPayload; }

 private:
  static int _statusCode;
  static int _contentLength;
  static int _endCallCount;
  static int _getCallCount;
  static int _postCallCount;
  static String _lastPostPayload;
};
