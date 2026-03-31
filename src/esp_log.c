#include "esp_log.h"

esp_log_level_t esp_log_current_level = ESP_LOG_INFO;

void esp_log_set_level(esp_log_level_t level) { esp_log_current_level = level; }
