#include "periphery/sensor/bme280_i2c.h"

static const char *TAG = "BME280";

// sensor init
static i2c_port_t _i2c_port = I2C_NUM_MAX;
static uint8_t _address = 0;

// calibration values
static uint16_t dig_T1;
static int16_t  dig_T2;
static int16_t  dig_T3;
static uint16_t dig_P1;
static int16_t  dig_P2;
static int16_t  dig_P3;
static int16_t  dig_P4;
static int16_t  dig_P5;
static int16_t  dig_P6;
static int16_t  dig_P7;
static int16_t  dig_P8;
static int16_t  dig_P9;
static uint8_t  dig_H1;
static int16_t  dig_H2;
static uint8_t  dig_H3;
static int16_t  dig_H4;
static int16_t  dig_H5;
static int8_t   dig_H6;

// global temp variable
static int32_t t_fine;

// typedefs to avoid renaming of datasheet types
typedef uint32_t BME280_U32_t;
typedef int32_t  BME280_S32_t;
typedef int64_t  BME280_S64_t;

esp_err_t BME280_init(
        i2c_port_t i2c_port,
        uint8_t address)
{
    esp_err_t err;
    uint8_t byte;
    uint64_t data;

    err = I2C_master_read_slave_byte(i2c_port, address, BME280_REG_ID, &byte);
    if (err) {
        ESP_LOGE(TAG, "Cannot read from slave device");
        return ESP_ERR_INVALID_RESPONSE;
    }

    if ((uint8_t) byte != BME280_ID && (uint8_t) byte != BMP280_ID) {
        ESP_LOGE(TAG, "Read data %u does not match BME280_ID %u", byte, BME280_ID);
        return ESP_ERR_INVALID_RESPONSE;
    }

    I2C_master_write_slave_byte(i2c_port, address, BME280_REG_CTRL_HUM,
            BME280_OVERSAMPL_HUM_X01);

    I2C_master_write_slave_byte(i2c_port, address, BME280_REG_CTRL_MEAS,
            BME280_MODE_NORMAL |
            BME280_OVERSAMPL_TEM_X01 |
            BME280_OVERSAMPL_PRS_X01);

    I2C_master_write_slave_byte(i2c_port, address, BME280_REG_CONFIG,
            BME280_T_STANDBY_MS_1000);

    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_T1_LSB, &data, 16);
    dig_T1 = (uint16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_T2_LSB, &data, 16);
    dig_T2 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_T3_LSB, &data, 16);
    dig_T3 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P1_LSB, &data, 16);
    dig_P1 = (uint16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P2_LSB, &data, 16);
    dig_P2 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P3_LSB, &data, 16);
    dig_P3 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P4_LSB, &data, 16);
    dig_P4 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P5_LSB, &data, 16);
    dig_P5 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P6_LSB, &data, 16);
    dig_P6 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P7_LSB, &data, 16);
    dig_P7 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P8_LSB, &data, 16);
    dig_P8 = (int16_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_P9_LSB, &data, 16);
    dig_P9 = (int16_t) data;
    I2C_master_read_slave_bits_msb(i2c_port, address, BME280_REG_DIG_H1_MSB, &data,  8);
    dig_H1 = (uint8_t) data;
    I2C_master_read_slave_bits_lsb(i2c_port, address, BME280_REG_DIG_H2_LSB, &data, 16);
    dig_H2 = (int16_t) data;
    I2C_master_read_slave_bits_msb(i2c_port, address, BME280_REG_DIG_H3_MSB, &data,  8);
    dig_H3 = (uint8_t) data;
    I2C_master_read_slave_bits_msb(i2c_port, address, BME280_REG_DIG_H4_MSB, &data, 12);
    dig_H4 = (int16_t) data;
    // dig_H5 is a special case, see below
    I2C_master_read_slave_bits_msb(i2c_port, address, BME280_REG_DIG_H6_MSB, &data,  8);
    dig_H6 = (int8_t) data;

    uint16_t d_H5;
    I2C_master_read_slave_bits_msb(i2c_port, address, BME280_REG_DIG_H5_MSB, &data, 12);
    d_H5 = (uint16_t) data;
    dig_H5 = (int16_t) ((d_H5 << 8) & 0x0F) | ((d_H5 >> 4) & 0xFF0);

    _i2c_port = i2c_port;
    _address = address;

    return ESP_OK;
}

