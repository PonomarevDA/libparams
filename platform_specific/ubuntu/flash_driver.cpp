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
#define STR_PARAMS_SIZE_BYTES NUM_OF_STR_PARAMS * MAX_STRING_LENGTH
#define INTEGER_PARAMS_SIZE_BYTES IntParamsIndexes::INTEGER_PARAMS_AMOUNT * 4
#define PARAMS_SIZE_BYTES (STR_PARAMS_SIZE_BYTES+ INTEGER_PARAMS_SIZE_BYTES)
#define PAGES_N (PARAMS_SIZE_BYTES / PAGE_SIZE_BYTES) + 1
#define f_name_len strlen(FLASH_DRIVER_STORAGE_FILE) + 10
#define sim_f_name_len strlen(FLASH_DRIVER_STORAGE_FILE) + 10

namespace fs = std::filesystem;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

uint8_t flash_memory[PAGES_N * PAGE_SIZE_BYTES];
static bool is_locked = true;

static uint8_t* flashGetPointer();
static int8_t __save_to_file();
static void __read_from_file();

void flashInit() {
    __read_from_file();
}

void flashUnlock() {
    is_locked = false;
}

void flashLock() {
    is_locked = true;
}

int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (is_locked || num_of_pages >= PAGES_N) {
        return LIBPARAMS_WRONG_ARGS;
    }
    memset(flash_memory + start_page_idx * PAGE_SIZE_BYTES, 0x00, num_of_pages * PAGE_SIZE_BYTES);
    return LIBPARAMS_OK;
}

int8_t flashWriteU64(uint32_t address, uint64_t data) {
    if (is_locked || address < FLASH_START_ADDR || address >= FLASH_START_ADDR + PAGE_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }

    memcpy(flash_memory + (address - FLASH_START_ADDR), (void*)(&data), flashGetWordSize());

    return __save_to_file();
}

static uint8_t* flashGetPointer() {
    return (uint8_t*)flash_memory;
}

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
    return __save_to_file();
}

uint16_t flashGetNumberOfPages() { return 2; }

uint16_t flashGetPageSize() {
    return PAGE_SIZE_BYTES;
}

uint8_t flashGetWordSize() { return 8; }

int8_t __save_to_file(){
#ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    std::string path = FLASH_DRIVER_SIM_STORAGE_FILE;

    auto last = path.find_last_of('.');
    char file_name[sim_f_name_len];
    std::tuple<uint8_t, uint8_t> last_idxs;
    for (uint8_t idx = 0; idx < PAGES_N; idx++) {
        std::ofstream params_storage_file;
        snprintf(file_name, sim_f_name_len, "%s_%d%s", path.substr(0, last).c_str(), idx, path.substr(last).c_str());
        params_storage_file.open(file_name, std::ios_base::out);

        if (!params_storage_file) {
            std::cout << "Flash driver: " << file_name
                    << " could not be opened for writing!" << std::endl;
            return LIBPARAMS_WRONG_ARGS;
        }
        last_idxs = YamlParameters::write_to_file(flash_memory + flashGetPageSize() * idx, params_storage_file, last_idxs);
        params_storage_file.close();
        std::cout << "Flash driver: data saved to " << file_name
                << unsigned(std::get<0>(last_idxs)) << unsigned(std::get<1>(last_idxs)) <<std::endl;
    }
#endif
    return LIBPARAMS_OK;
}

void __read_from_file(){
#ifdef FLASH_DRIVER_STORAGE_FILE
    std::cout << "Flash driver n pages: " << PAGES_N
                << std::endl;
    std::cout << "Flash driver n bytes: " << PARAMS_SIZE_BYTES
    << std::endl;
    std::string path = FLASH_DRIVER_STORAGE_FILE;
    auto last = path.find_last_of('.');
    char file_name[f_name_len];
    for (uint8_t idx = 0; idx < PAGES_N; idx++) {
        std::ifstream params_storage_file;
        snprintf(file_name, f_name_len, "%s_%d%s", path.substr(0, last).c_str(), idx, path.substr(last).c_str());
        params_storage_file.open(file_name, std::ios_base::in);

        if (!params_storage_file) {
            std::cout << "Flash driver: " << file_name
                    << " could not be opened for reading!" << std::endl;
            exit(-1);
        }
        std::cout << "Flash driver: data read from " << file_name
                << std::endl;
        YamlParameters::read_from_file(flash_memory + flashGetPageSize() * idx, params_storage_file);
        params_storage_file.close();
    }
#endif
}
