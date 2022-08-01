/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file stm32f1xx_hal.c
 * @author d.ponomarev
 * @date Jul 31, 2022
 */

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>

#define PAGE_SIZE 1024
static uint8_t flash_memory[PAGE_SIZE];
static bool is_locked = true;
static uint32_t initial_addr = 0x08000000;

void HAL_FLASH_Init(uint32_t new_initial_addr) {
    initial_addr = new_initial_addr;
}

HAL_StatusTypeDef HAL_FLASH_Unlock(void) {
    if (!is_locked) {
        return HAL_ERROR;
    }
    is_locked = false;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Lock(void) {
    if (is_locked) {
        return HAL_ERROR;
    }
    is_locked = true;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError) {
    if (pEraseInit->PageAddress != FLASH_TYPEERASE_PAGES ||
            pEraseInit->PageAddress != 127 ||
            pEraseInit->NbPages != 1) {
        return HAL_ERROR;
    }
    memset(initial_addr, 0x00, PAGE_SIZE);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data) {
    if (TypeProgram != FLASH_TYPEPROGRAM_WORD ||
            Address < initial_addr ||
            initial_addr >= initial_addr + PAGE_SIZE) {
        return HAL_ERROR;
    }
    return HAL_OK;
}
