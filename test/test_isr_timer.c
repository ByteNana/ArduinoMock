#include <unity.h>

#include "freertos/timers.h"

static int g_fired = 0;

static void timer_cb(TimerHandle_t t) {
    (void)t;
    g_fired++;
}

void setUp(void) {
    mockTimerClear();
    g_fired = 0;
}

void tearDown(void) { mockTimerClear(); }

/* --- xTimerStartFromISR ------------------------------------------------- */

void test_xTimerStartFromISR_returns_pdPASS(void) {
    TimerHandle_t t  = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    BaseType_t woken = pdTRUE; /* initialize to non-zero to detect write */
    TEST_ASSERT_EQUAL(pdPASS, xTimerStartFromISR(t, &woken));
    TEST_ASSERT_EQUAL(pdFALSE, woken);
}

void test_xTimerStartFromISR_activates_timer(void) {
    TimerHandle_t t  = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    BaseType_t woken = pdFALSE;
    xTimerStartFromISR(t, &woken);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
}

void test_xTimerStartFromISR_accepts_null_woken(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL(pdPASS, xTimerStartFromISR(t, NULL));
}

/* --- xTimerStopFromISR -------------------------------------------------- */

void test_xTimerStopFromISR_returns_pdPASS(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    BaseType_t woken = pdTRUE;
    TEST_ASSERT_EQUAL(pdPASS, xTimerStopFromISR(t, &woken));
    TEST_ASSERT_EQUAL(pdFALSE, woken);
}

void test_xTimerStopFromISR_deactivates_timer(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    BaseType_t woken = pdFALSE;
    xTimerStopFromISR(t, &woken);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
}

void test_xTimerStopFromISR_accepts_null_woken(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    TEST_ASSERT_EQUAL(pdPASS, xTimerStopFromISR(t, NULL));
}

/* --- xTimerResetFromISR ------------------------------------------------- */

void test_xTimerResetFromISR_returns_pdPASS(void) {
    TimerHandle_t t  = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    BaseType_t woken = pdTRUE;
    TEST_ASSERT_EQUAL(pdPASS, xTimerResetFromISR(t, &woken));
    TEST_ASSERT_EQUAL(pdFALSE, woken);
}

void test_xTimerResetFromISR_activates_timer(void) {
    TimerHandle_t t  = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    BaseType_t woken = pdFALSE;
    xTimerResetFromISR(t, &woken);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
}

void test_xTimerResetFromISR_accepts_null_woken(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL(pdPASS, xTimerResetFromISR(t, NULL));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_xTimerStartFromISR_returns_pdPASS);
    RUN_TEST(test_xTimerStartFromISR_activates_timer);
    RUN_TEST(test_xTimerStartFromISR_accepts_null_woken);
    RUN_TEST(test_xTimerStopFromISR_returns_pdPASS);
    RUN_TEST(test_xTimerStopFromISR_deactivates_timer);
    RUN_TEST(test_xTimerStopFromISR_accepts_null_woken);
    RUN_TEST(test_xTimerResetFromISR_returns_pdPASS);
    RUN_TEST(test_xTimerResetFromISR_activates_timer);
    RUN_TEST(test_xTimerResetFromISR_accepts_null_woken);
    return UNITY_END();
}
