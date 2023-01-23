/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rom.h"
#include <stddef.h>
#include <string.h>
#include "flash_driver.h"

///< Default values correspond to the last page access only.
static size_t rom_addr = FLASH_LAST_PAGE_ADDR;
uint32_t start_page_idx = FLASH_NUM_OF_PAGES - 1;
static size_t rom_size_bytes = PAGE_SIZE_BYTES;
static size_t rom_size_pages = 1;


int8_t romInit(uint8_t first_page_idx, uint8_t pages_amount) {
    if (first_page_idx + pages_amount > FLASH_NUM_OF_PAGES || pages_amount == 0) {
        return -1;
    }

    rom_addr = FLASH_START_ADDR + (size_t)first_page_idx * PAGE_SIZE_BYTES;
    start_page_idx = first_page_idx;
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
    memcpy(data, &(flashGetPointer()[start_page_idx * PAGE_SIZE_BYTES + offset]), bytes_to_read);
    return bytes_to_read;
}

void romBeginWrite() {
    flashUnlock();
    flashErase(start_page_idx, rom_size_pages);
}

void romEndWrite() {
    flashLock();
}

size_t romWrite(size_t offset, const uint8_t* data, size_t size) {
    if (data == NULL || offset >= rom_size_bytes || size == 0 || offset + size > rom_size_bytes ) {
        return 0;
    }

    int8_t status = 0;

    for (size_t idx = 0; idx < (size + FLASH_WORD_SIZE - 1) / FLASH_WORD_SIZE; idx++) {
        uint32_t addr = rom_addr + offset + FLASH_WORD_SIZE * idx;
#if FLASH_WORD_SIZE == 4
        uint32_t word = ((const uint32_t*)(const void*)data)[idx];
        status = flashWriteU32(addr, word);
#elif FLASH_WORD_SIZE == 8
        uint64_t word = ((const uint64_t*)(const void*)data)[idx];
        status = flashWriteU64(addr, word);
#endif
    }
    return (status != -1) ? size : 0;
}
