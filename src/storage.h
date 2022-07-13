/**
 * @file storage.h
 * @author d.ponomarev
 * @date March 5, 2021
 */

#ifndef LIBPARAM_STORAGE_H_
#define LIBPARAM_STORAGE_H_

#include <stdint.h>
#include "flash.h"


#define MAX_PARAM_NAME_LENGTH   32
#define MAX_STRING_LENGTH       20


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
    IntegerParamValue_t val;
    IntegerParamValue_t min;
    IntegerParamValue_t max;
    IntegerParamValue_t defval;
} IntegerCell_t;

typedef uint8_t StringParamValue_t[MAX_STRING_LENGTH];
typedef struct {
    uint8_t* name;
    StringParamValue_t val;
    StringParamValue_t defval;
} StringCell_t;


typedef uint8_t ParamIndex_t;

/**
 * @note Main Get/Set API
 */
void paramsInit(uint8_t int_params_amount, uint8_t str_params_amount);
IntegerParamValue_t paramsGetValue(ParamIndex_t param_idx);
IntegerParamValue_t paramsGetDefaultValue(ParamIndex_t param_idx);
IntegerParamValue_t paramsGetMinValue(ParamIndex_t param_idx);
IntegerParamValue_t paramsGetMaxValue(ParamIndex_t param_idx);
StringParamValue_t* paramsGetStringValue(ParamIndex_t param_idx);

void paramsSetIntegerValue(ParamIndex_t param_idx, IntegerParamValue_t param_value);
void paramsSetStringValue(ParamIndex_t param_idx, uint8_t str_len, StringParamValue_t param_value);

char* paramsGetParamName(ParamIndex_t param_idx);
ParamIndex_t paramsGetIndexByName(uint8_t* name, uint16_t name_length);
StorageCellType_t paramsGetType(ParamIndex_t param_idx);

void paramsLoadFromFlash();
void paramsLoadToFlash();

#endif  // LIBPARAM_STORAGE_H_
