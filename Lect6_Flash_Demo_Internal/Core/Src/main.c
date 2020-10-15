/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define FLASH_PAGE0 	0x08000000
#define FLASH_PAGE1 	0x08000800
#define FLASH_PAGE2 	0x08001000


#define FLASH_PAGE127	0x0803F800

#define PAGE_SIZE		0x00000800	// Page size for STM32L432 is 2 KB

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
struct DATA_A
{
	float x;
	float y;
	int z;
};

struct DATA_B
{
	int a;
	char password[40];
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void my_print(char *string)
{
	HAL_UART_Transmit(&huart2, string, strlen(string), 100);
}

uint8_t blankCheckFlashPage(uint32_t page)
{
	uint8_t rdBuf[32],rdByte;
	uint32_t flashAddress = FLASH_PAGE0 +page*PAGE_SIZE;
	uint16_t byte_cnt = 0;
	while(byte_cnt<PAGE_SIZE)
	{
		rdByte = *(uint8_t *) flashAddress;
		if(rdByte!= 0xFF)
		{
			return rdByte;
		}
		flashAddress++;
		byte_cnt++;
	}
	return 0xFF;
}

void readFlash(uint32_t address, uint32_t numBytes)
{
	char buf[80];
	uint8_t rdBuf;

	uint32_t flashAddress = address;

	for(uint32_t i=0;i<numBytes;++i)
	{
		if(i%16==0)
		{
			sprintf(buf,"\r\n0x%08X:  ",flashAddress);
			HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
		 }
		*((uint8_t *)&rdBuf) = *(uint8_t *) flashAddress;
		sprintf(buf,"0x%02X  ",rdBuf);
		HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
		flashAddress++;
	}
	sprintf(buf,"\r\n");
	HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
}

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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    struct DATA_A dataA;
    struct DATA_B dataB;
    char buf[120];
    uint8_t wrBuf[128], ret;
    uint8_t *ptr;
    char ch;
    uint16_t cnt;
    FLASH_EraseInitTypeDef EraseInit;
    uint16_t *flash_size = (uint16_t *)(0x1FFF75E0);  // Flash data size in KB store in this internal register location
    uint32_t UID0 = HAL_GetUIDw0();
    uint32_t UID1 = HAL_GetUIDw1();
    uint32_t UID2 = HAL_GetUIDw2();

