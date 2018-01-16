#include "display/ssd1306_i2c.h"

static const char *TAG = "SSD1306";

// display init
static i2c_port_t _i2c_port = I2C_NUM_MAX;
static uint8_t _address = 0;

esp_err_t SSD1306_init(
        i2c_port_t i2c_port,
        uint8_t address)
{
    esp_err_t err;

    err = I2C_master_write_slave_byte(i2c_port, address, 0x00, SSD1306_REG_ENT_DSPL_RAM);
    if (err) {
        printf("ERROR\n");
        return ESP_ERR_INVALID_RESPONSE;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    err = I2C_master_write_slave_byte(i2c_port, address, 0x00, SSD1306_REG_ENT_DSPL_ON);
    if (err) {
        printf("ERROR\n");
        return ESP_ERR_INVALID_RESPONSE;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    _i2c_port = i2c_port;
    _address = address;

    return ESP_OK;
}

uint8_t SSD1306_initialized()
{
    return _address != 0;
}