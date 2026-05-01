#pragma once

#include <stddef.h>
#include <stdint.h>

#include "freertos/projdefs.h"

typedef void* TimerHandle_t;                             // NOLINT(modernize-use-using)
typedef void (*TimerCallbackFunction_t)(TimerHandle_t);  // NOLINT(modernize-use-using)

#ifdef __cplusplus
extern "C" {
#endif

TimerHandle_t xTimerCreate(
    const char* name, uint32_t periodTicks, BaseType_t autoReload, void* timerID,
    TimerCallbackFunction_t callback);

BaseType_t xTimerStart(TimerHandle_t timer, TickType_t ticksToWait);
BaseType_t xTimerStop(TimerHandle_t timer, TickType_t ticksToWait);
BaseType_t xTimerReset(TimerHandle_t timer, TickType_t ticksToWait);
BaseType_t xTimerDelete(TimerHandle_t timer, TickType_t ticksToWait);
BaseType_t xTimerChangePeriod(TimerHandle_t timer, uint32_t newPeriod, TickType_t ticksToWait);
BaseType_t xTimerIsTimerActive(TimerHandle_t timer);
void* pvTimerGetTimerID(TimerHandle_t timer);
const char* pcTimerGetName(TimerHandle_t timer);

// Test helpers
void mockProcessTimers();
void mockFireTimer(TimerHandle_t timer);
size_t mockGetTimerCount();
void mockClearTimers();

#ifdef __cplusplus
}
#endif
