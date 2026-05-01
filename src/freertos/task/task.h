#pragma once

#include <stddef.h>
#include <stdint.h>

#include "freertos/projdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunction_t)(void*);

typedef struct TaskControlBlock* TaskHandle_t;

// Priorities
#ifndef tskIDLE_PRIORITY
#define tskIDLE_PRIORITY 0
#endif

#ifndef taskYIELD
#define taskYIELD() ((void)0)
#endif

// Task notification actions
typedef enum {
  eNoAction,
  eSetBits,
  eIncrement,
  eSetValueWithOverwrite,
  eSetValueWithoutOverwrite
} eNotifyAction;

// API
BaseType_t xTaskCreate(
    TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t usStackDepth,
    void* const pvParameters, UBaseType_t uxPriority, TaskHandle_t* const pxCreatedTask);

void vTaskDelete(TaskHandle_t xTask);

void vTaskDelay(const TickType_t xTicksToDelay);

inline void vTaskDelayUntil(TickType_t* pxPreviousWakeTime, const TickType_t xTimeIncrement) {
  vTaskDelay(xTimeIncrement);
  if (pxPreviousWakeTime) *pxPreviousWakeTime += xTimeIncrement;
}

void vTaskStartScheduler(void);
void vTaskEndScheduler(void);

TickType_t xTaskGetTickCount(void);

// Task notifications
BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction);
uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait);

// Optional helpers used by hooks/tests
size_t xPortGetFreeHeapSize(void);

#ifdef __cplusplus
}
#endif
