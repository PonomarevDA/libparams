/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "sq_MS4525DO.h"

// https://github.com/ArduPilot/PX4Firmware/blob/master/src/drivers/meas_airspeed/meas_airspeed.cpp#L203

void getMS4525DOdata(void){
	  HAL_I2C_Master_Receive(&hi2c1, (0x28<<1)  +1, airdata.rx, 4, HAL_MAX_DELAY);

	  //T = ((float)temp-511.0)/2047.0*150.0;
	  //D = ((float)data-8192.0)/8192.0*10.0*6894.76-zero;

	  	int16_t dp_raw = 0, dT_raw = 0;
		dp_raw = (airdata.rx[0] << 8) + airdata.rx[1];
		/* mask the used bits */
		dp_raw = 0x3FFF & dp_raw;
		dT_raw = (airdata.rx[2] << 8) + airdata.rx[3];
		dT_raw = (0xFFE0 & dT_raw) >> 5;
		float temperature = ((200.0f * dT_raw) / 2047) - 50;

		// Calculate differential pressure. As its centered around 8000
		// and can go positive or negative
		const float P_min = -1.0f;
		const float P_max = 1.0f;
		const float PSI_to_Pa = 6894.757f;
		/*
		  this equation is an inversion of the equation in the
		  pressure transfer function figure on page 4 of the datasheet
		  We negate the result so that positive differential pressures
		  are generated when the bottom port is used as the static
		  port on the pitot and top port is used as the dynamic port
		 */
		float diff_press_PSI = -((dp_raw - 0.1f * 16383) * (P_max - P_min) / (0.8f * 16383) + P_min);
		float diff_press_pa_raw = diff_press_PSI * PSI_to_Pa;

		airdata.temperature = temperature;
		airdata.diff_press_pa_raw = diff_press_pa_raw;
}
