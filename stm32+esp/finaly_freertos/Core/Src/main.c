/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_CHANNELS_NUM 1
#define BUFFER_SIZE_TRANSMITT 1
#define BUFFER_SIZE_RECIEVE 5

#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )
#define BIT_2	( 1 << 2 )
#define BIT_3	( 1 << 3 )
//for work
#define BIT_4	( 1 << 4 )

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* Declare a variable to hold the created event group. */
EventGroupHandle_t xCreatedEventGroup;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

osThreadId defaultTaskHandle;
osThreadId myTaskADCHandle;
osThreadId myTaskUARTHandle;
osThreadId myTaskPBHandle;
osMessageQId myQueueRXHandle;
/* USER CODE BEGIN PV */

uint16_t adcData[ADC_CHANNELS_NUM] = {0};
uint16_t adcValue[ADC_CHANNELS_NUM] = {0};

uint8_t transmitBuffer[BUFFER_SIZE_TRANSMITT];
uint8_t receiveBuffer[BUFFER_SIZE_RECIEVE];
uint8_t takeMaskBuff[BUFFER_SIZE_RECIEVE];

uint8_t maskFirstLed = 	BIT_0;
uint8_t maskSecondLed = BIT_1;
uint8_t maskThirdLed = 	BIT_2;
uint8_t maskFourLed = 	BIT_3;

