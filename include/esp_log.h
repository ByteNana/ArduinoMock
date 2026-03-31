#pragma once
#include <stdio.h>

typedef enum {
    ESP_LOG_NONE    = 0,
    ESP_LOG_ERROR   = 1,
    ESP_LOG_WARN    = 2,
    ESP_LOG_INFO    = 3,
    ESP_LOG_DEBUG   = 4,
    ESP_LOG_VERBOSE = 5,
} esp_log_level_t;

extern esp_log_level_t esp_log_current_level;

/* clang-format off */
#define ESP_LOGE(tag, fmt, ...) do { if (esp_log_current_level >= ESP_LOG_ERROR)   fprintf(stderr, "E (%s): " fmt "\n", tag, ##__VA_ARGS__); } while (0)
#define ESP_LOGW(tag, fmt, ...) do { if (esp_log_current_level >= ESP_LOG_WARN)    fprintf(stderr, "W (%s): " fmt "\n", tag, ##__VA_ARGS__); } while (0)
#define ESP_LOGI(tag, fmt, ...) do { if (esp_log_current_level >= ESP_LOG_INFO)    fprintf(stdout, "I (%s): " fmt "\n", tag, ##__VA_ARGS__); } while (0)
#define ESP_LOGD(tag, fmt, ...) do { if (esp_log_current_level >= ESP_LOG_DEBUG)   fprintf(stdout, "D (%s): " fmt "\n", tag, ##__VA_ARGS__); } while (0)
#define ESP_LOGV(tag, fmt, ...) do { if (esp_log_current_level >= ESP_LOG_VERBOSE) fprintf(stdout, "V (%s): " fmt "\n", tag, ##__VA_ARGS__); } while (0)
/* clang-format on */

void esp_log_set_level(esp_log_level_t level);
