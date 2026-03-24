#include <gtest/gtest.h>

#include "esp_now.h"
#include "esp_wifi.h"

class EspNowTest : public ::testing::Test {
 protected:
  void SetUp() override { esp_now_mock_reset(); }
  void TearDown() override { esp_now_mock_reset(); }
};

// ---- Init / deinit ----

TEST_F(EspNowTest, InitReturnsEspOk) { EXPECT_EQ(esp_now_init(), ESP_OK); }

TEST_F(EspNowTest, DeinitReturnsEspOk) {
  esp_now_init();
  EXPECT_EQ(esp_now_deinit(), ESP_OK);
}

TEST_F(EspNowTest, DeinitClearsCallbacks) {
  esp_now_init();
  esp_now_register_recv_cb([](const uint8_t*, const uint8_t*, int) {});
  esp_now_deinit();
  // Inject should be a no-op after deinit (no crash)
  const uint8_t mac[6] = {0};
  const uint8_t data[4] = {1, 2, 3, 4};
  esp_now_inject_recv(mac, data, 4);
}

// ---- Register / unregister recv callback ----

TEST_F(EspNowTest, RegisterRecvCbReturnsEspOk) {
  EXPECT_EQ(esp_now_register_recv_cb([](const uint8_t*, const uint8_t*, int) {}), ESP_OK);
}

TEST_F(EspNowTest, UnregisterRecvCbReturnsEspOk) {
  esp_now_register_recv_cb([](const uint8_t*, const uint8_t*, int) {});
  EXPECT_EQ(esp_now_unregister_recv_cb(), ESP_OK);
}

// ---- inject_recv fires the callback ----

TEST_F(EspNowTest, InjectRecvFiresRegisteredCallback) {
  const uint8_t expected_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  const uint8_t expected_data[] = {0x01, 0x02, 0x03};

  bool called = false;
  uint8_t got_mac[6] = {};
  uint8_t got_data[3] = {};
  int got_len = 0;

  esp_now_register_recv_cb([&](const uint8_t* mac, const uint8_t* data, int len) {
    called = true;
    memcpy(got_mac, mac, 6);
    memcpy(got_data, data, len);
    got_len = len;
  });

  esp_now_inject_recv(expected_mac, expected_data, 3);

  EXPECT_TRUE(called);
  EXPECT_EQ(memcmp(got_mac, expected_mac, 6), 0);
  EXPECT_EQ(memcmp(got_data, expected_data, 3), 0);
  EXPECT_EQ(got_len, 3);
}

TEST_F(EspNowTest, InjectRecvWithNoCallbackDoesNotCrash) {
  const uint8_t mac[6] = {0};
  const uint8_t data[2] = {0xDE, 0xAD};
  esp_now_inject_recv(mac, data, 2);  // should not crash
}

TEST_F(EspNowTest, InjectRecvAfterUnregisterDoesNotFire) {
  bool called = false;
  esp_now_register_recv_cb([&](const uint8_t*, const uint8_t*, int) { called = true; });
  esp_now_unregister_recv_cb();

  const uint8_t mac[6] = {0};
  const uint8_t data[1] = {0xFF};
  esp_now_inject_recv(mac, data, 1);

  EXPECT_FALSE(called);
}

TEST_F(EspNowTest, InjectRecvCanBeCalledMultipleTimes) {
  int call_count = 0;
  esp_now_register_recv_cb([&](const uint8_t*, const uint8_t*, int) { call_count++; });

  const uint8_t mac[6] = {0};
  const uint8_t data[1] = {0x42};
  esp_now_inject_recv(mac, data, 1);
  esp_now_inject_recv(mac, data, 1);
  esp_now_inject_recv(mac, data, 1);

  EXPECT_EQ(call_count, 3);
}

// ---- send callback ----

TEST_F(EspNowTest, SendFiresSendCallback) {
  bool send_cb_called = false;
  esp_now_register_send_cb([&](const uint8_t*, esp_err_t status) {
    send_cb_called = true;
    EXPECT_EQ(status, ESP_OK);
  });

  const uint8_t peer[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  const uint8_t data[] = {0xDE, 0xAD};
  EXPECT_EQ(esp_now_send(peer, data, 2), ESP_OK);
  EXPECT_TRUE(send_cb_called);
}

TEST_F(EspNowTest, UnregisterSendCbReturnsEspOk) {
  esp_now_register_send_cb([](const uint8_t*, esp_err_t) {});
  EXPECT_EQ(esp_now_unregister_send_cb(), ESP_OK);
}

// ---- peer management ----

TEST_F(EspNowTest, AddPeerReturnsEspOk) {
  esp_now_peer_info_t peer = {};
  peer.peer_addr[0] = 0x11;
  peer.channel = 1;
  EXPECT_EQ(esp_now_add_peer(&peer), ESP_OK);
}

TEST_F(EspNowTest, DelPeerReturnsEspOk) {
  const uint8_t addr[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  EXPECT_EQ(esp_now_del_peer(addr), ESP_OK);
}

TEST_F(EspNowTest, IsPeerExistReturnsTrue) {
  const uint8_t addr[6] = {0};
  EXPECT_TRUE(esp_now_is_peer_exist(addr));
}

// ---- misc API ----

TEST_F(EspNowTest, GetVersionReturnsEspOk) {
  uint32_t version = 0;
  EXPECT_EQ(esp_now_get_version(&version), ESP_OK);
  EXPECT_GE(version, 1u);
}

TEST_F(EspNowTest, EspErrToNameEspOk) { EXPECT_STREQ(esp_err_to_name(ESP_OK), "ESP_OK"); }

TEST_F(EspNowTest, EspNowEthAlenIsSix) { EXPECT_EQ(ESP_NOW_ETH_ALEN, 6); }

// ---- esp_wifi_set_mac ----

TEST_F(EspNowTest, EspWifiSetMacReturnsEspOk) {
  const uint8_t mac[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};
  EXPECT_EQ(esp_wifi_set_mac(WIFI_IF_STA, mac), ESP_OK);
}

// ---- mock_reset clears state ----

TEST_F(EspNowTest, MockResetClearsRecvCallback) {
  bool called = false;
  esp_now_register_recv_cb([&](const uint8_t*, const uint8_t*, int) { called = true; });
  esp_now_mock_reset();

  const uint8_t mac[6] = {0};
  const uint8_t data[1] = {0x01};
  esp_now_inject_recv(mac, data, 1);

  EXPECT_FALSE(called);
}