	while (1)
	{
	  	my_print("\r\n\r\n=== Internal Flash memory commands ===\r\n");
	  	sprintf(buf,"Flash Size:  %d KB (%d bytes)\r\n",*flash_size, *flash_size*1024);
	  	my_print(buf);
	  	sprintf(buf,"Device Unique ID:  0x%08X 0x%08X 0x%08X\r\n\r\n", UID0, UID1, UID2);
	  	my_print(buf);
	  	my_print("What would you like to do?\r\n");
	  	my_print("   (1) Blank Check All Pages\r\n");
	  	my_print("   (2) Blank Check Page 127\r\n");
	  	my_print("   (3) Erase Page 127\r\n");
	  	my_print("   (4) Read 1st 256 bytes of Page 127\r\n");
	  	my_print("   (5) Write data to 1st 128 bytes of Page 127\r\n");
	  	my_print("   (6) Write DATA Type A to flash\r\n");
	  	my_print("   (7) Write DATA Type B to flash\r\n");
	  	my_print("   (8) Read DATA Type A from flash\r\n");
	  	my_print("   (9) Read DATA Type B from flash\r\n");
	  	my_print("   ?  ");

	  	HAL_UART_Receive(&huart2, &ch, 1,HAL_MAX_DELAY);
	  	HAL_UART_Transmit(&huart2, &ch, 1, 100);

	  	switch(ch)
	  	{
	  	case '1':
	  		my_print("\r\n\r\nBlank checking all pages...\r\n\r\n");
	  		my_print("              0    1    2    3    4    5    6    7");
	  		for(uint8_t i=0;i<128;++i)
	  		{
	  			if(i%8==0)
	  			{
	  				sprintf(buf,"\r\nPage %3d:  ",i);
	  				my_print(buf);
	  			}
	  			sprintf(buf,"0x%02X ",blankCheckFlashPage(i));
	  			my_print(buf);
	  		}
	  		my_print("\r\n\r\nPages returning 0xFF are blank\r\n");
	  		break;
	  	case '2':
	  		ret = blankCheckFlashPage(127);
	  		if(ret == 0xFF)
	  			my_print("\r\n\r\nPage 127 is Blank!\r\n");
			else
				my_print("\r\n\r\nPage 127 is not Blank!\r\n");
	  		sprintf(buf,"    Blank Check Page 127:  0x%02X\r\n",ret);
	  		my_print(buf);
	  		break;
	  	case '3':
			EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			EraseInit.Banks = FLASH_BANK_1;
			EraseInit.Page = 127;
			EraseInit.NbPages = 1;
			uint32_t PageError;

			HAL_FLASH_Unlock();
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
			if(HAL_FLASHEx_Erase(&EraseInit, &PageError) != HAL_OK)
			{
			  sprintf(buf,"Error erasing page %d:  0x%02X\r\n",EraseInit.Page, PageError);
			  HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
			}
			else
			{
			  sprintf(buf,"\r\n\r\nPage %d erased successfully\r\n",EraseInit.Page);
			  HAL_UART_Transmit(&huart2, buf, strlen(buf), 100);
			}
			HAL_FLASH_Lock();
	  		break;
	  	case '4':
	  		readFlash(FLASH_PAGE127, 256);
	  		break;
	  	case '5':
	  		 for(uint16_t i=0;i<4;++i)
	  			 wrBuf[i] = 0x04;
	  		 for(uint16_t i=4;i<8;++i)
	  			 wrBuf[i] = 0x08;
			 for(uint16_t i=8;i<12;++i)
				 wrBuf[i] = 0x12;
			 for(uint16_t i=12;i<16;++i)
				 wrBuf[i] = 0x16;
			 for(uint16_t i=16;i<32;++i)
				 wrBuf[i] = 0x32;
			 for(uint16_t i=32;i<64;++i)
				 wrBuf[i] = 0x64;
			 for(uint16_t i=64;i<128;++i)
				 wrBuf[i] = i-64;
	  		 ptr = wrBuf;
	  		 HAL_FLASH_Unlock();
	  		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
	  		cnt = 0;
	  		while(cnt<128)
	  		{
	  			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_PAGE127+cnt, *((uint64_t *)ptr));
	  			ptr += 8;
	  			cnt += 8;
	  		}
	  		 HAL_FLASH_Lock();
	  		break;
	  	case '6':
	  		my_print("\r\n\r\nWriting dataA to flash memory starting at Page 127\r\n");
	  		dataA.x = 3.14;
	  		dataA.y = -10.;
	  		dataA.z = 1024;
	  		memset(&wrBuf[0], 0xff, sizeof(wrBuf));	// Set all elements of wrBuf array to 0xFF

	  		for(uint8_t i=0;i<sizeof(dataA);++i)
	  			wrBuf[i] = *((uint8_t *)&dataA+i);

			ptr = wrBuf;
			HAL_FLASH_Unlock();
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
			cnt = 0;
			while(cnt<128)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_PAGE127+cnt, *((uint64_t *)ptr));
				ptr += 8;
				cnt += 8;
			}
			HAL_FLASH_Lock();
	  		break;
	  	case '7':
	  		my_print("\r\n\r\nWriting dataB to flash memory starting at Page 127\r\n");
	  		dataB.a = 1023;
	  		strcpy(dataB.password,"Secret Password");
	  		memset(&wrBuf[0], 0xff, sizeof(wrBuf));	// Set all elements of wrBuf array to 0xFF

			for(uint8_t i=0;i<sizeof(dataB);++i)
				wrBuf[i] = *((uint8_t *)&dataB+i);

			ptr = wrBuf;
			HAL_FLASH_Unlock();
			__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
			cnt = 0;
			while(cnt<128)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_PAGE127+cnt, *((uint64_t *)ptr));
				ptr += 8;
				cnt += 8;
			}
			HAL_FLASH_Lock();
			break;
	  	case '8':
	  		my_print("\r\n\r\nReading dataA from flash memory\r\n");
	  		struct DATA_A *Aptr;	// Pointer to structure with format of data
	  		Aptr = (struct DATA_A *) FLASH_PAGE127;
	  		sprintf(buf,"DATA_A:  x = %f,  y = %f,  z = %d\r\n",Aptr->x, Aptr->y, Aptr->z);
	  		my_print(buf);
	  		break;
	  	case '9':
	  		my_print("\r\n\r\nReading dataA from flash memory\r\n");
			struct DATA_B *Bptr;	// Pointer to structure with format of data
			Bptr = (struct DATA_B *) FLASH_PAGE127;
			sprintf(buf,"DATA_B:  a = %d,  password = %s\r\n",Bptr->a, Bptr->password);
			my_print(buf);
	  		break;
	  	default:
	  		break;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
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
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
