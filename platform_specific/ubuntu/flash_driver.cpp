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
#include "platform_flash_driver.h"
#include "libparams_error_codes.h"
#include "YamlParameters.hpp"

#define PAGE_SIZE_BYTES                     2048
// paramsInit() accepts up to 512 integers and 512 strings.
constexpr uint32_t  max_params_size_bytes = 512 * 4 + 512 * MAX_STRING_LENGTH;
constexpr uint16_t  max_region_pages = (max_params_size_bytes + PAGE_SIZE_BYTES - 1) /
                                       PAGE_SIZE_BYTES;
// Two regions: the redundant one, then the primary one that is loaded from the YAML files.
static uint8_t flash_memory[2 * PAGE_SIZE_BYTES * max_region_pages];
static uint16_t region_pages = 1;
static bool is_locked = true;

extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

static YamlParameters yaml_params;

static void flashInit();
static int8_t flashUnlock();
static int8_t flashLock();
static int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages);
static size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read);
static int32_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write);
static uint16_t flashGetNumberOfPages();
static uint32_t flashGetPageSize();
static uint8_t* flashGetPointer();
static void updateRegionPages();
static bool updateYamlLayout(uint8_t* region);
static int32_t __save_to_files();
static int8_t __read_from_files();

static void flashInit() {
    // Deprecated since v0.15.0, kept for backward compatibility.
    // Can be removed once the supported version is above v0.15.x.
#ifdef LIBPARAMS_INIT_PARAMS_FILE_NAME
#pragma message("LIBPARAMS_INIT_PARAMS_FILE_NAME is deprecated, use LIBPARAMS_DEFAULT_PARAMS_FILE_NAME instead")
    yaml_params.set_default_file_name(LIBPARAMS_INIT_PARAMS_FILE_NAME);
#endif
#ifdef LIBPARAMS_TEMP_PARAMS_FILE_NAME
#pragma message("LIBPARAMS_TEMP_PARAMS_FILE_NAME is deprecated, use LIBPARAMS_NVM_PARAMS_FILE_NAME instead")
    yaml_params.set_nvm_file_name(LIBPARAMS_TEMP_PARAMS_FILE_NAME);
#endif

#ifdef LIBPARAMS_DEFAULT_PARAMS_FILE_NAME
    yaml_params.set_default_file_name(LIBPARAMS_DEFAULT_PARAMS_FILE_NAME);
#endif
#ifdef LIBPARAMS_NVM_PARAMS_FILE_NAME
    yaml_params.set_nvm_file_name(LIBPARAMS_NVM_PARAMS_FILE_NAME);
#endif

    // Load parameters to the last suited pages
    // (such that primary rom at always would be filled after initialization)
    updateRegionPages();
    if (updateYamlLayout(&flashGetPointer()[region_pages * PAGE_SIZE_BYTES])) {
        __read_from_files();
    }
}

static int8_t flashUnlock() {
    is_locked = false;
    return 0;
}

static int8_t flashLock() {
    is_locked = true;
    return 0;
}

static int8_t flashErase(uint32_t start_page_idx, uint32_t num_of_pages) {
    updateRegionPages();
    if (is_locked || start_page_idx + num_of_pages > 2u * region_pages || num_of_pages == 0) {
        return LIBPARAMS_WRONG_ARGS;
    }
    memset(flash_memory + start_page_idx * PAGE_SIZE_BYTES, 0x00, num_of_pages * PAGE_SIZE_BYTES);
    logger.debug("flashErase: page=", start_page_idx, ", count=", num_of_pages);
    return LIBPARAMS_OK;
}

static uint8_t* flashGetPointer() {
    return (uint8_t*)flash_memory;
}

static size_t flashRead(uint8_t* data, size_t offset, size_t bytes_to_read) {
    const uint8_t* rom = &(flashGetPointer()[offset]);
    memcpy(data, rom, bytes_to_read);

    return bytes_to_read;
}

static int32_t flashWrite(const uint8_t* data, size_t offset, size_t bytes_to_write) {
    updateRegionPages();
    const size_t region_size = region_pages * PAGE_SIZE_BYTES;
    if (is_locked || offset < FLASH_START_ADDR ||
            offset >= FLASH_START_ADDR + 2 * region_size ||
            bytes_to_write > FLASH_START_ADDR + 2 * region_size - offset) {
        return LIBPARAMS_WRONG_ARGS;
    }

    uint8_t* rom = &(flashGetPointer()[offset - FLASH_START_ADDR]);
    memcpy(rom, data, bytes_to_write);
    logger.debug("flashWrite: offset=", offset, ", bytes=", bytes_to_write);
    size_t region_idx = (offset - FLASH_START_ADDR) / region_size;
    if (!updateYamlLayout(&flashGetPointer()[region_idx * region_size])) {
        return LIBPARAMS_OK;
    }
    return __save_to_files();
}

static uint16_t flashGetNumberOfPages() {
    updateRegionPages();
    return 2 * region_pages;
}

static uint32_t flashGetPageSize() {
    return PAGE_SIZE_BYTES;
}

const FlashDriverOps* ubuntuFlashGetOps() {
    static const FlashDriverOps ops = {
        flashInit,
        flashUnlock,
        flashLock,
        flashErase,
        flashWrite,
        flashRead,
        flashGetNumberOfPages,
        flashGetPageSize,
        FLASH_START_ADDR,
    };
    return &ops;
}

void ubuntuFlashSetLogLevel(LibparamsLogLevel level) {
    SimpleLogger::level = level;
}

void ubuntuFlashSetLogHandler(LibparamsLogHandler handler) {
    SimpleLogger::handler = handler;
}

// The region size follows the amounts passed to paramsInit(), the same way the application
// picks pages_num, so the primary region matches the last pages_num pages.
static void updateRegionPages() {
    uint32_t params_size = paramsGetIntegersAmount() * sizeof(IntegerParamValue_t) +
                           paramsGetStringsAmount() * MAX_STRING_LENGTH;
    uint32_t pages = (params_size + PAGE_SIZE_BYTES - 1) / PAGE_SIZE_BYTES;
    region_pages = (pages == 0) ? 1 : pages;
}

// False until paramsInit() provides the parameters: there is nothing to read or save yet.
static bool updateYamlLayout(uint8_t* region) {
    ParamIndex_t int_num = paramsGetIntegersAmount();
    ParamIndex_t str_num = paramsGetStringsAmount();
    if (int_num == 0 && str_num == 0) {
        return false;
    }
    FlashMemoryLayout_t flash_layout = {
        .memory_ptr         = region,
        .page_size          = PAGE_SIZE_BYTES,
        .num_pages          = static_cast<uint8_t>(region_pages),
    };
    ParametersLayout_t params_layout = {
        .integer_desc_pool  = integer_desc_pool,
        .string_desc_pool   = string_desc_pool,
        .num_int_params     = int_num,
        .num_str_params     = str_num,
    };
    return yaml_params.set_layout(flash_layout, params_layout) == LIBPARAMS_OK;
}

int32_t __save_to_files() {
    const char* dir = paramsGetDir();
    if (dir == nullptr || dir[0] == '\0') {
        return LIBPARAMS_WRONG_ARGS;
    }
    return yaml_params.write_to_dir(dir);
}

int8_t __read_from_files() {
    const char* dir = paramsGetDir();
    if (dir == nullptr || dir[0] == '\0') {
        return LIBPARAMS_WRONG_ARGS;
    }
    return yaml_params.read_from_dir(dir);
}
