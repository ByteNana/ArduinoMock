#include <pthread.h>
#include <stdint.h>
#include <unity.h>

#include "freertos/queue.h"

void setUp(void) {}
void tearDown(void) {}

/* --- basic create / delete --- */

void test_xQueueCreate_returns_non_null(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    TEST_ASSERT_NOT_NULL(q);
    vQueueDelete(q);
}

void test_xQueueCreate_zero_length_returns_null(void) {
    TEST_ASSERT_NULL(xQueueCreate(0, sizeof(int)));
}

void test_xQueueCreate_zero_item_size_returns_null(void) { TEST_ASSERT_NULL(xQueueCreate(4, 0)); }

/* --- send / receive --- */

void test_send_and_receive_single_item(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    int val         = 42;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueSend(q, &val, 0));
    int out = 0;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(q, &out, 0));
    TEST_ASSERT_EQUAL(42, out);
    vQueueDelete(q);
}

void test_fifo_ordering(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    for (int i = 0; i < 4; i++) xQueueSend(q, &i, 0);
    for (int i = 0; i < 4; i++) {
        int out;
        xQueueReceive(q, &out, 0);
        TEST_ASSERT_EQUAL(i, out);
    }
    vQueueDelete(q);
}

void test_send_to_front_prepends(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    int a = 1, b = 2;
    xQueueSend(q, &a, 0);
    xQueueSendToFront(q, &b, 0);
    int out;
    xQueueReceive(q, &out, 0);
    TEST_ASSERT_EQUAL(2, out); /* b was sent to front */
    vQueueDelete(q);
}

/* --- queue fullness --- */

void test_send_to_full_queue_with_zero_wait_returns_false(void) {
    QueueHandle_t q = xQueueCreate(2, sizeof(int));
    int v           = 1;
    xQueueSend(q, &v, 0);
    xQueueSend(q, &v, 0);
    TEST_ASSERT_EQUAL(pdFALSE, xQueueSend(q, &v, 0));
    vQueueDelete(q);
}

void test_receive_from_empty_queue_with_zero_wait_returns_false(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    int out;
    TEST_ASSERT_EQUAL(pdFALSE, xQueueReceive(q, &out, 0));
    vQueueDelete(q);
}

/* --- counters --- */

void test_uxQueueMessagesWaiting(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    TEST_ASSERT_EQUAL(0u, uxQueueMessagesWaiting(q));
    int v = 1;
    xQueueSend(q, &v, 0);
    xQueueSend(q, &v, 0);
    TEST_ASSERT_EQUAL(2u, uxQueueMessagesWaiting(q));
    vQueueDelete(q);
}

void test_uxQueueSpacesAvailable(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    TEST_ASSERT_EQUAL(4u, uxQueueSpacesAvailable(q));
    int v = 1;
    xQueueSend(q, &v, 0);
    TEST_ASSERT_EQUAL(3u, uxQueueSpacesAvailable(q));
    vQueueDelete(q);
}

void test_xQueueReset_clears_items(void) {
    QueueHandle_t q = xQueueCreate(4, sizeof(int));
    int v           = 1;
    xQueueSend(q, &v, 0);
    xQueueSend(q, &v, 0);
    xQueueReset(q);
    TEST_ASSERT_EQUAL(0u, uxQueueMessagesWaiting(q));
    vQueueDelete(q);
}

/* --- blocking receive (cross-thread) --- */

typedef struct {
    QueueHandle_t q;
    int val;
} SendArg;

static void *sender_thread(void *arg) {
    SendArg *sa        = (SendArg *)arg;
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 20 * 1000000L};
    nanosleep(&ts, NULL); /* slight delay so receiver blocks first */
    xQueueSend(sa->q, &sa->val, portMAX_DELAY);
    return NULL;
}

void test_blocking_receive_wakes_on_send(void) {
    QueueHandle_t q = xQueueCreate(1, sizeof(int));
    SendArg sa      = {q, 77};
    pthread_t sender;
    pthread_create(&sender, NULL, sender_thread, &sa);
    int out = 0;
    TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(q, &out, 200));
    TEST_ASSERT_EQUAL(77, out);
    pthread_join(sender, NULL);
    vQueueDelete(q);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_xQueueCreate_returns_non_null);
    RUN_TEST(test_xQueueCreate_zero_length_returns_null);
    RUN_TEST(test_xQueueCreate_zero_item_size_returns_null);
    RUN_TEST(test_send_and_receive_single_item);
    RUN_TEST(test_fifo_ordering);
    RUN_TEST(test_send_to_front_prepends);
    RUN_TEST(test_send_to_full_queue_with_zero_wait_returns_false);
    RUN_TEST(test_receive_from_empty_queue_with_zero_wait_returns_false);
    RUN_TEST(test_uxQueueMessagesWaiting);
    RUN_TEST(test_uxQueueSpacesAvailable);
    RUN_TEST(test_xQueueReset_clears_items);
    RUN_TEST(test_blocking_receive_wakes_on_send);
    return UNITY_END();
}
