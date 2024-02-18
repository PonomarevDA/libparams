/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "rom.h"
#include <stddef.h>
#include <string.h>
#include "flash_driver.h"
#include "libparams_error_codes.h"


RomDriverInstance romInit(int32_t first_page_idx, size_t pages_amount) {
    if (first_page_idx < 0) {
        first_page_idx += FLASH_NUM_OF_PAGES;
    }

    RomDriverInstance rom = {0};

    if (first_page_idx < 0 ||
            first_page_idx + pages_amount > FLASH_NUM_OF_PAGES ||
            pages_amount == 0) {
        return rom;
    }

    flashInit();

    rom.addr = FLASH_START_ADDR + first_page_idx * PAGE_SIZE_BYTES;
    rom.first_page_idx = first_page_idx;
    rom.total_size = pages_amount * PAGE_SIZE_BYTES;
    rom.pages_amount = pages_amount;
    rom.inited = true;
    return rom;
}

size_t romRead(const RomDriverInstance* rom, size_t offset, uint8_t* data, size_t requested_size) {
    if (rom == NULL || data == NULL || offset >= rom->total_size || requested_size == 0) {
        return 0;
    }

    size_t allowed_size = rom->total_size - offset;
    size_t bytes_to_read;
    if (allowed_size < requested_size) {
        bytes_to_read = allowed_size;
    } else {
        bytes_to_read = requested_size;
    }

    return flashMemcpy(data, rom->first_page_idx * PAGE_SIZE_BYTES + offset, bytes_to_read);
}

void romBeginWrite(const RomDriverInstance* rom) {
    if (rom == NULL) {
        return;
    }

    flashUnlock();
    flashErase((uint32_t)rom->first_page_idx, (uint32_t)rom->pages_amount);
}

size_t romWrite(const RomDriverInstance* rom, size_t offset, const uint8_t* data, size_t size) {
    if (rom == NULL || data == NULL ||
            offset >= rom->total_size || size == 0 || offset + size > rom->total_size) {
        return 0;
    }

    int8_t status = 0;

    for (size_t idx = 0; idx < (size + FLASH_WORD_SIZE - 1) / FLASH_WORD_SIZE; idx++) {
        size_t addr = rom->addr + offset + FLASH_WORD_SIZE * idx;
#if FLASH_WORD_SIZE == 4
        uint32_t word = ((const uint32_t*)(const void*)data)[idx];
        status = flashWriteU32((uint32_t)addr, word);
#elif FLASH_WORD_SIZE == 8
        uint64_t word = ((const uint64_t*)(const void*)data)[idx];
        status = flashWriteU64((uint32_t)addr, word);
#endif
        if (status < 0) {
            break;
        }
    }

    return (status >= 0) ? size : 0;
}

uint32_t romGetAvailableMemory(const RomDriverInstance* rom) {
    if (rom == NULL) {
        return 0;
    }

    return (uint32_t)rom->total_size;
}

void romEndWrite(const RomDriverInstance* rom) {
    if (rom == NULL) {
        return;
    }

    flashLock();
}
