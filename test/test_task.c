#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <unity.h>

#include "freertos/task.h"

void setUp(void) {}
void tearDown(void) {}

/* --- helpers --- */

static volatile int g_ran = 0;

static void task_set_flag(void *arg) {
    volatile int *flag = (volatile int *)arg;
    *flag              = 1;
}

static void task_increment(void *arg) {
    volatile int *cnt = (volatile int *)arg;
    (*cnt)++;
}

static void task_self_delete(void *arg) {
    (void)arg;
    vTaskDelete(NULL); /* should not return */
    g_ran = 99;        /* must not be reached */
}

/* --- tests --- */

void test_xTaskCreate_returns_pass(void) {
    volatile int flag = 0;
    TaskHandle_t h    = NULL;
    BaseType_t rc     = xTaskCreate(task_set_flag, "t1", 2048, (void *)&flag, 5, &h);
    TEST_ASSERT_EQUAL(pdPASS, rc);
    TEST_ASSERT_NOT_NULL(h);
    vTaskDelete(h);
}

void test_task_function_runs(void) {
    volatile int flag = 0;
    TaskHandle_t h    = NULL;
    xTaskCreate(task_set_flag, "t2", 2048, (void *)&flag, 5, &h);
    vTaskDelete(h); /* join waits for task to complete */
    TEST_ASSERT_EQUAL(1, flag);
}

void test_null_handle_handle_is_optional(void) {
    volatile int flag = 0;
    BaseType_t rc     = xTaskCreate(task_set_flag, "t3", 2048, (void *)&flag, 5, NULL);
    TEST_ASSERT_EQUAL(pdPASS, rc);
    vTaskDelay(20); /* give it time to finish */
}

void test_vTaskDelay_outside_task(void) {
    /* should not crash and should take roughly the requested time */
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    vTaskDelay(20);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    long elapsed_ms = (t1.tv_sec - t0.tv_sec) * 1000L + (t1.tv_nsec - t0.tv_nsec) / 1000000L;
    TEST_ASSERT_GREATER_OR_EQUAL(15, elapsed_ms); /* allow 25% slack */
}

void test_vTaskDelayUntil_increments_prev(void) {
    TickType_t prev = 100;
    /* outside task context: just delays and increments */
    vTaskDelayUntil(&prev, 10);
    TEST_ASSERT_EQUAL(110u, prev);
}

void test_xTaskGetTickCount_is_monotonic(void) {
    TickType_t t0 = xTaskGetTickCount();
    vTaskDelay(10);
    TickType_t t1 = xTaskGetTickCount();
    TEST_ASSERT_GREATER_OR_EQUAL(t0, t1);
}

void test_xPortGetFreeHeapSize_nonzero(void) {
    TEST_ASSERT_GREATER_THAN(0u, xPortGetFreeHeapSize());
}

void test_multiple_tasks_run_concurrently(void) {
    volatile int a = 0, b = 0;
    TaskHandle_t ha, hb;
    xTaskCreate(task_set_flag, "ta", 2048, (void *)&a, 5, &ha);
    xTaskCreate(task_set_flag, "tb", 2048, (void *)&b, 5, &hb);
    vTaskDelete(ha);
    vTaskDelete(hb);
    TEST_ASSERT_EQUAL(1, a);
    TEST_ASSERT_EQUAL(1, b);
}

void test_self_delete_does_not_reach_after(void) {
    g_ran = 0;
    TaskHandle_t h;
    xTaskCreate(task_self_delete, "sd", 2048, NULL, 5, &h);
    vTaskDelay(30); /* give task time to self-delete */
    TEST_ASSERT_NOT_EQUAL(99, g_ran);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_xTaskCreate_returns_pass);
    RUN_TEST(test_task_function_runs);
    RUN_TEST(test_null_handle_handle_is_optional);
    RUN_TEST(test_vTaskDelay_outside_task);
    RUN_TEST(test_vTaskDelayUntil_increments_prev);
    RUN_TEST(test_xTaskGetTickCount_is_monotonic);
    RUN_TEST(test_xPortGetFreeHeapSize_nonzero);
    RUN_TEST(test_multiple_tasks_run_concurrently);
    RUN_TEST(test_self_delete_does_not_reach_after);
    return UNITY_END();
}
