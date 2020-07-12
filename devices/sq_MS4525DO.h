/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_MS4525DO_H_
#define INC_SQ_MS4525DO_H_

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

typedef struct
{
	uint8_t rx[4];
	float temperature;
	//float diff_press_PSI;
	float diff_press_pa_raw;
} ms4525do_t;

ms4525do_t airdata;
void getMS4525DOdata(void);

#endif /* INC_SQ_MS4525DO_H_ */
