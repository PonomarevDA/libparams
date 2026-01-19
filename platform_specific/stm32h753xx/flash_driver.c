/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include <stdint.h>
#include <string.h>
#include "libparams_error_codes.h"
#include "stm32h7xx_hal.h"

static uint8_t* flashGetPointer();
static int8_t flashEraseSectorsInSingleBank(uint32_t first_sector_idx,
                                            uint32_t num_of_sectors,
                                            uint32_t bank);
static int8_t flashWriteFlashword(uint32_t address, const uint8_t data[]);

static const size_t FLASH_WORD_SIZE = (FLASH_NB_32BITWORD_IN_FLASHWORD * 4U);

void flashInit() {
}

int8_t flashUnlock() {
    return -HAL_FLASH_Unlock();
}

int8_t flashLock() {
    return -HAL_FLASH_Lock();
}

/**
 * @brief Sector (128 KB) erase time
 * typ 1.8 s
 * max 2.6 s
 * @note from STM32H753 datasheet
 */
int8_t flashErase(uint32_t first_page_idx, uint32_t num_of_pages) {
    uint32_t last_page_idx = first_page_idx + num_of_pages;
    if (last_page_idx > flashGetNumberOfPages() || num_of_pages == 0) {
        return LIBPARAMS_WRONG_ARGS;
    }

    int8_t res;
#if defined(DUAL_BANK)
    const uint32_t bank_sectors = FLASH_SECTOR_TOTAL;
    if (first_page_idx < bank_sectors && last_page_idx > bank_sectors) {
        res = flashEraseSectorsInSingleBank(first_page_idx,
                                            bank_sectors - first_page_idx,
                                            FLASH_BANK_1);
        if (res < 0) {
            return res;
        }
        res = flashEraseSectorsInSingleBank(0U, last_page_idx - bank_sectors, FLASH_BANK_2);
    } else if (first_page_idx < bank_sectors) {
        res = flashEraseSectorsInSingleBank(first_page_idx, num_of_pages, FLASH_BANK_1);
    } else {
        res = flashEraseSectorsInSingleBank(first_page_idx - bank_sectors,
                                            num_of_pages,
                                            FLASH_BANK_2);
    }
#else
    res = flashEraseSectorsInSingleBank(first_page_idx, num_of_pages, FLASH_BANK_1);
#endif

    return res;
}

int32_t flashWrite(const uint8_t* data, size_t offset, size_t size) {
    int32_t status = 0;
    const size_t words = (size + FLASH_WORD_SIZE - 1U) / FLASH_WORD_SIZE;

    for (size_t idx = 0U; idx < words; idx++) {
        uint8_t word[FLASH_NB_32BITWORD_IN_FLASHWORD * 4U];
        const size_t addr = offset + FLASH_WORD_SIZE * idx;
        const size_t remaining = size - (idx * FLASH_WORD_SIZE);
        const size_t chunk = (remaining > FLASH_WORD_SIZE) ? FLASH_WORD_SIZE : remaining;

        memset(word, 0xFF, sizeof(word));
        memcpy(word, &data[idx * FLASH_WORD_SIZE], chunk);

        status = flashWriteFlashword((uint32_t)addr, word);
        if (status < 0) {
            break;
        }
    }

    return (status < 0) ? status : (int32_t)size;
}

size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    if (data == NULL) {
        return 0;
    }

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}

uint16_t flashGetNumberOfPages() {
#if defined(DUAL_BANK)
    return (uint16_t)(FLASH_SECTOR_TOTAL * 2U);
#else
    return (uint16_t)FLASH_SECTOR_TOTAL;
#endif
}

uint16_t flashGetPageSize() {
    return (uint16_t)FLASH_SECTOR_SIZE;
}

static uint8_t* flashGetPointer() {
    return (uint8_t*)FLASH_START_ADDR;
}

static int8_t flashEraseSectorsInSingleBank(uint32_t first_sector_idx,
                                            uint32_t num_of_sectors,
                                            uint32_t bank) {
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase = FLASH_TYPEERASE_SECTORS,
        .Banks = bank,
        .Sector = first_sector_idx,
        .NbSectors = num_of_sectors,
        .VoltageRange = FLASH_VOLTAGE_RANGE_3
    };
    uint32_t sector_error = 0U;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    if (sector_error == 0xFFFFFFFFU) {
        return LIBPARAMS_OK;
    }
    return -status;
}

/**
 * @brief 256-bit programming time
 * typ 1.5 ms
 * max 2.3 ms
 * @note from STM32H753 datasheet
 */
static int8_t flashWriteFlashword(uint32_t address, const uint8_t data[]) {
    return -HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address, (uint32_t)(uintptr_t)data);
}
