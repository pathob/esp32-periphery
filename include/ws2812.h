#ifndef __WS2812_H__
#define __WS2812_H__

#include "string.h"

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"

#include "driver/gpio.h"
#include "driver/rmt.h"

typedef struct WS2812_stripe_t {
    gpio_num_t gpio_num;
    rmt_channel_t rmt_channel;
    uint8_t rmt_interrupt_num;
    uint16_t length;
} WS2812_stripe_t;

typedef struct WS2812_color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} WS2812_color_t;

esp_err_t WS2812_init(
        struct WS2812_stripe_t *stripe);

esp_err_t WS2812_set_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color);

esp_err_t WS2812_get_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color);

esp_err_t WS2812_write(
        struct WS2812_stripe_t *stripe);

#endif
