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

#include "libparams_error_codes.h"
#include "storage.h"

#define PAGE_SIZE_BYTES 2048

namespace fs = std::filesystem;
extern IntParamsIndexes;
extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

uint8_t flash_memory[PAGE_SIZE_BYTES];

static bool is_locked = true;

static void flashLoadBufferFromFile();
static void flashSaveBufferToFile();
static uint8_t* flashGetPointer();

void flashInit() { flashLoadBufferFromFile(); }

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
    if (is_locked || address < FLASH_START_ADDR ||
        address >= FLASH_START_ADDR + PAGE_SIZE_BYTES) {
        return LIBPARAMS_WRONG_ARGS;
    }
    std::cout << "flashWriteU64";

    memcpy(flash_memory + (address - FLASH_START_ADDR), (void*)(&data),
           flashGetWordSize());
    flashSaveBufferToFile();
    return LIBPARAMS_OK;
}

void flashLoadBufferFromFile() {
#if defined(FLASH_DRIVER_SIM_STORAGE_FILE) && defined(FLASH_DRIVER_STORAGE_FILE)
    std::ifstream params_storage_file;
    auto file_dir = FLASH_DRIVER_SIM_STORAGE_FILE;
    fs::directory_entry entry{file_dir};
    if (!entry.exists()) {
        file_dir = FLASH_DRIVER_STORAGE_FILE;
    }
    params_storage_file.open(file_dir, std::ios::in);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << file_dir << " been found!"
                  << std::endl;
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
                memcpy(flash_memory + 4 * int_param_idx, &int_param_value, 4);
                std::cout << "(offset=" << 4 * int_param_idx << ") ";
                int_param_idx++;
            } catch (std::invalid_argument const& ex) {
                size_t offset =
                    PAGE_SIZE_BYTES - MAX_STRING_LENGTH * (1 + str_param_idx);
                memcpy(flash_memory + offset, param_value.c_str(),
                       MAX_STRING_LENGTH);
                std::cout << "(offset=" << offset << ") ";
                str_param_idx++;
            }
            std::cout << param_value << std::endl;
        }
        read_counter++;
    }
    std::cout << "flash is done" << std::endl;
    std::cout << std::endl;
#endif
}

void flashSaveBufferToFile() {
#ifdef FLASH_DRIVER_SIM_STORAGE_FILE
    std::cout << "Flash driver: save data to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << "..." << std::endl;

    std::ofstream params_storage_file;
    params_storage_file.open(FLASH_DRIVER_SIM_STORAGE_FILE, std::ios::out);
    if (!params_storage_file) {
        std::cout << "Flash driver: " << FLASH_DRIVER_SIM_STORAGE_FILE
                  << " could not be opened for writing!" << std::endl;
        return;
    }

    size_t idx = 0;
    size_t str_idx = 0;

    for (size_t index = 0; index < NUM_OF_STR_PARAMS; index++) {
        std::string str_param_value(
            reinterpret_cast<char*>(flash_memory + index * MAX_STRING_LENGTH),
            MAX_STRING_LENGTH);
        std::cout << "(" << string_desc_pool[str_idx].name << "="
                  <<(str_param_value.c_str()) << ") " << std::endl;
        params_storage_file << string_desc_pool[str_idx].name << ": "
                            << std::quoted(str_param_value.c_str()) << std::endl;
        str_idx++;
    }
    auto offset = NUM_OF_STR_PARAMS * str_idx;

    for (size_t index = 0; index < IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
         index++) {
        uint32_t int_param_value;
        memcpy(&int_param_value, flash_memory + offset + index * 4, 4);
        std::cout << "(" << integer_desc_pool[idx].name << "="
                  << int_param_value << ") " << std::endl;
        params_storage_file << integer_desc_pool[idx].name << ": "
                            << int_param_value << std::endl;
        idx++;
    }
    params_storage_file.close();
    std::cout << "Flash driver: data saved to " << FLASH_DRIVER_SIM_STORAGE_FILE
              << std::endl;
#endif
}

static uint8_t* flashGetPointer() { return (uint8_t*)flash_memory; }

size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    assert(data != NULL && "libparams internal error");
    assert(offset < PAGE_SIZE_BYTES && "ROM driver accessing non-existent mem");
    assert(bytes_to_read <= PAGE_SIZE_BYTES &&
           "ROM driver accessing non-existent mem");
    assert(offset + bytes_to_read <= PAGE_SIZE_BYTES &&
           "ROM driver accessing non-existent mem");

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);
    return bytes_to_read;
}

size_t flashWrite(uint8_t* data, size_t offset, size_t bytes_to_write) {
    assert(data != NULL && "libparams internal error");
    assert(offset < PAGE_SIZE_BYTES && "ROM driver accessing non-existent mem");
    assert(bytes_to_write <= PAGE_SIZE_BYTES &&
           "ROM driver accessing non-existent mem");
    assert(offset + bytes_to_write <= PAGE_SIZE_BYTES &&
           "ROM driver accessing non-existent mem");

    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(&rom, data, bytes_to_write);
    flashSaveBufferToFile();
    return bytes_to_write;
}

uint16_t flashGetNumberOfPages() { return 1; }

uint16_t flashGetPageSize() { return PAGE_SIZE_BYTES; }

uint8_t flashGetWordSize() { return 8; }
