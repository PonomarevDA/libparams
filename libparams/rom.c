/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file rom.c
 * @author d.ponomarev, ko.burdinov
 */

#include "rom.h"
#include <stddef.h>
#include <string.h>
#include "flash_stm32.h"

#ifdef STM32F103xB
    #define FLASH_START_ADDR            0x08000000
    #define FLASH_SIZE_KBYTES           128 * 1024
    #define FLASH_NUM_OF_PAGES          128
    #define FLASH_LAST_PAGE_ADDR        0x0801FC00
    #define FLASH_END_ADDR              0x0801FFFF
#endif

#define STORAGE_PAGE                    0x0801FC00


///< Default values correspond to the last page access only.
static const volatile int32_t* rom_ptr = (const volatile int32_t *)FLASH_LAST_PAGE_ADDR;
static size_t rom_size_bytes = PAGE_SIZE_BYTES;
static size_t rom_size_pages = 1;


int8_t romInit(uint8_t first_page_idx, uint8_t pages_amount) {
    size_t last_page_num = first_page_idx + pages_amount;
    if (last_page_num > FLASH_NUM_OF_PAGES || pages_amount == 0) {
        return -1;
    }

    rom_ptr = (const volatile int32_t*)(FLASH_START_ADDR + (size_t)first_page_idx * PAGE_SIZE_BYTES);
    rom_size_bytes = pages_amount * PAGE_SIZE_BYTES;
    rom_size_pages = pages_amount;
    return 0;
}
size_t romRead(size_t offset, uint8_t* data, size_t requested_size) {
    if (data == NULL || offset >= rom_size_bytes || requested_size == 0) {
        return 0;
    }

    size_t allowed_size = rom_size_bytes - offset;
    size_t bytes_to_read = (allowed_size < requested_size) ? allowed_size : requested_size;
    memcpy(data, ((const uint8_t*)rom_ptr) + offset, bytes_to_read);
    return bytes_to_read;
}
void romBeginWrite() {
    flashUnlock();
    flashErase((uint32_t)(intptr_t)rom_ptr, rom_size_pages);
}
void romEndWrite() {
    flashLock();
}
size_t romWrite(size_t offset, const uint8_t* data, size_t size) {
    if (data == NULL || offset >= rom_size_bytes || size == 0 || offset + size > rom_size_bytes ) {
        return 0;
    }

    uint32_t rom_word_address = (uint32_t)(intptr_t)rom_ptr + (uint32_t)offset;
    int8_t status = 0;
    for (size_t idx = 0; idx < size / 4; idx++) {
        uint32_t word = ((const uint32_t*)(void*)data)[idx];
        status = flashWriteWord(rom_word_address + 4 * idx, word);
    }

    return (status != -1) ? size : 0;
}
