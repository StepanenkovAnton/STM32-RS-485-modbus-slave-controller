/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;

/* USER CODE BEGIN Private defines */
#define RX_SIZE 256
#define TX_SIZE 256
	 
#define IDLE 2
	 
//#define ACK_HEADER      0xB0
//#define ACK_CRC         0x4F

#define ACK_CHECKSUM_POS 2
#define ACK_PACKET_SIZE  4
/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);
void MX_USART4_UART_Init(void);

/* USER CODE BEGIN Prototypes */
	 
extern uint8_t rxBuffer[RX_SIZE];
extern uint8_t txBuffer[TX_SIZE];
	 
extern uint8_t processBuffer[RX_SIZE];
extern uint8_t rxBuffer[RX_SIZE];

extern uint8_t txSerialBuffer[TX_SIZE];

extern  uint8_t rxFinished;
	 
	 
void reloadRxDMA(UART_HandleTypeDef *huart);
void packetProcessor(uint8_t* rxBuff, uint8_t mode);
void flashWritePayload(uint8_t* rxBuff);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
