/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_PWM_H_
#define INC_SQ_PWM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
//#include "main.h"

#define A1 			TIM_CHANNEL_2
#define A2 			TIM_CHANNEL_1
#define B1 			TIM_CHANNEL_4
#define B2 			TIM_CHANNEL_3

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

uint32_t setPWM(uint32_t ch, uint32_t pwm);
uint32_t pwmMapping(uint16_t *rc_pwm,int32_t rc_channel, int32_t def);
uint32_t pwmMappingInv(uint16_t *rc_pwm,int32_t rc_channel, int32_t def);
uint32_t pwmMap(int16_t *rc_pwm,int32_t rc_channel, int32_t min, int32_t max, int32_t def);
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);

#endif /* INC_SQ_PWM_H_ */
