/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "sq_pwm.h"

uint32_t setPWM(uint32_t ch, uint32_t pwm)
{
	//if(pwm<1000)
		//pwm=def;
	if( ch == A1 )
		TIM4->CCR2 = pwm;
	if( ch == A2 )
		TIM4->CCR1 = pwm;
	if( ch == B1 )
		TIM2->CCR4 = pwm;
	if( ch == B2 )
		TIM2->CCR3 = pwm;
	return pwm;
}

uint32_t pwmMapping(uint16_t *rc_pwm,int32_t rc_channel, int32_t def)
{
	if(rc_channel>=0){
			float pwm_in = (float)rc_pwm[ rc_channel ];

			if(pwm_in<0){
				pwm_in = def;
			}else{
				pwm_in = pwm_in/8191.0*1000.0 + 1000.0; // mapping input from 1000 us to 2000 us
			}
			//uint32_t pwm = (uint32_t)pwm_in;
			//setPWM(B1, pwm );
			//test = pwm;
			return pwm_in;
	}else{
		return -1;
	}
}
uint32_t pwmMappingInv(uint16_t *rc_pwm,int32_t rc_channel, int32_t def)
{
	if(rc_channel>=0){
			float pwm_in = (float)rc_pwm[ rc_channel ];
			//if(pwm_in<=0){
			//	pwm_in = def;
			//}else{
				pwm_in = (1000.0-pwm_in/8191.0*1000.0) + 1000.0; // mapping input from 1000 us to 2000 us
			//}
			uint32_t pwm = (uint32_t)pwm_in;
			//setPWM(B1, pwm );
			//test = pwm;
			return pwm;
	}else{
		return -1;
	}
}

uint32_t pwmMap(int16_t *rc_pwm,int32_t rc_channel, int32_t min, int32_t max, int32_t def)
{
	if(rc_channel>=0){
		int32_t pwm_in = (int32_t)rc_pwm[ rc_channel ];

		if(pwm_in<0){
			pwm_in = def;
		}else{
			pwm_in = map(pwm_in,0,8191,min,max);
		}
			//uint32_t pwm = (uint32_t)pwm_in;
			//setPWM(B1, pwm );
			//test = pwm;
		return pwm_in;
	}else{
		return -1;
	}
}
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{

  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}
