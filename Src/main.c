/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
float time=0;
float dt=0;
float f=2;
float f_inv = 0;
#define POLES 11.0
#define STATES 6.0
int phase = 0;
int count = 0;
int ch = 3;

float fpot=0;
uint32_t t0=0;
uint32_t n=0;
float fn=0;
float fadc=0;
uint32_t adc = 0;
uint32_t adc2 = 0;
uint32_t sm_adc = 0;
ADC_ChannelConfTypeDef adc1ch[5];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);

/* USER CODE BEGIN PFP */
void init(void);
void PlaySound(float f_,int delay);
int ReadAnalogADC1( int ch );
int ReadAnalogADC2( int ch );
void ResetControlLOW(void);
void ResetControlHIGH(void);
void SetControlHIGH(GPIO_PinState ah,GPIO_PinState al,GPIO_PinState bh,GPIO_PinState bl,GPIO_PinState ch,GPIO_PinState cl);
void SetControlLOW(GPIO_PinState ah,GPIO_PinState al,GPIO_PinState bh,GPIO_PinState bl,GPIO_PinState ch,GPIO_PinState cl);

/* Private function prototypes -----------------------------------------------*/
void ResetControl(void);
void SetControl(GPIO_PinState ah,GPIO_PinState al,GPIO_PinState bh,GPIO_PinState bl,GPIO_PinState ch,GPIO_PinState cl);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if (hadc->Instance == ADC2){
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==TIM3) //check if the interrupt comes from TIM3
	{
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		time+=dt;
		if( time>=f_inv )
			time=0;
			/*phase++;
			if(phase>=6)
				phase=0;
			switch(phase){
							case 0:SetControlHIGH(0,0,1,0,0,1);break;
							case 1:SetControlHIGH(0,1,1,0,0,0);break;
							case 2:SetControlHIGH(0,1,0,0,1,0);break;
							case 3:SetControlHIGH(0,0,0,1,1,0);break;
							case 4:SetControlHIGH(1,0,0,1,0,0);break;
							case 5:SetControlHIGH(1,0,0,0,0,1);break;
						}
			  switch(phase){
				case 0:ch=2;break;
				case 1:ch=1;break;
				case 2:ch=0;break;
				case 3:ch=2;break;
				case 4:ch=1;break;
				case 5:ch=0;break;
			  }
			  count=0;*/
			//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		//}
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		switch(phase){
						case 0:SetControlLOW(0,0,1,0,0,1);break;
						case 1:SetControlLOW(0,1,1,0,0,0);break;
						case 2:SetControlLOW(0,1,0,0,1,0);break;
						case 3:SetControlLOW(0,0,0,1,1,0);break;
						case 4:SetControlLOW(1,0,0,1,0,0);break;
						case 5:SetControlLOW(1,0,0,0,0,1);break;
					}
	}
	if (htim->Instance==TIM4) //check if the interrupt comes from TIM3
	{
		//HAL_GPIO_TogglePin(AH_GPIO_Port, AH_Pin);
		phase++;
					if(phase>=6)
						phase=0;
					switch(phase){
									case 0:SetControlHIGH(0,0,1,0,0,1);break;
									case 1:SetControlHIGH(0,1,1,0,0,0);break;
									case 2:SetControlHIGH(0,1,0,0,1,0);break;
									case 3:SetControlHIGH(0,0,0,1,1,0);break;
									case 4:SetControlHIGH(1,0,0,1,0,0);break;
									case 5:SetControlHIGH(1,0,0,0,0,1);break;
								}
					  switch(phase){
						case 0:ch=2;break;
						case 1:ch=1;break;
						case 2:ch=0;break;
						case 3:ch=2;break;
						case 4:ch=1;break;
						case 5:ch=0;break;
					  }
					  count=0;
	}
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance==TIM3) //check if the interrupt comes from TIM3
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){
			ResetControlLOW();
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2){
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	}

}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance==TIM4) //check if the interrupt comes from TIM3
		{
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

		}
}
//void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim){

//}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();

  /* USER CODE BEGIN 2 */
  init();

  f=3;
  f_inv =  1/(f*POLES*STATES);
  TIM4->ARR = (uint32_t)round(12800.0*f_inv);
  TIM3->CCR1 = 300;
  TIM3->CCR2 = TIM3->CCR1;//copy PWM to LED

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */


	  float f0 = 4;



	  if(f>=f0) {
		  if(HAL_GetTick()-t0>3000){
			  int old_phase = phase;

			  if(count>=0){
				  count++;
			  }
			  adc = ReadAnalogADC2(ch);
			  fadc = (float)adc;
			  if(old_phase==phase && count>=0){
				  if(old_phase==0 || old_phase==2 || old_phase==4){
					  if(fadc<fn) {
						  //f_inv = (f_inv*39.0+time*6.0)/40.0;
						  f_inv = (f_inv*39.0+((float)TIM4->CNT)*6.0/12800.0)/40.0;
						  TIM4->ARR = (uint32_t)round(12800.0*f_inv);
						  //f_inv =  (f_inv*19.0+time*SOKLSHENIE)/20.0;

						  count=-1;
						  //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
						  //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
					  }
				  }
			  }
		  }

	  }else{

		  f=2+(float)(HAL_GetTick())/300.0;
		  f_inv =  1/(f*POLES*STATES);
		  TIM4->ARR = (uint32_t)round(12800.0*f_inv);
		  /*f=1.5+(float)(HAL_GetTick()-t0)/300.0;
		  f_inv =  1/(f*POLES*STATES);*/
	  }

	  if(phase==1 || phase==3 || phase==5){
		  n  = ReadAnalogADC1(3);
		  fn = ((float)n +fn*299.0)/300.0;

		  adc2 = ReadAnalogADC1(4);
		  fpot = ((float)adc2 +fpot*199.0)/200.0;

		  TIM3->CCR1 = 300+(uint32_t)round(fpot/18.0);
		  TIM3->CCR2 = TIM3->CCR1;//copy PWM to LED
	  }

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
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

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* ADC2 init function */
static void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config 
    */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Regular Channel 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_OC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigOC.Pulse = 2500;
  if (HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

}

