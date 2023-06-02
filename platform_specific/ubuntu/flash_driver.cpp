/*
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include <stdbool.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "libparams_error_codes.h"

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
        return LIBPARAMS_WRONG_ARGS;
    }
    memset(flash_memory, 0x00, PAGE_SIZE_BYTES);
    return LIBPARAMS_OK;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    if (is_locked || address < FLASH_START_ADDR || address >= FLASH_START_ADDR + PAGE_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }
    memcpy(flash_memory + (address - FLASH_START_ADDR),
           (void*)(&data),
           FLASH_WORD_SIZE);
    return LIBPARAMS_OK;
}

void flashLoadBufferFromFile() {
#ifdef FLASH_DRIVER_STORAGE_FILE
    std::ifstream myfile;
    myfile.open(FLASH_DRIVER_STORAGE_FILE, std::ios::in);
    size_t read_counter = 0;
    while (myfile) {
        if (read_counter % 2) {
            uint32_t param_value;
            uint32_t param_idx = read_counter / 2;
            myfile >> param_value;
            memcpy(flash_memory + 4*param_idx, &param_value, 4);
            std::cout << param_value << std::endl;
        } else {
            std::string mystring;
            myfile >> mystring;
            std::cout << mystring;
        }
        read_counter++;
    }
    std::cout << std::endl;
#endif
}

uint8_t* flashGetPointer() {
    flashLoadBufferFromFile();
    return (uint8_t*) flash_memory;
}
