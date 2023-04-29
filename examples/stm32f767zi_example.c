/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwrb.h"
#include <stdbool.h>
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

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define UART_RX_INCREMENT 10
#define UART_RX_SAFETY_MARGIN 100
/*NEO_6M_UART_TRIGGER must be enough to hold one complete batch from neo6m,
to make sure that a full GPGGA msg is there. To reduce this size, the gps needed to be configured to
only output gpgga msgs */
#define NEO_6M_UART_TRIGGER 750
#define NEO_6M_DATA_SIZE 5
#define NEO_6M_BUFFER_MSGS 2

static lwrb_t Neo6mUartRingBuf;
static uint8_t Neo6mUartBuf[NEO_6M_UART_TRIGGER*NEO_6M_BUFFER_MSGS];
static uint8_t Uart2RxBuf[UART_RX_INCREMENT];
static volatile bool Neo6mDataReadyFlag = false;
static volatile bool Neo6mUartRingBufBusy = false;
volatile bool myvar = false;


static uint8_t Neo6mDataBuf[NEO_6M_DATA_SIZE];

size_t Stm32_IORead(void* DataPtr, size_t ReadSize)
{
	Neo6mUartRingBufBusy = true;
	size_t ReadBytes = lwrb_read(&Neo6mUartRingBuf, DataPtr, ReadSize);
	Neo6mUartRingBufBusy = false;
	return ReadBytes;
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

  lwrb_init(&Neo6mUartRingBuf, Neo6mUartBuf, sizeof(Neo6mUartBuf)); /* Initialize buffer */

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, Uart2RxBuf, UART_RX_INCREMENT);
  /* USER CODE END 2 */

  char Msg1[] = "Welcome to neo6m IO read function test! \n ";
  char Msg3[] = "No more bytes to read! \n";
	char Msg2[] = "\n \n !!!!!!!!  BUFFER HAD TO BE RESET !!!!!!!! \n \n ";
  HAL_UART_Transmit(&huart4, (uint8_t *)Msg1, sizeof(Msg1), 100);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  if (myvar)
	  {
		  HAL_UART_Transmit(&huart4, (uint8_t *)Msg2, sizeof(Msg2), 100);
		  myvar = false;
	  }

	if (Neo6mDataReadyFlag)
	{
		int ReadBytes = Stm32_IORead(Neo6mDataBuf,NEO_6M_DATA_SIZE);
		while (ReadBytes == NEO_6M_DATA_SIZE)
		{
		//handle data
		//HAL_UART_Transmit(&huart4, (uint8_t *)Msg2, sizeof(Msg2), 100);
		HAL_UART_Transmit(&huart4, Neo6mDataBuf, NEO_6M_DATA_SIZE, 100);
		ReadBytes = Stm32_IORead(Neo6mDataBuf,NEO_6M_DATA_SIZE);
		}
		HAL_UART_Transmit(&huart4, (uint8_t *)Msg3, sizeof(Msg3), 100); //This gets sent so, the whole chunk is processed before a new one arrives
		Neo6mDataReadyFlag = false; //This race condition is not a serious problem, since it only causes 10B to be added */
	}
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	size_t FreeBytes;

	FreeBytes = lwrb_get_free(&Neo6mUartRingBuf);
    if (FreeBytes < UART_RX_SAFETY_MARGIN)
    {
    	if (!Neo6mUartRingBufBusy)
    	{
    		lwrb_reset(&Neo6mUartRingBuf);
    		myvar = true;
    	}
    }

    lwrb_write(&Neo6mUartRingBuf, Uart2RxBuf, UART_RX_INCREMENT);

    if (lwrb_get_full(&Neo6mUartRingBuf) >= NEO_6M_UART_TRIGGER )
    {
    	Neo6mDataReadyFlag = true;
    }

    HAL_UART_Receive_IT(&huart2, Uart2RxBuf, UART_RX_INCREMENT);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
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
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

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
  huart2.Init.BaudRate = 9600;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
