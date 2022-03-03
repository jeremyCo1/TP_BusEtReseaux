/*
 * bmp280.c
 *
 *  Created on: Nov 6, 2021
 *      Author: jerem
 */

#include "bmp280.h"

/* Trimming parameters */
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;

static uint16_t dig_P1;
static int16_t dig_P2, dig_P3;
static int16_t dig_P4, dig_P5;
static int16_t dig_P6, dig_P7;
static int16_t dig_P8, dig_P9;

static int32_t t_fine;


uint8_t BMP280_check_ID(){
	HAL_StatusTypeDef ret;
	uint8_t sda_buf[1];
	sda_buf[0] = BMP280_ID_REG;

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_check_ID() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_check_ID() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}

	printf("--------------- Device Id ---------------\r\n");
	printf("Registre ID : 0x%x \r\n",sda_buf[0]);
	return 0;
}

uint8_t BMP280_init(){
	HAL_StatusTypeDef ret;
	uint8_t sda_buf[2];
	sda_buf[0] = BMP280_CONFIG_REG;
	sda_buf[1] = 3 | (1<<2) | (1<<4) | (1<<6);
	//(mode normal)	 (pressure oversampling *16) 	(temperature oversampling *2)

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_init() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_init() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}

	printf("--------------- BMP280 Config ---------------\r\n");
	printf("Registre CONFIG : 0x%x \r\n",sda_buf[0]);
	return 0;
}

uint8_t BMP280_calibration(){
	HAL_StatusTypeDef ret;
	uint8_t buf_len = 26;
	uint8_t sda_buf[buf_len];
	sda_buf[0] = BMP280_CALIB_REG;

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_calibration() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 26, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_calibration() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}

	printf("-------- BMP280 Registres Etalonnage --------\r\n");
	for(int i=0; i<26; i++){
		printf("  0x%x  ",sda_buf[i]);
	}printf("\r\n");

	return 0;
}

int32_t BMP280_getTemperature(){
	HAL_StatusTypeDef ret;
	int32_t temp_S32 = 0;
	uint8_t sda_buf[3];
	sda_buf[0] = BMP280_TEMP_REG;

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTemperature() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 3, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTemperature() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}

	temp_S32 = ((int32_t)(sda_buf[0])<<12) | ((int32_t)(sda_buf[1])<<4) | ((int32_t)(sda_buf[2])>>4);
	temp_S32 = BMP280_compensate_T_int32(temp_S32);  // A enlever si fonctionne pas !!

	return temp_S32;
}

uint32_t BMP280_getPressure(){
	HAL_StatusTypeDef ret;
	int32_t press_S32 = 0;
	uint32_t press_U32 = 0;
	uint8_t sda_buf[3];
	sda_buf[0] = BMP280_PRESS_REG;

	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getPressure() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 3, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getPressure() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}

	press_S32 = ((int32_t)(sda_buf[0])<<12) | ((int32_t)(sda_buf[1])<<4) | ((int32_t)(sda_buf[2])>>4);
	press_U32 = BMP280_compensate_P_int32(press_S32);

	return press_U32;
}

uint8_t BMP280_init_trimmingParameters(){
	uint8_t BMP280_DIG_T_REG;
	uint8_t BMP280_DIG_P_REG;

	// dig_T1
	BMP280_DIG_T_REG = 0x88; 		// LSB (0x89 : MSB)
	dig_T1 = BMP280_getTrimmingParametersU16(BMP280_DIG_T_REG);

	// dig_T1
	BMP280_DIG_T_REG = 0x8A; 		// LSB (0x8B : MSB)
	dig_T2 = BMP280_getTrimmingParametersS16(BMP280_DIG_T_REG);

	// dig_T1
	BMP280_DIG_T_REG = 0x8C; 		// LSB (0x8D : MSB)
	dig_T3 = BMP280_getTrimmingParametersS16(BMP280_DIG_T_REG);


	// dig_P1
	BMP280_DIG_P_REG = 0x8E; 		// LSB (0x8F : MSB)
	dig_P1 = BMP280_getTrimmingParametersU16(BMP280_DIG_P_REG);

	// dig_P2
	BMP280_DIG_P_REG = 0x90; 		// LSB (0x91 : MSB)
	dig_P2 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P3
	BMP280_DIG_P_REG = 0x92; 		// LSB (0x93 : MSB)
	dig_P3 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P4
	BMP280_DIG_P_REG = 0x94; 		// LSB (0x95 : MSB)
	dig_P4 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P5
	BMP280_DIG_P_REG = 0x96; 		// LSB (0x97 : MSB)
	dig_P5 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P6
	BMP280_DIG_P_REG = 0x98; 		// LSB (0x99 : MSB)
	dig_P6 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P7
	BMP280_DIG_P_REG = 0x9A; 		// LSB (0x9B : MSB)
	dig_P7 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P8
	BMP280_DIG_P_REG = 0x9C; 		// LSB (0x9D : MSB)
	dig_P8 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	// dig_P9
	BMP280_DIG_P_REG = 0x9E; 		// LSB (0x9F : MSB)
	dig_P9 = BMP280_getTrimmingParametersS16(BMP280_DIG_P_REG);

	return 0;
}

uint16_t BMP280_getTrimmingParametersU16(uint8_t BMP280_DIG_REG){
	HAL_StatusTypeDef ret;
	uint16_t digParameter;
	uint8_t sda_buf[2];

	sda_buf[0] = BMP280_DIG_REG;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTrimmingParametersU16() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTrimmingParametersU16() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}
	digParameter = ((uint16_t)(sda_buf[1])<<8) | ((uint16_t)(sda_buf[0]));

	return digParameter;
}

int16_t BMP280_getTrimmingParametersS16(uint8_t BMP280_DIG_REG){
	HAL_StatusTypeDef ret;
	int16_t digParameter;
	uint8_t sda_buf[2];

	sda_buf[0] = BMP280_DIG_REG;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BMP280_ADDR, sda_buf, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTrimmingParametersS16() - I2C TRANSMIT ERROR : %d\r\n", ret);
		return ret;
	}
	ret = HAL_I2C_Master_Receive(&hi2c1, BMP280_ADDR, sda_buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("BMP280_getTrimmingParametersS16() - I2C RECEIVE ERROR : %d\r\n", ret);
		return ret;
	}
	digParameter = ((int16_t)(sda_buf[1])<<8) | ((int16_t)(sda_buf[0]));

	return digParameter;
}

int32_t BMP280_compensate_T_int32(int32_t adc_T){
	int32_t var1, var2, T;

	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;

	return T;
}

uint32_t BMP280_compensate_P_int32(int32_t adc_P){
	int32_t var1, var2;
	uint32_t p;

	var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11) * ((int32_t)dig_P6);
	var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >>13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
	var1 = ((((32768+var1))*((int32_t)dig_P1))>>15);

	if(var1 == 0){
		return 0; // Avoid excession caused by division by zero
	}
	p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if(p<0x80000000){
		p = (p << 1) / ((uint32_t)var1);
	}else{
		p = (p / (uint32_t)var1) * 2;
	}

	var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
	p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));

	return (uint32_t)p;
}


