#include <unity.h>

#include "freertos/FreeRTOS.h"

void setUp(void) {}
void tearDown(void) {}

/* --- portMUX_TYPE / portMUX_INITIALIZER_UNLOCKED ----------------------- */

void test_portMUX_TYPE_is_usable(void) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    TEST_ASSERT_EQUAL(0, mux);
}

/* --- portENTER_CRITICAL / portEXIT_CRITICAL ----------------------------- */

void test_portENTER_CRITICAL_is_noop(void) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    TEST_ASSERT_EQUAL(0, mux); /* mux unchanged */
}

void test_portEXIT_CRITICAL_is_noop(void) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portEXIT_CRITICAL(&mux);
    TEST_ASSERT_EQUAL(0, mux);
}

/* --- portENTER_CRITICAL_ISR / portEXIT_CRITICAL_ISR -------------------- */

void test_portENTER_CRITICAL_ISR_is_noop(void) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_ISR(&mux);
    TEST_ASSERT_EQUAL(0, mux);
}

void test_portEXIT_CRITICAL_ISR_is_noop(void) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portEXIT_CRITICAL_ISR(&mux);
    TEST_ASSERT_EQUAL(0, mux);
}

/* --- portYIELD_FROM_ISR ------------------------------------------------- */

void test_portYIELD_FROM_ISR_is_noop(void) {
    /* Must compile and not crash; no observable side-effect to assert. */
    portYIELD_FROM_ISR(1);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_portMUX_TYPE_is_usable);
    RUN_TEST(test_portENTER_CRITICAL_is_noop);
    RUN_TEST(test_portEXIT_CRITICAL_is_noop);
    RUN_TEST(test_portENTER_CRITICAL_ISR_is_noop);
    RUN_TEST(test_portEXIT_CRITICAL_ISR_is_noop);
    RUN_TEST(test_portYIELD_FROM_ISR_is_noop);
    return UNITY_END();
}
