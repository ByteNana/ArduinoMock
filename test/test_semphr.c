#include <pthread.h>
#include <stdint.h>
#include <unity.h>

#include "freertos/semphr.h"

void setUp(void) {}
void tearDown(void) {}

/* --- binary semaphore --- */

void test_binary_starts_unavailable(void) {
    SemaphoreHandle_t s = xSemaphoreCreateBinary();
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s, 0));
    vSemaphoreDelete(s);
}

void test_binary_give_then_take(void) {
    SemaphoreHandle_t s = xSemaphoreCreateBinary();
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGive(s));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s, 0));
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s, 0)); /* consumed */
    vSemaphoreDelete(s);
}

void test_binary_give_twice_saturates(void) {
    SemaphoreHandle_t s = xSemaphoreCreateBinary();
    xSemaphoreGive(s);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreGive(s)); /* already full */
    TEST_ASSERT_EQUAL(1u, uxSemaphoreGetCount(s));
    vSemaphoreDelete(s);
}

/* --- mutex --- */

void test_mutex_starts_available(void) {
    SemaphoreHandle_t s = xSemaphoreCreateMutex();
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s, 0));
    vSemaphoreDelete(s);
}

void test_mutex_take_give_take(void) {
    SemaphoreHandle_t s = xSemaphoreCreateMutex();
    xSemaphoreTake(s, 0);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s, 0)); /* locked */
    xSemaphoreGive(s);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s, 0));  /* unlocked */
    vSemaphoreDelete(s);
}

/* --- counting semaphore --- */

void test_counting_initial_count(void) {
    SemaphoreHandle_t s = xSemaphoreCreateCounting(5, 3);
    TEST_ASSERT_EQUAL(3u, uxSemaphoreGetCount(s));
    vSemaphoreDelete(s);
}

void test_counting_take_decrements(void) {
    SemaphoreHandle_t s = xSemaphoreCreateCounting(5, 3);
    xSemaphoreTake(s, 0);
    TEST_ASSERT_EQUAL(2u, uxSemaphoreGetCount(s));
    vSemaphoreDelete(s);
}

void test_counting_give_increments(void) {
    SemaphoreHandle_t s = xSemaphoreCreateCounting(5, 3);
    xSemaphoreGive(s);
    TEST_ASSERT_EQUAL(4u, uxSemaphoreGetCount(s));
    vSemaphoreDelete(s);
}

void test_counting_give_above_max_returns_false(void) {
    SemaphoreHandle_t s = xSemaphoreCreateCounting(3, 3);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreGive(s));
    vSemaphoreDelete(s);
}

void test_counting_take_below_zero_returns_false(void) {
    SemaphoreHandle_t s = xSemaphoreCreateCounting(3, 0);
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s, 0));
    vSemaphoreDelete(s);
}

/* --- give from ISR --- */

void test_give_from_isr_gives(void) {
    SemaphoreHandle_t s = xSemaphoreCreateBinary();
    BaseType_t        woken;
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreGiveFromISR(s, &woken));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s, 0));
    vSemaphoreDelete(s);
}

/* --- blocking take (cross-thread) --- */

typedef struct { SemaphoreHandle_t s; } GiveArg;

static void *giver_thread(void *arg) {
    GiveArg           *ga = (GiveArg *)arg;
    struct timespec ts    = { .tv_sec = 0, .tv_nsec = 20 * 1000000L };
    nanosleep(&ts, NULL);
    xSemaphoreGive(ga->s);
    return NULL;
}

void test_blocking_take_wakes_on_give(void) {
    SemaphoreHandle_t s  = xSemaphoreCreateBinary();
    GiveArg           ga = { s };
    pthread_t         pt;
    pthread_create(&pt, NULL, giver_thread, &ga);
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(s, 200));
    pthread_join(pt, NULL);
    vSemaphoreDelete(s);
}

void test_take_timeout_returns_false(void) {
    SemaphoreHandle_t s = xSemaphoreCreateBinary();
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(s, 10));
    vSemaphoreDelete(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_binary_starts_unavailable);
    RUN_TEST(test_binary_give_then_take);
    RUN_TEST(test_binary_give_twice_saturates);
    RUN_TEST(test_mutex_starts_available);
    RUN_TEST(test_mutex_take_give_take);
    RUN_TEST(test_counting_initial_count);
    RUN_TEST(test_counting_take_decrements);
    RUN_TEST(test_counting_give_increments);
    RUN_TEST(test_counting_give_above_max_returns_false);
    RUN_TEST(test_counting_take_below_zero_returns_false);
    RUN_TEST(test_give_from_isr_gives);
    RUN_TEST(test_blocking_take_wakes_on_give);
    RUN_TEST(test_take_timeout_returns_false);
    return UNITY_END();
}
