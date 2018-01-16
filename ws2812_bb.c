#include "ws2812_bb.h"

static WS2812_color_t *color_buffers[RMT_CHANNEL_MAX] = { NULL };

static const char *TAG = "WS2812_BB";

// TODO: This is wrong yet
static uint16_t WS2812_t0h_us = WS2812_T0H;
static uint16_t WS2812_t0l_us = WS2812_T0L;
static uint16_t WS2812_t1h_us = WS2812_T1H;
static uint16_t WS2812_t1l_us = WS2812_T1L;

esp_err_t WS2812_bb_init(
        struct WS2812_stripe_t *stripe) {

    esp_err_t esp_err;
    
    esp_err = gpio_set_direction(stripe->gpio_num, GPIO_MODE_OUTPUT);
    
    if (esp_err) {
        return esp_err;
    }

    // uint32_t buffer_size = sizeof(WS2812_color_t) * stripe->length;
    // color_buffers[stripe->rmt_channel] = (WS2812_color_t*) malloc(buffer_size);

    /*
    WS2812_color_t black = { 0, 0, 0 };
    for (uint16_t num = 0; num < stripe->length; num++) {
        WS2812_bb_set_color(stripe, num, &black);
    }
    WS2812_bb_write(stripe);
    */

    return ESP_OK;
}

esp_err_t WS2812_bb_set_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color)
{
    color_buffers[stripe->rmt_channel][num] = *color;
    return ESP_OK;
}

esp_err_t WS2812_bb_get_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color)
{
    *color = color_buffers[stripe->rmt_channel][num];
    return ESP_OK;
}

void IRAM_ATTR WS2812_bb_write_bit(
        struct WS2812_stripe_t *stripe,
        uint8_t bit) {

    if (bit) {
        gpio_set_level(stripe->gpio_num, 1);
        delay_hundred_ns(WS2812_T1H);
        gpio_set_level(stripe->gpio_num, 0);
        delay_hundred_ns(WS2812_T1L);
    } else {
        gpio_set_level(stripe->gpio_num, 1);
        delay_hundred_ns(WS2812_T0H);
        gpio_set_level(stripe->gpio_num, 0);
        delay_hundred_ns(WS2812_T0L);
    }
}

esp_err_t WS2812_bb_write(
        struct WS2812_stripe_t *stripe)
{
    // ESP_LOGI(TAG, "RMT TX DATA");
    uint8_t channel = stripe->rmt_channel;
    uint32_t item_num = 8 * 24;
    uint32_t size = sizeof(rmt_item32_t) * item_num;
    //each item represent a cycle of waveform.
    rmt_item32_t* items = (rmt_item32_t*) malloc(size);
    memset((void*) items, 0, size);

    uint8_t item = 0;

    for (uint16_t num = 0; num < stripe->length; num++) {
        WS2812_color_t color = color_buffers[stripe->rmt_channel][num];

        // RGB -> GRB

        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (color.g >> (i-1)) & 1;

            // printf("%u", bit);
            WS2812_bb_write_bit(stripe, bit);
        }

        // printf("\n");

        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (color.r >> (i-1)) & 1;

            // printf("%u", bit);
            WS2812_bb_write_bit(stripe, bit);
        }

        // printf("\n");

        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (color.b >> (i-1)) & 1;

            // printf("%u", bit);
            WS2812_bb_write_bit(stripe, bit);
        }

        ESP_LOGI(TAG, "%03u %03u %03u", color.g, color.r, color.g);

        // printf("\n");
    }

    delay_us(250);

    return ESP_OK;
}


