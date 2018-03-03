#include "periph/i2c.h"

static const char *TAG = "I2C";
static const uint32_t timeout = 100000;

esp_err_t I2C_init(
        i2c_port_t i2c_port,
        i2c_config_t *i2c_conf)
{
    esp_err_t err;

    if (i2c_port >= I2C_NUM_MAX) {
        ESP_LOGE(TAG, "I2C port is not available");
        return ESP_ERR_INVALID_ARG;
    }

    err = i2c_param_config(i2c_port, i2c_conf);
    if (err) {
        ESP_LOGE(TAG, "I2C parameter initialization failed");
        return err;
    }

    err = i2c_driver_install(i2c_port, i2c_conf->mode, 0, 0, 0);
    if (err) {
        ESP_LOGE(TAG, "I2C driver install failed");
        return err;
    }

    return ESP_OK;
}

esp_err_t I2C_master_write_slave_byte(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t data)
{
    return I2C_master_write_slave_buffer(
            i2c_port,
            i2c_slave_addr,
            i2c_slave_reg,
            &data, 1);
}

esp_err_t I2C_master_write_slave_buffer(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data_bytes,
        uint8_t data_bytes_num)
{
    // esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    ESP_ERROR_CHECK( i2c_master_start(cmd) );
    ESP_ERROR_CHECK( i2c_master_write_byte(cmd, (i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN) );
    ESP_ERROR_CHECK( i2c_master_write_byte(cmd, i2c_slave_reg, I2C_ACK_CHECK_EN) );

    if (data_bytes_num) {
        ESP_ERROR_CHECK( i2c_master_write(cmd, data_bytes, data_bytes_num, I2C_ACK_CHECK_EN) );
    }
    
    ESP_ERROR_CHECK( i2c_master_stop(cmd) );

    ESP_ERROR_CHECK( i2c_master_cmd_begin(i2c_port, cmd, timeout / portTICK_RATE_MS) );
    i2c_cmd_link_delete(cmd);

    return ESP_OK;
}

esp_err_t I2C_master_read_slave_byte(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data)
{
    return I2C_master_read_slave_buffer(
            i2c_port,
            i2c_slave_addr,
            i2c_slave_reg,
            data, 1);
}

esp_err_t I2C_master_read_slave_buffer(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint8_t *data_bytes,
        uint8_t data_bytes_num)
{
    if (data_bytes_num == 0) {
        return ESP_OK;
    }

    // esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // write register to read from
    ESP_ERROR_CHECK( i2c_master_start(cmd) );
    ESP_ERROR_CHECK( i2c_master_write_byte(cmd, (i2c_slave_addr << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_EN) );
    ESP_ERROR_CHECK( i2c_master_write_byte(cmd, i2c_slave_reg, I2C_ACK_CHECK_EN) );

    // read from register
    ESP_ERROR_CHECK( i2c_master_start(cmd) );
    ESP_ERROR_CHECK( i2c_master_write_byte(cmd, (i2c_slave_addr << 1) | I2C_MASTER_READ, I2C_ACK_CHECK_EN) );

    if (data_bytes_num > 1) {
        ESP_ERROR_CHECK( i2c_master_read(cmd, data_bytes, data_bytes_num - 1, I2C_ACK) );
    }

    ESP_ERROR_CHECK( i2c_master_read_byte(cmd, data_bytes + data_bytes_num - 1, I2C_NACK) );
    ESP_ERROR_CHECK( i2c_master_stop(cmd) );

    ESP_ERROR_CHECK( i2c_master_cmd_begin(i2c_port, cmd, timeout / portTICK_RATE_MS) );
    i2c_cmd_link_delete(cmd);

    return ESP_OK;
}

esp_err_t I2C_master_read_slave_bits_msb(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint64_t *data_bits,
        uint8_t data_bits_num)
{
    // esp_err_t err;
    uint64_t result = 0;

    uint8_t data_bits_num_mod = data_bits_num % 8;
    uint8_t data_bits_num_div = data_bits_num / 8;

    uint8_t data_bytes_num = (data_bits_num_mod == 0) ? data_bits_num_div : data_bits_num_div + 1;
    uint8_t data_bytes[data_bytes_num];
    ESP_ERROR_CHECK( I2C_master_read_slave_buffer(i2c_port, i2c_slave_addr, i2c_slave_reg, data_bytes, data_bytes_num) );

    uint8_t i = 0;

    while (data_bits_num > 8) {
        result = (result << 8) | data_bytes[i];
        data_bits_num -= 8;
        i++;
    }

    result = (result << data_bits_num) | (data_bytes[i] >> (8 - data_bits_num));
    *data_bits = result;

    return ESP_OK;
}

esp_err_t I2C_master_read_slave_bits_lsb(
        i2c_port_t i2c_port,
        uint8_t i2c_slave_addr,
        uint8_t i2c_slave_reg,
        uint64_t *data_bits,
        uint8_t data_bits_num)
{
    // esp_err_t err;
    uint64_t result = 0;

    uint8_t data_bits_num_mod = data_bits_num % 8;
    uint8_t data_bits_num_div = data_bits_num / 8;

    uint8_t data_bytes_num = (data_bits_num_mod == 0) ? data_bits_num_div : data_bits_num_div + 1;
    uint8_t data_bytes[data_bytes_num];
    ESP_ERROR_CHECK( I2C_master_read_slave_buffer(i2c_port, i2c_slave_addr, i2c_slave_reg, data_bytes, data_bytes_num) );

    uint8_t i = 0;
    data_bits_num = data_bytes_num * 8;

    while (data_bits_num != 0) {
        result |= ((uint64_t) data_bytes[i] << (i * 8));
        data_bits_num -= 8;
        i++;
    }

    *data_bits = result;

    return ESP_OK;
}
