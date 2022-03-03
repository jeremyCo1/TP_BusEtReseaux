/*
 * request_RPi.h
 *
 *  Created on: Nov 7, 2021
 *      Author: jerem
 */

#ifndef INC_REQUEST_RPI_H_
#define INC_REQUEST_RPI_H_

#include "bmp280.h"

uint8_t RPi_GET_T();
uint8_t RPi_GET_P();
uint8_t RPi_SET_K(int K);

int K;

#endif /* INC_REQUEST_RPI_H_ */
