/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "storage.h"
#include <string.h>
#include <stdio.h>
#include "flash_driver.h"
#include "rom.h"
#include "libparams_error_codes.h"
#include "params.hpp"

extern IntegerDesc_t integer_desc_pool[];
extern IntegerParamValue_t integer_values_pool[];
extern StringDesc_t string_desc_pool[];
extern StringParamValue_t string_values_pool[];

static ParamIndex_t integers_amount = 0;
static ParamIndex_t strings_amount = 0;
static ParamIndex_t all_params_amount = 0;

static bool _isCorrectStringParamIndex(ParamIndex_t param_idx);
static uint32_t _getStringMemoryPoolAddress();
static int8_t _save();

#define INT_POOL_SIZE           integers_amount * sizeof(IntegerParamValue_t)
#define STR_POOL_SIZE           MAX_STRING_LENGTH * strings_amount


///< Default values correspond to the last page access only.
static RomDriverInstance rom = {
    .inited = false,
};


int8_t paramsInit(ParamIndex_t int_num,
                  ParamIndex_t str_num,
                  int32_t first_page_idx,
                  size_t pages_num) {
    uint32_t need_memory_bytes = sizeof(IntegerParamValue_t) * int_num +\
                                 MAX_STRING_LENGTH * str_num;
    rom = romInit(first_page_idx, pages_num);

    if (!rom.inited) {
        return LIBPARAMS_UNKNOWN_ERROR;
    }

    if (romGetAvailableMemory(&rom) < need_memory_bytes) {
        return LIBPARAMS_WRONG_ARGS;
    }

    integers_amount = int_num;
    strings_amount = str_num;
    all_params_amount = integers_amount + strings_amount;
    return LIBPARAMS_OK;
}

int8_t paramsLoad() {
    romRead(&rom, 0, (uint8_t*)integer_values_pool, INT_POOL_SIZE);
    romRead(&rom, _getStringMemoryPoolAddress(), (uint8_t*)&string_values_pool, STR_POOL_SIZE);

    for (uint_fast8_t idx = 0; idx < integers_amount; idx++) {
        IntegerParamValue_t val = integer_values_pool[idx];
        if (val < integer_desc_pool[idx].min || val > integer_desc_pool[idx].max) {
            integer_values_pool[idx] = integer_desc_pool[idx].def;
        }
    }

    for (uint_fast8_t idx = 0; idx < strings_amount; idx++) {
        // 255 value is default value for stm32, '\0' for ubuntu
        if (string_values_pool[idx][0] == 255 || string_values_pool[idx][0] == '\0') {
            memcpy(string_values_pool[idx], string_desc_pool[idx].def, MAX_STRING_LENGTH);
        }
    }

    return LIBPARAMS_OK;
}

int8_t paramsSave() {
    if (all_params_amount == 0) {
        return LIBPARAMS_NOT_INITIALIZED;
    }

    romBeginWrite(&rom);
    int8_t res = _save();
    romEndWrite(&rom);
    return res;
}

int8_t paramsResetToDefault() {
    if (all_params_amount == 0) {
        return LIBPARAMS_NOT_INITIALIZED;
    }

    for (ParamIndex_t idx = 0; idx < integers_amount; idx++) {
        if (!integer_desc_pool[idx].is_required) {
            integer_values_pool[idx] = integer_desc_pool[idx].def;
        }
    }

    memset(string_values_pool, 0x00, STR_POOL_SIZE);
    return LIBPARAMS_OK;
}

const char* paramsGetName(ParamIndex_t param_idx) {
    if (param_idx < integers_amount) {
        return integer_desc_pool[param_idx].name;
    } else if (param_idx < all_params_amount) {
        return string_desc_pool[param_idx - integers_amount].name;
    }
    return NULL;
}

ParamIndex_t paramsFind(const uint8_t* name, uint16_t len) {
    ParamIndex_t idx;
    for (idx = 0; idx < integers_amount; idx++) {
        if (strncmp((const char*)name, integer_desc_pool[idx].name, len) == 0) {
            return idx;
        }
    }
    for (idx = integers_amount; idx < all_params_amount; idx++) {
        size_t str_idx = idx - integers_amount;
        if (strncmp((const char*)name, string_desc_pool[str_idx].name, len) == 0) {
            return idx;
        }
    }
    return all_params_amount;
}

