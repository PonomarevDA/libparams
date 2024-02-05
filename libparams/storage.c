/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "storage.h"
#include <string.h>
#include "flash_driver.h"
#include "rom.h"
#include "libparams_error_codes.h"

extern IntegerDesc_t integer_desc_pool[];
extern IntegerParamValue_t integer_values_pool[];
extern StringDesc_t string_desc_pool[];
extern StringParamValue_t string_values_pool[];

static ParamIndex_t integers_amount = 0;
static ParamIndex_t strings_amount = 0;
static ParamIndex_t all_params_amount = 0;

static bool _paramsIsCorrectStringParamIndex(ParamIndex_t param_idx);
static uint32_t _paramsGetStringMemoryPoolAddress();
static int8_t _paramsLoadToFlash();

#define INT_POOL_SIZE           integers_amount * sizeof(IntegerParamValue_t)
#define STR_POOL_SIZE           MAX_STRING_LENGTH * strings_amount


int8_t paramsInit(ParamIndex_t integers_requested, ParamIndex_t strings_requested) {
    uint32_t need_memory_bytes = sizeof(IntegerParamValue_t) * integers_requested +\
                                 MAX_STRING_LENGTH * strings_requested;
    if (romGetAvailableMemory() < need_memory_bytes) {
        return LIBPARAMS_WRONG_ARGS;
    }

    integers_amount = integers_requested;
    strings_amount = strings_requested;
    all_params_amount = integers_amount + strings_amount;
    return LIBPARAMS_OK;
}

void paramsLoadFromFlash() {
    romRead(0, (uint8_t*)integer_values_pool, INT_POOL_SIZE);
    romRead(_paramsGetStringMemoryPoolAddress(), (uint8_t*)&string_values_pool, STR_POOL_SIZE);

    for (uint_fast8_t idx = 0; idx < integers_amount; idx++) {
        IntegerParamValue_t val = integer_values_pool[idx];
        if (val < integer_desc_pool[idx].min || val > integer_desc_pool[idx].max) {
            integer_values_pool[idx] = integer_desc_pool[idx].def;
        }
    }
}

int8_t paramsLoadToFlash() {
    if (all_params_amount == 0) {
        return LIBPARAMS_NOT_INITIALIZED;
    }

    romBeginWrite();
    int8_t res = _paramsLoadToFlash();
    romEndWrite();
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

const char* paramsGetParamName(ParamIndex_t param_idx) {
    if (param_idx < integers_amount) {
        return integer_desc_pool[param_idx].name;
    } else if (param_idx < all_params_amount) {
        return string_desc_pool[param_idx - integers_amount].name;
    }
    return NULL;
}

ParamIndex_t paramsGetIndexByName(const uint8_t* name, uint16_t name_len) {
    ParamIndex_t idx;
    for (idx = 0; idx < integers_amount; idx++) {
        if (strncmp((const char*)name, (char*)integer_desc_pool[idx].name, name_len) == 0) {
            return idx;
        }
    }
    for (idx = integers_amount; idx < all_params_amount; idx++) {
        size_t str_idx = idx - integers_amount;
        if (strncmp((const char*)name, (char*)string_desc_pool[str_idx].name, name_len) == 0) {
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

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t value) {
    if (param_idx >= integers_amount) {
        return;
    }

    const IntegerDesc_t* desc = &integer_desc_pool[param_idx];
    if (!desc->is_mutable || value > desc->max || value < desc->min) {
        return;
    }

    integer_values_pool[param_idx] = value;
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
    if (str_len > MAX_STRING_LENGTH || _paramsIsCorrectStringParamIndex(param_idx)) {
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
    if (_paramsIsCorrectStringParamIndex(param_idx)) {
        return NULL;
    }

    param_idx -= integers_amount;

    return &string_desc_pool[param_idx];
}

/************************************ PRIVATE FUNCTIONS AREA *************************************/

static bool _paramsIsCorrectStringParamIndex(ParamIndex_t param_idx) {
    return param_idx < integers_amount || param_idx >= all_params_amount;
}

static uint32_t _paramsGetStringMemoryPoolAddress() {
    return romGetAvailableMemory() - MAX_STRING_LENGTH * strings_amount;
}

/**
 * @note From romWrite() it is always expected to be successfully executed withing this file.
 * An error means either a library internal error or the provided flash driver is incorrect.
 * If such errir is detected, stop writing immediately to avoid doing something wrong.
 */
static int8_t _paramsLoadToFlash() {
    if (INT_POOL_SIZE != 0 && 0 == romWrite(0, (uint8_t*)integer_values_pool, INT_POOL_SIZE)) {
        return LIBPARAMS_UNKNOWN_ERROR;
    }

    size_t offset = _paramsGetStringMemoryPoolAddress();
    if (STR_POOL_SIZE != 0 && 0 == romWrite(offset, (uint8_t*)string_values_pool, STR_POOL_SIZE)) {
        return LIBPARAMS_UNKNOWN_ERROR;
    }

    return LIBPARAMS_OK;
}
