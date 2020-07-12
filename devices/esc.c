/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file esc.c
 * @author sainquake
 */

#include "esc.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "main.h"

#define POLES 11.0
#define STATES 6.0

float time = 0;
float dt = 0;
float f = 2;
float f_inv = 0;
int phase = 0;
int count = 0;
int ch = 3;

float fpot = 0;
uint32_t t0 = 0;
uint32_t n = 0;
float fn0 = 0;  //initial neutral
float fn = 0;  //neutral
float fadc = 0;  //phase value
uint32_t adc = 0;
float adc0=0;
uint32_t adc2 = 0;
uint32_t sm_adc = 0;
int ncount = 0;
ADC_ChannelConfTypeDef adc1ch[6];

uint32_t t4old = 0;
typedef struct{
	uint32_t adc;
	uint32_t neutral;
	uint32_t pot;
	uint32_t u_cur;

	float fpot;
	float current0;
	float current;
}ADC_Struct;
ADC_Struct ADC;

typedef struct{
 unsigned char RW;
 unsigned char Reg_Adress;
 unsigned char Data;
 unsigned char OK;

 unsigned char WHO_AM_I;

 unsigned char OUTX_L_XL;
 unsigned char OUTX_H_XL;
 unsigned char OUTY_L_XL;
 unsigned char OUTY_H_XL;
 unsigned char OUTZ_L_XL;
 unsigned char OUTZ_H_XL;

 short ax;
 short ay;
 short az;
}SPI_Struct;
SPI_Struct ACC;

typedef struct{
 uint8_t data[200];
 int count;
 char lock;
}TX_Struct;
TX_Struct TXBuf;

void init();
void PlaySound(float f_, int delay);
void StartAnalogADC2(int ch);
int GetAnalogADC2();
int ReadAnalogADC1(int ch);
int ReadAnalogADC2(int ch);
void ResetControlLOW();
void ResetControlHIGH();
void SetControlHIGH(GPIO_PinState ah, GPIO_PinState al, GPIO_PinState bh, GPIO_PinState bl, GPIO_PinState ch, GPIO_PinState cl);
void SetControlLOW(GPIO_PinState ah, GPIO_PinState al, GPIO_PinState bh, GPIO_PinState bl, GPIO_PinState ch, GPIO_PinState cl);
void Accelerometer(unsigned char rw, unsigned char reg, unsigned char data_);
void ResetControl();
void SetControl(GPIO_PinState ah, GPIO_PinState al, GPIO_PinState bh, GPIO_PinState bl, GPIO_PinState ch, GPIO_PinState cl);

void esc_init(){
  init();
  ADC.current0 = 0;
  for(int i = 0; i<200; i++){
	  ADC.u_cur = ReadAnalogADC1(5);
	  ADC.current0 +=  ((float)ADC.u_cur)/4095.0*3300.0;
  }
  ADC.current0 = ADC.current0/200.0;

  f=1.2;
  f_inv =  1/(f*POLES*STATES);
  TIM4->ARR = (uint32_t)round(12800.0*f_inv);
  TIM3->CCR1 = 350;
  TIM3->CCR2 = TIM3->CCR1;//copy PWM to LED
  HAL_Delay(5000);

  uint8_t  data = 'A';
  TXBuf.lock = 0;
  TXBuf.count = 0;

  //check who am i
  Accelerometer(READ, LSM6DS3_WHO_AM_I_REG, 0);
  if(ACC.Data == 0x69)
    ACC.OK = 1;

  //Enable accelerometer. 52 Hz output. 200 Hz filter bandwidth.
  Accelerometer(WRITE, LSM6DS3_CTRL1_XL, 0x31);
  Accelerometer(READ, LSM6DS3_CTRL1_XL, 0);//check control register

  if(ACC.Data == 0x31)
	  ACC.OK = 1;
  else
	  ACC.OK = 0;

  t4old = TIM4->ARR;
}