uint8_t BME280_initialized()
{
    return _address != 0;
}

int32_t BME280_get_temperature_int32()
{
    uint64_t data;
    int32_t adc_T;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_TEMP_MSB, &data, 20);

    ESP_LOGD(TAG, "data:   %lld", data);
    ESP_LOGD(TAG, "dig_T1: %u", dig_T1);
    ESP_LOGD(TAG, "dig_T2: %d", dig_T2);
    ESP_LOGD(TAG, "dig_T3: %d", dig_T3);

    adc_T = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    BME280_S32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((BME280_S32_t) dig_T1 << 1)))
            * ((BME280_S32_t) dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t) dig_T1))
            * ((adc_T >> 4) - ((BME280_S32_t) dig_T1))) >> 12)
            * ((BME280_S32_t) dig_T3)) >> 14;
    t_fine = var1 + var2;
    BME280_temp_sint = (t_fine * 5 + 128) >> 8;
    return BME280_temp_sint;
}

uint32_t BME280_get_pressure_int64()
{
    uint64_t data;
    int32_t adc_P;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_PRESS_MSB, &data, 20);

    ESP_LOGD(TAG, "data:   %llu", data);
    ESP_LOGD(TAG, "t_fine: %d", t_fine);
    ESP_LOGD(TAG, "dig_P1: %u", dig_P1);
    ESP_LOGD(TAG, "dig_P2: %d", dig_P2);
    ESP_LOGD(TAG, "dig_P3: %d", dig_P3);
    ESP_LOGD(TAG, "dig_P4: %d", dig_P4);
    ESP_LOGD(TAG, "dig_P5: %d", dig_P5);
    ESP_LOGD(TAG, "dig_P6: %d", dig_P6);
    ESP_LOGD(TAG, "dig_P7: %d", dig_P7);
    ESP_LOGD(TAG, "dig_P8: %d", dig_P8);
    ESP_LOGD(TAG, "dig_P9: %d", dig_P9);

    adc_P = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    BME280_S64_t var1, var2, p;
    var1 = ((BME280_S64_t) t_fine) - 128000;
    var2 = var1 * var1 * (BME280_S64_t) dig_P6;
    var2 = var2 + ((var1 * (BME280_S64_t) dig_P5) << 17);
    var2 = var2 + (((BME280_S64_t) dig_P4) << 35);
    var1 = ((var1 * var1 * (BME280_S64_t) dig_P3) >> 8)
            + ((var1 * (BME280_S64_t) dig_P2) << 12);
    var1 = (((((BME280_S64_t) 1) << 47) + var1)) * ((BME280_S64_t) dig_P1)
            >> 33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BME280_S64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BME280_S64_t) dig_P8) * p) >> 19;
    BME280_press_uint = (BME280_U32_t) ((p + var1 + var2) >> 8) + (((BME280_S64_t) dig_P7) << 4);
    return BME280_press_uint;
}

