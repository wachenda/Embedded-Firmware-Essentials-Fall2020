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
#include "w25qxx.h"

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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void my_print(char *string)
{
	HAL_UART_Transmit(&huart2, string, strlen(string), 100);
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  char buf[80];
  char ch;
  uint8_t rxBuf[256];
  uint8_t wrBuf[256];


  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
  W25qxx_Init();
    /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  my_print("\r\n\r\n=== External W25Qxx Flash memory commands ===\r\n");
	  sprintf(buf,"Read ID:  %x\r\n",W25qxx_ReadID());
	  my_print(buf);
	  my_print("64-bit Unique ID:  ");
	  for(int i=0;i<8;++i)
	  {
		  sprintf(buf,"0x%02X ",w25qxx.UniqID[i]);
		  my_print(buf);
	  }
	  my_print("\r\n");
	sprintf(buf,"Flash Size:  %d KB (%d bytes)\r\n",w25qxx.CapacityInKiloByte, w25qxx.CapacityInKiloByte*1024);
	my_print(buf);
	sprintf(buf,"Page Size: %d Bytes, Page Count: %d\r\n",w25qxx.PageSize,w25qxx.PageCount);
//	sprintf(buf,"Device Unique ID:  0x%08X 0x%08X 0x%08X\r\n\r\n", UID0, UID1, UID2);
	my_print(buf);
	sprintf(buf,"Sector Size: %d Bytes, Sector Count:  %d\r\n",w25qxx.SectorSize,w25qxx.SectorCount);
	my_print(buf);
	sprintf(buf,"Block Size: %d Bytes, Block Count:  %d\r\n",w25qxx.BlockSize,w25qxx.BlockCount);
	my_print(buf);

	my_print("\r\nWhat would you like to do:\r\n");
	my_print("   (1) Blank Check All Blocks\r\n");
	my_print("   (2) Blank Check Page 0\r\n");
	my_print("   (3) Erase Sector 0\r\n");
	my_print("   (4) Read Page 0\r\n");
	my_print("   (5) Write data to Page 0\r\n");
	my_print("   (6) Write data to Page 1000\r\n");
	my_print("   (7) Erase Page 1000\r\n");
	my_print("   (8) Read Page 1000\r\n");
	my_print("   ?  ");

	HAL_UART_Receive(&huart2, &ch, 1,HAL_MAX_DELAY);	// NOTE:  this function blocks forever unless a byte is sent over the serial line
	HAL_UART_Transmit(&huart2, &ch, 1, 100);

	switch(ch)
	{
	case '1':
		my_print("\r\n\r\n              0    1    2    3    4    5    6    7");
		for(int i=0;i<w25qxx.BlockCount;++i)
		{
			if(i%8==0)
			{
				sprintf(buf,"\r\nBlock %2d:  ",i);
				my_print(buf);
			}
			if(W25qxx_IsEmptyBlock(i,0, w25qxx.BlockSize))
				my_print("0xFF ");
			else
				my_print("**** ");
		}
		break;
	case '2':
		my_print("\r\n\r\nBlank checking Sector 0:  ");
		if(W25qxx_IsEmptySector(0,0, w25qxx.SectorSize))
			my_print("0xFF\r\n");
		else
			my_print("0x00\r\n");
		break;
	case '3':
		my_print("\r\n\r\nErasing Sector 0...\r\n");
		W25qxx_EraseSector(0);
		my_print("Sector 0 Erased\r\n");
		break;
	case '4':
		my_print("\r\n\r\nReading Page 0...\r\n");

		my_print("\r\n\r\nAddress:      0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");
		W25qxx_ReadPage(rxBuf,0,0,w25qxx.PageSize);
		for(int i=0;i<w25qxx.PageSize;++i)
		{
			if(i%16==0)
			{
				sprintf(buf,"\r\n0x%06X:  ",i);
				my_print(buf);
			}
			sprintf(buf,"0x%02X ",rxBuf[i]);
			my_print(buf);
		}
		my_print("\r\n");
		break;
	case '5':
		my_print("\r\n\r\nWriting data to Page 0...\r\n");
		memset(&wrBuf[0], 0x1A, sizeof(wrBuf));
		W25qxx_WritePage(wrBuf,0,128,128);	// write 128 bytes offset by 128 from beginning of Page 0
		break;
	case '6':
		my_print("\r\n\r\nWriting data to Page 1000...\r\n");
		memset(&wrBuf[0], 0x42, sizeof(wrBuf));
		W25qxx_WritePage(wrBuf,1000,64,32);	// write 32 bytes offset by 64 from beginning of Page 1000
		break;
	case '7':
		my_print("\r\n\r\nErasing 1st Sector of Page 1000...\r\n");
		uint32_t eraseSectorAddress = 1000*w25qxx.PageSize/w25qxx.SectorSize;
		W25qxx_EraseSector(eraseSectorAddress);
		my_print("Sector 0 Erased\r\n");
		break;
	case '8':
		my_print("\r\n\r\nReading Page 1000...\r\n");

				my_print("\r\n\r\nAddress:      0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F");
				W25qxx_ReadPage(rxBuf,1000,0,w25qxx.PageSize);
				for(int i=0;i<w25qxx.PageSize;++i)
				{
					if(i%16==0)
					{
						sprintf(buf,"\r\n0x%06X:  ",i);
						my_print(buf);
					}
					sprintf(buf,"0x%02X ",rxBuf[i]);
					my_print(buf);
				}
				my_print("\r\n");
				break;_WritePage(wrBuf,1000,64,32);	// write 32 bytes offset by 64 from beginning of Page 0
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

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