void esc_process(){
  if(count>=0){
    count++;
    StartAnalogADC2(ch);
  }
  int ph = phase;
  int chh = ch;
  adc = GetAnalogADC2();
  fadc = (float)adc;

  if(count>=0){
    if( phase==0 || phase==2 || phase==4 ){
      if(fadc<fn && TIM4->CNT>0 && ph==phase && chh==ch) {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        f_inv = (f_inv*39.0+((float)TIM4->CNT)*2.0/12800.0)/40.0;
        TIM4->ARR = (uint32_t)round(12800.0*f_inv);
        count=-1;
      }
    }

    if( phase==1 || phase==3 || phase==5){
      if(fadc>fn  && TIM4->CNT>0 && ph==phase && chh==ch) {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        f_inv = (f_inv*39.0+((float)TIM4->CNT)*2.0/12800.0)/40.0;
        TIM4->ARR = (uint32_t)round(12800.0*f_inv);
        count=-1;
      }
    }
  }

  if(phase==1 && count>0){
    n  = ReadAnalogADC1(3);//neutral
    fn = ((float)n +fn*299.0)/300.0;

    adc2 = ReadAnalogADC1(4);
    fpot = ((float)adc2 +fpot*199.0)/200.0;

    TIM3->CCR1 = 300+(uint32_t)round(fpot/7.0);
    TIM3->CCR2 = TIM3->CCR1;//copy PWM to LED

    ADC.u_cur = ReadAnalogADC1(5);
    ADC.current = ( (((float)ADC.u_cur)/4095.0*3300.0)*0.1 +ADC.current*49.0)/50.0;

    Accelerometer(READ, LSM6DS3_WHO_AM_I_REG, 0);
    if(ACC.Data == 0x69){
      ACC.OK = 1;

      Accelerometer(READ, LSM6DS3_OUTX_L_XL, 0);
      ACC.OUTX_L_XL = ACC.Data;
      Accelerometer(READ, LSM6DS3_OUTX_H_XL, 0);
      ACC.OUTX_H_XL = ACC.Data;
      Accelerometer(READ, LSM6DS3_OUTY_L_XL, 0);
      ACC.OUTY_L_XL = ACC.Data;
      Accelerometer(READ, LSM6DS3_OUTY_H_XL, 0);
      ACC.OUTY_H_XL = ACC.Data;
      Accelerometer(READ, LSM6DS3_OUTZ_L_XL, 0);
      ACC.OUTZ_L_XL = ACC.Data;
      Accelerometer(READ, LSM6DS3_OUTZ_H_XL, 0);
      ACC.OUTZ_H_XL = ACC.Data;

      ACC.ax = (((int)ACC.OUTX_H_XL)<<8) | ACC.OUTX_L_XL;
      ACC.ay = (((int)ACC.OUTY_H_XL)<<8) | ACC.OUTY_L_XL;
      ACC.az = (((int)ACC.OUTZ_H_XL)<<8) | ACC.OUTZ_L_XL;

      if(TXBuf.lock != 1){
      sprintf(TXBuf.data, "Sain\t%d\t%d\t%d\t%f\t%d/3000\t%f\t%f\n", ACC.ax, ACC.ay, ACC.az, 186.95/(f_inv*POLES*STATES),TIM3->CCR1,ADC.current,f_inv );
      TXBuf.lock = 1;
      }
    }else{
      ACC.OK = 0;
    }

    if(TXBuf.lock==1){
      data = TXBuf.data[TXBuf.count];
      if(data=='\n'){
        //HAL_Delay(200);
        TXBuf.count=0;
        TXBuf.lock=0;
      }
      HAL_UART_Transmit(&huart3,&data,1,100);
      TXBuf.count++;
    }
  }

  n  = ReadAnalogADC1(3);//neutral
  fn = ((float)n +fn*299.0)/300.0;
  adc2 = ReadAnalogADC1(4);
  fpot = ((float)adc2 +fpot*499.0)/500.0;

  TIM3->CCR1 = 300+(uint32_t)round(fpot/6.0);
  TIM3->CCR2 = TIM3->CCR1;//copy PWM to LED*/

  TIM2->CCR1 = 1000 + (uint32_t)round(fpot/3.0);//+1000*sin( ((float)HAL_GetTick())/1000.0 );
  TIM2->CCR2 = 2000 - (uint32_t)round(fpot/3.0);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if (hadc->Instance == ADC2){
		//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==TIM2) //check if the interrupt comes from TIM3
		{
		HAL_GPIO_WritePin(SERVO1_GPIO_Port, SERVO1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SERVO2_GPIO_Port, SERVO2_Pin, GPIO_PIN_SET);
		}
	if (htim->Instance==TIM3) //check if the interrupt comes from TIM3
	{
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
		count=-1;

		 HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		phase++;
		if(phase>=6)
			phase=0;
		switch(phase){
									case 0:SetControlHIGH(0,0,1,0,0,1);ch=2;break;
									case 1:SetControlHIGH(0,1,1,0,0,0);ch=1;break;
									case 2:SetControlHIGH(0,1,0,0,1,0);ch=0;break;
									case 3:SetControlHIGH(0,0,0,1,1,0);ch=2;break;
									case 4:SetControlHIGH(1,0,0,1,0,0);ch=1;break;
									case 5:SetControlHIGH(1,0,0,0,0,1);ch=0;break;
		}
		HAL_ADC_ConfigChannel(&hadc2, &adc1ch[ch]);
		HAL_ADC_Start(&hadc2);
		count=0;
	}
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance==TIM2) //check if the interrupt comes from TIM3
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){
			HAL_GPIO_WritePin(SERVO1_GPIO_Port, SERVO1_Pin, GPIO_PIN_RESET);

			HAL_GPIO_WritePin(SERVO2_GPIO_Port, SERVO2_Pin, GPIO_PIN_RESET);
		}
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2){
		}


	}
	if (htim->Instance==TIM3) //check if the interrupt comes from TIM3
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1){
			ResetControlLOW();
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
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

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
  if(hi2c->Instance == I2C1){
    HAL_Delay(100);
  }
}

