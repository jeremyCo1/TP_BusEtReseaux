/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp280.h"
#include "request_RPi.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* --- USART2 --- */
char msg[1];
int it_uart;

/* --- USART3 --- */
char msg_Pi[1];
int it_uart_Pi;
char buf_Pi[20];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_CAN1_Init();
	MX_USART1_UART_Init();
	MX_USART3_UART_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	/* --- USART2 --- */
	it_uart = 0;
	HAL_UART_Receive_IT(&huart2,(uint8_t*)msg,1);
	/* -------------- */

	/* --- USART3 --- */
	it_uart_Pi = 0;
	HAL_UART_Receive_IT(&huart3,(uint8_t*)msg_Pi,1);
	int cptr_buf_Pi = 0;
	for(int i=0; i<20; i++){
		buf_Pi[i] = 0;
	}
	/* -------------- */

	/* --- BMP280 --- */
	HAL_I2C_Init(&hi2c1);

	int32_t temp_S32;
	uint32_t press_U32;

	BMP280_check_ID();							// print the device ID : 0x58
	BMP280_init();								// configure the device : mode normal, etc.
	BMP280_calibration();						// print the calibration registers
	BMP280_init_trimmingParameters();			// get the trimming parameters : dig_T1, etc.

	printf("---------------- TEMPERATURE ----------------\r\n");
	temp_S32 = BMP280_getTemperature();
	printf("Temperature : %ld_C\r\n", temp_S32/100);
	printf("------------------ PRESSURE -----------------\r\n");
	press_U32 = BMP280_getPressure();
	printf("Pression : %ld Pa\r\n", press_U32);
	/* -------------- */

	/* ---- CAN ---- */
	MOTOR_init();
	K = 1;
	char buff_K[4];
	/* ------------- */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
				MOTOR_angle(temp_S32*K);
		//		HAL_Delay(500);
		//		MOTOR_angle(0);
		//		HAL_Delay(500);

		if(it_uart){
			if(msg[0] == '\r'){
				printf("\r\n");
			}else{
				HAL_UART_Transmit(&huart2, (uint8_t*)msg, 1, HAL_MAX_DELAY);
			}
			it_uart = 0;
		}
		if(it_uart_Pi){
			if(msg_Pi[0] == '\r'){
				buf_Pi[cptr_buf_Pi] = '\0';
				//printf("%s\r\n", buf_Pi);
				if(strcmp(buf_Pi, "GET_T") == 0){
					RPi_GET_T();
				}else if(strcmp(buf_Pi, "GET_P") == 0){
					RPi_GET_P();
				}else{
					// SETK=XXXX
					for(int i=0; i<4; i++){
						buff_K[i] = buf_Pi[i+5];
					}
					buf_Pi[5] = '\0';
					if(strcmp(buf_Pi,"SETK=") == 0){
						RPi_SET_K(atoi(buff_K));
					}
				}

				for(int i=0; i<cptr_buf_Pi+1; i++){
					buf_Pi[i] = 0;
				}
				cptr_buf_Pi = 0;
			}else{
				buf_Pi[cptr_buf_Pi] = msg_Pi[0];
				cptr_buf_Pi++;
			}
			it_uart_Pi = 0;
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2){
		HAL_UART_Receive_IT(&huart2,(uint8_t*)msg,1);
		it_uart = 1;
	}else
		if(huart->Instance == USART3){
			HAL_UART_Receive_IT(&huart3,(uint8_t*)msg_Pi,1);
			it_uart_Pi = 1;
		}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
