/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "storage.h"
#include "flash_driver.h"
#include "libparams_error_codes.h"
#include "params.hpp"
#include "YamlParameters.hpp"

#define PAGE_SIZE_BYTES                     2048
constexpr uint16_t  str_params_size      = NUM_OF_STR_PARAMS * MAX_STRING_LENGTH;
constexpr uint16_t  int_parms_size       = IntParamsIndexes::INTEGER_PARAMS_AMOUNT * 4;
constexpr uint32_t  params_size_bytes    = (str_params_size + int_parms_size);
constexpr uint8_t   n_flash_pages        = (params_size_bytes / PAGE_SIZE_BYTES) + 1;
// Extend flash size for redundant pages
static uint8_t flash_memory[2 * PAGE_SIZE_BYTES * (n_flash_pages)];
static bool is_locked = true;

extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

static FlashMemoryLayout_t mem_layout = {
    .memory_ptr         = flash_memory,
    .page_size          = PAGE_SIZE_BYTES,
    .num_pages          = n_flash_pages,
};

static ParametersLayout_t params_layout = {
    .integer_desc_pool  = integer_desc_pool,
    .string_desc_pool   = string_desc_pool,
    .num_int_params     = IntParamsIndexes::INTEGER_PARAMS_AMOUNT,
    .num_str_params     = NUM_OF_STR_PARAMS,
};

static YamlParameters yaml_params = YamlParameters(mem_layout, params_layout);

static uint8_t* flashGetPointer();
static int8_t __save_to_files();
static int8_t __read_from_files();

void flashInit() {
#ifdef LIBPARAMS_INIT_PARAMS_FILE_NAME
    yaml_params.set_init_file_name(LIBPARAMS_INIT_PARAMS_FILE_NAME);
#endif
#ifdef LIBPARAMS_TEMP_PARAMS_FILE_NAME
    yaml_params.set_temp_file_name(LIBPARAMS_TEMP_PARAMS_FILE_NAME);
#endif
    __read_from_files();
}

void flashUnlock() {
    is_locked = false;
}

void flashLock() {
    is_locked = true;
}

int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    if (is_locked || start_page_idx + num_of_pages > n_flash_pages || num_of_pages == 0) {
        return LIBPARAMS_WRONG_ARGS;
    }
    memset(flash_memory + start_page_idx * PAGE_SIZE_BYTES, 0x00, num_of_pages * PAGE_SIZE_BYTES);
    return LIBPARAMS_OK;
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
    if (is_locked || offset < FLASH_START_ADDR || offset >= FLASH_START_ADDR + 2 * PAGE_SIZE_BYTES)
    {
        return LIBPARAMS_WRONG_ARGS;
    }

    uint8_t* rom = &(flashGetPointer()[offset - FLASH_START_ADDR]);
    memcpy(rom, data, bytes_to_write);
    uint8_t redundant = (offset - FLASH_START_ADDR) / PAGE_SIZE_BYTES;
    mem_layout.memory_ptr = &flashGetPointer()
                                    [offset - FLASH_START_ADDR - redundant * mem_layout.flash_size];
    return __save_to_files();
}

uint16_t flashGetNumberOfPages() {
    return 2 * n_flash_pages;
}

uint16_t flashGetPageSize() {
    return PAGE_SIZE_BYTES;
}

int8_t __save_to_files() {
    return yaml_params.write_to_dir(LIBPARAMS_PARAMS_DIR);
}

int8_t __read_from_files() {
    return yaml_params.read_from_dir(LIBPARAMS_PARAMS_DIR);
}