void init(void){
	//timer 2
	HAL_TIM_Base_MspInit(&htim2);
	HAL_TIM_OC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

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
  adc1ch[0].Channel = ADC_CHANNEL_12;//B//11
  adc1ch[0].Rank = 1;
  adc1ch[0].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  adc1ch[1].Channel = ADC_CHANNEL_13;//C//10
  adc1ch[1].Rank = 1;
  adc1ch[1].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  adc1ch[2].Channel = ADC_CHANNEL_14;//A//1
  adc1ch[2].Rank = 1;
  adc1ch[2].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  adc1ch[3].Channel = ADC_CHANNEL_4;//CNT
  adc1ch[3].Rank = 1;
  adc1ch[3].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  adc1ch[4].Channel = ADC_CHANNEL_0;//pot
  adc1ch[4].Rank = 1;
  adc1ch[4].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;

  adc1ch[5].Channel = ADC_CHANNEL_6;//cur
  adc1ch[5].Rank = 1;
  adc1ch[5].SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  //adc2
  HAL_ADC_MspInit(&hadc2);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);

  //i2c
  /*HAL_I2C_MspInit(&hi2c1);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);*/

  //uart3
  HAL_UART_MspInit(&huart3);

  //uart2
  HAL_UART_MspInit(&huart2);

  //spi2
  HAL_SPI_MspInit(&hspi2);

  /*PlaySound(100,100);
  PlaySound(3,100);
  PlaySound(100,100);
  PlaySound(3,100);
  PlaySound(100,100);
  PlaySound(1,100);*/
}
void Accelerometer(unsigned char rw, unsigned char reg, unsigned char data_){
	ACC.RW = rw;
	ACC.Data = data_;
	ACC.Reg_Adress = reg | ACC.RW;

	HAL_GPIO_WritePin(CS_Acc_GPIO_Port, CS_Acc_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,&ACC.Reg_Adress,1,1);
	if(ACC.RW==READ)
		HAL_SPI_Receive(&hspi2,&ACC.Data,1,1);
	else
		HAL_SPI_Transmit(&hspi2,&ACC.Data,1,1);
	HAL_GPIO_WritePin(CS_Acc_GPIO_Port, CS_Acc_Pin, GPIO_PIN_SET);
}

void PlaySound(float f_,int delay){
	f=f_;
	f_inv =  1/(f*POLES*STATES);
	TIM4->ARR = (uint32_t)round(12800.0*f_inv);
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
void StartAnalogADC2( int ch ){
	HAL_ADC_ConfigChannel(&hadc2, &adc1ch[ch]);
	HAL_ADC_Start(&hadc2);
}
int GetAnalogADC2( void ){
	while( __HAL_ADC_GET_FLAG(&hadc2, ADC_FLAG_EOC)==0 ){}
	return HAL_ADC_GetValue(&hadc2);
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