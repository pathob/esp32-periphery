#include "periphery/ws2812.h"

static WS2812_color_t *color_buffers[RMT_CHANNEL_MAX] = { NULL };

extern const uint8_t WS2812_gamma_correction[];
extern const uint8_t WS2812_rgb_correction_g[];
extern const uint8_t WS2812_rgb_correction_b[];

static const char *TAG = "WS2812";

esp_err_t WS2812_init(struct WS2812_stripe_t *stripe) {
    esp_err_t esp_err;
    rmt_config_t rmt_cfg;

    rmt_cfg.rmt_mode      = RMT_MODE_TX;
    rmt_cfg.channel       = stripe->rmt_channel;
    rmt_cfg.gpio_num      = stripe->gpio_num;
    rmt_cfg.mem_block_num = 8 - stripe->rmt_channel;
    rmt_cfg.clk_div       = 8;                                  // 10 MHz (100ns period)

    rmt_cfg.tx_config.loop_en              = 0;
    rmt_cfg.tx_config.carrier_freq_hz      = 10000;             // Not used, but has to be set to avoid divide by 0 err
    rmt_cfg.tx_config.carrier_duty_percent = 50;
    rmt_cfg.tx_config.carrier_en           = 0;                 // disable carrier
    rmt_cfg.tx_config.carrier_level        = RMT_CARRIER_LEVEL_HIGH;
    rmt_cfg.tx_config.idle_output_en       = 1;
    rmt_cfg.tx_config.idle_level           = RMT_IDLE_LEVEL_LOW;

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

esp_err_t IRAM_ATTR WS2812_set_color(
        struct WS2812_stripe_t *stripe,
        uint16_t num,
        const struct WS2812_color_t *color)
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
    uint32_t item_num = stripe->length * 24;
    uint32_t size = sizeof(rmt_item32_t) * item_num;
    //each item represent a cycle of waveform.
    rmt_item32_t* items = (rmt_item32_t*) malloc(size);
    memset((void*) items, 0, size);

    uint16_t item = 0;

    for (uint16_t num = 0; num < stripe->length; num++) {
        WS2812_color_t color = color_buffers[stripe->rmt_channel][num];

        // RGB -> GRB

        uint8_t g = WS2812_rgb_correction_g[color.g];
        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (g >> (i-1)) & 1;

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T0L;
            }
            item++;
        }

        uint8_t r = color.r;
        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (r >> (i-1)) & 1;

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T0L;
            }
            item++;
        }

        uint8_t b = WS2812_rgb_correction_b[color.b];
        for (uint8_t i = 8; i > 0; i--) {
            uint8_t bit = (b >> (i-1)) & 1;

            if (bit) {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T1H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T1L;
            } else {
                items[item].level0 = 1;
                items[item].duration0 = WS2812_T0H;
                items[item].level1 = 0;
                items[item].duration1 = WS2812_T0L;
            }
            item++;
        }
    }

    rmt_write_items(channel, items, item_num, 1);

    free(items);
    delay_us(50);

    return ESP_OK;
}

esp_err_t WS2812_reset(
        struct WS2812_stripe_t *stripe)
{
    WS2812_color_t black = { 0, 0, 0 };

    for (uint8_t x = 0; x < stripe->length; x++) {
        WS2812_set_color(stripe, x, &black);
    }

    WS2812_write(stripe);

    return ESP_OK;
}

const uint8_t WS2812_rgb_correction_g[] = {
    0, 0, 1, 2, 3, 3, 4, 5,
    6, 7, 7, 8, 9, 10, 10, 11,
    12, 13, 14, 14, 15, 16, 17, 18,
    18, 19, 20, 21, 21, 22, 23, 24,
    25, 25, 26, 27, 28, 29, 29, 30,
    31, 32, 32, 33, 34, 35, 36, 36,
    37, 38, 39, 40, 40, 41, 42, 43,
    43, 44, 45, 46, 47, 47, 48, 49,
    50, 50, 51, 52, 53, 54, 54, 55,
    56, 57, 58, 58, 59, 60, 61, 61,
    62, 63, 64, 65, 65, 66, 67, 68,
    69, 69, 70, 71, 72, 72, 73, 74,
    75, 76, 76, 77, 78, 79, 80, 80,
    81, 82, 83, 83, 84, 85, 86, 87,
    87, 88, 89, 90, 90, 91, 92, 93,
    94, 94, 95, 96, 97, 98, 98, 99,
    100, 101, 101, 102, 103, 104, 105, 105,
    106, 107, 108, 109, 109, 110, 111, 112,
    112, 113, 114, 115, 116, 116, 117, 118,
    119, 120, 120, 121, 122, 123, 123, 124,
    125, 126, 127, 127, 128, 129, 130, 130,
    131, 132, 133, 134, 134, 135, 136, 137,
    138, 138, 139, 140, 141, 141, 142, 143,
    144, 145, 145, 146, 147, 148, 149, 149,
    150, 151, 152, 152, 153, 154, 155, 156,
    156, 157, 158, 159, 160, 160, 161, 162,
    163, 163, 164, 165, 166, 167, 167, 168,
    169, 170, 170, 171, 172, 173, 174, 174,
    175, 176, 177, 178, 178, 179, 180, 181,
    181, 182, 183, 184, 185, 185, 186, 187,
    188, 189, 189, 190, 191, 192, 192, 193,
    194, 195, 196, 196, 197, 198, 199, 200
};

const uint8_t WS2812_rgb_correction_b[] = {
    0, 0, 1, 1, 2, 3, 3, 4,
    5, 5, 6, 6, 7, 8, 8, 9,
    10, 10, 11, 11, 12, 13, 13, 14,
    15, 15, 16, 16, 17, 18, 18, 19,
    20, 20, 21, 21, 22, 23, 23, 24,
    25, 25, 26, 26, 27, 28, 28, 29,
    30, 30, 31, 32, 32, 33, 33, 34,
    35, 35, 36, 37, 37, 38, 38, 39,
    40, 40, 41, 42, 42, 43, 43, 44,
    45, 45, 46, 47, 47, 48, 48, 49,
    50, 50, 51, 52, 52, 53, 53, 54,
    55, 55, 56, 57, 57, 58, 58, 59,
    60, 60, 61, 62, 62, 63, 64, 64,
    65, 65, 66, 67, 67, 68, 69, 69,
    70, 70, 71, 72, 72, 73, 74, 74,
    75, 75, 76, 77, 77, 78, 79, 79,
    80, 80, 81, 82, 82, 83, 84, 84,
    85, 85, 86, 87, 87, 88, 89, 89,
    90, 90, 91, 92, 92, 93, 94, 94,
    95, 96, 96, 97, 97, 98, 99, 99,
    100, 101, 101, 102, 102, 103, 104, 104,
    105, 106, 106, 107, 107, 108, 109, 109,
    110, 111, 111, 112, 112, 113, 114, 114,
    115, 116, 116, 117, 117, 118, 119, 119,
    120, 121, 121, 122, 122, 123, 124, 124,
    125, 126, 126, 127, 128, 128, 129, 129,
    130, 131, 131, 132, 133, 133, 134, 134,
    135, 136, 136, 137, 138, 138, 139, 139,
    140, 141, 141, 142, 143, 143, 144, 144,
    145, 146, 146, 147, 148, 148, 149, 149,
    150, 151, 151, 152, 153, 153, 154, 154,
    155, 156, 156, 157, 158, 158, 159, 160
};

