#include <gtest/gtest.h>

#include "HTTPClient.h"
#include "WiFiClient.h"

class HTTPClientTest : public ::testing::Test {
 protected:
  void SetUp() override { HTTPClient::reset(); }
};

// --- Defaults after reset ---

TEST_F(HTTPClientTest, GetReturnsOkByDefault) {
  HTTPClient http;
  EXPECT_EQ(http.GET(), HTTP_CODE_OK);
}

TEST_F(HTTPClientTest, PostReturnsOkByDefault) {
  HTTPClient http;
  EXPECT_EQ(http.POST("payload"), HTTP_CODE_OK);
}

TEST_F(HTTPClientTest, GetSizeReturnsMinusOneByDefault) {
  HTTPClient http;
  EXPECT_EQ(http.getSize(), -1);
}

TEST_F(HTTPClientTest, EndCallCountStartsAtZero) { EXPECT_EQ(HTTPClient::endCallCount(), 0); }

TEST_F(HTTPClientTest, GetCallCountStartsAtZero) { EXPECT_EQ(HTTPClient::getCallCount(), 0); }

TEST_F(HTTPClientTest, PostCallCountStartsAtZero) { EXPECT_EQ(HTTPClient::postCallCount(), 0); }

TEST_F(HTTPClientTest, LastPostPayloadIsEmptyAfterReset) {
  EXPECT_EQ(HTTPClient::lastPostPayload(), String(""));
}

// --- begin() variants ---

TEST_F(HTTPClientTest, BeginWithWiFiClientReturnsTrue) {
  HTTPClient http;
  WiFiClient client;
  EXPECT_TRUE(http.begin(client, "http://example.com/ota"));
}

TEST_F(HTTPClientTest, BeginWithStringReturnsTrue) {
  HTTPClient http;
  EXPECT_TRUE(http.begin(String("http://example.com/ota")));
}

TEST_F(HTTPClientTest, AddHeaderDoesNotCrash) {
  HTTPClient http;
  http.addHeader("Content-Type", "application/json");
}

// --- Test configuration ---

TEST_F(HTTPClientTest, SetNextStatusCodeAffectsGet) {
  HTTPClient::setNextStatusCode(HTTP_CODE_NOT_FOUND);
  HTTPClient http;
  EXPECT_EQ(http.GET(), HTTP_CODE_NOT_FOUND);
}

TEST_F(HTTPClientTest, SetNextStatusCodeAffectsPost) {
  HTTPClient::setNextStatusCode(HTTP_CODE_INTERNAL_SERVER_ERROR);
  HTTPClient http;
  EXPECT_EQ(http.POST("data"), HTTP_CODE_INTERNAL_SERVER_ERROR);
}

TEST_F(HTTPClientTest, SetNextContentLength) {
  HTTPClient::setNextContentLength(1024);
  HTTPClient http;
  EXPECT_EQ(http.getSize(), 1024);
}

// --- Observability ---

TEST_F(HTTPClientTest, GetCallCountIncrements) {
  HTTPClient http;
  http.GET();
  http.GET();
  EXPECT_EQ(HTTPClient::getCallCount(), 2);
}

TEST_F(HTTPClientTest, PostCallCountIncrements) {
  HTTPClient http;
  http.POST("a");
  http.POST("b");
  EXPECT_EQ(HTTPClient::postCallCount(), 2);
}

TEST_F(HTTPClientTest, EndCallCountIncrements) {
  HTTPClient http;
  http.end();
  http.end();
  EXPECT_EQ(HTTPClient::endCallCount(), 2);
}

TEST_F(HTTPClientTest, LastPostPayloadTracksLastCall) {
  HTTPClient http;
  http.POST("first");
  http.POST("second");
  EXPECT_EQ(HTTPClient::lastPostPayload(), String("second"));
}

TEST_F(HTTPClientTest, ResetClearsAllState) {
  HTTPClient::setNextStatusCode(HTTP_CODE_NOT_FOUND);
  HTTPClient::setNextContentLength(512);
  HTTPClient http;
  http.GET();
  http.POST("x");
  http.end();

  HTTPClient::reset();

  EXPECT_EQ(HTTPClient::getCallCount(), 0);
  EXPECT_EQ(HTTPClient::postCallCount(), 0);
  EXPECT_EQ(HTTPClient::endCallCount(), 0);
  EXPECT_EQ(HTTPClient::lastPostPayload(), String(""));
  EXPECT_EQ(http.GET(), HTTP_CODE_OK);
  EXPECT_EQ(http.getSize(), -1);
}

// --- Scenario: non-200 triggers error callback ---

TEST_F(HTTPClientTest, Http404ScenarioObservable) {
  HTTPClient::setNextStatusCode(HTTP_CODE_NOT_FOUND);

  HTTPClient http;
  WiFiClient client;
  http.begin(client, "http://example.com/firmware.bin");
  int code = http.GET();
  http.end();

  EXPECT_EQ(code, HTTP_CODE_NOT_FOUND);
  EXPECT_EQ(HTTPClient::endCallCount(), 1);
}

TEST_F(HTTPClientTest, SuccessfulRequestScenario) {
  HTTPClient::setNextStatusCode(HTTP_CODE_OK);
  HTTPClient::setNextContentLength(2048);

  HTTPClient http;
  http.begin(String("http://example.com/firmware.bin"));
  int code = http.GET();
  int size = http.getSize();
  http.end();

  EXPECT_EQ(code, HTTP_CODE_OK);
  EXPECT_EQ(size, 2048);
  EXPECT_EQ(HTTPClient::endCallCount(), 1);
  EXPECT_EQ(HTTPClient::getCallCount(), 1);
}
