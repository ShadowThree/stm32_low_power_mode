/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// @ref AN2629
#define SLEEP_MODE		(1)		// be waked by any interrupt and wakeup-event
#define STOP_MODE		(2)		// be waked be Any EXTI line(so uart interrupt can NOT wakeup stop mode!)
								// NRST can NOT wakeup stop mode, just reset the MCU.
#define STANDBY_MODE	(3)		// be waked by wakeup key(PA0), RTC alarm, NRST, IWDG reset.
								// (Power-On reset can NOT wakeup standby mode, just reset everything and start run from beginning)
								// after config OB_STDBY_RST in Option Byte, it same as the Power-On reset.
#define LOW_POWER_MODE	SLEEP_MODE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char str[64];
uint8_t rxBuf;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void stm32_OB_userConfig(uint8_t cfg)
{
	FLASH_OBProgramInitTypeDef OBInit = {0};
	
	HAL_FLASHEx_OBGetConfig(&OBInit);
	sprintf(str, "change UserConfig: [0x%02X] to [0x%02X]\n", OBInit.USERConfig, cfg);
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	if(OBInit.USERConfig == cfg) {
		return;
	}
	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();
	OBInit.OptionType = OPTIONBYTE_USER;
	OBInit.USERConfig = cfg;
	HAL_FLASHEx_OBErase();
	if(HAL_OK == HAL_FLASHEx_OBProgram(&OBInit)) {
		HAL_FLASH_OB_Launch();	
	} else {
		sprintf(str, "change UserConfig err\n");
		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	}
	
}

#if LOW_POWER_MODE == SLEEP_MODE
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t count = 0;
	
	HAL_UART_Receive_IT(huart, &rxBuf, 1);
	if(huart == &huart1) {
		sprintf(str, "UART rece[0x%02X] count[%d]\n", rxBuf, count);
		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
		if(++count >= 3) {
			count = 0;
			HAL_PWR_DisableSleepOnExit();
		}
	}
}
#elif LOW_POWER_MODE == STOP_MODE
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	sprintf(str, "Stop mode can NOT be wakeup by Uart interrupt\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint8_t count = 0;
	if(GPIO_Pin == GPIO_PIN_0) {
		sprintf(str, "count[%d] (uart can NOT work in stop mode)\n", count);
		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
		if(++count >= 3) {
			count = 0;
			SystemClock_Config();
			HAL_PWR_DisableSleepOnExit();
		}
	}
}
#endif

void stm32_sleep_mode_test(void)
{
	sprintf(str, "Enter Sleep mode, WakeUp by send data to uart1\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	
	HAL_SuspendTick();	// disable systick interrupt, otherwise this will wake-up the MCU
	
	// if enable SleepOnExit, the MCU will auto enter sleep mode after handler the wake-up interrupt;
	// if comment this line, the MCU will be wake-up until call EnterSLEEPMode() again;
	HAL_PWR_EnableSleepOnExit();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	
	// MCU will stop at here and
	// waiting for interrupt to awake the MCU
	
	HAL_ResumeTick();	// enable systick interrupt, otherwise systick will not work
	sprintf(str, "WakeUp from sleep mode\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
}

void stm32_stop_mode_test(void)
{
	sprintf(str, "Enter Stop mode, WakeUp by pull-up PA0\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	
	HAL_SuspendTick();	// disable systick interrupt, otherwise this will wake-up the MCU
	
	// if enable SleepOnExit, the MCU will auto enter sleep mode after handler the wake-up interrupt;
	// if comment this line, the MCU will be wake-up until call EnterSLEEPMode() again;
	HAL_PWR_EnableSleepOnExit();
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
	//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	
	// MCU will stop at here and
	// waiting for interrupt to awake the MCU
	
	HAL_ResumeTick();	// enable systick interrupt, otherwise systick will not work
	sprintf(str, "WakeUp from stop mode\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
}
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
  MX_USART1_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	sprintf(str, "\n\n/******** STM32 Low Power Mode ********/\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	
	stm32_OB_userConfig(OB_IWDG_SW | OB_STOP_NO_RST | OB_STDBY_NO_RST);
	
	#if LOW_POWER_MODE == SLEEP_MODE
	HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
	#endif
	
	#if LOW_POWER_MODE == STANDBY_MODE
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {	// wakeup from Standby mode
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		sprintf(str, "wakeup from Standby mode\n");
		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
	}
	for(uint8_t i = 0; i < 6; i++) {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(500);
	}
	// enter standby mode
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	sprintf(str, "enter Standby mode\n");
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_EnterSTANDBYMode();
	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	for(uint8_t i = 0; i < 6; i++) {
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(500);
	}
	
	#if LOW_POWER_MODE == SLEEP_MODE
	stm32_sleep_mode_test();
	#elif LOW_POWER_MODE == STOP_MODE
	stm32_stop_mode_test();
	#endif
	
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
