/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LIBPARAM_STORAGE_H_
#define LIBPARAM_STORAGE_H_

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "rom.h"
#include "libparams_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PARAM_NAME_LENGTH   32

#ifndef MAX_STRING_LENGTH
#define MAX_STRING_LENGTH 56
#endif
static_assert(MAX_STRING_LENGTH % 8 == 0, "String size must be a multiple of 8");


// Mutable means that the register can be written using this service.
// Immutable registers cannot be written, but that doesn't imply that their values are constant.
#define MUTABLE                 true
#define IMMUTABLE               false


typedef enum {
    PARAM_TYPE_UNDEFINED,
    PARAM_TYPE_INTEGER,
    PARAM_TYPE_REAL,
    PARAM_TYPE_BOOLEAN,
    PARAM_TYPE_STRING,
} ParamType_t;


typedef int32_t IntegerParamValue_t;
typedef struct {
    const char* name;
    IntegerParamValue_t min;
    IntegerParamValue_t max;
    IntegerParamValue_t def;

    bool is_mutable;

    /**
     * @note The flag means that the parameter's default value is not essential.
     * Practically, it means that the paramter will not be reset during the paramsResetToDefault().
     */
    bool is_required;
} IntegerDesc_t;

typedef uint8_t StringParamValue_t[MAX_STRING_LENGTH];
typedef struct {
    const char* name;
    StringParamValue_t def;
    bool is_mutable;
} StringDesc_t;


typedef uint16_t ParamIndex_t;

/**
 * @note Common functions
 */
int8_t paramsInit(ParamIndex_t int_params_amount, ParamIndex_t str_params_amount);
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
const StringDesc_t* paramsGetStringDesc(ParamIndex_t param_idx);

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_STORAGE_H_
