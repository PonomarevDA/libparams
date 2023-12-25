/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "flash_driver.h"
#include <stdbool.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cassert>
#include "libparams_error_codes.h"
#include "storage.h"

uint8_t flash_memory[PAGE_SIZE_BYTES];
static bool is_locked = true;

static void flashLoadBufferFromFile();
static uint8_t* flashGetPointer();

void flashInit() {
    flashLoadBufferFromFile();
}

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
    std::cout << "Flash driver: load data from " << FLASH_DRIVER_STORAGE_FILE << "..." << std::endl;
    std::ifstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_STORAGE_FILE, std::ios::in);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_STORAGE_FILE << " been found!" << std::endl;
    }
    size_t read_counter = 0;
    size_t int_param_idx = 0;
    size_t str_param_idx = 0;
    while (params_storage_file) {
        if (read_counter % 2 == 0) {
            std::string mystring;
            params_storage_file >> mystring;
            std::cout << std::setfill(' ') << std::setw(30) << mystring << " ";
        } else {
            std::string param_value;
            params_storage_file >> param_value;
            try {
                uint32_t int_param_value = std::stoi(param_value);
                memcpy(flash_memory + 4*int_param_idx, &int_param_value, 4);
                std::cout << "(offset=" << 4*int_param_idx << ") ";
                int_param_idx++;
            } catch (std::invalid_argument const& ex) {
                size_t offset = PAGE_SIZE_BYTES - MAX_STRING_LENGTH*(1 + str_param_idx);
                memcpy(flash_memory + offset, param_value.c_str(), MAX_STRING_LENGTH);
                std::cout << "(offset=" << offset << ") ";
                str_param_idx++;
            }
            std::cout << param_value << std::endl;
        }
        read_counter++;
    }
    std::cout << std::endl;
#endif
}

static uint8_t* flashGetPointer() {
    return (uint8_t*) flash_memory;
}

size_t flashMemcpy(uint8_t* data, size_t offset, size_t bytes_to_read) {
    assert(data != NULL && "libparams internal error");
    assert(offset < PAGE_SIZE_BYTES && "ROM driver accessing non-existent mem");
    assert(bytes_to_read <= PAGE_SIZE_BYTES && "ROM driver accessing non-existent mem");
    assert(offset + bytes_to_read <= PAGE_SIZE_BYTES && "ROM driver accessing non-existent mem");

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}
