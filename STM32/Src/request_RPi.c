/*
 * request_RPi.c
 *
 *  Created on: Nov 7, 2021
 *      Author: jerem
 */

#include "request_RPi.h"

// temp = 1250
// print T=+12.50_C


uint8_t RPi_GET_T(){
	int32_t temp_S32 = 0;
	int16_t temp_1, temp_2;

	temp_S32 = BMP280_getTemperature();
	temp_1 = (int16_t)(temp_S32/100);
	temp_2 = (int16_t)(temp_S32%100);

	printf("T=+%02d.%02d_C\r\n", temp_1, temp_2);

	return 0;
}

// P=102300Pa
uint8_t RPi_GET_P(){
	uint32_t press_U32 = 0;

	press_U32 = BMP280_getPressure();
	printf("P=%ldPa\r\n", press_U32);

	return 0;
}

uint8_t RPi_SET_K(int coeff){
	K = coeff/100;

	return 0;
}

