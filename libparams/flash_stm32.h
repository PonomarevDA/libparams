/*
 * Copyright (C) 2017-2022 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file flash_stm32.h
 * @author d.ponomarev
 * @date Jul 12, 2022
 */

#ifndef LIBPARAM_FLASH_STM32_H_
#define LIBPARAM_FLASH_STM32_H_

#include <stdint.h>

void flashUnlock();
void flashLock();
void flashErase(uint32_t page_address, uint32_t num_pf_pages);
int8_t flashWriteWord(uint32_t address, uint32_t data);

#endif  // LIBPARAM_FLASH_STM32_H_
