/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwip/udp.h"
#include <string.h>
#include "share.h"
#include "parson.h"
#include "DeviceTime.h"
#include "ServerModbusTCP.h"
#include "ClientModbusTCP.h"
#include "Transport.h"
#include "modbus.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define STEP_TCP 				60000
#define STEP_GPRS 				60000

#define STEP_SHARE 				10000
#define STEP_SERVER_MODBUS_TCP 	1000
#define STEP_CLIENT_MODBUS_TCP 	10000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart4;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for DebugLogger */
osThreadId_t DebugLoggerHandle;
const osThreadAttr_t DebugLogger_attributes = {
  .name = "DebugLogger",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for ToServerTCP */
osThreadId_t ToServerTCPHandle;
const osThreadAttr_t ToServerTCP_attributes = {
  .name = "ToServerTCP",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for ServerModbusTCP */
osThreadId_t ServerModbusTCPHandle;
const osThreadAttr_t ServerModbusTCP_attributes = {
  .name = "ServerModbusTCP",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ClientModbusTCP */
osThreadId_t ClientModbusTCPHandle;
const osThreadAttr_t ClientModbusTCP_attributes = {
  .name = "ClientModbusTCP",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for FromServerTCP */
osThreadId_t FromServerTCPHandle;
const osThreadAttr_t FromServerTCP_attributes = {
  .name = "FromServerTCP",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ToServerGPRS */
osThreadId_t ToServerGPRSHandle;
const osThreadAttr_t ToServerGPRS_attributes = {
  .name = "ToServerGPRS",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for FromServerGPRS */
osThreadId_t FromServerGPRSHandle;
const osThreadAttr_t FromServerGPRS_attributes = {
  .name = "FromServerGPRS",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TCPTransport */
osThreadId_t TCPTransportHandle;
const osThreadAttr_t TCPTransport_attributes = {
  .name = "TCPTransport",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
void StartDefaultTask(void *argument);
void StartDebugLogger(void *argument);
void StartToServerTCP(void *argument);
void StartServerModbusTCP(void *argument);
void StartClientModbusTCP(void *argument);
void StartFromServerTCP(void *argument);
void StartToServerGPRS(void *argument);
void StartFromServerGPRS(void *argument);
void StartTCPTransport(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char ReadyETH=false;			//Готовность Ehernet
char ReadyShare=false;			//Готовность Share
int TransportNeed=0;		//Готовность работы транспортного протокола
int GPRSNeed=0;			//Готовность работы GPRS если есть

//Очереди для Ethernet
osMessageQueueId_t ToServerQueue;
osMessageQueueId_t FromServerQueue;
osMessageQueueId_t ToServerSecQueue;
osMessageQueueId_t FromServerSecQueue;

//Очереди для GPRS
osMessageQueueId_t GPRSToServerQueue;
osMessageQueueId_t GPRSFromServerQueue;
osMessageQueueId_t GPRSToServerSecQueue;
osMessageQueueId_t GPRSFromServerSecQueue;

//Очереди от основной программы управления
osMessageQueueId_t MainChangeStatus;
osMessageQueueId_t MainToServerQueue;
osMessageQueueId_t MainFromServerQueue;
osMessageQueueId_t MainToServerSecQueue;
osMessageQueueId_t MainFromServerSecQueue;


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  Debug_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  DeviceTimeInit();
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */


  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of DebugLogger */
  DebugLoggerHandle = osThreadNew(StartDebugLogger, NULL, &DebugLogger_attributes);

  /* creation of ToServerTCP */
  ToServerTCPHandle = osThreadNew(StartToServerTCP, NULL, &ToServerTCP_attributes);

  /* creation of ServerModbusTCP */
  ServerModbusTCPHandle = osThreadNew(StartServerModbusTCP, NULL, &ServerModbusTCP_attributes);

  /* creation of ClientModbusTCP */
  ClientModbusTCPHandle = osThreadNew(StartClientModbusTCP, NULL, &ClientModbusTCP_attributes);

  /* creation of FromServerTCP */
  FromServerTCPHandle = osThreadNew(StartFromServerTCP, NULL, &FromServerTCP_attributes);

  /* creation of ToServerGPRS */
  ToServerGPRSHandle = osThreadNew(StartToServerGPRS, NULL, &ToServerGPRS_attributes);

  /* creation of FromServerGPRS */
  FromServerGPRSHandle = osThreadNew(StartFromServerGPRS, NULL, &FromServerGPRS_attributes);

  /* creation of TCPTransport */
  TCPTransportHandle = osThreadNew(StartTCPTransport, NULL, &TCPTransport_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
//		osDelay(DeviceTimeStep);
//		UpdateDeviceTime();
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_OTG_FS_PWR_EN_GPIO_Port, USB_OTG_FS_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STLINK_RX_Pin STLINK_TX_Pin */
  GPIO_InitStruct.Pin = STLINK_RX_Pin|STLINK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_PWR_EN_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_OTG_FS_PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OTG_FS_OVCR_Pin */
  GPIO_InitStruct.Pin = USB_OTG_FS_OVCR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OTG_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */

  ShareInit();
  Debug_Message(LOG_INFO, "Запущена основная задача");

  for (;;) {
	  osDelay(STEP_SHARE);

	  ShareSaveChange();
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartDebugLogger */
/**
* @brief Function implementing the DebugLogger thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugLogger */
void StartDebugLogger(void *argument)
{
  /* USER CODE BEGIN StartDebugLogger */
  /* Infinite loop */
	while (!ReadyShare) {
		osDelay(100);
	}
	DebugLoggerLoop();
  /* USER CODE END StartDebugLogger */
}

/* USER CODE BEGIN Header_StartToServerTCP */
/**
* @brief Function implementing the ToServerTCP thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartToServerTCP */
void StartToServerTCP(void *argument)
{
  /* USER CODE BEGIN StartToServerTCP */
  /* Infinite loop */
	while(TransportNeed==0){
		osDelay(100);
	}
	while(1){
		Debug_Message(LOG_INFO, "Запускаем ToServerTCP");
		ToServerTCPLoop();
		osDelay(STEP_TCP);

	}
  /* USER CODE END StartToServerTCP */
}

/* USER CODE BEGIN Header_StartServerModbusTCP */
/**
* @brief Function implementing the ServerModbusTCP thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartServerModbusTCP */
void StartServerModbusTCP(void *argument)
{
  /* USER CODE BEGIN StartServerModbusTCP */
  /* Infinite loop */
	while (!ReadyShare) {
		osDelay(100);
	}
	while(1){
		Debug_Message(LOG_INFO, "Запускаем сервер Modbus");
		ServerModbusTCPLoop();
		osDelay(STEP_SERVER_MODBUS_TCP);
	}
  /* USER CODE END StartServerModbusTCP */
}

/* USER CODE BEGIN Header_StartClientModbusTCP */
/**
* @brief Function implementing the ClientModbusTCP thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartClientModbusTCP */
void StartClientModbusTCP(void *argument)
{
  /* USER CODE BEGIN StartClientModbusTCP */
  /* Infinite loop */
	while (!ReadyShare) {
		osDelay(100);
	}
	while(1){
		Debug_Message(LOG_INFO, "Запускаем клиента Modbus");
		ClientModbusTCPLoop();
		osDelay(STEP_CLIENT_MODBUS_TCP);
	}
  /* USER CODE END StartClientModbusTCP */
}

/* USER CODE BEGIN Header_StartFromServerTCP */
/**
* @brief Function implementing the FromServerTCP thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFromServerTCP */
void StartFromServerTCP(void *argument)
{
  /* USER CODE BEGIN StartFromServerTCP */
  /* Infinite loop */
	while(TransportNeed==0){
		osDelay(100);
	}
	while(1){
		Debug_Message(LOG_INFO, "Запускаем FromServerTCP");
		FromServerTCPLoop();
		osDelay(STEP_TCP);

	}
  /* USER CODE END StartFromServerTCP */
}

/* USER CODE BEGIN Header_StartToServerGPRS */
/**
* @brief Function implementing the ToServerGPRS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartToServerGPRS */
void StartToServerGPRS(void *argument)
{
  /* USER CODE BEGIN StartToServerGPRS */
  /* Infinite loop */
	while(TransportNeed==0){
		osDelay(100);
	}
	while(1){
		while(GPRSNeed==0){
			osDelay(1000);
		}
		Debug_Message(LOG_INFO, "Запускаем ToServerGPRS");
		ToServerGPRSLoop();
		osDelay(STEP_GPRS);

	}
  /* USER CODE END StartToServerGPRS */
}

/* USER CODE BEGIN Header_StartFromServerGPRS */
/**
* @brief Function implementing the FromServerGPRS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFromServerGPRS */
void StartFromServerGPRS(void *argument)
{
  /* USER CODE BEGIN StartFromServerGPRS */
  /* Infinite loop */
	while(TransportNeed==0){
		osDelay(100);
	}
	while(1){
		while(GPRSNeed==0){
			osDelay(1000);
		}
		Debug_Message(LOG_INFO, "Запускаем FromServerGPRS");
		FromServerGPRSLoop();
		osDelay(STEP_GPRS);

	}
  /* USER CODE END StartFromServerGPRS */
}

/* USER CODE BEGIN Header_StartTCPTransport */
/**
* @brief Function implementing the TCPTransport thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTCPTransport */
void StartTCPTransport(void *argument)
{
  /* USER CODE BEGIN StartTCPTransport */
  /* Infinite loop */
	while (!ReadyShare) {
		osDelay(100);
	}
	while(1){
		Debug_Message(LOG_INFO, "Запускаем Transport");
		mainTransportLoop();
		Debug_Message(LOG_ERROR, "Вышли из Transport");
	}
  /* USER CODE END StartTCPTransport */
}

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
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
	while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