ParamType_t paramsGetType(ParamIndex_t param_idx) {
    if (param_idx < integers_amount) {
        return PARAM_TYPE_INTEGER;
    } else if (param_idx < all_params_amount) {
        return PARAM_TYPE_STRING;
    } else {
        return PARAM_TYPE_UNDEFINED;
    }
}

const IntegerDesc_t* paramsGetIntegerDesc(ParamIndex_t param_idx) {
    if (param_idx >= integers_amount) {
        printf("paramsGetIntegerDesc: %s\n", integer_desc_pool[param_idx].name);
        return NULL;
    }
    return &integer_desc_pool[param_idx];
}

int32_t paramsGetIntegerValue(ParamIndex_t param_idx) {
    if (param_idx >= integers_amount) {
        return -1;
    }

    if (!integer_desc_pool[param_idx].is_mutable) {
        return integer_desc_pool[param_idx].def;
    }

    return integer_values_pool[param_idx];
}

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value) {
    if (param_idx >= integers_amount) {
        return;
    }

    const IntegerDesc_t* desc = &integer_desc_pool[param_idx];
    if (!desc->is_mutable || param_value > desc->max || param_value < desc->min) {
        return;
    }

    integer_values_pool[param_idx] = param_value;
}

StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx) {
    if (param_idx < integers_amount) {
        return NULL;
    }

    ParamIndex_t str_param_idx = param_idx - integers_amount;

    if (str_param_idx >= strings_amount) {
        return NULL;
    }

    StringParamValue_t* str;
    if (!string_desc_pool[str_param_idx].is_mutable) {
        str = (StringParamValue_t*)string_desc_pool[str_param_idx].def;
    } else {
        str = (StringParamValue_t*)string_values_pool[str_param_idx];
    }
    return str;
}

uint8_t paramsSetStringValue(ParamIndex_t param_idx,
                             uint8_t str_len,
                             const StringParamValue_t param_value) {
    if (str_len > MAX_STRING_LENGTH || _isCorrectStringParamIndex(param_idx)) {
        return 0;
    }

    param_idx -= integers_amount;

    if (!string_desc_pool[param_idx].is_mutable == true) {
        return 0;
    }

    memcpy(string_values_pool[param_idx], param_value, str_len);
    memset(string_values_pool[param_idx] + str_len, 0x00, MAX_STRING_LENGTH - str_len);
    return str_len;
}

const StringDesc_t* paramsGetStringDesc(ParamIndex_t param_idx) {
    if (_isCorrectStringParamIndex(param_idx)) {
        return NULL;
    }

    param_idx -= integers_amount;

    return &string_desc_pool[param_idx];
}

/************************************ PRIVATE FUNCTIONS AREA *************************************/

static bool _isCorrectStringParamIndex(ParamIndex_t param_idx) {
    return param_idx < integers_amount || param_idx >= all_params_amount;
}

static uint32_t _getStringMemoryPoolAddress() {
    return romGetAvailableMemory(&rom) - MAX_STRING_LENGTH * strings_amount;
}

/**
 * @note From romWrite() it is always expected to be successfully executed withing this file.
 * An error means either a library internal error or the provided flash driver is incorrect.
 * If such errir is detected, stop writing immediately to avoid doing something wrong.
 */
static int8_t _save() {
    if (INT_POOL_SIZE != 0 &&
            0 == romWrite(&rom, 0, (uint8_t*)integer_values_pool, INT_POOL_SIZE)) {
        return LIBPARAMS_UNKNOWN_ERROR;
    }

    size_t offset = _getStringMemoryPoolAddress();
    if (STR_POOL_SIZE != 0 &&
            0 == romWrite(&rom, offset, (uint8_t*)string_values_pool, STR_POOL_SIZE)) {
        return LIBPARAMS_UNKNOWN_ERROR;
    }

    return LIBPARAMS_OK;
}
