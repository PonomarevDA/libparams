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
 * @param[in] address - actual address on flash memory
 * @param[in] data - 64 bit of data
 * @return 0 if success, otherwise < 0
 */
void flashUnlock();
int8_t flashWriteU64(uint32_t address, uint64_t data);
void flashLock();

/**
 * @brief Read chunk of data like memcpy
 * @return bytes_to_read if success, otherwise 0
 */
size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read);

/**
 * @brief Write chunk of data like memcpy
 * @return bytes_to_read if success, otherwise 0
 */
int8_t flashWrite(uint8_t* data, size_t offset, size_t bytes_to_write);

/**
 * @return Info about the flash memory
 */
uint16_t flashGetNumberOfPages();
uint16_t flashGetPageSize();
uint8_t flashGetWordSize();

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_ROM_FLASH_DRIVER_H_