const TickType_t xTicksToWait = 100;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
void StartTaskADC(void const * argument);
void StartTaskUART(void const * argument);
void StartTaskPB(void const * argument);

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
	xCreatedEventGroup = xEventGroupCreate();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of myQueueRX */
  osMessageQDef(myQueueRX, 5, uint8_t);
  myQueueRXHandle = osMessageCreate(osMessageQ(myQueueRX), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTaskADC */
  osThreadDef(myTaskADC, StartTaskADC, osPriorityNormal, 0, 128);
  myTaskADCHandle = osThreadCreate(osThread(myTaskADC), NULL);

  /* definition and creation of myTaskUART */
  osThreadDef(myTaskUART, StartTaskUART, osPriorityNormal, 0, 128);
  myTaskUARTHandle = osThreadCreate(osThread(myTaskUART), NULL);

  /* definition and creation of myTaskPB */
  osThreadDef(myTaskPB, StartTaskPB, osPriorityNormal, 0, 128);
  myTaskPBHandle = osThreadCreate(osThread(myTaskPB), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|LED_OUT1_Pin|LED_OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_OUT2_GPIO_Port, LED_OUT2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_OUT4_GPIO_Port, LED_OUT4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin LED_OUT1_Pin LED_OUT3_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|LED_OUT1_Pin|LED_OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB_IN3_Pin PB_IN2_Pin PB_IN1_Pin */
  GPIO_InitStruct.Pin = PB_IN3_Pin|PB_IN2_Pin|PB_IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_OUT2_Pin */
  GPIO_InitStruct.Pin = LED_OUT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_OUT2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB_IN4_Pin */
  GPIO_InitStruct.Pin = PB_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PB_IN4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_OUT4_Pin */
  GPIO_InitStruct.Pin = LED_OUT4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_OUT4_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

//callback for adc
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	//коллбек когда считалось знаение АЦП

	if(hadc->Instance == ADC1)
	{
		for (uint8_t i = 0; i < ADC_CHANNELS_NUM; i++)
		{
			adcValue[i] = adcData[i]; // * 3.3 / 4095;
//			xQueueSendToBack(myQTXHandle, &adcValue[i], 500);
		}
		//можно отправлять
//		vTaskResume( myTaskUARTHandle );
	}
	//bazovaya electronika code
//	val_adc1 = HAL_ADC_GetValue(hadc);
//	HAL_ADC_Stop_IT(hadc);
}


//callback uart


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */

	//коллбек когда что-то пришло на uart1
	if(huart == &huart1)
	{


//		HAL_UART_Transmit(&huart1, receiveBuffer, 8, 1000);
//		HAL_UART_Receive_IT(&huart1, receiveBuffer, 8);
//		HAL_UART_Receive_DMA(&huart1, receiveBuffer, BUFFER_SIZE_RECIEVE);
//		vTaskResume( myTaskUARTHandle );

	}
//  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
	//коллбек когда что-то пришло на uart1
	if(huart == &huart1)
	{

//		HAL_UART_Transmit(&huart1, receiveBuffer, 8, 1000);
//		HAL_UART_Receive_IT(&huart1, receiveBuffer, 8);
//		  HAL_UART_Transmit_DMA(&huart1, transmitBuffer, sizeof(transmitBuffer)-1);
//		  vTaskResume( myTaskUARTHandle );

	}
//  UNUSED(huart);

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
	//look at LED state
	EventBits_t  uxBits;

  /* Infinite loop */
  for(;;)
  {

	  //white
	  uxBits = xEventGroupWaitBits(
			  xCreatedEventGroup,   /* The event group being tested. */
			  maskFirstLed | BIT_4, /* The bits within the event group to wait for. */
				pdFALSE,        /* BIT_0 + BIT_3 should be not cleared before returning. */
	            pdFALSE,       /* Don't wait for both bits, either bit will do. */
				xTicksToWait );/* Wait a maximum of 100ms for either bit to be set. */
	  if( (uxBits & ( maskFirstLed | BIT_4 )) ==  maskFirstLed )
	  {
		  HAL_GPIO_WritePin(LED_OUT1_GPIO_Port, LED_OUT1_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_OUT1_GPIO_Port, LED_OUT1_Pin, GPIO_PIN_RESET);
	  }

	  //green
	  uxBits = xEventGroupWaitBits(
			  xCreatedEventGroup,   /* The event group being tested. */
			  maskSecondLed | BIT_4, /* The bits within the event group to wait for. */
				pdFALSE,        /* BIT_1 should be not cleared before returning. */
	            pdFALSE,       /* Don't wait for both bits, either bit will do. */
				xTicksToWait );/* Wait a maximum of 100ms for either bit to be set. */
	  if( (uxBits & ( maskSecondLed | BIT_4 )) == ( maskSecondLed  ))		// (uxBits & BIT_1 )
	  {
		  HAL_GPIO_WritePin(LED_OUT2_GPIO_Port, LED_OUT2_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_OUT2_GPIO_Port, LED_OUT2_Pin, GPIO_PIN_RESET);
	  }

	  //yellow
	  uxBits = xEventGroupWaitBits(
			  xCreatedEventGroup,   /* The event group being tested. */
			  maskThirdLed | BIT_4, /* The bits within the event group to wait for. */
				pdFALSE,        /* BIT_0 + BIT_1 should be not cleared before returning. */
	            pdFALSE,       /* Don't wait for both bits, either bit will do. */
				xTicksToWait );/* Wait a maximum of 100ms for either bit to be set. */

	  if( (uxBits & ( maskThirdLed | BIT_4 )) == ( maskThirdLed  )  )
	  {
		  HAL_GPIO_WritePin(LED_OUT3_GPIO_Port, LED_OUT3_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_OUT3_GPIO_Port, LED_OUT3_Pin, GPIO_PIN_RESET);
	  }

	  //blue
	  uxBits = xEventGroupWaitBits(
	  			  xCreatedEventGroup,   /* The event group being tested. */
				  maskFourLed | BIT_4, /* The bits within the event group to wait for. */
	  				pdFALSE,        /* BIT_0 .. BIT_3 should be not cleared before returning. */
	  	            pdFALSE,       /* Don't wait for both bits, either bit will do. */
	  				xTicksToWait );/* Wait a maximum of 100ms for either bit to be set. */
	  	  if((uxBits & ( maskFourLed | BIT_4 )) == ( maskFourLed  ) )
	  	  {
	  		  HAL_GPIO_WritePin(LED_OUT4_GPIO_Port, LED_OUT4_Pin, GPIO_PIN_SET);
	  	  }
	  	  else
	  	  {
	  		  HAL_GPIO_WritePin(LED_OUT4_GPIO_Port, LED_OUT4_Pin, GPIO_PIN_RESET);
	  	  }

    osDelay(50);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskADC */
/**
* @brief Function implementing the myTaskADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskADC */
void StartTaskADC(void const * argument)
{
  /* USER CODE BEGIN StartTaskADC */

	//take value from ADC to DMA

	  //старт прерывания для ацп1 и калибровка ацп1
	  HAL_ADC_Start_IT(&hadc1);
	  HAL_ADCEx_Calibration_Start(&hadc1);

  /* Infinite loop */
  for(;;)
  {
	  //start adc convertation
	  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcData, ADC_CHANNELS_NUM);

	  vTaskResume( myTaskUARTHandle );
	  osDelay(1000);
  }
  /* USER CODE END StartTaskADC */
}

/* USER CODE BEGIN Header_StartTaskUART */
/**
* @brief Function implementing the myTaskUART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUART */
void StartTaskUART(void const * argument)
{
  /* USER CODE BEGIN StartTaskUART */
	//transmitt data to uart

	  //обнуляем буфер
	  for (unsigned char i = 0; i < BUFFER_SIZE_RECIEVE; i++)
	  {
	          receiveBuffer[i] = 0;
	  }
	  for (unsigned char i = 0; i < BUFFER_SIZE_TRANSMITT; i++)
	  {
	          transmitBuffer[i] = 0;
	  }

//	  portBASE_TYPE xStatus;
//	  int temp = 0;

  /* Infinite loop */
  for(;;)
  {
//	  //recieve
	  if(receiveBuffer[0])
	  {
		  //if new mask - clear old mask
			  switch (receiveBuffer[0])
			  {
			  case 1:
				  maskFirstLed = 0;
				  break;
			  case 2:
				  maskSecondLed = 0;
				  break;
			  case 3:
				  maskThirdLed = 0;
				  break;
			  case 4:
				  maskFourLed = 0;
				  break;
			  default:
				  break;
			  }

		  //take new mask
		  for(int i = 0; i+1<BUFFER_SIZE_RECIEVE; i++)
		  {
			  switch (receiveBuffer[0])
			  {
			  case 1:
				  maskFirstLed += receiveBuffer[i+1] << i;
				  break;
			  case 2:
				  maskSecondLed = receiveBuffer[i] << i;
				  break;
			  case 3:
				  maskThirdLed = receiveBuffer[i] << i;
				  break;
			  case 4:
				  maskFourLed = receiveBuffer[i] << i;
				  break;
			  default:
				  break;
			  }
		  }
	  }
		  receiveBuffer[0]=0;

	  //если будет больше буфер - написать цикл
	  transmitBuffer[0] = adcValue[0]/16;

	  HAL_UART_Transmit_DMA(&huart1, transmitBuffer, sizeof(transmitBuffer));
	  HAL_UART_Receive_DMA(&huart1, receiveBuffer, 5);

	  vTaskSuspend(myTaskUARTHandle);
//    osDelay(1000);
  }
  /* USER CODE END StartTaskUART */
}

/* USER CODE BEGIN Header_StartTaskPB */
/**
* @brief Function implementing the myTaskPB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskPB */
void StartTaskPB(void const * argument)
{
  /* USER CODE BEGIN StartTaskPB */
  /* Infinite loop */
	EventBits_t  uxBits;

  for(;;)
  {

	  //read input's
	  	  if(HAL_GPIO_ReadPin(PB_IN1_GPIO_Port, PB_IN1_Pin) == GPIO_PIN_SET)
	  	  {
	  		  xEventGroupSetBits(xCreatedEventGroup, 0b0001);
	  	  }
	  	  else
	  	  {
	  		  uxBits = xEventGroupClearBits (xCreatedEventGroup, BIT_0);
	  	  }

	  	  if(HAL_GPIO_ReadPin(PB_IN2_GPIO_Port, PB_IN2_Pin) == GPIO_PIN_SET)
	  	  {
	  		  xEventGroupSetBits(xCreatedEventGroup, 0b0010);
	  	  }
	  	  else
	  	  {
	  		  uxBits = xEventGroupClearBits (xCreatedEventGroup, BIT_1);
	  	  }

	  	  if(HAL_GPIO_ReadPin(PB_IN3_GPIO_Port, PB_IN3_Pin) == GPIO_PIN_SET)
	  	  {
	  		  xEventGroupSetBits(xCreatedEventGroup, 0b0100);
	  	  }
	  	  else
	  	  {
	  		  uxBits = xEventGroupClearBits (xCreatedEventGroup, BIT_2);
	  	  }

	  	  if(HAL_GPIO_ReadPin(PB_IN4_GPIO_Port, PB_IN4_Pin) == GPIO_PIN_SET)
	  	  {
	  		  xEventGroupSetBits(xCreatedEventGroup, 0b1000);
	  	  }
	  	  else
	  	  {
	  		  uxBits = xEventGroupClearBits (xCreatedEventGroup, BIT_3);
	  	  }

	  	osDelay(20);
  }
  /* USER CODE END StartTaskPB */
}

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
