#pragma once
#include "FreeRTOS.h"

typedef void *TaskHandle_t;
typedef void (*TaskFunction_t)(void *);

BaseType_t xTaskCreate(TaskFunction_t fn, const char *name, uint32_t stack_depth, void *params,
                       UBaseType_t priority, TaskHandle_t *handle);
void vTaskDelete(TaskHandle_t handle);
void vTaskDelay(TickType_t ticks);
void vTaskDelayUntil(TickType_t *prev, TickType_t increment);
TickType_t xTaskGetTickCount(void);
void vTaskStartScheduler(void);
void vTaskEndScheduler(void);
size_t xPortGetFreeHeapSize(void);
