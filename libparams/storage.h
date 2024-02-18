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

#ifndef MAX_PARAM_NAME_LENGTH
#define MAX_PARAM_NAME_LENGTH 32
#endif

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
 * @brief           Initialize the parameters. Call this on startup.
 * @param int_num   The amount of integers parameters
 * @param str_num   The amount of string parameters
 * @param first_page Index of the first page. Negative values are counted from the end.
 *                  For example, -1 means the latest page. For stm32f103 it will be page_idx=127.
 * @param pages_num The amount of allocated pages. At least 1 page is required.
 * @return          LIBPARAMS_OK on success, otherwise < 0.
 */
int8_t paramsInit(ParamIndex_t int_num, ParamIndex_t str_num,
                  int32_t first_page, size_t pages_num);

/**
 * @brief           Load parameters from a persistent memory: flash for stm32 and file for ubuntu.
 * @return          LIBPARAMS_OK on success, otherwise < 0.
 */
int8_t paramsLoad();

/**
 * @brief           Save parameters to a persistent memory: flash for stm32 and file for ubuntu.
 * @return          LIBPARAMS_OK on success, otherwise < 0.
 */
int8_t paramsSave();

/**
 * @brief           Reset all parameters to their default values.
 * @return          LIBPARAMS_OK on success, otherwise < 0.
 */
int8_t paramsResetToDefault();

/**
 * @note            Get the parameter name
 * @return          C-string on success, otherwise NULL.
 */
const char* paramsGetName(ParamIndex_t param_idx);

/**
 * @brief           Find the index of the parameter by his name
 * @param name      The name of the paramter
 * @param len       The length of the parameter name
 * @return          Tthe index of the parameter on success, otherwise the amount of parameters
 */
ParamIndex_t paramsFind(const uint8_t* name, uint16_t len);

/**
 * @brief           Get the type of the parameter
 * @param param_idx The name of the parameter
 * @return          The type of the paramter on success, otherwise PARAM_TYPE_UNDEFINED
 */
ParamType_t paramsGetType(ParamIndex_t param_idx);

/**
 * @brief           Get the descriptor of the parameter
 * @return          Pointer to the structure on success, otherwise NULL
 */
const IntegerDesc_t* paramsGetIntegerDesc(ParamIndex_t param_idx);
const StringDesc_t* paramsGetStringDesc(ParamIndex_t param_idx);

/**
 * @brief           Get the parameter value
 * @return          The value of success, otherwise NULL
 */
IntegerParamValue_t paramsGetIntegerValue(ParamIndex_t param_idx);
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx);

/**
 * @brief           Set the parameter value
 */
void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value);
uint8_t paramsSetStringValue(ParamIndex_t param_idx,
                             uint8_t str_len,
                             const StringParamValue_t param_value);

#ifdef __cplusplus
}
#endif

#endif  // LIBPARAM_STORAGE_H_
