/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_ROM_FLASH_DRIVER_H_
#define LIBPARAM_ROM_FLASH_DRIVER_H_

#include <stdint.h>
#include <stddef.h>

#define FLASH_START_ADDR            0x08000000

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Do nothing at the moment, but reserved for future possible updates
 */
void flashInit();

/**
 * @param start_page_idx starts from 0 up to maximum number of pages
 * @param num_of_pages last page should not be above flashGetNumberOfPages()
 * @return 0 if success, otherwise < 0
 */
int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages);

/**
 * @brief Before writing you must call flashUnlock(), after writing you must call flashLock()
 *        Write chunk of data like memcpy
 * @return number of written bytes if success, otherwise < 0
 */
int8_t flashUnlock();
int32_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write);
int8_t flashLock();

/**
 * @brief Read chunk of data like memcpy
 * @return bytes_to_read if success, otherwise 0
 */
size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read);


/**
 * @return Info about the flash memory
 */
uint16_t flashGetNumberOfPages();
uint16_t flashGetPageSize();

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_ROM_FLASH_DRIVER_H_
