/*
 * Copyright (C) 2017-2022 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file flash_driver.h
 * @author d.ponomarev
 * @date Jul 12, 2022
 */

#ifndef LIBPARAM_FLASH_DRIVER_H_
#define LIBPARAM_FLASH_DRIVER_H_

#include "flash_info.h"

void flashUnlock();
void flashLock();

/**
 * @return 0 if success, otherwise -1
 */
int8_t flashErase(uint32_t page_address, uint32_t num_pf_pages);

#endif  // LIBPARAM_FLASH_DRIVER_H_
