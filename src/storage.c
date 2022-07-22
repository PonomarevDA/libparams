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
#include "flash.h"


static StorageCellType_t paramsGetByIndex(ParamIndex_t index, IntegerCell_t**);
extern IntegerCell_t parameters[];
extern StringCell_t str_params[];
static uint16_t integer_params_amount = 0;
static uint16_t string_params_amount = 0;
static uint16_t all_params_amount = 0;

IntegerCell_t __attribute__((weak)) parameters[] = {
    // name                         val     min     max     default
    {(uint8_t*)"identifier",        50,     0,      100,    50},
};

StringCell_t __attribute__((weak)) str_params[] = {
    // name                         val             default
    {(uint8_t*)"name",              "custom_name",  "default_name"},
};

void paramsInit(uint8_t int_params_amount, uint8_t str_params_amount) {
    integer_params_amount = int_params_amount;
    string_params_amount = str_params_amount;
    all_params_amount = integer_params_amount + string_params_amount;
}

int32_t paramsGetValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? parameters[param_idx].val : 1000;
}
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx) {
    int8_t str_param_idx = param_idx - integer_params_amount;
    if (str_param_idx >= 0 && str_param_idx < string_params_amount) {
        return (StringParamValue_t*)str_params[str_param_idx].val;
    }
    return NULL;
}

IntegerParamValue_t paramsGetDefaultValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? parameters[param_idx].defval : 1000;
}
IntegerParamValue_t paramsGetMinValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? parameters[param_idx].min : 1000;
}
IntegerParamValue_t paramsGetMaxValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? parameters[param_idx].max : 1000;
}
char* paramsGetParamName(ParamIndex_t param_idx) {
    if (param_idx < integer_params_amount) {
        return (char*)parameters[param_idx].name;
    } else if (param_idx < all_params_amount) {
        return (char*)str_params[param_idx - integer_params_amount].name;
    }
    return NULL;
}

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value) {
    if (param_idx >= integer_params_amount) {
        return;
    }
    parameters[param_idx].val = param_value;
}

void paramsSetStringValue(ParamIndex_t idx, uint8_t str_len, StringParamValue_t param_value) {
    if (str_len > MAX_STRING_LENGTH || idx < integer_params_amount || idx >= all_params_amount) {
        return;
    }

    idx -= integer_params_amount;

    memcpy(str_params[idx].val, param_value, str_len);
    memset(str_params[idx].val + str_len, 0x00, MAX_STRING_LENGTH - str_len);
}


StorageCellType_t paramsGetByIndex(ParamIndex_t index, IntegerCell_t** param) {
    if (index < integer_params_amount) {
        *param = &parameters[index];
        return CELL_TYPE_INTEGER;
    } else {
        *param = NULL;
        return CELL_TYPE_UNDEFINED;
    }
}

ParamIndex_t paramsGetIndexByName(uint8_t* name, uint16_t name_len) {
    ParamIndex_t idx;
    for (idx = 0; idx < integer_params_amount; idx++) {
        if (strncmp((char const*)name, (char const*)parameters[idx].name, name_len) == 0) {
            return idx;
        }
    }
    for (idx = integer_params_amount; idx < all_params_amount; idx++) {
        size_t param_idx = idx - integer_params_amount;
        if (strncmp((char const*)name, (char const*)str_params[param_idx].name, name_len) == 0) {
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
    IntegerCell_t* param = NULL;
    for (uint_fast8_t param_idx = 0; param_idx < integer_params_amount; param_idx++) {
        int64_t val = flashReadI32ByIndex(param_idx);
        paramsGetByIndex(param_idx, &param);
        if (val == 0xffffffff || param == NULL || val < param->min || val > param->max) {
            param->val = param->defval;
        } else {
            param->val = val;
        }
    }
}

void paramsLoadToFlash() {
    flashUnlock();
    flashEraseAllocatedSpace();
    for (int32_t param_idx = 0; param_idx < integer_params_amount; param_idx++) {
        flashWriteU32ByIndex(param_idx, parameters[param_idx].val);
    }
    flashLock();
}
