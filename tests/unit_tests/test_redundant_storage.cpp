/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "storage.h"
#include "libparams_error_codes.h"

typedef enum {
    NODE_ID,
    MAGNETOMETER_ID,
    PERSISTENT_INTEGET_ID,
    INTEGER_PARAMS_AMOUNT
} IntParamsIndexes;

typedef enum {
    NODE_NAME,
    MAGNETOMETER_TYPE,
    STRING_PARAMS_AMOUNT
} StrParamsIndexes;

void init() {
    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsInitRedundantPage(255);
    paramsChooseRom();
    paramsLoad();
}

void mutable_string_write_read_check(ParamIndex_t param_idx, const char* str) {
    size_t string_length = strlen(str);
    paramsSetStringValue(param_idx, string_length, (const uint8_t*)str);
    StringParamValue_t* read_str_param = paramsGetStringValue(param_idx);

    ASSERT_NE((size_t)read_str_param, 0);
    ASSERT_EQ(memcmp(str, read_str_param, string_length), 0);
}

TEST(TestStorage, test_paramsInit) {
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1));
}

TEST(TestStorage, test_paramsLoad) {
    paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1);
    paramsInitRedundantPage(255);
    paramsChooseRom();
    RomDriverInstance rom = romInit(0, 1);

    int32_t data;

    // Out of range value in flash (more than max)
    data = 128;
    romBeginWrite(&rom);
    ASSERT_EQ(4, romWrite(&rom, 0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite(&rom);
    paramsLoad();
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));

    // Out of range value in flash (less than min)
    data = -1;
    romBeginWrite(&rom);
    ASSERT_EQ(4, romWrite(&rom, 0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite(&rom);
    paramsLoad();
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));

    // Normal
    data = 42;
    romBeginWrite(&rom);
    ASSERT_EQ(4, romWrite(&rom, 0, static_cast<const uint8_t*>((void*)&data), 4));
    romEndWrite(&rom);
    paramsLoad();
    ASSERT_EQ(42, paramsGetIntegerValue(NODE_ID));
}

TEST(TestStorage, test_paramsSave) {
    // Normal
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1));
    paramsInitRedundantPage(255);
    paramsChooseRom();
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Zero integers is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(0, STRING_PARAMS_AMOUNT, -1, 1));
    paramsInitRedundantPage(255);
    paramsChooseRom();
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Zero strings is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, 0, -1, 1));
    paramsInitRedundantPage(255);
    paramsChooseRom();
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Full storage is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit((ParamIndex_t)512, 0, -1, 1));
    paramsInitRedundantPage(255);
    paramsChooseRom();
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // More parameters than possible is not ok
    paramsInit(0, 0, -1, 1);  // Reset the storage
    ASSERT_EQ(LIBPARAMS_WRONG_ARGS, paramsInit((ParamIndex_t)513, 0, -1, 1));
    ASSERT_EQ(LIBPARAMS_NOT_INITIALIZED, paramsSave());
}

TEST(TestStorage, test_paramsResetToDefault) {
    init();
    ASSERT_EQ(0, paramsResetToDefault());
}

TEST(TestStorage, test_paramsGetName) {
    init();
    size_t expected_param_name_length;

    ParamIndex_t int_param_idx = NODE_ID;
    auto int_param_expected_name = "uavcan.node.id";
    expected_param_name_length = strlen(int_param_expected_name);
    auto read_param_name = paramsGetName(int_param_idx);
    ASSERT_EQ(memcmp(int_param_expected_name, read_param_name, expected_param_name_length), 0);

    ParamIndex_t str_param_idx = INTEGER_PARAMS_AMOUNT + NODE_NAME;
    auto str_param_expected_name = "name";
    expected_param_name_length = strlen(str_param_expected_name);
    read_param_name = paramsGetName(str_param_idx);
    ASSERT_EQ(memcmp(str_param_expected_name, read_param_name, expected_param_name_length), 0);

    // Out of bounds
    ParamIndex_t wrong_param_idx = INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT;
    read_param_name = paramsGetName(wrong_param_idx);
    ASSERT_EQ(read_param_name, nullptr);
}

TEST(TestStorage, test_paramsFind) {
    init();

    ASSERT_EQ(MAGNETOMETER_ID, paramsFind((const uint8_t*)"uavcan.pub.mag.id", 18));
    ASSERT_EQ(INTEGER_PARAMS_AMOUNT + NODE_ID, paramsFind((const uint8_t*)"name", 4));
    ASSERT_EQ(INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT, paramsFind((const uint8_t*)"none", 4));
}

TEST(TestStorage, test_paramsGetType) {
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

TEST(TestStorage, test_paramsGetIntegerDesc) {
    init();

    const IntegerDesc_t* desc = paramsGetIntegerDesc(NODE_ID);
    ASSERT_EQ(desc->def, 50);
    ASSERT_EQ(desc->min, 0);
    ASSERT_EQ(desc->max, 127);

    // Out of parameters range
    ASSERT_EQ(NULL, paramsGetIntegerDesc(INTEGER_PARAMS_AMOUNT));
}

TEST(TestStorage, test_paramsGetStringDesc) {
    init();
    ASSERT_EQ(NULL, paramsGetStringDesc(NODE_ID));

    auto node_name_desc = paramsGetStringDesc(INTEGER_PARAMS_AMOUNT + NODE_NAME);
    EXPECT_TRUE(std::string("Unknown") == std::string((const char*)node_name_desc->def));
    EXPECT_TRUE(std::string("name") == std::string((const char*)node_name_desc->name));
}

TEST(TestStorage, test_paramsGetSetIntegerValue) {
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
    ASSERT_EQ(param_value, -1);
}

TEST(TestStorage, test_paramsGetSetStringValue) {
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

TEST(TestStorage, test_paramsSetIntegerValue_persistent_param) {
    init();
    IntegerParamValue_t param_value;

    paramsSetIntegerValue(PERSISTENT_INTEGET_ID, 0);
    param_value = paramsGetIntegerValue(PERSISTENT_INTEGET_ID);
    ASSERT_EQ(param_value, 1000000);
}

TEST(TestStorage, test_paramsSetStringValue) {
    init();
    auto test_string = (const uint8_t*)"test_string";

    // Wrong inputs
    ASSERT_EQ(0, paramsSetStringValue(INTEGER_PARAMS_AMOUNT + NODE_NAME, 100, test_string));
    ASSERT_EQ(0, paramsSetStringValue(0, 12, test_string));
    ASSERT_EQ(0, paramsSetStringValue(INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT, 12, test_string));
}


int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
