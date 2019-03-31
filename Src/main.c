/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "uavcan.h"
#include "sq_flash.h"
#include <math.h>
#include "sq_nmea.h"
#include "sq_BMP280.h"
#include "sq_HMC5883L.h"
#include "sq_MS4525DO.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for myTask02_I2C */
osThreadId_t myTask02_I2CHandle;
const osThreadAttr_t myTask02_I2C_attributes = {
  .name = "myTask02_I2C",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for myCountingSem01 */
osSemaphoreId_t myCountingSem01Handle;
osStaticSemaphoreDef_t myCountingSem01ControlBlock;
const osSemaphoreAttr_t myCountingSem01_attributes = {
  .name = "myCountingSem01",
  .cb_mem = &myCountingSem01ControlBlock,
  .cb_size = sizeof(myCountingSem01ControlBlock),
};
/* USER CODE BEGIN PV */
uint8_t trig=0;

/*uint8_t GNSSrx[256];
uint8_t GNSScommand[32];
uint8_t GNSSdata[128];
uint8_t GNSStime[10];
uint8_t GNSSstatus;
uint8_t GNSSlatitude[7];//широта
uint8_t GNSSlongitude[7];//догота*/
uint8_t tx[8];
uint8_t a[16];
uint16_t temp,data;
float T,D;
float zero;

uint32_t x;

uint32_t count=0;
uint32_t toggle_count=0;

I2C_HandleTypeDef hi2c1;
//uint32_t b[3];

//const volatile uint32_t *fl=(const volatile uint32_t *)0x0801FF00;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CAN_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void *argument);
void StartTask02_I2C(void *argument);

/* USER CODE BEGIN PFP */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	int i=0;
	int j=0;
	int k=0;
	/*for(j=0;j<32;j++){
		GNS.command[j]=0;
	}*/
	for(i=0;i<256;i++){
		if(GNS.buffer[i]=='$'){

			if( GNS.buffer[i+3]=='R' && GNS.buffer[i+4] == 'M' && GNS.buffer[i+5]=='C' ){
				for(k=0;k<128;k++){
					if(i+k>255){
						RMC_data.end = 0;
						break;
					}
					if(GNS.buffer[i+k]==0x0D){
						RMC_data.end=1;
						parseRMC();
						break;
					}
					RMC_data.sentence[k] = GNS.buffer[i+k];
				}
				//dataToFix2();
			}
			if( GNS.buffer[i+3]=='G' && GNS.buffer[i+4] == 'G' && GNS.buffer[i+5]=='A' ){
				for(k=0;k<128;k++){
					if(i+k>255){
						GGA_data.end=0;
						break;
					}
					if(GNS.buffer[i+k]==0x0D){
						GGA_data.end=1;
						parseGGA();
						break;
					}
					GGA_data.sentence[k] = GNS.buffer[i+k];
				}
				//dataToFix2();
			}
			if( GNS.buffer[i+3]=='R' && GNS.buffer[i+4] == 'M' && GNS.buffer[i+5]=='F' ){
				for(k=0;k<128;k++){
					if(i+k>255){
						RMF_data.end=0;
						break;
					}
					if(GNS.buffer[i+k]==0x0D){
						RMF_data.end=1;
						break;
					}
					RMF_data.sentence[k] = GNS.buffer[i+k];
				}
				//dataToFix2();
			}
			/*for(j=0;j<32;j++){
				if(i+j>=32)
					break;
				if(GNS.buffer[i+j]==',')
					break;

				GNS.command[j] = GNS.buffer[i+j];
			}
			i++;
			if(GNS.command[3]=='R' && GNS.command[4] == 'M' && GNS.command[5]=='C'){

				GNS.sentence[RMC].source = GN;
				GNS.sentence[RMC].header = RMC;

				for(k=0;k<128;k++){
					if(i+j+k>=256)
						break;
					if(GNS.buffer[i+j+k]==0x0D)
						break;

					GNS.data[k] = GNS.buffer[i+j+k];
					GNS.sentence[RMC].data[k] = GNS.buffer[i+j+k];
				}

				dataToFix2();
				//break;
				for(j=0;j<32;j++){
					GNS.command[j]=0;
				}
			}*/

		}
	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//xSemaphoreHandle BigMac;
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
  MX_DMA_Init();
  MX_CAN_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  uavcanInit();
  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  //HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);
  zero=0;

  /*b[0] = 12;
  b[1] = 21;
  b[2] = 32;

  flash_unlock();

  flash_write( (uint32_t)STORAGE_PAGE , b, 3);*/
  //HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0801FF00, 88);

/*
  FLASH_EraseInitTypeDef FLASH_EraseInitStruct =
  {
      	.TypeErase = FLASH_TYPEERASE_PAGES,
		.PageAddress = (uint32_t)0x0801FC00,
		.NbPages = 1
   };
   uint32_t error = 0;
   HAL_FLASHEx_Erase(&FLASH_EraseInitStruct,&error);
*/
  	//  flash_erase();
   //flash_lock();
 // FLASH_PageErase(PageAddress)


  //uint32_t x = *fl;
  //fl;
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  //TIM2->CCR4 = 1000;
  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);

  //char tx_[] = "$PMTK220,100*2F/r/n";
  //HAL_UART_Transmit(&huart1, (uint8_t*)tx_ , 17, 3000);
  //HAL_UART_Receive_IT(&huart1, (uint8_t *)GNSSrx, 64);
  HAL_UART_Receive_DMA(&huart1, (uint8_t*) GNS.buffer, 256);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of myCountingSem01 */
  myCountingSem01Handle = osSemaphoreNew(1, 1, &myCountingSem01_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02_I2C */
  myTask02_I2CHandle = osThreadNew(StartTask02_I2C, NULL, &myTask02_I2C_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
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
	  /*HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	  HAL_Delay(200);
	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  HAL_Delay(200);*/

	  //HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_RESET);
	  //HAL_SPI_Receive(&hspi2, a, 2, 100);
	  //HAL_GPIO_WritePin(NSS_GPIO_Port, NSS_Pin, GPIO_PIN_SET);

	  // https://github.com/ArduPilot/PX4Firmware/blob/master/src/drivers/meas_airspeed/meas_airspeed.cpp#L203

	  /*HAL_I2C_Master_Receive(&hi2c1, (0x28<<1)  +1, a, 4, HAL_MAX_DELAY);
	  data = (a[0]<<8) + (a[1]);
	  temp = (a[2]<<3) + (a[3]>>5);

	  T = ((float)temp-511.0)/2047.0*150.0;
	  D = ((float)data-8192.0)/8192.0*10.0*6894.76-zero;
	  if (zero==0)
		  zero = D;*/
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 16;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 72;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 72;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 20000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	    sendCanard();
	    receiveCanard();
	    spinCanard();
	    //publishCanard();

	    //publishActuatorStatus();

	    //publishFix2();

	    /*
	    if (GGA_data.release){
	    	publishFix();
	    	GGA_data.release = 0;
	    }
*/
	    //publishMagneticFieldStrength2();

	    publishAirDataRaw();
	    //myBinarySem01Handle
		//osSemaphoreRelease(myBinarySem01Handle);
	    //osSemaphoreAcquire(storageSemHandle, osWaitForever);
    //osDelay(10);
    //if(osSemaphoreGetCount(myBinarySem01Handle)==1)
    	//osSemaphoreRelease(myCountingSem01Handle);

    //HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
    //if(trig==1)
    	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTask02_I2C */
/**
* @brief Function implementing the myTask02_I2C thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02_I2C */
void StartTask02_I2C(void *argument)
{
  /* USER CODE BEGIN StartTask02_I2C */

	  //a[0] = 0x02;a[1] = 0x00;
	  //HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 2, HAL_MAX_DELAY);
	  //osDelay(100);
  /* Infinite loop */
  for(;;)
  {
	  //getBMP280data();

	  /*uint8_t aaaa = 0xD0;
	  HAL_I2C_Master_Transmit(&hi2c1, 0xEE, &aaaa, 1, HAL_MAX_DELAY);

//a[0] = 0x00;
	  HAL_I2C_Master_Receive(&hi2c1,  0xEE + 1, a, 1, HAL_MAX_DELAY);
*/
	  /*a[0] = 0x00;a[1] = 0x70;
	  HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 2, HAL_MAX_DELAY);

	  a[0] = 0x01;a[1] = 0xA0;
	  HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 2, HAL_MAX_DELAY);
*/

	  //HMC
	  /*a[0] = 0x00;a[1] = (2<<5)+(5<<2);//numeber of samples 2 // output rate 30 hz
	  HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 2, HAL_MAX_DELAY);

	  a[0] = 0x02;a[1] = 0x00;
	  HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 2, HAL_MAX_DELAY);

	  //osDelay(100);

	  a[0] = 0x03;
	  HAL_I2C_Master_Transmit(&hi2c1, 0x3C, a, 1, HAL_MAX_DELAY);

	  a[0] = 0;a[1] = 0;
	  HAL_I2C_Master_Receive(&hi2c1, 0x3C, a, 6, HAL_MAX_DELAY);

	  HMC5883.data[0] = (a[0]<<8) + a[1];
	  HMC5883.data[1] = (a[2]<<8) + a[3];
	  HMC5883.data[2] = (a[4]<<8) + a[5];

	  HMC5883.mag[0] = HMC5883.data[0]/1090.0;
	  HMC5883.mag[1] = HMC5883.data[1]/1090.0;
	  HMC5883.mag[2] = HMC5883.data[2]/1090.0;
	  */

	  // https://github.com/ArduPilot/PX4Firmware/blob/master/src/drivers/meas_airspeed/meas_airspeed.cpp#L203
getMS4525DOdata();
	  	  /*HAL_I2C_Master_Receive(&hi2c1, (0x28<<1)  +1, a, 4, HAL_MAX_DELAY);

	  	  //T = ((float)temp-511.0)/2047.0*150.0;
	  	  //D = ((float)data-8192.0)/8192.0*10.0*6894.76-zero;

	  	  	int16_t dp_raw = 0, dT_raw = 0;
	  		dp_raw = (a[0] << 8) + a[1];
	  		dp_raw = 0x3FFF & dp_raw;
	  		dT_raw = (a[2] << 8) + a[3];
	  		dT_raw = (0xFFE0 & dT_raw) >> 5;
	  		float temperature = ((200.0f * dT_raw) / 2047) - 50;

	  		// Calculate differential pressure. As its centered around 8000
	  		// and can go positive or negative
	  		const float P_min = -1.0f;
	  		const float P_max = 1.0f;
	  		const float PSI_to_Pa = 6894.757f;
	  		float diff_press_PSI = -((dp_raw - 0.1f * 16383) * (P_max - P_min) / (0.8f * 16383) + P_min);
	  		float diff_press_pa_raw = diff_press_PSI * PSI_to_Pa;
*/
	  		// correct for 5V rail voltage if possible
	  		//voltage_correction(diff_press_pa_raw, temperature);

	  		// the raw value still should be compensated for the known offset
	  		//diff_press_pa_raw -= _diff_pres_offset;


	  	//  data = (a[0]<<8) + (a[1]);
	  	//  temp = (a[2]<<3) + (a[3]>>5);

	  	//  T = ((float)temp-511.0)/2047.0*150.0;
	  	//  D = ((float)data-8192.0)/8192.0*10.0*6894.76-zero;
	  	 // if (zero==0)
	  	//	  zero = D;
	  osDelay(40);
  }
  /* USER CODE END StartTask02_I2C */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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
