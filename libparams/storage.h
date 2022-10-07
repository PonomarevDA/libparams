/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

/**
 * @file storage.h
 * @author d.ponomarev
 * @date March 5, 2021
 */

#ifndef LIBPARAM_STORAGE_H_
#define LIBPARAM_STORAGE_H_

#include <stdint.h>
#include "rom.h"


#define MAX_PARAM_NAME_LENGTH   32
#define MAX_STRING_LENGTH       24


typedef enum {
    CELL_TYPE_UNDEFINED,
    CELL_TYPE_INTEGER,
    CELL_TYPE_REAL,
    CELL_TYPE_BOOLEAN,
    CELL_TYPE_STRING,
} StorageCellType_t;


typedef int32_t IntegerParamValue_t;
typedef struct {
    uint8_t* name;
    IntegerParamValue_t min;
    IntegerParamValue_t max;
    IntegerParamValue_t def;
} IntegerDesc_t;

typedef uint8_t StringParamValue_t[MAX_STRING_LENGTH];
typedef struct {
    uint8_t* name;
    StringParamValue_t def;
} StringDesc_t;


typedef uint8_t ParamIndex_t;

/**
 * @note Main Get/Set API
 */
void paramsInit(uint8_t int_params_amount, uint8_t str_params_amount);
const IntegerDesc_t* paramsGetIntDesc(ParamIndex_t param_id);
IntegerParamValue_t paramsGetValue(ParamIndex_t param_idx);
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx);

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value);
void paramsSetStringValue(ParamIndex_t param_idx, uint8_t str_len, const StringParamValue_t param_value);

char* paramsGetParamName(ParamIndex_t param_idx);
ParamIndex_t paramsGetIndexByName(uint8_t* name, uint16_t name_length);
StorageCellType_t paramsGetType(ParamIndex_t param_idx);

void paramsLoadFromFlash();
int8_t paramsLoadToFlash();

#endif  // LIBPARAM_STORAGE_H_