/* TIM4 init function */
static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 5000;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin AL_Pin AH_Pin BH_Pin 
                           CH_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|AL_Pin|AH_Pin|BH_Pin 
                          |CH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BL_Pin CL_Pin PB5 */
  GPIO_InitStruct.Pin = BL_Pin|CL_Pin|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|AL_Pin|AH_Pin|BH_Pin 
                          |CH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BL_Pin|CL_Pin|GPIO_PIN_5, GPIO_PIN_RESET);

}

/* USER CODE BEGIN 4 */
void init(void){
	  //TIMER3 16 khz PWM
	  HAL_TIM_Base_MspInit(&htim3);
	  HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_1);
	  //HAL_TIM_OC_Start_IT(&htim3,TIM_CHANNEL_2);
	  HAL_TIM_Base_Start_IT(&htim3);
	  HAL_NVIC_EnableIRQ(TIM3_IRQn);

	  dt = (float)(htim3.Init.Prescaler+1)*(float)(htim3.Init.Period)/64000000.0;
	  //
	  HAL_TIM_Base_MspInit(&htim4);
	  HAL_TIM_Base_Start_IT(&htim4);
	  //HAL_TIM_IC_Start(&htim4,TIM_CHANNEL_1);
	  HAL_NVIC_EnableIRQ(TIM4_IRQn);

	  //ADC1
	  HAL_ADC_MspInit(&hadc1);
	  adc1ch[0].Channel = ADC_CHANNEL_11;//B
	  adc1ch[0].Rank = 1;
	  adc1ch[0].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	  adc1ch[1].Channel = ADC_CHANNEL_10;//C
	  adc1ch[1].Rank = 1;
	  adc1ch[1].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	  adc1ch[2].Channel = ADC_CHANNEL_1;//A
	  adc1ch[2].Rank = 1;
	  adc1ch[2].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	  adc1ch[3].Channel = ADC_CHANNEL_4;//CNT
	  adc1ch[3].Rank = 1;
	  adc1ch[3].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

	  adc1ch[4].Channel = ADC_CHANNEL_0;//pot
	  adc1ch[4].Rank = 1;
	  adc1ch[4].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  //adc2
	  HAL_ADC_MspInit(&hadc2);
	  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

	  /*PlaySound(100,100);
	  PlaySound(3,100);
	  PlaySound(100,100);
	  PlaySound(3,100);
	  PlaySound(100,100);
	  PlaySound(1,100);*/
}
void PlaySound(float f_,int delay){
	f=f_;
	f_inv =  1/(f*POLES*STATES);
	TIM3->CCR1 = 250;
	TIM3->CCR2 = 250;
	HAL_Delay(delay);
}
int ReadAnalogADC1( int ch ){
	HAL_ADC_ConfigChannel(&hadc1, &adc1ch[ch]);//A4 / B
	HAL_ADC_Start(&hadc1);
	while( __HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC)==0 ){}
	return HAL_ADC_GetValue(&hadc1);
}
int ReadAnalogADC2( int ch ){
	HAL_ADC_ConfigChannel(&hadc2, &adc1ch[ch]);
	HAL_ADC_Start(&hadc2);
	while( __HAL_ADC_GET_FLAG(&hadc2, ADC_FLAG_EOC)==0 ){}
	return HAL_ADC_GetValue(&hadc2);
}
void ResetControlLOW(void){
	HAL_GPIO_WritePin(AL_GPIO_Port, AL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BL_GPIO_Port, BL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CL_GPIO_Port, CL_Pin, GPIO_PIN_RESET);
}
void ResetControlHIGH(void){
	HAL_GPIO_WritePin(AH_GPIO_Port, AH_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BH_GPIO_Port, BH_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CH_GPIO_Port, CH_Pin, GPIO_PIN_RESET);
}
void SetControlHIGH(GPIO_PinState ah,GPIO_PinState al,GPIO_PinState bh,GPIO_PinState bl,GPIO_PinState ch,GPIO_PinState cl){
	HAL_GPIO_WritePin(AH_GPIO_Port, AH_Pin, ah);
	//HAL_GPIO_WritePin(AL_GPIO_Port, AL_Pin, al);
	HAL_GPIO_WritePin(BH_GPIO_Port, BH_Pin, bh);
	//HAL_GPIO_WritePin(BL_GPIO_Port, BL_Pin, bl);
	HAL_GPIO_WritePin(CH_GPIO_Port, CH_Pin, ch);
	//HAL_GPIO_WritePin(CL_GPIO_Port, CL_Pin, cl);
}
void SetControlLOW(GPIO_PinState ah,GPIO_PinState al,GPIO_PinState bh,GPIO_PinState bl,GPIO_PinState ch,GPIO_PinState cl){
	//HAL_GPIO_WritePin(AH_GPIO_Port, AH_Pin, ah);
	HAL_GPIO_WritePin(AL_GPIO_Port, AL_Pin, al);
	//HAL_GPIO_WritePin(BH_GPIO_Port, BH_Pin, bh);
	HAL_GPIO_WritePin(BL_GPIO_Port, BL_Pin, bl);
	//HAL_GPIO_WritePin(CH_GPIO_Port, CH_Pin, ch);
	HAL_GPIO_WritePin(CL_GPIO_Port, CL_Pin, cl);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