uint32_t BME280_get_humidity_int32()
{
    uint64_t data;
    int32_t adc_H;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_HUM_MSB, &data, 16);

    ESP_LOGD(TAG, "data:   %lld", data);
    ESP_LOGD(TAG, "t_fine: %d", t_fine);
    ESP_LOGD(TAG, "dig_H1: %u", dig_H1);
    ESP_LOGD(TAG, "dig_H2: %d", dig_H2);
    ESP_LOGD(TAG, "dig_H3: %u", dig_H3);

    adc_H = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    BME280_S32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((BME280_S32_t) 76800));
    v_x1_u32r =
            (((((adc_H << 14) - (((BME280_S32_t) dig_H4) << 20)
                    - (((BME280_S32_t) dig_H5) * v_x1_u32r))
                    + ((BME280_S32_t) 16384)) >> 15)
                    * (((((((v_x1_u32r * ((BME280_S32_t) dig_H6)) >> 10)
                            * (((v_x1_u32r * ((BME280_S32_t) dig_H3)) >> 11)
                                    + ((BME280_S32_t) 32768))) >> 10)
                            + ((BME280_S32_t) 2097152))
                            * ((BME280_S32_t) dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r
            - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
                    * ((BME280_S32_t) dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    BME280_hum_uint = (BME280_U32_t) (v_x1_u32r >> 12);
    return BME280_hum_uint;
}

double BME280_get_temperature_double()
{
    uint64_t data;
    int32_t adc_T;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_TEMP_MSB, &data, 20);

    ESP_LOGD(TAG, "data:   %lld", data);
    ESP_LOGD(TAG, "t_fine: %d", t_fine);
    ESP_LOGD(TAG, "dig_T1: %u", dig_T1);
    ESP_LOGD(TAG, "dig_T2: %d", dig_T2);
    ESP_LOGD(TAG, "dig_T3: %d", dig_T3);

    adc_T = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    double var1, var2, T;
    var1 = (((double)adc_T)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T)/131072.0 - ((double)dig_T1)/8192.0) *
    (((double)adc_T)/131072.0 - ((double) dig_T1)/8192.0)) * ((double)dig_T3);
    t_fine = (BME280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;
}

double BME280_get_pressure_double()
{
    uint64_t data;
    int32_t adc_P;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_PRESS_MSB, &data, 20);

    ESP_LOGD(TAG, "data:   %lld", data);
    ESP_LOGD(TAG, "t_fine: %d", t_fine);
    ESP_LOGD(TAG, "dig_P1: %u", dig_P1);
    ESP_LOGD(TAG, "dig_P2: %d", dig_P2);
    ESP_LOGD(TAG, "dig_P3: %d", dig_P3);
    ESP_LOGD(TAG, "dig_P4: %d", dig_P4);
    ESP_LOGD(TAG, "dig_P5: %d", dig_P5);
    ESP_LOGD(TAG, "dig_P6: %d", dig_P6);
    ESP_LOGD(TAG, "dig_P7: %d", dig_P7);
    ESP_LOGD(TAG, "dig_P8: %d", dig_P8);
    ESP_LOGD(TAG, "dig_P9: %d", dig_P9);

    adc_P = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    double var1, var2, p;
    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)dig_P4) * 65536.0);
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((double)dig_P1);
    if (var1 == 0.0)
    {
    return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    return p;
}

double BME280_get_humidity_double()
{
    uint64_t data;
    int32_t adc_H;

    I2C_master_read_slave_bits_msb(_i2c_port, _address, BME280_REG_HUM_MSB, &data, 16);

    ESP_LOGD(TAG, "data:   %lld", data);
    ESP_LOGD(TAG, "t_fine: %d", t_fine);
    ESP_LOGD(TAG, "dig_H1: %u", dig_H1);
    ESP_LOGD(TAG, "dig_H2: %d", dig_H2);
    ESP_LOGD(TAG, "dig_H3: %u", dig_H3);

    adc_H = (int32_t) data;

    // Code from datasheet, formatted by Eclipse

    double var_H;
    var_H = (((double)t_fine) - 76800.0);
    var_H = (adc_H - (((double)dig_H4) * 64.0 + ((double)dig_H5) / 16384.0 * var_H)) *
    (((double)dig_H2) / 65536.0 * (1.0 + ((double)dig_H6) / 67108864.0 * var_H *
    (1.0 + ((double)dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
    var_H = 100.0;
    else if (var_H < 0.0)
    var_H = 0.0;
    return var_H;
}
