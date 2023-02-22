#!/usr/bin/env python3

class CppHeader:
    INTEGER_HEAD = """#pragma once
#include "storage.h"
enum IntParamsIndexes : ParamIndex_t {
"""
    INTEGER_TAIL="""
    INTEGER_PARAMS_AMOUNT
};
"""

    STRING_HEAD="""#pragma once
"""
    STRING_TAIL="""
"""

class CHeader:
    INTEGER_HEAD="""#pragma once
typedef enum {
"""
    INTEGER_TAIL="""
    INTEGER_PARAMS_AMOUNT
} IntParamsIndexes;
"""

    STRING_HEAD="""#pragma once
"""
    STRING_TAIL="""
"""

class CSource:
    INTEGER_HEAD="""#include "params.h"
#include "storage.h"
IntegerDesc_t integer_desc_pool[] = {
"""

    INTEGER_TAIL="""
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
"""

    STRING_HEAD="""#include "string_params.h"
#include "storage.h"
StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
"""
    STRING_TAIL="""
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
"""

class CppSource:
    INTEGER_HEAD="""#include "params.hpp"
#include "storage.h"
IntegerDesc_t integer_desc_pool[] = {
"""

    INTEGER_TAIL="""
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
"""

    STRING_HEAD="""#include "string_params.hpp"
#include "storage.h"
StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
"""
    STRING_TAIL="""
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
"""