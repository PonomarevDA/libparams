#!/usr/bin/env python3
#
# Copyright (c) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

class CppHeader:
    INTEGER_HEAD = """#pragma once
#include "libparams/storage.h"
#include "string_params.hpp"
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
#include "string_params.h"
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
#include "libparams/storage.h"
IntegerDesc_t integer_desc_pool[] = {
"""

    INTEGER_TAIL="""
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
"""

    STRING_HEAD="""#include "string_params.h"
#include "libparams/storage.h"
StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
"""
    STRING_TAIL="""
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
"""

class CppSource:
    INTEGER_HEAD="""#include "params.hpp"
IntegerDesc_t integer_desc_pool[] = {
"""

    INTEGER_TAIL="""
};
IntegerParamValue_t integer_values_pool[sizeof(integer_desc_pool) / sizeof(IntegerDesc_t)];
"""

    STRING_HEAD="""#include "string_params.hpp"
#include "libparams/storage.h"
StringDesc_t __attribute__((weak)) string_desc_pool[NUM_OF_STR_PARAMS] = {
"""
    STRING_TAIL="""
};
StringParamValue_t string_values_pool[sizeof(string_desc_pool) / sizeof(StringDesc_t)];
"""