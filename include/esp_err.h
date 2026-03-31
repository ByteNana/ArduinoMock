#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int32_t esp_err_t;

#define ESP_OK ((esp_err_t)0)
#define ESP_FAIL ((esp_err_t) - 1)
#define ESP_ERR_NO_MEM ((esp_err_t)0x101)
#define ESP_ERR_INVALID_ARG ((esp_err_t)0x102)
#define ESP_ERR_INVALID_STATE ((esp_err_t)0x103)
#define ESP_ERR_INVALID_SIZE ((esp_err_t)0x104)
#define ESP_ERR_NOT_FOUND ((esp_err_t)0x105)
#define ESP_ERR_NOT_SUPPORTED ((esp_err_t)0x106)
#define ESP_ERR_TIMEOUT ((esp_err_t)0x107)
#define ESP_ERR_INVALID_RESPONSE ((esp_err_t)0x108)
#define ESP_ERR_INVALID_CRC ((esp_err_t)0x109)

#define ESP_ERROR_CHECK(x)                                                                     \
    do {                                                                                       \
        esp_err_t _rc = (x);                                                                   \
        if (_rc != ESP_OK) {                                                                   \
            fprintf(stderr, "ESP_ERROR_CHECK failed: %d at %s:%d\n", _rc, __FILE__, __LINE__); \
            abort();                                                                           \
        }                                                                                      \
    } while (0)

static inline const char *esp_err_to_name(esp_err_t err) {
    switch (err) {
        case ESP_OK:
            return "ESP_OK";
        case ESP_FAIL:
            return "ESP_FAIL";
        case ESP_ERR_NO_MEM:
            return "ESP_ERR_NO_MEM";
        case ESP_ERR_INVALID_ARG:
            return "ESP_ERR_INVALID_ARG";
        case ESP_ERR_INVALID_STATE:
            return "ESP_ERR_INVALID_STATE";
        case ESP_ERR_INVALID_SIZE:
            return "ESP_ERR_INVALID_SIZE";
        case ESP_ERR_NOT_FOUND:
            return "ESP_ERR_NOT_FOUND";
        case ESP_ERR_NOT_SUPPORTED:
            return "ESP_ERR_NOT_SUPPORTED";
        case ESP_ERR_TIMEOUT:
            return "ESP_ERR_TIMEOUT";
        default:
            return "ESP_ERR_UNKNOWN";
    }
}
