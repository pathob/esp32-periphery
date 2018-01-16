#ifndef DISPLAY_SSD1306_I2C_H
#define DISPLAY_SSD1306_I2C_H

/**
 * For informations and the datasheet for SSD1306, visit
 * https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

#include "i2c.h"
#include "display/ssd1306_reg.h"

/**
 * Initializes SSD1306 and stores address.
 */
esp_err_t SSD1306_init(
        i2c_port_t i2c_port,
        uint8_t address);

#endif