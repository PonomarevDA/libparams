/*
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include "main.h"
#include "libparams_error_codes.h"

void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}
int8_t flashErase(uint32_t continuous_page_idx, uint32_t num_of_pages) {
    uint32_t actual_page_index;
    uint32_t flash_bank;
    if (continuous_page_idx < 128) {
        actual_page_index = continuous_page_idx;
        flash_bank = FLASH_BANK_1;
    } else if (continuous_page_idx < 256) {
        actual_page_index = 128 + continuous_page_idx;
        flash_bank = FLASH_BANK_2;
    } else {
        return LIBPARAMS_WRONG_ARGS;
    }

    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = flash_bank,
        .Page = actual_page_index,
        .NbPages = num_of_pages
    };
    uint32_t page_error = 0;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &page_error);
    if (page_error == 0xFFFFFFFF) {
        return LIBPARAMS_OK;
    }
    return -status;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    return -HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
}

uint8_t* flashGetPointer() {
    return (uint8_t*) FLASH_START_ADDR;
}
