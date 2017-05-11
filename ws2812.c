#include "ws2812.h"

/*
 * Data transfer times
 *
 * WS2812:
 *
 * T0H 0.35 us
 * T0L 0.70 us
 * T1H 0.80 us
 * T1L 0.60 us
 *
 * WS2812B:
 *
 * T0H 0.40 us
 * T0L 0.80 us
 * T1H 0.85 us
 * T1L 0.45 us
 *
 */

#define WS2812_RMT_T0H 4
#define WS2812_RMT_T0L 8
#define WS2812_RMT_T1H 8
#define WS2812_RMT_T1L 5

static WS2812_color_t *color_buffers[RMT_CHANNEL_MAX] = { NULL };

static const char *TAG = "WS2812";

esp_err_t WS2812_init(struct WS2812_stripe_t *stripe) {
    esp_err_t esp_err;
    rmt_config_t rmt_cfg;

    rmt_cfg.rmt_mode = RMT_MODE_TX;
    rmt_cfg.channel = stripe->rmt_channel;
    rmt_cfg.gpio_num = stripe->gpio_num;
    rmt_cfg.mem_block_num = 1;
    rmt_cfg.clk_div = 8;                                        // 10 MHz (100ns period)

    rmt_cfg.tx_config.loop_en = 0;
    rmt_cfg.tx_config.carrier_freq_hz = 100;                    // Not used, but has to be set to avoid divide by 0 err
    rmt_cfg.tx_config.carrier_duty_percent = 50;
    rmt_cfg.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    rmt_cfg.tx_config.carrier_en = 0;                           // disable carrier
    rmt_cfg.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_cfg.tx_config.idle_output_en = 1;

    esp_err = rmt_config(&rmt_cfg);
    if (esp_err) {
        // TODO: Improve message
        ESP_LOGE(TAG, "Configuring stripe on RMT channel %u failed", rmt_cfg.channel);
        return esp_err;
    }

    esp_err = rmt_driver_install(rmt_cfg.channel, 0, 0);
    if (esp_err) {
        // TODO: Improve message
        ESP_LOGE(TAG, "Installing stripe on RMT channel %u failed", rmt_cfg.channel);
        return esp_err;
    }

    uint32_t buffer_size = sizeof(WS2812_color_t) * stripe->length;
    color_buffers[stripe->rmt_channel] = (WS2812_color_t*) malloc(buffer_size);

    WS2812_color_t black = { 0, 0, 0 };
    for (uint16_t num = 0; num < stripe->length; num++) {
        WS2812_set_color(stripe, num, &black);
    }

    return ESP_OK;
}

esp_err_t WS2812_set_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color)
{
    color_buffers[stripe->rmt_channel][num] = *color;
    return ESP_OK;
}

esp_err_t WS2812_get_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        struct WS2812_color_t *color)
{
    *color = color_buffers[stripe->rmt_channel][num];
    return ESP_OK;
}

esp_err_t WS2812_write(
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

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T0L;
            }

            item++;
        }

        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (color.r >> (i-1)) & 1;

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T0L;
            }

            item++;
        }

        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (color.b >> (i-1)) & 1;

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_RMT_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_RMT_T0L;
            }

            item++;
        }

        // not sure why this is necessary, although rmt_write_items
        // should be able to handle much more items
        if (num % 8 == 7) {
            rmt_write_items(channel, items, item_num, 1);
            // rmt_write_items is blocking, so rmt_wait_tx_done is not necessary
            item = 0;
        }
    }

    free(items);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    return ESP_OK;
}
