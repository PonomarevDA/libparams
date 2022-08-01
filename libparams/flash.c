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

#include "flash.h"
#include <stddef.h>
#include <string.h>
#include "flash_stm32.h"

#ifndef MCU_TYPE
    #define STM32F103_128KB
#endif
#ifdef STM32F103_128KB
    #define FLASH_START_ADDR            0x08000000
    #define FLASH_SIZE_KBYTES           128 * 1024
    #define FLASH_NUM_OF_PAGES          128
    #define FLASH_LAST_PAGE_ADDR        0x0801FC00
    #define FLASH_END_ADDR              0x0801FFFF
#endif

#define STORAGE_PAGE                    0x0801FC00


///< Default values correspond to the last page access only.
static const volatile int32_t *flash_memory_ptr = (const volatile int32_t *)FLASH_LAST_PAGE_ADDR;
static size_t flash_size = PAGE_SIZE_BYTES;
static size_t num_of_pages = 1;


int8_t flashInit(uint8_t first_page_idx, uint8_t pages_amount) {
    size_t last_page_num = first_page_idx + pages_amount;
    if (last_page_num > FLASH_NUM_OF_PAGES || pages_amount == 0) {
        return -1;
    }

    size_t flash_memory_addr = (FLASH_START_ADDR + (size_t)first_page_idx * PAGE_SIZE_BYTES);
    flash_memory_ptr = (const volatile int32_t*)(flash_memory_addr);
    flash_size = pages_amount * PAGE_SIZE_BYTES;
    num_of_pages = pages_amount;
    return 0;
}
size_t flashRead(size_t offset, uint8_t* data, size_t requested_size) {
    if (data == NULL || offset >= flash_size || requested_size == 0) {
        return 0;
    }

    size_t allowed_size = flash_size - offset;
    size_t bytes_to_read = (allowed_size < requested_size) ? allowed_size : requested_size;
    memcpy(data, ((const uint8_t*)flash_memory_ptr) + offset, bytes_to_read);
    return bytes_to_read;
}
size_t flashWrite(size_t offset, const uint8_t* data, size_t size) {
    if (data == NULL || offset >= flash_size || size == 0 || offset + size > flash_size ) {
        return 0;
    }

    // flashUnlock();
    // flashEraseAllocatedSpace();
    uint32_t address = (uint32_t)(intptr_t)flash_memory_ptr + (uint32_t)offset;
    int8_t status = 0;
    for (size_t idx = 0; idx < size / 4; idx++) {
        uint32_t written_integer = ((const uint32_t*)(void*)data)[idx];
        status = flashWriteWord(address + 4 * idx, written_integer);
    }
    // flashLock();

    return (status != -1) ? size : 0;
}

void flashEraseAllocatedSpace() {
    flashErase((uint32_t)(intptr_t)flash_memory_ptr, num_of_pages);
}
