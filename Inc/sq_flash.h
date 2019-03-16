/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file sq_flash.c
 * @author sainquake
 */

#ifndef INC_SQ_FLASH_H_
#define INC_SQ_FLASH_H_

#include <stdint.h>


// Include the appropriate HAL Library here
#include "main.h"
//#include "uavcan.h"

#define STORAGE_PAGE		0x0801FC00


void flash_unlock();
void flash_lock();
void flash_erase();
uint8_t flash_write(uint32_t address, uint32_t *data, uint16_t size);
//uint8_t flash_write_params(uint32_t address, param_t *data, uint16_t size);
void flash_write_param(uint32_t address, uint32_t data);

#endif /* INC_SQ_FLASH_H_ */
