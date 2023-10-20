/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"

 char hex_file_1[64] = {':', '0','2','0','0','0','0','0','4','0','8','0','0','F','2'};

 //02 -

 uint8_t num = 0;
 uint8_t str_end = 0;
 uint8_t check_sum = 0;
 uint8_t culc_check_sum = 0;
 uint16_t addr_hight = 0x0000;		//старшая часть адреса
 uint16_t addr_low = 0x0000;			//младшая часть адреса
 uint32_t addr_full = 0x00000000;	//итоговый адрес

 uint32_t addr_for_start = 0x00000000;	//адрес для передачи управления

 uint8_t data_length = 0x0000;

 uint64_t data = 0;

 uint8_t data_number = 0;

 uint16_t adr_memory = 0x0000;

 uint8_t type_data = 0;

 uint8_t char_to_0x(char symbol);


uint16_t FLASH_USER_START_ADDR	= (uint16_t)0x08012000; // ADDR_FLASH_PAGE_48  потом перепишем на считанное значение (код 05 в intel hex)
uint16_t FLASH_USER_END_ADDR	= (uint16_t)(0x0801FC00 + FLASH_PAGE_SIZE);	//ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE

// конец записи надо привязать к объему полученной информации.
// сейчас 37 страниц (0x0801FC00 - 0x08012000)/FLASH_PAGE_SIZE = 37. это 37 КБ.

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	//разблокируем память
	HAL_FLASH_Unlock();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* Infinite loop */
  while (1)
  {

	  //данный код, который надо зациклить на принимаемое сообщение/данные.
	  //чтобы прочитать не 1 строчку, как тут, а целиком.

	  //проверка, что начало ":"
	  if(hex_file_1[0] != 0x3A)
	  {
		  //что-то делаем если не сошлось
		  while(1)
		  {
		  }
	  }
	  //кол-во данных
	  data_number = (char_to_0x(hex_file_1[1]) << 4) + char_to_0x(hex_file_1[2]);

	  //обнуляем адрес
	  adr_memory = 0x0000;

	  //читаем адрес
	  for(uint8_t i = 3; i<7; i++)
	  {
		  adr_memory = (adr_memory << 4);
		  adr_memory += char_to_0x(hex_file_1[i]);
	  }

	  //что делаем с данными (7 и 8 символы)
	  type_data = (char_to_0x(hex_file_1[7]) << 4) + char_to_0x(hex_file_1[8]);


	  //последние два символа - CRC. переводим в нужный формат и запоминаем
	  check_sum = (char_to_0x(hex_file_1[8+data_number*2+1]) << 4) + char_to_0x(hex_file_1[8+data_number*2 + 2]);

	  //считаем CRC для остальной строчки
	  for(uint8_t step = 1; step <= (8+data_number*2); step=step+2)
	  {
		  culc_check_sum += (char_to_0x(hex_file_1[step]) << 4) + char_to_0x(hex_file_1[step+1]);
	  }

	  culc_check_sum = ~culc_check_sum;
	  culc_check_sum ++;
	  //посчитали CRC

	  //контрольная сумма не сошлась - всё плохо
	  if(check_sum != culc_check_sum)
	  {
		  //что-то делаем если не сошлось
		  while(1)
		  {
		  }
	  }

	  //если сошлось - можно с этим работать дальше

	  //длина данных
	  data_length = 0;
	  data_length = (char_to_0x(hex_file_1[1]) << 4) + char_to_0x(hex_file_1[2]);

	  switch (type_data)
	  {
	  case 0:
		  // идут данные - их пишем.

		  //чистим адрес с пред операции.
		  addr_low = 0x0000;
		  //младший адрес это 3..6 символы
		  for(uint8_t i = 0; i<4 ; i++)
		  {
			  addr_low += char_to_0x(hex_file_1[i+3]) << 4*(3-i);
		  }

		  //полный адрес
		  addr_full = (addr_hight << 16) + addr_low;

		  //куда писать знаем, берем что писать. 9й - первый символ данных

		  //очищаем данные с пред раза
		  data = 0;

		  //пишем данные.
		  for(uint16_t i = 0; i < 2*data_length; i++)
		  {
			  data += (char_to_0x(hex_file_1[i+9])) << 4*(15-i);
		  }

		  //пишем во флеш
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr_full, data);


		  break;
	  case 1:
		  //конец файла. если CRC == FF, то передаем управление

		  //отключаем запись во флеш
		  HAL_FLASH_Lock();

		  //кусок из "void AppStart(void)
		  uint32_t appJumpAddress;
		  void (*GoToApp)(void);
			appJumpAddress = *((volatile uint32_t*)(addr_for_start + 4));
			GoToApp = (void (*)(void))appJumpAddress;


		  __disable_irq(); 	//наверное сначала убрать прерывания, а только потом деинитHAL

		  HAL_DeInit();
	      __set_MSP(*((volatile uint32_t*)addr_for_start));
	      GoToApp();


	      /*
	       *
	       * 	нужен этот кусок в новой программе!
				__disable_irq();
				SCB->VTOR = addr_for_start; // тут должен быть addr_for_start
				__enable_irq();

				HAL_Init();
				SystemClock_Config();
	       *
	       * */

		  break;
	  case 2:
		  // запись адреса сегмента
		  break;
	  case 3:
		  // 
		  break;
	  case 4:
		  // расширенный ажрес. меняется редко, пишем редко.
		  addr_hight = 0;
		  addr_hight = (char_to_0x(hex_file_1[9]) << 4*3) + (char_to_0x(hex_file_1[10]) << 4*2) + (char_to_0x(hex_file_1[11]) << 4*1) + char_to_0x(hex_file_1[12]);

		  break;
	  case 5:
		  // адрес старта программы (по нему передаем управление?). ставим флаг для готовности передачи управления

		  for(uint16_t i = 0; i < 2*data_length; i++)
		  {
			  addr_for_start += (char_to_0x(hex_file_1[i+9])) << 4*(7-i);	//сдвиг на 4 бита - сдвиг 1 символа
		  }
		  break;
	  default:
		  break;
	  }

	  //точка для отладки
	  hex_file_1[1]++;
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4  --------------------------------------------------------------------------------------------*/

uint8_t char_to_0x(char symbol)
{
	//если начало строки, возвращаем 0
	if(symbol == 0x3A)
	{
		return 0;
	}

	//если 0..9
	if(symbol > 0x2F && symbol < 0x3A)
	{
		return (symbol - 0x30);
	}

	//если A..F
	if(symbol > 0x40 && symbol < 0x47)
	{
		uint8_t val = symbol - 0x40;
		switch (val)
		{
		case 1:
			return 0xA;
		case 2:
			return 0xB;
		case 3:
			return 0xC;
		case 4:
			return 0xD;
		case 5:
			return 0xE;
		case 6:
			return 0xF;
		default:
			return 0;
		}
	}

	return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
