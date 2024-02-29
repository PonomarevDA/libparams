/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include <string.h>
#include "main.h"
#include "libparams_error_codes.h"

static uint8_t* flashGetPointer();


void flashInit() {
}

void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}

int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    uint32_t page_address = FLASH_START_ADDR + flashGetPageSize() * start_page_idx;
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = 0,
        .PageAddress = (uint32_t)page_address,
        .NbPages = num_of_pages
    };
    uint32_t page_error = 0;
    HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &page_error);
    return (page_error == 0xFFFFFFFF) ? 0 : -1;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    HAL_StatusTypeDef hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
    return (hal_status != HAL_OK) ? -1 : 0;
}

static uint8_t* flashGetPointer() {
    return (uint8_t*) FLASH_START_ADDR;
}

size_t flashMemcpy(uint8_t* data, size_t offset, size_t bytes_to_read) {
    if (data == NULL) {
        return 0;
    }

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}

uint16_t flashGetNumberOfPages() {
    return 128;
}

uint16_t flashGetPageSize() {
    return 1024;
}

uint8_t flashGetWordSize() {
    return 4;
}
