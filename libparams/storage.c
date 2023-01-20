/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "storage.h"
#include <string.h>
#include "flash_driver.h"
#include "rom.h"


extern IntegerDesc_t integer_desc_pool[];
extern IntegerParamValue_t integer_values_pool[];

extern StringDesc_t string_desc_pool[];
extern StringParamValue_t string_values_pool[];

static uint16_t integer_params_amount = 0;
static uint16_t string_params_amount = 0;
static uint16_t all_params_amount = 0;


#define INT_VAL_POOL_SIZE           integer_params_amount * sizeof(IntegerParamValue_t)
#define STR_VAL_POOL_SIZE           MAX_STRING_LENGTH * string_params_amount
#define STR_VAL_POOL_FIRST_ADDR     PAGE_SIZE_BYTES - STR_VAL_POOL_SIZE


void paramsInit(uint8_t int_params_amount, uint8_t str_params_amount) {
    integer_params_amount = int_params_amount;
    string_params_amount = str_params_amount;
    all_params_amount = integer_params_amount + string_params_amount;
}

const IntegerDesc_t* paramsGetIntDesc(ParamIndex_t param_idx) {
    return &integer_desc_pool[param_idx];
}

int32_t paramsGetValue(ParamIndex_t param_idx) {
    return (param_idx < integer_params_amount) ? integer_values_pool[param_idx] : 1000;
}
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx) {
    int8_t str_param_idx = param_idx - integer_params_amount;
    StringParamValue_t* str;
    if (str_param_idx < 0 || str_param_idx >= string_params_amount) {
        str = NULL;
    } else if (string_desc_pool[str_param_idx].is_persistent) {
        str = (StringParamValue_t*)string_desc_pool[str_param_idx].def;
    } else {
        str = (StringParamValue_t*)string_values_pool[str_param_idx];
    }
    return str;
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

void paramsSetStringValue(ParamIndex_t idx, uint8_t str_len, const StringParamValue_t param_value) {
    if (str_len > MAX_STRING_LENGTH || idx < integer_params_amount || idx >= all_params_amount) {
        return;
    }

    idx -= integer_params_amount;

    memcpy(string_values_pool[idx], param_value, str_len);
    memset(string_values_pool[idx] + str_len, 0x00, MAX_STRING_LENGTH - str_len);
}


ParamIndex_t paramsGetIndexByName(uint8_t* name, uint16_t name_len) {
    ParamIndex_t idx;
    for (idx = 0; idx < integer_params_amount; idx++) {
        if (strncmp((char*)name, (char*)integer_desc_pool[idx].name, name_len) == 0) {
            return idx;
        }
    }
    for (idx = integer_params_amount; idx < all_params_amount; idx++) {
        size_t str_idx = idx - integer_params_amount;
        if (strncmp((char*)name, (char*)string_desc_pool[str_idx].name, name_len) == 0) {
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
    romRead(0, (uint8_t*)integer_values_pool, INT_VAL_POOL_SIZE);
    romRead(STR_VAL_POOL_FIRST_ADDR, (uint8_t*)&string_values_pool, STR_VAL_POOL_SIZE);

    for (uint_fast8_t idx = 0; idx < integer_params_amount; idx++) {
        IntegerParamValue_t val = integer_values_pool[idx];
        if (val < integer_desc_pool[idx].min || val > integer_desc_pool[idx].max) {
            integer_values_pool[idx] = integer_desc_pool[idx].def;
        }
    }
}

int8_t paramsLoadToFlash() {
    romBeginWrite();

    int8_t res;
    if (0 == romWrite(0, (uint8_t*)integer_values_pool, INT_VAL_POOL_SIZE)) {
        res = -1;
    } else if (0 == romWrite(STR_VAL_POOL_FIRST_ADDR,
                             (uint8_t*)string_values_pool,
                             STR_VAL_POOL_SIZE)) {
        res = -1;
    } else {
        res = 0;
    }

    romEndWrite();
    return res;
}

int8_t paramsResetToDefault() {
    ParamIndex_t idx;
    for (idx = 0; idx < integer_params_amount; idx++) {
        integer_values_pool[idx] = integer_desc_pool[idx].def;
    }
    return 0;
}
