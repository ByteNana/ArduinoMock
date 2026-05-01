#pragma once

#include <stdint.h>

#include "freertos/projdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SemaphoreControlBlock* SemaphoreHandle_t;  // NOLINT(modernize-use-using)

SemaphoreHandle_t xSemaphoreCreateBinary(void);
SemaphoreHandle_t xSemaphoreCreateMutex(void);
void vSemaphoreDelete(SemaphoreHandle_t xSemaphore);

BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore);
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait);

#ifdef __cplusplus
}
#endif
