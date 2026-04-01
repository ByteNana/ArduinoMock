#include <gtest/gtest.h>

#include "freertos/FreeRTOS.h"

TEST(QueueTest, BasicSendReceive) {
  QueueHandle_t q = xQueueCreate(2, sizeof(uint32_t));
  ASSERT_NE(q, nullptr);

  uint32_t v1 = 0xAABBCCDD;
  uint32_t v2 = 0x01020304;
  uint32_t v3 = 0x12345678;

  EXPECT_EQ(xQueueSend(q, &v1, 0), pdTRUE);
  EXPECT_EQ(xQueueSend(q, &v2, 0), pdTRUE);
  EXPECT_EQ(xQueueSend(q, &v3, 0), pdFALSE);
  EXPECT_EQ(uxQueueMessagesWaiting(q), 2u);

  uint32_t r1 = 0, r2 = 0;
  EXPECT_EQ(xQueueReceive(q, &r1, 0), pdTRUE);
  EXPECT_EQ(xQueueReceive(q, &r2, 0), pdTRUE);
  EXPECT_EQ(r1, v1);
  EXPECT_EQ(r2, v2);

  EXPECT_EQ(uxQueueMessagesWaiting(q), 0u);

  vQueueDelete(q);
}

TEST(QueueTest, SendToBackWorks) {
  QueueHandle_t q = xQueueCreate(2, sizeof(uint32_t));
  ASSERT_NE(q, nullptr);

  uint32_t v = 0xDEADBEEF;
  EXPECT_EQ(xQueueSendToBack(q, &v, 0), pdTRUE);

  uint32_t r = 0;
  EXPECT_EQ(xQueueReceive(q, &r, 0), pdTRUE);
  EXPECT_EQ(r, v);

  vQueueDelete(q);
}

TEST(QueueTest, SendToFrontWorks) {
  QueueHandle_t q = xQueueCreate(3, sizeof(uint32_t));
  ASSERT_NE(q, nullptr);

  uint32_t v1 = 0x11111111;
  uint32_t v2 = 0x22222222;
  uint32_t vf = 0xFFFFFFFF;

  EXPECT_EQ(xQueueSendToBack(q, &v1, 0), pdTRUE);
  EXPECT_EQ(xQueueSendToBack(q, &v2, 0), pdTRUE);
  EXPECT_EQ(xQueueSendToFront(q, &vf, 0), pdTRUE);

  uint32_t r0 = 0, r1 = 0, r2 = 0;
  EXPECT_EQ(xQueueReceive(q, &r0, 0), pdTRUE);
  EXPECT_EQ(xQueueReceive(q, &r1, 0), pdTRUE);
  EXPECT_EQ(xQueueReceive(q, &r2, 0), pdTRUE);
  EXPECT_EQ(r0, vf);
  EXPECT_EQ(r1, v1);
  EXPECT_EQ(r2, v2);

  vQueueDelete(q);
}

TEST(QueueTest, PeekDoesNotRemove) {
  QueueHandle_t q = xQueueCreate(2, sizeof(uint32_t));
  ASSERT_NE(q, nullptr);

  uint32_t v = 0xCAFEBABE;
  EXPECT_EQ(xQueueSend(q, &v, 0), pdTRUE);

  uint32_t peeked = 0;
  EXPECT_EQ(xQueuePeek(q, &peeked, 0), pdTRUE);
  EXPECT_EQ(peeked, v);

  uint32_t received = 0;
  EXPECT_EQ(xQueueReceive(q, &received, 0), pdTRUE);
  EXPECT_EQ(received, v);

  vQueueDelete(q);
}

TEST(ProjdefsTest, TicksToMsRespectsPortTickPeriod) {
  EXPECT_EQ(pdTICKS_TO_MS(1000), 1000u * portTICK_PERIOD_MS);
  EXPECT_EQ(pdTICKS_TO_MS(0), 0u);
}
