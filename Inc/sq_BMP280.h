/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_BMP280_H_
#define INC_SQ_BMP280_H_

#include "stm32f1xx_hal.h"

/*! @name Function pointer type definitions */
typedef int8_t (*bmp280_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
typedef void (*bmp280_delay_fptr_t)(uint32_t period);

/*! @name Calibration parameters' structure */
struct bmp280_calib_param
{
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    int32_t t_fine;
};


typedef struct
{
    uint8_t id;
    uint8_t reset;
    uint8_t status;
    uint8_t ctrl_meas;
    uint8_t config;
    uint8_t press[3];
    uint8_t temp[3];
} bmp280_dev;

bmp280_dev BMP280;

extern I2C_HandleTypeDef hi2c1;

void getBMP280data(void);
uint8_t BMP280_rx_data[32];

#endif /* INC_SQ_BMP280_H_ */
