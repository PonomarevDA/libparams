/*
 * Copyright (C) 2017-2022 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file specific_info.h
 * @author d.ponomarev
 * @date Jul 12, 2022
 */

#ifndef PLATFORM_SPECIFIC_STM32G0B1_FLASH_DRIVER_H_
#define PLATFORM_SPECIFIC_STM32G0B1_FLASH_DRIVER_H_

#include <stdint.h>

#define FLASH_START_ADDR            0x08000000
#define PAGE_SIZE_BYTES             2048
#define FLASH_NUM_OF_PAGES          256
#define FLASH_WORD_SIZE             8

#define FLASH_SIZE_KBYTES           (PAGE_SIZE_BYTES * FLASH_NUM_OF_PAGES)
#define FLASH_LAST_PAGE_ADDR        (FLASH_START_ADDR + PAGE_SIZE_BYTES * (FLASH_NUM_OF_PAGES - 1))


void flashUnlock();
void flashLock();

/**
 * @param start_page_idx starts from 0 up to maximum number of pages
 * @param num_of_pages last page should not be above FLASH_NUM_OF_PAGES
 * @return 0 if success, otherwise -1
 */
int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages);

/**
 * @return 0 if success, otherwise -1
 */
int8_t flashWriteU64(uint32_t address, uint64_t data);

uint8_t* flashGetPointer();

#endif  // PLATFORM_SPECIFIC_STM32G0B1_FLASH_DRIVER_H_
