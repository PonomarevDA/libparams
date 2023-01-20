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


void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}
int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_2,
        .Page = start_page_idx,
        .NbPages = num_of_pages
    };
    uint32_t page_error = 0;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &page_error);
    int8_t res = (page_error == 0xFFFFFFFF && status == HAL_OK) ? 0 : -1;
    return res;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    HAL_StatusTypeDef hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
    return (hal_status != HAL_OK) ? -1 : 0;
}

uint8_t* flashGetPointer() {
    return (uint8_t*) FLASH_START_ADDR;
}
