#include <stdint.h>
#include <time.h>
#include <unity.h>

#include "freertos/timers.h"

static int g_fired = 0;
static TimerHandle_t g_last_timer = NULL;

static void timer_cb(TimerHandle_t t) {
    g_fired++;
    g_last_timer = t;
}

void setUp(void) {
    mockTimerClear();
    g_fired      = 0;
    g_last_timer = NULL;
}

void tearDown(void) { mockTimerClear(); }

/* --- create / delete --- */

void test_xTimerCreate_returns_non_null(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_NOT_NULL(t);
}

void test_pcTimerGetName(void) {
    TimerHandle_t t = xTimerCreate("mytimer", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL_STRING("mytimer", pcTimerGetName(t));
}

void test_pvTimerGetTimerID(void) {
    int           id = 99;
    TimerHandle_t t  = xTimerCreate("t", 1000, pdFALSE, &id, timer_cb);
    TEST_ASSERT_EQUAL_PTR(&id, pvTimerGetTimerID(t));
}

void test_xTimerDelete_removes_timer(void) {
    xTimerCreate("t1", 1000, pdFALSE, NULL, timer_cb);
    TimerHandle_t t2 = xTimerCreate("t2", 1000, pdFALSE, NULL, timer_cb);
    xTimerCreate("t3", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL(3u, mockTimerCount());
    xTimerDelete(t2, 0);
    TEST_ASSERT_EQUAL(2u, mockTimerCount());
}

/* --- active state --- */

void test_timer_inactive_after_create(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
}

void test_timer_active_after_start(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
}

void test_timer_inactive_after_stop(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    xTimerStop(t, 0);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
}

void test_timer_active_after_reset(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerReset(t, 0);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
}

/* --- time semantics --- */

void test_oneshot_deactivates_after_period(void) {
    TimerHandle_t t = xTimerCreate("one", 0, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 5 * 1000000L };
    nanosleep(&ts, NULL);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
    TEST_ASSERT_EQUAL(1, g_fired);
}

void test_autoreload_stays_active_after_period(void) {
    TimerHandle_t t = xTimerCreate("rep", 0, pdTRUE, NULL, timer_cb);
    xTimerStart(t, 0);
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 5 * 1000000L };
    nanosleep(&ts, NULL);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
    TEST_ASSERT_EQUAL(1, g_fired);
}

void test_reset_restarts_period(void) {
    TimerHandle_t t = xTimerCreate("one", 0, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 5 * 1000000L };
    nanosleep(&ts, NULL);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
    /* reset with long period — should become active again */
    xTimerChangePeriod(t, 1000, 0);
    xTimerReset(t, 0);
    TEST_ASSERT_EQUAL(pdTRUE, xTimerIsTimerActive(t));
}

/* --- mock helpers --- */

void test_mockTimerProcess_fires_active(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    mockTimerProcess();
    TEST_ASSERT_EQUAL(1, g_fired);
    TEST_ASSERT_EQUAL(t, g_last_timer);
}

void test_mockTimerProcess_deactivates_oneshot(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    mockTimerProcess();
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
}

void test_mockTimerProcess_keeps_autoreload_active(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdTRUE, NULL, timer_cb);
    xTimerStart(t, 0);
    mockTimerProcess();
    TEST_ASSERT_EQUAL(pdTRUE, xTimerStart(t, 0)); /* still stoppable => active */
}

void test_mockTimerFire_fires_specific(void) {
    TimerHandle_t t1 = xTimerCreate("t1", 1000, pdFALSE, NULL, timer_cb);
    TimerHandle_t t2 = xTimerCreate("t2", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t1, 0);
    xTimerStart(t2, 0);
    mockTimerFire(t2);
    TEST_ASSERT_EQUAL(1, g_fired);
    TEST_ASSERT_EQUAL(t2, g_last_timer);
}

void test_mockTimerFire_deactivates_oneshot(void) {
    TimerHandle_t t = xTimerCreate("t", 1000, pdFALSE, NULL, timer_cb);
    xTimerStart(t, 0);
    mockTimerFire(t);
    TEST_ASSERT_EQUAL(pdFALSE, xTimerIsTimerActive(t));
}

void test_mockTimerClear_removes_all(void) {
    xTimerCreate("a", 100, pdFALSE, NULL, timer_cb);
    xTimerCreate("b", 200, pdFALSE, NULL, timer_cb);
    TEST_ASSERT_EQUAL(2u, mockTimerCount());
    mockTimerClear();
    TEST_ASSERT_EQUAL(0u, mockTimerCount());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_xTimerCreate_returns_non_null);
    RUN_TEST(test_pcTimerGetName);
    RUN_TEST(test_pvTimerGetTimerID);
    RUN_TEST(test_xTimerDelete_removes_timer);
    RUN_TEST(test_timer_inactive_after_create);
    RUN_TEST(test_timer_active_after_start);
    RUN_TEST(test_timer_inactive_after_stop);
    RUN_TEST(test_timer_active_after_reset);
    RUN_TEST(test_oneshot_deactivates_after_period);
    RUN_TEST(test_autoreload_stays_active_after_period);
    RUN_TEST(test_reset_restarts_period);
    RUN_TEST(test_mockTimerProcess_fires_active);
    RUN_TEST(test_mockTimerProcess_deactivates_oneshot);
    RUN_TEST(test_mockTimerProcess_keeps_autoreload_active);
    RUN_TEST(test_mockTimerFire_fires_specific);
    RUN_TEST(test_mockTimerFire_deactivates_oneshot);
    RUN_TEST(test_mockTimerClear_removes_all);
    return UNITY_END();
}
