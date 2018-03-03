#ifndef SENSOR_BME280_I2C_H
#define SENSOR_BME280_I2C_H

/**
 * For informations and the datasheet for BME280, visit
 * https://www.bosch-sensortec.com/bst/products/all_products/bme280
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

#include "periph/i2c.h"
#include "periph/sensor/bme280_reg.h"

int32_t  BME280_temp_sint;
uint32_t BME280_press_uint;
uint32_t BME280_hum_uint;

/**
 * Initializes BME280 and stores address.
 */
esp_err_t BME280_init(
        i2c_port_t i2c_port,
        uint8_t address);

/**
 * Returns temperature in DegC, resolution is 0.01 DegC. Output value of "5123" equals 51.23 DegC.
 * t_fine carries fine temperature as global value
 */
int32_t BME280_get_temperature_int32();

/**
 * Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
 * Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
 */
uint32_t BME280_get_pressure_int64();

/**
 * Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
 * Output value of "47445" represents 47445/1024 = 46.333 %RH
 */
uint32_t BME280_get_humidity_int32();

/**
 * Returns temperature in DegC, double precision.
 * Output value of "21.53" equals 21.53 DegC.
 * t_fine carries fine temperature as global value
 */
double BME280_get_temperature_double();

/**
 * Returns pressure in Pa as double.
 * Output value of "96386.2" equals 96386.2 Pa = 963.862 hPa
 */
double BME280_get_pressure_double();

/**
 * Returns humidity in %rH as as double.
 * Output value of "46.332" represents 46.332 %rH
 */
double BME280_get_humidity_double();

#endif
