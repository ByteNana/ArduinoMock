#pragma once

#include "freertos/FreeRTOS.h"

typedef int esp_err_t;
#define ESP_OK 0

inline esp_err_t esp_task_wdt_add(TaskHandle_t) { return ESP_OK; }
inline esp_err_t esp_task_wdt_reset(void) { return ESP_OK; }
inline esp_err_t esp_task_wdt_delete(TaskHandle_t) { return ESP_OK; }
inline esp_err_t esp_task_wdt_init(uint32_t, bool) { return ESP_OK; }
inline esp_err_t esp_task_wdt_deinit(void) { return ESP_OK; }