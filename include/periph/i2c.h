#ifndef ESP32_DRIVER_I2C
#define ESP32_DRIVER_I2C

#include "esp_err.h"
#include "esp_log.h"
#include "esp_types.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

typedef enum {
    I2C_ACK_CHECK_EN = 0,
    I2C_ACK_CHECK_DIS,
} i2c_ack_check_t;

typedef enum {
    I2C_ACK = 0,
    I2C_NACK,
} i2c_ack_val_t;

esp_err_t I2C_init(
        i2c_port_t i2c_port,
        i2c_config_t *i2c_conf);

esp_err_t I2C_master_write_slave_byte(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t data);

esp_err_t I2C_master_write_slave_buffer(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data,
        uint8_t data_length);

esp_err_t I2C_master_read_slave_byte(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data);

esp_err_t I2C_master_read_slave_buffer(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data,
        uint8_t data_length);

esp_err_t I2C_master_read_slave_bits_msb(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint64_t *data_bits,
        uint8_t data_bits_num);

esp_err_t I2C_master_read_slave_bits_lsb(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint64_t *data_bits,
        uint8_t data_bits_num);

#endif
