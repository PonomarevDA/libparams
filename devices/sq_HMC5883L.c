/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "sq_HMC5883L.h"
extern I2C_HandleTypeDef hi2c1;
void getHMC5883Ldata(void){
	  a[0] = 0x00;a[1] = (2<<5)+(5<<2);//numeber of samples 2 // output rate 30 hz
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
}
