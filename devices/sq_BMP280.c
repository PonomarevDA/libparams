/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "sq_BMP280.h"



void getBMP280data(void){
	  uint8_t aaaa[2];
	  aaaa[0] = 0xD0;
	  HAL_I2C_Master_Transmit(&hi2c1, 0xEE, aaaa, 1, HAL_MAX_DELAY);
	  HAL_I2C_Master_Receive(&hi2c1,  0xEE + 1, BMP280_rx_data, 1, HAL_MAX_DELAY);
	  BMP280.id = BMP280_rx_data[0];

	  aaaa[0] = 0xF4;
	  aaaa[1] = 0x03;
	  HAL_I2C_Master_Transmit(&hi2c1, 0xEE, aaaa, 2, HAL_MAX_DELAY);
	  //HAL_I2C_Master_Receive(&hi2c1,  0xEE, BMP280_rx_data[7], 1, HAL_MAX_DELAY);

	  aaaa[0] = 0xF7;
	  HAL_I2C_Master_Transmit(&hi2c1, 0xEE, aaaa, 1, HAL_MAX_DELAY);

	  HAL_I2C_Master_Receive(&hi2c1,  0xEE + 1, &BMP280_rx_data[1], 6, HAL_MAX_DELAY);
}

