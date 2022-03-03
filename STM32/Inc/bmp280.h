/*
 * bmp280.h
 *
 *  Created on: Nov 6, 2021
 *      Author: jerem
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stdio.h"
#include "i2c.h"

//extern I2C_HandleTypeDef hi2c1;

static const uint8_t BMP280_ADDR = 0x77<<1;

static const uint8_t BMP280_ID_REG = 0xD0;
static const uint8_t BMP280_CONFIG_REG = 0xF4;
static const uint8_t BMP280_CALIB_REG = 0xA1;
static const uint8_t BMP280_TEMP_REG = 0xFA;   // MSB
static const uint8_t BMP280_PRESS_REG = 0xF7;  // MSB

uint8_t BMP280_check_ID();
uint8_t BMP280_init();
uint8_t BMP280_calibration();

uint8_t BMP280_init_trimmingParameters();
uint16_t BMP280_getTrimmingParametersU16(uint8_t BMP280_DIG_REG);
int16_t BMP280_getTrimmingParametersS16(uint8_t BMP280_DIG_REG);

int32_t BMP280_getTemperature();	// The user just have to call getTemperature()
int32_t BMP280_compensate_T_int32(int32_t adc_T);

uint32_t BMP280_getPressure();		// The user just have to call getPressure()
uint32_t BMP280_compensate_P_int32(int32_t adc_P);

#endif /* INC_BMP280_H_ */
