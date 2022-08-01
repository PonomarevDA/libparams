/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file storage.c
 * @author d.ponomarev
 * @date March 5, 2021
 */

#include "storage.h"
#include <string.h>
#include "flash_stm32.h"
#include "flash.h"


static StorageCellType_t paramsGetByIndex(ParamIndex_t index, IntegerDesc_t**);

extern IntegerDesc_t integer_desc_pool[];
extern IntegerParamValue_t integer_values_pool[];

extern StringDesc_t string_desc_pool[];
extern StringParamValue_t string_values_pool[];

static uint16_t integer_params_amount = 0;
static uint16_t string_params_amount = 0;
static uint16_t all_params_amount = 0;

void paramsInit(uint8_t int_params_amount, uint8_t str_params_amount) {
    integer_params_amount = int_params_amount;
    string_params_amount = str_params_amount;
    all_params_amount = integer_params_amount + string_params_amount;
}

int32_t paramsGetValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? integer_values_pool[param_idx] : 1000;
}
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx) {
    int8_t str_param_idx = param_idx - integer_params_amount;
    if (str_param_idx >= 0 && str_param_idx < string_params_amount) {
        return (StringParamValue_t*)string_values_pool[str_param_idx];
    }
    return NULL;
}

IntegerParamValue_t paramsGetDefaultValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? integer_desc_pool[param_idx].defval : 1000;
}
IntegerParamValue_t paramsGetMinValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? integer_desc_pool[param_idx].min : 1000;
}
IntegerParamValue_t paramsGetMaxValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? integer_desc_pool[param_idx].max : 1000;
}
char* paramsGetParamName(ParamIndex_t param_idx) {
    if (param_idx < integer_params_amount) {
        return (char*)integer_desc_pool[param_idx].name;
    } else if (param_idx < all_params_amount) {
        return (char*)string_desc_pool[param_idx - integer_params_amount].name;
    }
    return NULL;
}

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value) {
    if (param_idx >= integer_params_amount) {
        return;
    }
    integer_values_pool[param_idx] = param_value;
}

void paramsSetStringValue(ParamIndex_t idx, uint8_t str_len, StringParamValue_t param_value) {
    if (str_len > MAX_STRING_LENGTH || idx < integer_params_amount || idx >= all_params_amount) {
        return;
    }

    idx -= integer_params_amount;

    memcpy(string_values_pool[idx], param_value, str_len);
    memset(string_values_pool[idx] + str_len, 0x00, MAX_STRING_LENGTH - str_len);
}


StorageCellType_t paramsGetByIndex(ParamIndex_t index, IntegerDesc_t** param) {
    if (index < integer_params_amount) {
        *param = &integer_desc_pool[index];
        return CELL_TYPE_INTEGER;
    } else {
        *param = NULL;
        return CELL_TYPE_UNDEFINED;
    }
}

ParamIndex_t paramsGetIndexByName(uint8_t* name, uint16_t name_len) {
    ParamIndex_t idx;
    for (idx = 0; idx < integer_params_amount; idx++) {
        if (strncmp((char const*)name, (char const*)integer_desc_pool[idx].name, name_len) == 0) {
            return idx;
        }
    }
    for (idx = integer_params_amount; idx < all_params_amount; idx++) {
        size_t param_idx = idx - integer_params_amount;
        if (strncmp((char const*)name, (char const*)string_desc_pool[param_idx].name, name_len) == 0) {
            return idx;
        }
    }
    return all_params_amount;
}

StorageCellType_t paramsGetType(ParamIndex_t param_idx) {
    if (param_idx < integer_params_amount) {
        return CELL_TYPE_INTEGER;
    } else if (param_idx < all_params_amount) {
        return CELL_TYPE_STRING;
    } else {
        return CELL_TYPE_UNDEFINED;
    }
}


void paramsLoadFromFlash() {
    const size_t INT_VAL_POOL_SIZE = sizeof(IntegerParamValue_t) * integer_params_amount;
    const size_t STR_VAL_POOL_SIZE = MAX_STRING_LENGTH * string_params_amount;
    const size_t STR_VAL_POOL_FIRST_ADDR = PAGE_SIZE_BYTES - STR_VAL_POOL_SIZE;

    IntegerDesc_t* param = NULL;
    flashRead(0, (uint8_t*)integer_values_pool, INT_VAL_POOL_SIZE);
    for (uint_fast8_t param_idx = 0; param_idx < integer_params_amount; param_idx++) {
        paramsGetByIndex(param_idx, &param);
        IntegerParamValue_t val = integer_values_pool[param_idx];
        if (val < param->min || val > param->max) {
            integer_values_pool[param_idx] = param->defval;
        }
    }

    flashRead(STR_VAL_POOL_FIRST_ADDR, (uint8_t*)&string_values_pool, STR_VAL_POOL_SIZE);
}

int8_t paramsLoadToFlash() {
    int8_t res = 0;
    int32_t write_value, read_value;
    flashUnlock();
    flashEraseAllocatedSpace();

    for (int32_t param_idx = 0; param_idx < integer_params_amount; param_idx++) {
        write_value = integer_values_pool[param_idx];
        if (flashWriteU32ByIndex(param_idx, write_value) == -1) {
            res = -1;
        }
        read_value = flashReadI32ByIndex(param_idx);
        if (write_value != read_value) {
            res = -1;
        }
    }

    flashLock();
    return res;
}
