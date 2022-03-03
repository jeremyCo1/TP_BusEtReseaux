/*
 * motor.h
 *
 *  Created on: Nov 10, 2021
 *      Author: jerem
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_


#include "can.h"
#include "stdio.h"

uint8_t MOTOR_init();
uint8_t MOTOR_angle(int16_t angle);



#endif /* INC_MOTOR_H_ */
