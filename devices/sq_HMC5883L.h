/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_HMC5883L_H_
#define INC_SQ_HMC5883L_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

uint8_t a[16];
typedef struct
{
	uint8_t configA;
	uint8_t configB;
	uint8_t mode;
	int16_t data[3];
	uint8_t status;
	uint32_t id;

	float mag[3];
} HMC5883_t;

HMC5883_t HMC5883;

#endif /* INC_SQ_HMC5883L_H_ */
