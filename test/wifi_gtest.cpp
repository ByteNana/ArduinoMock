#include <gtest/gtest.h>

#include "Client.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiClientSecure.h"

class WiFiTest : public ::testing::Test {
 protected:
  void SetUp() override { WiFi.reset(); }
};

TEST_F(WiFiTest, BeginConnectsByDefault) {
  EXPECT_TRUE(WiFi.begin("TestSSID", "password"));
  EXPECT_EQ(WiFi.status(), WL_CONNECTED);
  EXPECT_TRUE(WiFi.isConnected());
}

TEST_F(WiFiTest, BeginRespectsSetBeginConnects) {
  WiFi.setBeginConnects(false);
  EXPECT_FALSE(WiFi.begin("TestSSID", "password"));
  EXPECT_EQ(WiFi.status(), WL_CONNECT_FAILED);
  EXPECT_FALSE(WiFi.isConnected());
}

TEST_F(WiFiTest, DisconnectChangesStatus) {
  WiFi.begin("TestSSID", "password");
  WiFi.disconnect();
  EXPECT_EQ(WiFi.status(), WL_DISCONNECTED);
  EXPECT_FALSE(WiFi.isConnected());
}

TEST_F(WiFiTest, DisconnectWithArgs) {
  WiFi.begin("TestSSID", "password");
  WiFi.disconnect(true, true);
  EXPECT_EQ(WiFi.status(), WL_DISCONNECTED);
}

TEST_F(WiFiTest, DisconnectSingleArgCompiles) {
  EXPECT_NO_THROW(WiFi.disconnect(true));
  EXPECT_NO_THROW(WiFi.disconnect(false));
}

TEST_F(WiFiTest, RSSIReturnsMockValue) {
  EXPECT_EQ(WiFi.RSSI(), -50);
  WiFi.setRSSI(-75);
  EXPECT_EQ(WiFi.RSSI(), -75);
}

TEST_F(WiFiTest, ScanNetworksWithResults) {
  std::vector<MockScanResult> results = {{"Network1", -40}, {"Network2", -80}, {"Hidden", -90}};
  WiFi.setScanResults(results);
  EXPECT_EQ(WiFi.scanNetworks(), 3);
  EXPECT_STREQ(WiFi.SSID(0).c_str(), "Network1");
  EXPECT_STREQ(WiFi.SSID(1).c_str(), "Network2");
  EXPECT_STREQ(WiFi.SSID(2).c_str(), "Hidden");
  EXPECT_EQ(WiFi.RSSI(0), -40);
  EXPECT_EQ(WiFi.RSSI(1), -80);
  EXPECT_EQ(WiFi.RSSI(2), -90);
}

TEST_F(WiFiTest, SSIDReturnsConnectedSSID) {
  WiFi.begin("MyWiFi", "pass");
  EXPECT_STREQ(WiFi.SSID().c_str(), "MyWiFi");
}

TEST_F(WiFiTest, HostByNameSuccess) {
  IPAddress result;
  EXPECT_EQ(WiFi.hostByName("example.com", result), 1);
  EXPECT_EQ(result[0], 8);
  EXPECT_EQ(result[1], 8);
  EXPECT_EQ(result[2], 8);
  EXPECT_EQ(result[3], 8);
}

TEST_F(WiFiTest, HostByNameFailure) {
  WiFi.setDnsResult(false);
  IPAddress result;
  EXPECT_EQ(WiFi.hostByName("example.com", result), 0);
}

TEST_F(WiFiTest, GatewaySubnetDns) {
  IPAddress gw = WiFi.gatewayIP();
  EXPECT_EQ(gw[0], 192);
  EXPECT_EQ(gw[1], 168);
  EXPECT_EQ(gw[2], 1);
  EXPECT_EQ(gw[3], 1);

  IPAddress sm = WiFi.subnetMask();
  EXPECT_EQ(sm[0], 255);
  EXPECT_EQ(sm[1], 255);
  EXPECT_EQ(sm[2], 255);
  EXPECT_EQ(sm[3], 0);

  IPAddress dns = WiFi.dnsIP();
  EXPECT_EQ(dns[0], 8);
  EXPECT_EQ(dns[1], 8);
  EXPECT_EQ(dns[2], 8);
  EXPECT_EQ(dns[3], 8);
}

