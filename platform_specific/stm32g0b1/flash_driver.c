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
static int8_t flashErasePagessInSingleBank(uint32_t first_page_idx, uint32_t num_of_pages);

void flashInit() {
}

void flashUnlock() {
    HAL_FLASH_Unlock();
}
void flashLock() {
    HAL_FLASH_Lock();
}

/**
 * @brief Page (2 KB) erase time
 * typ 22 ms
 * max 40 ms
 * @note from https://www.st.com/resource/en/datasheet/stm32g030c6.pdf
 */
int8_t flashErase(uint32_t first_page_idx, uint32_t num_of_pages) {
    uint32_t last_page_idx = first_page_idx + num_of_pages;
    if (last_page_idx > flashGetNumberOfPages() || num_of_pages == 0) {
        return LIBPARAMS_WRONG_ARGS;
    }

    int8_t res;
    if (first_page_idx <= 127 && last_page_idx >= 128) {
        res = flashErasePagessInSingleBank(first_page_idx, 128 - first_page_idx);
        if (res < 0) {
            return res;
        }
        res = flashErasePagessInSingleBank(128, last_page_idx - 128);
    } else {
        res = flashErasePagessInSingleBank(first_page_idx, num_of_pages);
    }

    return res;
}

/**
 * @brief 64-bit programming time
 * typ 85 us
 * max 125 us
 * @note from https://www.st.com/resource/en/datasheet/stm32g030c6.pdf
 */
int8_t flashWriteU64(uint32_t address, uint64_t data) {
    return -HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
}

static uint8_t* flashGetPointer() {
    return (uint8_t*) FLASH_START_ADDR;
}

int8_t flashWrite(const uint8_t* data, size_t offset, size_t size) {
    int8_t status = flashWaitForLastOperation(FLASH_TIMEOUT_VALUE);

    if (status < 0) {
        return status;
    }
    for (size_t idx = 0U; idx < (size + flashGetWordSize() - 1)/ flashGetWordSize(); idx++) {
        uint64_t word = ((const uint64_t*)(const void*)data)[idx];
        size_t addr = offset + flashGetWordSize() * idx;

        status = flashWriteU64((uint32_t)addr, word);
        if (status < 0) {
            break;
        }
    }

    return size;  // Return the total number of bytes written
}
size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    if (data == NULL) {
        return 0;
    }

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}

int8_t flashErasePagessInSingleBank(uint32_t first_page_idx, uint32_t num_of_pages) {
    uint32_t actual_page_index;
    uint32_t flash_bank;
    if (first_page_idx < 128) {
        actual_page_index = first_page_idx;
        flash_bank = FLASH_BANK_1;
    } else {
        actual_page_index = 128 + first_page_idx;
        flash_bank = FLASH_BANK_2;
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

uint16_t flashGetNumberOfPages() {
    return 256;
}

uint16_t flashGetPageSize() {
    return 2048;
}

uint8_t flashGetWordSize() {
    return 8;
}
