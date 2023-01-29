/*
 * Copyright (C) 2020-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Copyright (C) 2020-2023 Raccoon Lab team
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "storage.h"

typedef enum {
    NODE_ID,
    MAGNETOMETER_ID,
    INTEGER_PARAMS_AMOUNT
} IntParamsIndexes;

typedef enum {
    NODE_NAME,
    MAGNETOMETER_TYPE,
    STRING_PARAMS_AMOUNT
} StrParamsIndexes;

void init() {
    romInit(0, 1);
    paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT);
    paramsLoadFromFlash();
}
void mutable_string_write_read_check(ParamIndex_t param_idx, const char* str) {
    size_t string_length = strlen(str);
    paramsSetStringValue(param_idx, string_length, (const uint8_t*)str);
    StringParamValue_t* read_str_param = paramsGetStringValue(param_idx);

    ASSERT_NE((size_t)read_str_param, 0);
    ASSERT_EQ(memcmp(str, read_str_param, string_length), 0);
}

TEST(TestStorage, test_paramsLoadToFlash) {
    // Normal
    romInit(0, 1);
    paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT);
    ASSERT_EQ(0, paramsLoadToFlash());

    // Write Integer to ROM failed
    romInit(0, 1);
    paramsInit(0, STRING_PARAMS_AMOUNT);
    ASSERT_EQ(-1, paramsLoadToFlash());

    // Write String to ROM failed
    romInit(0, 1);
    paramsInit(INTEGER_PARAMS_AMOUNT, 0);
    ASSERT_EQ(-1, paramsLoadToFlash());
}

TEST(TestStorage, test_paramsLoadFromFlash) {
    init();
    int32_t data;

    // Out of range value in flash (more than max)
    data = 128;
    romBeginWrite();
    ASSERT_EQ(4, romWrite(0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite();
    paramsLoadFromFlash();
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));

    // Out of range value in flash (less than min)
    data = -1;
    romBeginWrite();
    ASSERT_EQ(4, romWrite(0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite();
    paramsLoadFromFlash();
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));

    // Normal
    data = 42;
    romBeginWrite();
    ASSERT_EQ(4, romWrite(0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite();
    paramsLoadFromFlash();
    ASSERT_EQ(42, paramsGetIntegerValue(NODE_ID));
}

TEST(TestStorage, test_write_read_integers) {
    init();
    IntegerParamValue_t param_value;

    paramsSetIntegerValue(NODE_ID, 0);
    param_value = paramsGetIntegerValue(NODE_ID);
    ASSERT_EQ(param_value, 0);

    paramsSetIntegerValue(NODE_ID, 42);
    param_value = paramsGetIntegerValue(NODE_ID);
    ASSERT_EQ(param_value, 42);

    paramsSetIntegerValue(NODE_ID, 127);
    param_value = paramsGetIntegerValue(NODE_ID);
    ASSERT_EQ(param_value, 127);

    // Write out of parameters range
    paramsSetIntegerValue(INTEGER_PARAMS_AMOUNT, 42);
    param_value = paramsGetIntegerValue(NODE_ID);
    ASSERT_EQ(param_value, 127);

    // Read out of parameters range
    param_value = paramsGetIntegerValue(INTEGER_PARAMS_AMOUNT);
    ASSERT_EQ(param_value, 1000);  // magic number?
}

TEST(TestStorage, test_get_integer_desc) {
    init();

    const IntegerDesc_t* desc = paramsGetIntegerDesc(NODE_ID);
    ASSERT_EQ(desc->def, 50);
    ASSERT_EQ(desc->min, 0);
    ASSERT_EQ(desc->max, 127);

    // Out of parameters range
    ASSERT_EQ(NULL, paramsGetIntegerDesc(INTEGER_PARAMS_AMOUNT));
}

TEST(TestStorage, test_paramsSetStringValue) {
    init();
    auto test_string = (const uint8_t*)"test_string";

    // Wrong inputs
    ASSERT_EQ(0, paramsSetStringValue(INTEGER_PARAMS_AMOUNT + NODE_NAME, 100, test_string));
    ASSERT_EQ(0, paramsSetStringValue(0, 12, test_string));
    ASSERT_EQ(0, paramsSetStringValue(INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT, 12, test_string));
}


TEST(TestStorage, test_write_read_strings) {
    init();

    // Mutable string
    ParamIndex_t node_name_idx = INTEGER_PARAMS_AMOUNT + NODE_NAME;
    mutable_string_write_read_check(node_name_idx, "custom_string");
    mutable_string_write_read_check(node_name_idx, "another_custom_string");

    // Persistent string
    ParamIndex_t mag_type_idx = INTEGER_PARAMS_AMOUNT + MAGNETOMETER_TYPE;
    auto mag_type = (const uint8_t*)"uavcan.si.sample.magnetic_field_strength.Vector3";
    paramsSetStringValue(mag_type_idx, 12, (const uint8_t*)"some_string");
    StringParamValue_t* read_str_param = paramsGetStringValue(mag_type_idx);
    ASSERT_EQ(memcmp(mag_type, read_str_param, 49), 0);

    // Out of parameters range
    ParamIndex_t less_than_need_idx = INTEGER_PARAMS_AMOUNT - 1;
    ASSERT_EQ(NULL, paramsGetStringValue(less_than_need_idx));

    ParamIndex_t more_than_need_idx = INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT;
    ASSERT_EQ(NULL, paramsGetStringValue(more_than_need_idx));
}

TEST(TestStorage, test_paramsGetParamName) {
    init();
    size_t expected_param_name_length;
    char* read_param_name;

    ParamIndex_t int_param_idx = NODE_ID;
    auto int_param_expected_name = "uavcan.node.id";
    expected_param_name_length = strlen(int_param_expected_name);
    read_param_name = paramsGetParamName(int_param_idx);
    ASSERT_EQ(memcmp(int_param_expected_name, read_param_name, expected_param_name_length), 0);

    ParamIndex_t str_param_idx = INTEGER_PARAMS_AMOUNT + NODE_NAME;
    auto str_param_expected_name = "name";
    expected_param_name_length = strlen(str_param_expected_name);
    read_param_name = paramsGetParamName(str_param_idx);
    ASSERT_EQ(memcmp(str_param_expected_name, read_param_name, expected_param_name_length), 0);

    // Out of bounds
    ParamIndex_t wrong_param_idx = INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT;
    read_param_name = paramsGetParamName(wrong_param_idx);
    ASSERT_EQ(read_param_name, nullptr);
}

TEST(TestStorage, test_params_get_type) {
    init();

    std::vector<std::pair<ParamIndex_t, ParamType_t>> data_set = {
        std::make_pair(NODE_ID,                                         PARAM_TYPE_INTEGER),
        std::make_pair(INTEGER_PARAMS_AMOUNT + NODE_NAME,               PARAM_TYPE_STRING),
        std::make_pair(INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT,    PARAM_TYPE_UNDEFINED)
    };

    for (auto data : data_set) {
        ASSERT_EQ(paramsGetType(data.first), data.second);
    }
}

TEST(TestStorage, test_paramsGetIndexByName) {
    init();

    ASSERT_EQ(1, paramsGetIndexByName((const uint8_t*)"uavcan.pub.mag.id", 18));
    ASSERT_EQ(2, paramsGetIndexByName((const uint8_t*)"name", 4));
    ASSERT_EQ(INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT, paramsGetIndexByName((const uint8_t*)"none", 4));
}

TEST(TestStorage, test_paramsResetToDefault) {
    init();
    ASSERT_EQ(0, paramsResetToDefault());
}


int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