TEST_F(WiFiTest, WaitForConnectResultReturnsStatus) {
  WiFi.setStatus(WL_DISCONNECTED);
  EXPECT_EQ(WiFi.waitForConnectResult(), WL_DISCONNECTED);
}

TEST_F(WiFiTest, ResetRestoresDefaults) {
  WiFi.setRSSI(-99);
  WiFi.setBeginConnects(false);
  WiFi.setDnsResult(false);
  WiFi.setScanResults({{"net", -50}});
  WiFi.reset();
  EXPECT_EQ(WiFi.RSSI(), -50);
  EXPECT_EQ(WiFi.status(), WL_CONNECTED);
  EXPECT_EQ(WiFi.scanNetworks(), 0);
}

// --- wifi_mode_t constants ---

TEST_F(WiFiTest, WifiModeConstantsMatchESP32Values) {
  EXPECT_EQ(WIFI_OFF, 0);
  EXPECT_EQ(WIFI_STA, 1);
  EXPECT_EQ(WIFI_AP, 2);
  EXPECT_EQ(WIFI_AP_STA, 3);
}

// --- mode() / getMode() ---

TEST_F(WiFiTest, DefaultModeIsWifiSta) { EXPECT_EQ(WiFi.getMode(), WIFI_STA); }

TEST_F(WiFiTest, ModeStoresValue) {
  WiFi.mode(WIFI_AP);
  EXPECT_EQ(WiFi.getMode(), WIFI_AP);
}

TEST_F(WiFiTest, ModeApSta) {
  WiFi.mode(WIFI_AP_STA);
  EXPECT_EQ(WiFi.getMode(), WIFI_AP_STA);
}

TEST_F(WiFiTest, ModeOff) {
  WiFi.mode(WIFI_OFF);
  EXPECT_EQ(WiFi.getMode(), WIFI_OFF);
}

TEST_F(WiFiTest, ResetRestoresDefaultMode) {
  WiFi.mode(WIFI_AP);
  WiFi.reset();
  EXPECT_EQ(WiFi.getMode(), WIFI_STA);
}

// WiFiClient tests

class WiFiClientTest : public ::testing::Test {
 protected:
  void SetUp() override { WiFiClient::setCanConnect(true); }
};

TEST_F(WiFiClientTest, ConnectsByDefault) {
  WiFiClient client;
  EXPECT_TRUE(client.connect(IPAddress(8, 8, 8, 8), 80));
  EXPECT_TRUE(static_cast<bool>(client));
}

TEST_F(WiFiClientTest, SetCanConnectFalse) {
  WiFiClient client;
  client.setCanConnect(false);
  EXPECT_FALSE(client.connect(IPAddress(8, 8, 8, 8), 80));
  EXPECT_FALSE(static_cast<bool>(client));
}

TEST_F(WiFiClientTest, StopDisconnects) {
  WiFiClient client;
  client.connect(IPAddress(8, 8, 8, 8), 80);
  client.stop();
  EXPECT_FALSE(static_cast<bool>(client));
}

TEST(IPAddressTest, EqualityOperator) {
  IPAddress a(192, 168, 1, 1);
  IPAddress b(192, 168, 1, 1);
  IPAddress c(10, 0, 0, 1);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

TEST(IPAddressTest, InequalityOperator) {
  IPAddress a(192, 168, 1, 1);
  IPAddress b(10, 0, 0, 1);
  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a != a);
}

TEST(IPAddressTest, StringConversion) {
  IPAddress ip(192, 168, 1, 100);
  String s = ip;  // implicit conversion
  EXPECT_STREQ(s.c_str(), "192.168.1.100");
}

TEST_F(WiFiClientTest, InheritsFromClient) {
  WiFiClient client;
  Client *base = &client;
  EXPECT_NE(base, nullptr);
  EXPECT_TRUE(base->connect(IPAddress(1, 2, 3, 4), 80));
}

// WiFiClientSecure tests

TEST(WiFiClientSecureTest, InheritsFromWiFiClient) {
  WiFiClientSecure secure;
  WiFiClient *base = &secure;
  EXPECT_NE(base, nullptr);
}

TEST(WiFiClientSecureTest, SetInsecureCompiles) {
  WiFiClientSecure secure;
  secure.setInsecure();
  secure.setCACert("cert");
  secure.setCertificate("cert");
  secure.setPrivateKey("key");
}
