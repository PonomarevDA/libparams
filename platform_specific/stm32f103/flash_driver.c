/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include <string.h>
#include "libparams_error_codes.h"
#include "flash_registers.h"


#define FLASH_TIMEOUT_VALUE     50000U
#define FLASH_MAX_DELAY         0xFFFFFFFFU

#define SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))
#define WRITE_REG(REG, VAL)     ((REG) = (VAL))

uint32_t HAL_GetTick();

static uint8_t* flashGetPointer();
static int8_t flashWaitForLastOperation(uint32_t timeout);
static void flashPageErase(uint32_t page_address);
static void flashProgramHalfWord(uint32_t address, uint16_t data);


void flashInit() {
}

void flashUnlock() {
    WRITE_REG(FLASH->KEYR, FLASH_KEYR_KEY1);
    WRITE_REG(FLASH->KEYR, FLASH_KEYR_KEY2);
}
void flashLock() {
    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}

/**
 * @brief Page (1 KB) erase time
 * min 20 ms
 * max 40 ms
 * @note from https://www.st.com/resource/en/datasheet/stm32f103c8.pdf
 */
int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (flashWaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE) != 0) {
        return -1;
    }

    int8_t status = -1;
    uint32_t first_page_address = FLASH_START_ADDR + FLASH_PAGE_SIZE * start_page_idx;
    uint32_t last_page_address = first_page_address + (num_of_pages * FLASH_PAGE_SIZE);
    for (uint32_t addr = first_page_address; addr < last_page_address; addr += FLASH_PAGE_SIZE) {
        flashPageErase(addr);
        status = flashWaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
        CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
        if (status != 0) {
            break;
        }
    }

    return status;
}

/**
 * @brief 16-bit programming time
 * min 40 us
 * typ 52.5 us
 * max 70 us
 * @note from https://www.st.com/resource/en/datasheet/stm32f103c8.pdf
 */
int8_t flashWriteU64(uint32_t address, uint64_t data) {
    int8_t status = flashWaitForLastOperation(FLASH_TIMEOUT_VALUE);

    if (status < 0) {
        return status;
    }

    for (uint8_t index = 0U; index < 4; index++) {
        flashProgramHalfWord((address + (2U*index)), (uint16_t)(data >> (16U*index)));

        status = flashWaitForLastOperation(FLASH_TIMEOUT_VALUE);

        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
        if (status < 0) {
            break;
        }
    }

    return status;
}

static uint8_t* flashGetPointer() {
    return (uint8_t*) FLASH_START_ADDR;
}

int8_t flashWrite(const uint8_t* data, size_t offset, size_t size) {
    int8_t status = flashWaitForLastOperation(FLASH_TIMEOUT_VALUE);

    if (status < 0) {
        return status;
    }
    size_t n_half_words = (size + 1)/2;

    for (size_t i = 0; i < n_half_words; i += 1) {
        const uint16_t halfword = (data[2U*i + 1]) << 8 | data[2U*i];
        flashProgramHalfWord(offset + 2U*i, halfword);

        status = flashWaitForLastOperation(FLASH_TIMEOUT_VALUE);
        if (status < 0) {
            break;
        }
        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
    }

    return status;
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
    return 128;
}

uint16_t flashGetPageSize() {
    return FLASH_PAGE_SIZE;
}

uint8_t flashGetWordSize() {
    return 8;
}

static int8_t flashWaitForLastOperation(uint32_t timeout) {
    uint32_t tickstart = HAL_GetTick();

    while (FLASH->SR & FLASH_SR_BSY) {
        if (timeout != FLASH_MAX_DELAY) {
            if ((timeout == 0U) || ((HAL_GetTick() - tickstart) > timeout)) {
                return -3;
            }
        }
    }

    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }

    if (FLASH->SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR) || FLASH->OBR & FLASH_OBR_OPTERR) {
        return -1;
    }

    return 0;
}

static void flashPageErase(uint32_t page_address) {
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    WRITE_REG(FLASH->AR, page_address);
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
}

static void flashProgramHalfWord(uint32_t address, uint16_t data) {
    SET_BIT(FLASH->CR, FLASH_CR_PG);
    *(volatile uint16_t*)address = data;
}
