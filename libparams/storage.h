/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_STORAGE_H_
#define LIBPARAM_STORAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include "rom.h"


#define MAX_PARAM_NAME_LENGTH   32
#define MAX_STRING_LENGTH       60


typedef enum {
    PARAM_TYPE_UNDEFINED,
    PARAM_TYPE_INTEGER,
    PARAM_TYPE_REAL,
    PARAM_TYPE_BOOLEAN,
    PARAM_TYPE_STRING,
} ParamType_t;


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
    bool is_persistent;
} StringDesc_t;


typedef uint8_t ParamIndex_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note Common functions
 */
void paramsInit(ParamIndex_t int_params_amount, ParamIndex_t str_params_amount);
void paramsLoadFromFlash();
int8_t paramsLoadToFlash();
int8_t paramsResetToDefault();

/**
 * @note Common parameters getters
 */
char* paramsGetParamName(ParamIndex_t param_idx);
ParamIndex_t paramsGetIndexByName(const uint8_t* name, uint16_t name_length);
ParamType_t paramsGetType(ParamIndex_t param_idx);

/**
 * @note Integer parameters specific setters/getters
 */
const IntegerDesc_t* paramsGetIntegerDesc(ParamIndex_t param_idx);
IntegerParamValue_t paramsGetIntegerValue(ParamIndex_t param_idx);
void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value);

/**
 * @note String parameters specific setters/getters
 */
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx);
uint8_t paramsSetStringValue(ParamIndex_t param_idx,
                             uint8_t str_len,
                             const StringParamValue_t param_value);

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_STORAGE_H_
