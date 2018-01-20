#ifndef SENSOR_BME280_REG_H
#define SENSOR_BME280_REG_H


#define BMP280_ID                 0x58
#define BME280_ID                 0x60

#define BME280_ADDR_LOW           0x76
#define BME280_ADDR_HIGH          0x77

#define BME280_REG_ID             0xD0
#define BME280_REG_RESET          0xE0
#define BME280_REG_CTRL_HUM       0xF2
#define BME280_REG_STATUS         0xF3
#define BME280_REG_CTRL_MEAS      0xF4
#define BME280_REG_CONFIG         0xF5

#define BME280_REG_HUM_MSB        0xFD
#define BME280_REG_HUM_LSB        0xFE

#define BME280_REG_PRESS_MSB      0xF7
#define BME280_REG_PRESS_LSB      0xF8
#define BME280_REG_PRESS_XLSB     0xF9

#define BME280_REG_TEMP_MSB       0xFA
#define BME280_REG_TEMP_LSB       0xFB
#define BME280_REG_TEMP_XLSB      0xFC

#define BME280_REG_DIG_T1_LSB     0x88
#define BME280_REG_DIG_T2_LSB     0x8A
#define BME280_REG_DIG_T3_LSB     0x8C
#define BME280_REG_DIG_P1_LSB     0x8E
#define BME280_REG_DIG_P2_LSB     0x90
#define BME280_REG_DIG_P3_LSB     0x92
#define BME280_REG_DIG_P4_LSB     0x94
#define BME280_REG_DIG_P5_LSB     0x96
#define BME280_REG_DIG_P6_LSB     0x98
#define BME280_REG_DIG_P7_LSB     0x9A
#define BME280_REG_DIG_P8_LSB     0x9C
#define BME280_REG_DIG_P9_LSB     0x9E
#define BME280_REG_DIG_H1_MSB     0xA1
#define BME280_REG_DIG_H2_LSB     0xE1
#define BME280_REG_DIG_H3_MSB     0xE3
#define BME280_REG_DIG_H4_MSB     0xE4
#define BME280_REG_DIG_H5_MSB     0xE5
#define BME280_REG_DIG_H6_MSB     0xE7

#define BME280_MODE_SLEEP         0x00
#define BME280_MODE_FORCED        0x01
#define BME280_MODE_NORMAL        0x03

#define BME280_OVERSAMPL_HUM_SKIP 0x00
#define BME280_OVERSAMPL_HUM_X01  0x01
#define BME280_OVERSAMPL_HUM_X02  0x02
#define BME280_OVERSAMPL_HUM_X04  0x03
#define BME280_OVERSAMPL_HUM_X08  0x04
#define BME280_OVERSAMPL_HUM_X16  0x05

#define BME280_OVERSAMPL_TEM_SKIP 0x00
#define BME280_OVERSAMPL_TEM_X01  0x20
#define BME280_OVERSAMPL_TEM_X02  0x40
#define BME280_OVERSAMPL_TEM_X04  0x60
#define BME280_OVERSAMPL_TEM_X08  0x80
#define BME280_OVERSAMPL_TEM_X16  0xA0

#define BME280_OVERSAMPL_PRS_SKIP 0x00
#define BME280_OVERSAMPL_PRS_X01  0x04
#define BME280_OVERSAMPL_PRS_X02  0x08
#define BME280_OVERSAMPL_PRS_X04  0x0C
#define BME280_OVERSAMPL_PRS_X08  0x10
#define BME280_OVERSAMPL_PRS_X16  0x14

#define BME280_T_STANDBY_MS_1     0x00
#define BME280_T_STANDBY_MS_10    0xC0
#define BME280_T_STANDBY_MS_20    0xE0
#define BME280_T_STANDBY_MS_63    0x20
#define BME280_T_STANDBY_MS_125   0x40
#define BME280_T_STANDBY_MS_250   0x60
#define BME280_T_STANDBY_MS_500   0x80
#define BME280_T_STANDBY_MS_1000  0xA0

#define BME280_IIR_FLTR_COEFF_OFF 0x00
#define BME280_IIR_FLTR_COEFF_2   0x04
#define BME280_IIR_FLTR_COEFF_4   0x08
#define BME280_IIR_FLTR_COEFF_8   0x0C
#define BME280_IIR_FLTR_COEFF_16  0x10

#endif

