#ifndef __WS2812_BB_H__
#define __WS2812_BB_H__

#include "ws2812.h"
#include "delay.h"

esp_err_t WS2812_bb_init(
        struct WS2812_stripe_t *stripe);

esp_err_t WS2812_bb_set_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color);

esp_err_t WS2812_bb_get_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color);

void IRAM_ATTR WS2812_bb_write_bit(
        struct WS2812_stripe_t *stripe,
        uint8_t bit);

esp_err_t WS2812_bb_write(
        struct WS2812_stripe_t *stripe);

#endif
