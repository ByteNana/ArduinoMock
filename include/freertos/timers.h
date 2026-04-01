#pragma once
#include "FreeRTOS.h"

typedef void *TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t timer);

TimerHandle_t xTimerCreate(const char *name, TickType_t period, BaseType_t auto_reload,
                           void *timer_id, TimerCallbackFunction_t cb);
BaseType_t xTimerStart(TimerHandle_t timer, TickType_t wait);
BaseType_t xTimerStop(TimerHandle_t timer, TickType_t wait);
BaseType_t xTimerReset(TimerHandle_t timer, TickType_t wait);
BaseType_t xTimerDelete(TimerHandle_t timer, TickType_t wait);
BaseType_t xTimerIsTimerActive(TimerHandle_t timer);
BaseType_t xTimerChangePeriod(TimerHandle_t timer, TickType_t new_period, TickType_t wait);
void *pvTimerGetTimerID(TimerHandle_t timer);
const char *pcTimerGetName(TimerHandle_t timer);

/* ISR-safe variants — on native, delegate to the standard functions.
 * The pxHigherPriorityTaskWoken out-param is set to pdFALSE (no preemption). */
static inline BaseType_t xTimerStartFromISR(TimerHandle_t timer,
                                            BaseType_t *pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken != NULL) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerStart(timer, 0);
}
static inline BaseType_t xTimerStopFromISR(TimerHandle_t timer,
                                           BaseType_t *pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken != NULL) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerStop(timer, 0);
}
static inline BaseType_t xTimerResetFromISR(TimerHandle_t timer,
                                            BaseType_t *pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken != NULL) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerReset(timer, 0);
}

/* Test helpers */
void mockTimerProcess(void);
void mockTimerFire(TimerHandle_t timer);
size_t mockTimerCount(void);
void mockTimerClear(void);
