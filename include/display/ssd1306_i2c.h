#ifndef DISPLAY_SSD1306_I2C_H
#define DISPLAY_SSD1306_I2C_H

/**
 * For informations and the datasheet for SSD1306, visit
 * https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
 */

#include "esp_log.h"
#include "driver/i2c.h"

#include <stdio.h>
#include <string.h>

#include "font.h"
#include "i2c.h"
#include "display/ssd1306_reg.h"

#include "font/lcd5x7.h"

/**
 * Initializes SSD1306 and stores address.
 */
esp_err_t SSD1306_init(
        i2c_port_t i2c_port,
        uint8_t address);

void SSD1306_set_text_6x8(
    FONT_6x8_t *font,
    char *text,
    uint8_t x_start,
    uint8_t y_start);

void SSD1306_fill_pixel(
    uint8_t x,
    uint8_t y);

void SSD1306_fill_rect(
    uint8_t x_start,
    uint8_t y_start,
    uint8_t x_end,
    uint8_t y_end);

void SSD1306_set_bitmap(
    uint8_t *bitmap,
    uint8_t width,
    uint8_t height,
    uint8_t x_start,
    uint8_t y_start);

void SSD1306_display();

#endif