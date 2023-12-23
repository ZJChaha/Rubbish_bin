/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */


#include <stdio.h>
#include <stdarg.h>
#include "string.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */
typedef __packed struct
{
	uint8_t begin1_data;//0x0D
	uint8_t begin2_data;//0x0A
	float yaw;
	float pitch;
	float roll;
	float x_buffer;
	float y_buffer;
	float yaw_angle_speed;
	uint8_t end1_data;//0x0A
	uint8_t end2_data;//0x0D
} action_data_t;

typedef struct
{

short x;
short y;


}action_now_t;



extern uint32_t voice_command;
extern uint8_t Rx_buffer;
extern uint8_t Tx_buffer;	
extern uint8_t Tx_on_demand[16];
extern void uart_command(uint32_t ID);


extern action_data_t  Action;
extern action_now_t Action_now;
void uart_receive_handler(UART_HandleTypeDef *huart);
void uart_rx_idle_callback(UART_HandleTypeDef *huart);
void usart_init(void);







/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void usart_printf(const char *fmt,...);



/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

