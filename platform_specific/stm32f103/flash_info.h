/*
 * Copyright (C) 2017-2022 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file flash_info.h
 * @author d.ponomarev
 * @date Jul 12, 2022
 */

#ifndef PLATFORM_SPECIFIC_STM32F103_FLASH_INFO_H_
#define PLATFORM_SPECIFIC_STM32F103_FLASH_INFO_H_

#include <stdint.h>

#define FLASH_START_ADDR            0x08000000
#define PAGE_SIZE_BYTES             1024
#define FLASH_NUM_OF_PAGES          128
#define FLASH_WORD_SIZE       4

#define FLASH_SIZE_KBYTES           (PAGE_SIZE_BYTES * FLASH_NUM_OF_PAGES)
#define FLASH_LAST_PAGE_ADDR        (FLASH_START_ADDR + PAGE_SIZE_BYTES * (FLASH_NUM_OF_PAGES - 1))

int8_t flashWriteU32(uint32_t address, uint32_t data);

#endif  // PLATFORM_SPECIFIC_STM32F103_FLASH_INFO_H_