/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ****************************************************printImage(client);**************************
  * @attention
  *
  * <h2><center>&copy; Copyright timeTick(c) 2020 STMicroelectronics.
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
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flash_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* Private variables ---------------------------------------------------------*/
typedef void (*pFunction)(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define SERIAL_DEBUG
#define RS485_RTS_LOW	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)
#define RS485_RTS_HIGH 	HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)

#define RS485_RE_LOW	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET)
#define RS485_RE_HIGH 	HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET)


#define ERASE_FLASH 0
#define PROGRAM_FLASH 1
#define PATCH_FLASH 2
#define PATCH_REQUEST 3
#define STATUS 4
#define RESTART_PROCESS 5
#define RESET_MCU 6

#define PACKET_TYPES 7

//state machine
#define WAITING_PACKET 0
#define PROCESSING_SLAVE_ADDRESS 1
#define PROCESSING_HEADER 2

//Some sizes of arrays
#define FAILED_PACKETS_ARRAY_SIZE 200

//DEVICE ADDRESS
#define MASTER_ADDRESS 0x00
#define SLAVE_ADDRESS  0x01


//INDEX
//RX
#define SLAVE_ADDRESS_RX_POS 0
#define HEADER 1
#define PACKET_RX_POS 2

//TX
#define MASTER_ADDRESS_POS   0
#define SLAVE_ADDRESS_POS    1
#define FUNCTION_ADDRESS_POS 2
#define PACKET_POS           3

#define PAYLOAD_OFFSET    2
#define PATCH_OFFSET      4
#define FLASH_SIZE_OFFSET 2

//List of Requests
#define RESEND_CHUNK 0

//Sizes
#define MASTER_ADDRESS_SIZE   1
#define SLAVE_ADDRESS_SIZE	  1
#define FUNCTION_ADDRESS_SIZE 1
#define PACKET_SIZE           2
#define STATUS_CODE_SIZE	  2
#define CRC_SIZE              1

#define PAYLOAD_SIZE 128

#define STATUS_PACKET_SIZE  (MASTER_ADDRESS_SIZE + SLAVE_ADDRESS_SIZE + STATUS_CODE_SIZE + CRC_SIZE)
#define REQUEST_PACKET_SIZE (MASTER_ADDRESS_SIZE + SLAVE_ADDRESS_SIZE + FUNCTION_ADDRESS_SIZE + PACKET_SIZE + CRC_SIZE)

//Status Codes
#define NO_ERROR 0
//enum
//{
//	FLASHIF_OK = 0,
//	FLASHIF_ERASEKO, 1 << 0
//	FLASHIF_WRITINGCTRL_ERROR, 1 << 1
//	FLASHIF_WRITING_ERROR, 1 << 2
//	FLASHIF_PROTECTION_ERRROR 1 << 3
//};
#define LOST_PACKETS 1 << 4
#define TOO_MANY_PACKETS 1 << 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t statusPacket[STATUS_PACKET_SIZE] = {0};
uint8_t requestPacket[REQUEST_PACKET_SIZE] = {0};

//packet sizes
uint8_t packetSize[PACKET_TYPES];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t checkCRC(uint8_t *rxBuff);
void packetProcessor(uint8_t *rxBuff, uint8_t mode);
uint8_t addCRC(uint8_t *buff, uint8_t size);

pFunction JumpToApplication;
uint32_t JumpAddress;
uint32_t timeTick;
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
	//initialise variables
	packetSize[ERASE_FLASH] = 7;
	packetSize[PROGRAM_FLASH] = 131;
	packetSize[PATCH_FLASH] = 133;
	packetSize[PATCH_REQUEST] = 6;
	packetSize[STATUS] = 3;
	packetSize[RESTART_PROCESS] = 3;
	packetSize[RESET_MCU] = 3;



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
  MX_DMA_Init();
#ifdef SERIAL_DEBUG
  MX_USART2_UART_Init();
#endif
  MX_USART4_UART_Init();
	
  /* USER CODE BEGIN 2 */

	//start uart4
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	

	
	//drive the DE pin low in order to receive
#ifdef SERIAL_DEBUG
	RS485_RTS_LOW;
	RS485_RE_LOW;			  
#endif // SERIAL_DEBUG

	
	
	//make sure we enable idle line detection
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE); // enable idle line interrupt
	


	//start with interrupts
	//	if(HAL_UART_Receive_IT(&huart4, (uint8_t *)rxBuffer, RX_SIZE) != HAL_OK)
	//	{
	//		Error_Handler();
	//	}
	if (HAL_UART_Receive_DMA(&huart4, (uint8_t *)rxBuffer, RX_SIZE) != HAL_OK)
	{
		Error_Handler();
	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	timeTick = 10000;
#ifdef SERIAL_DEBUG
	HAL_UART_Transmit(&huart2, "Bootloader start\r\n", 16, 1000);			  
#endif // SERIAL_DEBUG

	
	HAL_Delay(150);
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		timeTick--;
			
		//if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
		if (timeTick == 0)
		{
#ifdef SERIAL_DEBUG
			HAL_UART_Transmit(&huart2, "Bootloader timout\r\n", 16, 1000);
#endif
			HAL_Delay(150);
#ifdef SERIAL_DEBUG
			HAL_UART_Transmit(&huart2, "Starting main program...\r\n", 16, 1000);
#endif
			HAL_Delay(150);
			
			/* Jump to user application */
			JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
			//make sure an application is actually there
			if (JumpAddress >= APPLICATION_ADDRESS)
			{
				
				__disable_irq();       //deny interrupts

				/* DeInit All */
				NVIC->ICER[0] = 0xFFFFFFFF;  // disable all IRQ in NVIC
				//NVIC->ICER[1] = 0xFFFFFFFF;
				//NVIC->ICER[2] = 0xFFFFFFFF;
   
				NVIC->ICPR[0] = 0xFFFFFFFF;  // clear all pending bits
				//NVIC->ICPR[1] = 0xFFFFFFFF;
				//NVIC->ICPR[2] = 0xFFFFFFFF;
  
				RCC->AHBRSTR =  0x22E017FF;   // peripherial reset enable
				//RCC->AHB1RSTR = 0x22E017FF;  // peripherial reset enable
				//RCC->AHB2RSTR = 0x000000F1;
				//RCC->AHB3RSTR = 0x00000001;
				RCC->APB1RSTR = 0xF6FEC9FF;
				RCC->APB2RSTR = 0x04777933;
   
				RCC->AHBRSTR = 0;
				//RCC>AHB1RSTR = 0;  // peripherial reset disable
				//RCC->AHB2RSTR = 0;
				//RCC->AHB3RSTR = 0;
				RCC->APB1RSTR = 0;
				RCC->APB2RSTR = 0;
   
				RCC->AHBENR = 0;
				//RCC->AHB1ENR = 0;  // peripherial disable clock
				//RCC->AHB2ENR = 0;
				//RCC->AHB3ENR = 0;
				RCC->APB1ENR = 0;
				RCC->APB2ENR = 0;

				//Systick off
				SysTick->CTRL = 0;
				SysTick->VAL = 0;
				
				
				JumpToApplication = (pFunction)JumpAddress;
				/* Initialize user application's Stack Pointer */
				__set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
				
				JumpToApplication();
			}
			
		}

		if (rxFinished == SET)
		{
			packetProcessor(processBuffer, RESET);
			timeTick = 40000;
			//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			//HAL_Delay(100);
			//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			//		  for (int i = 0; i < RX_SIZE; i++)  printf("%x", rxBuffer[i]);
			//		  printf("\r\n");
			rxFinished = RESET;
		}

		/*if (rxFinished != IDLE )*/ packetProcessor(processBuffer, SET);

		static int i = 0;
		//printf("Hello: %d\r\n", i++);

		//uint8_t buffer[6] = "Hello";
		//HAL_UART_Transmit(&huart2, buffer, 7, 1000);

		//printf("Hello World!\r\n");
		//HAL_Delay(1000);
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(32000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//	printf("Receive Data(length = %d): ", RX_SIZE);
	//	for (int i = 0; i < RX_SIZE; i++) printf("%x", rxBuffer[i]);
	//	printf("\r\n");

	//Zero Receiving Buffer
	//memset(rxBuffer, 0, RX_SIZE);

	reloadRxDMA(&huart4);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void packetProcessor(uint8_t *rxBuff, uint8_t mode)
{
	static uint8_t currentMode = WAITING_PACKET;
	static uint32_t address = 0;
	static uint16_t packet = 0;
	static uint8_t failedPackets[FAILED_PACKETS_ARRAY_SIZE] = {0};

	uint32_t payload[PAYLOAD_SIZE] = {0};

	static uint16_t totalExpectedPackets = 0;
	static uint32_t binarySize = 0;

	uint8_t tempError = 0;

	static uint8_t status = NO_ERROR;

	if (mode == RESET)
	{
		currentMode = PROCESSING_SLAVE_ADDRESS;
	}

	if (currentMode == PROCESSING_SLAVE_ADDRESS)
	{
		//if it is proceed building packet
		if (rxBuff[SLAVE_ADDRESS_RX_POS] == 0xFF)
		{
			HAL_UART_Transmit(&huart2, "Slave address detected!\r\n", 25, 1000);

			//checksum before anything else
			if (checkCRC(rxBuff))
			{
				currentMode = PROCESSING_HEADER;
			}
			else
			{
				//check sum failed
				HAL_UART_Transmit(&huart2, "Incorrect Checksum!\r\n", 21, 1000);
				//if this was a data packet for the flash, record address of failure
				if (rxBuff[HEADER] == PROGRAM_FLASH)
				{
					//store failed packet as a bit in an array of bytes MSB 01 02 A0 LSB    means packets with 1 failed, [Nth packet] ‭00010000001010100000‬ [packet 0]
					failedPackets[packet / 8] = (1 << (packet % 8));
					status |= LOST_PACKETS;
				}
				//abort this packet, wait for next one
				currentMode = WAITING_PACKET;
			}

			//check if we are beyond total number of packets expected and stop process
			if (packet > totalExpectedPackets)
			{
				status |= TOO_MANY_PACKETS;
				currentMode = WAITING_PACKET;
			}
		}
	}

	if (currentMode == PROCESSING_HEADER)
	{
		switch (rxBuff[HEADER])
		{
		//receive request for erasing flash: erase and send ack back
		case ERASE_FLASH:
			HAL_UART_Transmit(&huart2, "Erase Function detected!\r\n", 26, 1000);
			timeTick = 400000;
			//get binary size
			for (int i = 0; i < 4; i++)
				binarySize += (rxBuff[FLASH_SIZE_OFFSET + i]) << ((3 - i) * 8);

			totalExpectedPackets = 2*binarySize / 128;//////2* is added just by chance/////////////
			//find if we have some extra loose bytes, the packet will be terminated with multiple 0x00 by default
			if (binarySize % 128)
				totalExpectedPackets += 1;

			//initialise flash
			FLASH_If_Init();

			//erase flash
			tempError = (uint8_t)FLASH_If_Erase(APPLICATION_ADDRESS, ERASE_ALL);

			//reuse provided error codes in enum
			if (tempError)
			{
				status = 1 << (tempError - 1);
			}

			currentMode = WAITING_PACKET;
			break;
		//incoming data for flash
		case PROGRAM_FLASH:
#ifdef SERIAL_DEBUG
			HAL_UART_Transmit(&huart2, "Program Function detected!\r\n", 28, 1000);
#endif
			//copy payload bytes to uint32_t aligned buffer
			memcpy(payload, (rxBuff + PAYLOAD_OFFSET), PAYLOAD_SIZE);
#ifdef SERIAL_DEBUG
			HAL_UART_Transmit(&huart2, "Address: ", 9, 1000);

			uint8_t addressStr[12] = {0};
			sprintf(addressStr, "%ul", APPLICATION_ADDRESS + PAYLOAD_SIZE * packet);
			HAL_UART_Transmit(&huart2, addressStr, 10, 1000);
#endif
			//flash the payload
			if (FLASH_If_Write(APPLICATION_ADDRESS + PAYLOAD_SIZE * packet, payload, PAYLOAD_SIZE / 4))
			{
#ifdef SERIAL_DEBUG
				HAL_UART_Transmit(&huart2, "Programming Flash Failed!\r\n", 27, 1000);
#endif
				//something went wrong, so flag packet as problematic (did packet++ before, so need to -1)
				failedPackets[packet / 8] = (1 << (packet % 8));
				status |= LOST_PACKETS;
			}

			//increment packet counter if it's a data packet for the flash
			if (rxBuff[HEADER] == PROGRAM_FLASH)
				packet++;

			currentMode = WAITING_PACKET;
			break;
		case PATCH_FLASH:
			//read incoming patch packet
			//packet index
			packet = rxBuff[PACKET_RX_POS] << 8;
			packet += rxBuff[PACKET_RX_POS + 1];

			//convert packet index to real flash address
			address = APPLICATION_ADDRESS + (uint32_t )(packet * 128);

			//erase patch before we can write
			tempError = (uint8_t)FLASH_If_Erase(address, ERASE_PATCH);

			//reuse provided error codes in enum
			if (tempError)
			{
				status = 1 << (tempError - 1);
			}

			//load data
			memcpy(payload, (rxBuff + PATCH_OFFSET), PAYLOAD_SIZE);

			//flash the payload
			if (FLASH_If_Write(address, payload, PAYLOAD_SIZE / 4))
			{
#ifdef SERIAL_DEBUG
				HAL_UART_Transmit(&huart2, "Programming Flash Failed!\r\n", 27, 1000);
#endif
				//something went wrong, so flag packet as problematic (did packet++ before, so need to -1)
				failedPackets[packet / 8] = (1 << (packet % 8));
				status |= LOST_PACKETS;
			}

			currentMode = WAITING_PACKET;
			break;
		case PATCH_REQUEST:
			//build packet
			requestPacket[MASTER_ADDRESS_POS] = MASTER_ADDRESS;
			requestPacket[SLAVE_ADDRESS_POS] = SLAVE_ADDRESS;
			requestPacket[FUNCTION_ADDRESS_POS] = RESEND_CHUNK;

			//hold temporary found packet as -1 to detect issue
			uint16_t failedPacket = 0xFFFF;

			//scan through missed packets array and find the next one
			for (int i = 0; i < FAILED_PACKETS_ARRAY_SIZE; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if (failedPackets[j] & (1 << j))
					{
						failedPacket = j + i * 8;
						break;
					}
				}
				//make sure to exit the outer loop as well if we hit the first failed packet flag
				if (failedPacket != 0xFFFF)
					break;
			}

			//only send if found a valid packet index
			requestPacket[PACKET_POS] = (failedPacket >> 8) & 0xFF;
			requestPacket[PACKET_POS + 1] = failedPacket & 0xFF;

			addCRC(requestPacket, REQUEST_PACKET_SIZE);

			//if Master receives a request for 0xFFFF it means all patches were cleared
			if (failedPacket == 0xFFFF)
				status &= ~LOST_PACKETS;

			HAL_UART_Transmit_DMA(&huart4, requestPacket, REQUEST_PACKET_SIZE);
			
			RS485_RTS_HIGH;
			RS485_RE_HIGH;
			HAL_Delay(250);
			RS485_RTS_LOW;
			RS485_RE_LOW;

			currentMode = WAITING_PACKET;
			break;
		case STATUS:
			//build status packet
			statusPacket[MASTER_ADDRESS_POS] = MASTER_ADDRESS;
			statusPacket[SLAVE_ADDRESS_POS] = SLAVE_ADDRESS;
			statusPacket[FUNCTION_ADDRESS_POS] = status;

			//calculate and add checksum
			addCRC(statusPacket, STATUS_PACKET_SIZE);

			//send ack
			RS485_RTS_HIGH;
			RS485_RE_HIGH;
			HAL_Delay(100);
			HAL_UART_Transmit_DMA(&huart4, statusPacket, ACK_PACKET_SIZE);
			HAL_Delay(250);
			RS485_RTS_LOW;
			RS485_RE_LOW;
			
			currentMode = WAITING_PACKET;
			break;
		case RESTART_PROCESS:
			address = 0;
			packet = 0;
			totalExpectedPackets = 0;
			binarySize = 0;
			currentMode = WAITING_PACKET;
			status = NO_ERROR;
			break;
		case RESET_MCU:
			//issue reset
			HAL_NVIC_SystemReset();
			//nothing below this line should be executed
			currentMode = WAITING_PACKET;
			break;

		default:
#ifdef SERIAL_DEBUG
			HAL_UART_Transmit(&huart2, "Function address invalid!\r\n", 27, 1000);
#endif
			currentMode = WAITING_PACKET;
			break;
		}
	}
}

uint8_t checkCRC(uint8_t *rxBuff)
{
	uint8_t ret = SET;
	uint8_t checksum = 0xFF;
	uint8_t checksumStr[5];

	for (int i = 0; i < packetSize[rxBuff[1]] - 1; i++)
	{
		checksum ^= rxBuff[i];

		sprintf(checksumStr, "%x ", rxBuff[i]);
		//HAL_UART_Transmit(&huart2, checksumStr, 5, 1000);
	}
#ifdef SERIAL_DEBUG
	HAL_UART_Transmit(&huart2, "\r\n", 2, 1000);


	//received
	HAL_UART_Transmit(&huart2, "Received Checksum: ", 19, 1000);
	sprintf(checksumStr, "%2x\r\n", rxBuff[packetSize[rxBuff[1]] - 1]);
	HAL_UART_Transmit(&huart2, checksumStr, 5, 1000);
#endif
	//clear string
	memset(checksumStr, 0, 2);

	//calculated
#ifdef SERIAL_DEBUG
	HAL_UART_Transmit(&huart2, "Calculated Checksum: ", 21, 1000);
	sprintf(checksumStr, "%2x\r\n", checksum);
	HAL_UART_Transmit(&huart2, checksumStr, 5, 1000);
#endif
	if (checksum != rxBuff[packetSize[rxBuff[1]] - 1])
		ret = RESET;

	return ret;
}

uint8_t addCRC(uint8_t* buff, uint8_t size)
{
	//calculate and add checksum
	buff[size-1] = 0xFF;
	for (int i = 0; i < (size - 1); i++)
		statusPacket[size - 1] ^= buff[i];
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
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
