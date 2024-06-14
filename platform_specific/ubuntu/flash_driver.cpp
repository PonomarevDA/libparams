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
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "YamlParameters.hpp"
#include "libparams_error_codes.h"
#include "params.hpp"
#include "storage.h"

#define PAGE_SIZE_BYTES 2048

namespace fs = std::filesystem;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

uint8_t flash_memory[PAGE_SIZE_BYTES];

static bool is_locked = true;

static uint8_t* flashGetPointer();

void flashInit() {
#ifdef FLASH_DRIVER_STORAGE_FILE

    std::ifstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_STORAGE_FILE, std::ios_base::in);

    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_STORAGE_FILE
                  << " could not be opened for reading!" << std::endl;
        exit(-1);
    }
    std::cout << "Flash driver: data read from " << FLASH_DRIVER_STORAGE_FILE
              << std::endl;
    YamlParameters::read_from_file(flash_memory, params_storage_file);
    params_storage_file.close();
#endif
}

void flashUnlock() { is_locked = false; }

void flashLock() { is_locked = true; }

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

    memcpy(flash_memory + (address - FLASH_START_ADDR), (void*)(&data), flashGetWordSize());

#ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    std::ofstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_SIM_STORAGE_FILE, std::ios_base::out);

    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_SIM_STORAGE_FILE
                  << " could not be opened for writing!" << std::endl;
        return LIBPARAMS_WRONG_ARGS;
    }
    YamlParameters::write_to_file(flash_memory, params_storage_file);
    params_storage_file.close();
    std::cout << "Flash driver: data saved to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << std::endl;

#endif
    return LIBPARAMS_OK;
}

static uint8_t* flashGetPointer() { return (uint8_t*)flash_memory; }

size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}

int8_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write) {
    if (is_locked || offset < FLASH_START_ADDR || offset >= FLASH_START_ADDR + PAGE_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    uint8_t* rom = &(flashGetPointer()[offset - FLASH_START_ADDR]);
    memcpy(rom, data, bytes_to_write);
#ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    std::ofstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_SIM_STORAGE_FILE, std::ios_base::out);

    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_SIM_STORAGE_FILE
                  << " could not be opened for writing!" << std::endl;
        return LIBPARAMS_WRONG_ARGS;
    }
    YamlParameters::write_to_file(flash_memory, params_storage_file);
    params_storage_file.close();
    std::cout << "Flash driver: data saved to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << std::endl;

#endif
    return 0;
}

uint16_t flashGetNumberOfPages() { return 1; }

uint16_t flashGetPageSize() { return PAGE_SIZE_BYTES; }

uint8_t flashGetWordSize() { return 8; }
