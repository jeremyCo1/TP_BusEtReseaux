/*
 * motor.c
 *
 *  Created on: Nov 10, 2021
 *      Author: jerem
 */


#include "motor.h"

uint32_t TxMailbox;
CAN_TxHeaderTypeDef pHeader;

uint8_t MOTOR_init(){
	HAL_CAN_Start(&hcan1);

	pHeader.StdId = 0x61;
	pHeader.ExtId = 0;
	pHeader.IDE = CAN_ID_STD;
	pHeader.RTR = CAN_RTR_DATA;
	pHeader.DLC = 2;
	pHeader.TransmitGlobalTime = DISABLE;

	return 0;
}

uint8_t MOTOR_angle(int16_t angle){
	uint8_t aData[2];
	int signe;
	if(angle<0){
		angle = -angle;
		signe = 1;
	}else{
		signe = 0;
	}
	aData[0] = angle;
	aData[1] = signe;

	HAL_CAN_AddTxMessage(&hcan1, &pHeader, aData, &TxMailbox);

	return 0;
}


