/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
//#define SERIAL_DEBUG
uint8_t rxBuffer[RX_SIZE];
uint8_t txBuffer[TX_SIZE];

uint8_t processBuffer[RX_SIZE];

uint8_t txSerialBuffer[TX_SIZE];

uint8_t rxFinished = IDLE;



/* USER CODE END 0 */

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart4_rx;
DMA_HandleTypeDef hdma_usart4_tx;

#ifdef SERIAL_DEBUG
/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}
#endif

/* USART4 init function */

void MX_USART4_UART_Init(void)
{

  huart4.Instance = USART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
#ifdef SERIAL_DEBUG
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
#endif // SERIAL_DEBUG
	  
	  
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
#ifdef SERIAL_DEBUG
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART4)
  {
  /* USER CODE BEGIN USART4_MspInit 0 */

  /* USER CODE END USART4_MspInit 0 */
    /* USART4 clock enable */
    __HAL_RCC_USART4_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART4 GPIO Configuration
    PC10     ------> USART4_TX
    PC11     ------> USART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART4 DMA Init */
    /* USART4_RX Init */
    hdma_usart4_rx.Instance = DMA1_Channel6;
    hdma_usart4_rx.Init.Request = DMA_REQUEST_12;
    hdma_usart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart4_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart4_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart4_rx);

    /* USART4_TX Init */
    hdma_usart4_tx.Instance = DMA1_Channel7;
    hdma_usart4_tx.Init.Request = DMA_REQUEST_12;
    hdma_usart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart4_tx.Init.Mode = DMA_NORMAL;
    hdma_usart4_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart4_tx);

    /* USART4 interrupt Init */
    HAL_NVIC_SetPriority(USART4_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART4_5_IRQn);
  /* USER CODE BEGIN USART4_MspInit 1 */

  /* USER CODE END USART4_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
#ifdef SERIAL_DEBUG
				  


    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
#endif // SERIAL_DEBUG
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART4)
  {
  /* USER CODE BEGIN USART4_MspDeInit 0 */

  /* USER CODE END USART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART4_CLK_DISABLE();

    /**USART4 GPIO Configuration
    PC10     ------> USART4_TX
    PC11     ------> USART4_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* USART4 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART4_5_IRQn);
  /* USER CODE BEGIN USART4_MspDeInit 1 */

  /* USER CODE END USART4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
	//Stop this DMA transmission
    HAL_UART_DMAStop(&huart4);  
                                                       
	//Calculate the length of the received data
	uint8_t data_length  = RX_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart4_rx);   
    
	//Test function: Print out the received data
    //printf("Receive Data(length = %d): ", data_length);
	//for (int i=0;i < data_length; i++) printf("%x", rxBuffer[i]);                     
	//printf("\r\n");
    
	//Zero Receiving Buffer
	
	memcpy(processBuffer, rxBuffer, data_length);
	
    memset(rxBuffer, 0, data_length);                                            
	data_length = 0;       
	
	//if (processBuffer[0] == 0xFF)  HAL_UART_Transmit(&huart2, "Slave address detected!\r\n", 30, 1000);
	//if (processBuffer[1] == 0x0A) HAL_UART_Transmit(&huart2, "Function address detected!\r\n", 30, 1000);
	//HAL_UART_Transmit(&huart2, processBuffer, 41, 1000);
	//HAL_UART_Transmit(&huart2, "\r\n", 30, 1000);
	//printf("\r\n");
	
	
	reloadRxDMA(&huart4);
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	static uint8_t firstIdle = SET;
	
	if(huart->Instance == USART4)                                   //Determine whether it is serial port 4
	{
		if (__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE) != RESET)   //Judging whether it is idle interruption
			{
				__HAL_UART_CLEAR_IDLEFLAG(&huart4);                      //Clear idle interrupt sign (otherwise it will continue to enter interrupt)
				//printf("\r\nUART4 Idle IQR Detected\r\n");
				if (firstIdle != SET )USAR_UART_IDLECallback(&huart4);                           //Call interrupt handler
				else firstIdle = RESET;
			}
	}
}

void reloadRxDMA(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART4)
	{
		rxFinished = SET;
		
		//make sure buffer is empty for next reception
		__HAL_UART_FLUSH_DRREGISTER(&huart4);
		__HAL_UART_SEND_REQ(&huart4, UART_RXDATA_FLUSH_REQUEST);
		__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_OREF);
				
		//reload interrupt
		if(HAL_UART_Receive_DMA(huart, (uint8_t *)rxBuffer, RX_SIZE) != HAL_OK)
		{
			Error_Handler();
		}	
	}
}



void flashWritePayload(uint8_t* rxBuff)
{
	//HAL_UART_Transmit(&huart2, "Payload: ", 20, 1000);
	//HAL_UART_Transmit(&huart2, rxBuff+2, 38, 1000);
	//HAL_UART_Transmit(&huart2, "\r\n", 20, 1000);
}




/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
