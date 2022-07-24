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

#ifndef LIBPARAM_FLASH_H_
#define LIBPARAM_FLASH_H_

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE_BYTES         (1024)
#define PARAM_STRING_MAX_SIZE   20

/**
 * @brief By default it is initialized as last page only.
 */
int8_t flashInit(uint8_t first_page_idx, uint8_t pages_amount);

/**
 * @brief Return the number of bytes read (may be less than size).
 */
size_t flashRead(size_t offset, uint8_t* data, size_t size);

/**
 * @brief Return the number of bytes wrote (0 in case of error).
 */
size_t flashWrite(size_t offset, const uint8_t* data, size_t size);

/**
 * @brief ???Deprecated???
 */
int64_t flashReadI32ByIndex(uint8_t param_idx);
void flashEraseAllocatedSpace();
int8_t flashWriteU32ByIndex(uint8_t param_idx, uint32_t data);

#endif  // LIBPARAM_FLASH_H_
