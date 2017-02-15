/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
//#include "stm32f1xx_hal_gpio.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define AL_Pin GPIO_PIN_7
#define AL_GPIO_Port GPIOA
#define BL_Pin GPIO_PIN_0
#define BL_GPIO_Port GPIOB
#define CL_Pin GPIO_PIN_1
#define CL_GPIO_Port GPIOB
#define AH_Pin GPIO_PIN_8
#define AH_GPIO_Port GPIOA
#define BH_Pin GPIO_PIN_9
#define BH_GPIO_Port GPIOA
#define CH_Pin GPIO_PIN_10
#define CH_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define CS_Acc_Pin GPIO_PIN_5
#define CS_Acc_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/************** Device Register  *******************/
// datasheet: http://www.st.com/content/ccc/resource/technical/document/datasheet/a3/f5/4f/ae/8e/44/41/d7/DM00133076.pdf/files/DM00133076.pdf/jcr:content/translations/en.DM00133076.pdf
#define LSM6DS3_TEST_PAGE  			0x00
#define LSM6DS3_RAM_ACCESS  			0x01
#define LSM6DS3_SENSOR_SYNC_TIME  		0x04
#define LSM6DS3_SENSOR_SYNC_EN  		0x05
#define LSM6DS3_FIFO_CTRL1  			0x06
#define LSM6DS3_FIFO_CTRL2  			0x07
#define LSM6DS3_FIFO_CTRL3  			0x08
#define LSM6DS3_FIFO_CTRL4  			0x09
#define LSM6DS3_FIFO_CTRL5  			0x0A
#define LSM6DS3_ORIENT_CFG_G  			0x0B
#define LSM6DS3_REFERENCE_G  			0x0C
#define LSM6DS3_INT1_CTRL  			0x0D
#define LSM6DS3_INT2_CTRL  			0x0E
#define LSM6DS3_WHO_AM_I_REG  			0x0F
#define LSM6DS3_CTRL1_XL  			0x10
#define LSM6DS3_CTRL2_G  			0x11
#define LSM6DS3_CTRL3_C  			0x12
#define LSM6DS3_CTRL4_C  			0x13
#define LSM6DS3_CTRL5_C  			0x14
#define LSM6DS3_CTRL6_G  			0x15
#define LSM6DS3_CTRL7_G  			0x16
#define LSM6DS3_CTRL8_XL  			0x17
#define LSM6DS3_CTRL9_XL  			0x18
#define LSM6DS3_CTRL10_C  			0x19
#define LSM6DS3_MASTER_CONFIG  		0x1A
#define LSM6DS3_WAKE_UP_SRC  			0x1B
#define LSM6DS3_TAP_SRC  			0x1C
#define LSM6DS3_D6D_SRC  			0x1D
#define LSM6DS3_STATUS_REG  			0x1E
#define LSM6DS3_OUT_TEMP_L  			0x20
#define LSM6DS3_OUT_TEMP_H  			0x21
#define LSM6DS3_OUTX_L_G  			0x22
#define LSM6DS3_OUTX_H_G  			0x23
#define LSM6DS3_OUTY_L_G  			0x24
#define LSM6DS3_OUTY_H_G  			0x25
#define LSM6DS3_OUTZ_L_G  			0x26
#define LSM6DS3_OUTZ_H_G  			0x27
#define LSM6DS3_OUTX_L_XL  			0x28
#define LSM6DS3_OUTX_H_XL  			0x29
#define LSM6DS3_OUTY_L_XL  			0x2A
#define LSM6DS3_OUTY_H_XL  			0x2B
#define LSM6DS3_OUTZ_L_XL  			0x2C
#define LSM6DS3_OUTZ_H_XL  			0x2D
#define LSM6DS3_SENSORHUB1_REG  		0x2E
#define LSM6DS3_SENSORHUB2_REG  		0x2F
#define LSM6DS3_SENSORHUB3_REG  		0x30
#define LSM6DS3_SENSORHUB4_REG  		0x31
#define LSM6DS3_SENSORHUB5_REG  		0x32
#define LSM6DS3_SENSORHUB6_REG  		0x33
#define LSM6DS3_SENSORHUB7_REG  		0x34
#define LSM6DS3_SENSORHUB8_REG  		0x35
#define LSM6DS3_SENSORHUB9_REG  		0x36
#define LSM6DS3_SENSORHUB10_REG  		0x37
#define LSM6DS3_SENSORHUB11_REG  		0x38
#define LSM6DS3_SENSORHUB12_REG  		0x39
#define LSM6DS3_FIFO_STATUS1  			0x3A
#define LSM6DS3_FIFO_STATUS2  			0x3B
#define LSM6DS3_FIFO_STATUS3  			0x3C
#define LSM6DS3_FIFO_STATUS4  			0x3D
#define LSM6DS3_FIFO_DATA_OUT_L  		0x3E
#define LSM6DS3_FIFO_DATA_OUT_H  		0x3F
#define LSM6DS3_TIMESTAMP0_REG  		0x40
#define LSM6DS3_TIMESTAMP1_REG  		0x41
#define LSM6DS3_TIMESTAMP2_REG  		0x42
#define LSM6DS3_STEP_COUNTER_L  		0x4B
#define LSM6DS3_STEP_COUNTER_H  		0x4C
#define LSM6DS3_FUNC_SRC  			0x53
#define LSM6DS3_TAP_CFG1  			0x58
#define LSM6DS3_TAP_THS_6D  			0x59
#define LSM6DS3_INT_DUR2  			0x5A
#define LSM6DS3_WAKE_UP_THS  			0x5B
#define LSM6DS3_WAKE_UP_DUR  			0x5C
#define LSM6DS3_FREE_FALL  			0x5D
#define LSM6DS3_MD1_CFG  			0x5E
#define LSM6DS3_MD2_CFG  			0x5F

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
