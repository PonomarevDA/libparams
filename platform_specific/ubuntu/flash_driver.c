/*
 * Copyright (C) 2017-2022 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file flash_driver.c
 * @author d.ponomarev
 * @date Apr 1, 2020
 */

#include "flash_driver.h"
#include <stdbool.h>
#include <string.h>
#include <cstddef>

static uint8_t flash_memory[PAGE_SIZE_BYTES];
static bool is_locked = true;

void flashUnlock() {
    is_locked = false;
}

void flashLock() {
    is_locked = true;
}

int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (is_locked || start_page_idx != 0 || num_of_pages != 1) {
        return -1;
    }
    memset(flash_memory, 0x00, PAGE_SIZE_BYTES);
    return 0;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    if (is_locked || address < FLASH_START_ADDR || address >= FLASH_START_ADDR + PAGE_SIZE_BYTES) {
        return -1;
    }
    memcpy(flash_memory + (address - FLASH_START_ADDR),
           (void*)(&data),
           FLASH_WORD_SIZE);
    return 0;
}
