/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "libparams/storage.h"
#include "libparams/libparams_error_codes.h"
#include "common/algorithms.hpp"

typedef enum {
    NODE_ID,
    MAGNETOMETER_ID,
    IMMUTABLE_INTEGER_ID,
    INTEGER_PARAMS_AMOUNT
} IntParamsIndexes;

#define NODE_NAME               (INTEGER_PARAMS_AMOUNT + 0)
#define MAGNETOMETER_TYPE       (INTEGER_PARAMS_AMOUNT + 1)
#define STRING_PARAMS_AMOUNT    ((ParamIndex_t)2)

#define NUMBER_OF_PARAMS        (INTEGER_PARAMS_AMOUNT + STRING_PARAMS_AMOUNT)
extern RomDriverInstance* active_rom;
extern RomDriverInstance* standby_rom;
extern IntegerParamValue_t integer_values_pool[];


class RedundantRomStorageDriverTest : public ::testing::Test {
protected:
    size_t primary_rom_addr;
    size_t redundant_rom_addr;
    void SetUp() override {
        paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1);
        EXPECT_NE(active_rom, nullptr);
        paramsInitRedundantPage();
        EXPECT_NE(standby_rom, nullptr);
        primary_rom_addr = active_rom->addr;
        redundant_rom_addr = standby_rom->addr;
        paramsLoad();
    }

    void TearDown() override {
        active_rom = nullptr;
        standby_rom = nullptr;
    }
};

class SinglePageStorageDriverTest : public ::testing::Test {
protected:
    RomDriverInstance rom;

    void SetUp() override {
        rom = romInit(-1, 1);
        paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1);
        paramsLoad();
    }
};

class EmptyStorageDriverTest : public ::testing::Test {
protected:
    void SetUp() override {
        paramsInit(0, 0, -1, 1);  // reset storage
    }
};



// Test Case 1: Initialization of Parameters
// Test 1.1: Initialize with Valid Inputs
TEST_F(EmptyStorageDriverTest, initializeWithValidInput) {
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1));
}
// Test 1.2: Initialize with zero params
TEST_F(EmptyStorageDriverTest, initializeWithZeroParams) {
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(0, 0, -1, 1));
}
// Test 1.3: Initialize with too much params
TEST_F(EmptyStorageDriverTest, initializeWithTooMuchParams) {
    ASSERT_EQ(LIBPARAMS_WRONG_ARGS, paramsInit(1000, 1000, -1, 1));
}
// Test 1.4: Initialize with Zero Pages
TEST_F(EmptyStorageDriverTest, initializeZeroPages) {
    ASSERT_EQ(LIBPARAMS_UNKNOWN_ERROR, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, 0, 0));
}
// Test 1.5: Initialize with Invalid Page Index
TEST_F(EmptyStorageDriverTest, initializeWithInvalidaPageIndex) {
    ASSERT_EQ(LIBPARAMS_UNKNOWN_ERROR, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 2));
}

// Test Case 2: Load Parameters
// Test 2.1: Load Parameters Successfully
TEST_F(SinglePageStorageDriverTest, loadParametersSuccessfully) {
    int32_t node_id;

    // 1/3. The stored parameter value is within [min, max]
    node_id = rand() % 127;
    romBeginWrite(&rom);
    ASSERT_EQ(sizeof(node_id), romWrite(&rom, 0, (uint8_t*)(void*)&node_id, sizeof(node_id)));
    romEndWrite(&rom);
    ASSERT_EQ(LIBPARAMS_OK, paramsLoad());
    ASSERT_EQ(node_id, paramsGetIntegerValue(NODE_ID));

    // 2/3. Out of range value in flash (more than max)
    node_id = 128;
    romBeginWrite(&rom);
    ASSERT_EQ(4, romWrite(&rom, 0, static_cast<const uint8_t*>((void*)&node_id), 4));
    romEndWrite(&rom);
    ASSERT_EQ(LIBPARAMS_OK, paramsLoad());
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));

    // 3/3. Out of range value in flash (less than min)
    node_id = -1;
    romBeginWrite(&rom);
    ASSERT_EQ(4, romWrite(&rom, 0, static_cast<const uint8_t*>((void*)&node_id), 4));
    romEndWrite(&rom);
    ASSERT_EQ(LIBPARAMS_OK, paramsLoad());
    ASSERT_EQ(50, paramsGetIntegerValue(NODE_ID));
}
// Test 2.2: Load Parameters with Uninitialized Parameters
// Actually, it is better to return an error here
TEST_F(EmptyStorageDriverTest, loadParametersSuccessfully) {
    ASSERT_EQ(LIBPARAMS_OK, paramsLoad());
}


// Test Case 3: Save Parameters
// Test 3.1: Save Parameters Successfully
TEST_F(EmptyStorageDriverTest, saveParametersSuccessfully) {
    // Normal
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, STRING_PARAMS_AMOUNT, -1, 1));
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Zero integers is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(0, STRING_PARAMS_AMOUNT, -1, 1));
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Zero strings is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit(INTEGER_PARAMS_AMOUNT, 0, -1, 1));
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());

    // Full storage is ok
    ASSERT_EQ(LIBPARAMS_OK, paramsInit((ParamIndex_t)512, 0, -1, 1));
    ASSERT_EQ(LIBPARAMS_OK, paramsSave());
}

// Test 3.2: Save Parameters with Uninitialized Parameters
TEST_F(EmptyStorageDriverTest, saveParametersWithUninitializedParameters) {
    ASSERT_EQ(LIBPARAMS_NOT_INITIALIZED, paramsSave());
}


// Test Case 4: Reset Parameters to Default
// Test 4.1: Reset Parameters Successfully
TEST_F(SinglePageStorageDriverTest, resetParametersSuccessfully) {
    ASSERT_EQ(LIBPARAMS_OK, paramsResetToDefault());
}
TEST_F(EmptyStorageDriverTest, resetParametersWithUninitializedParameters) {
    ASSERT_EQ(LIBPARAMS_NOT_INITIALIZED, paramsResetToDefault());
}


// Test Case 5: Access Parameter Names
// Test 5.1: Get Parameter Name Successfully
TEST_F(SinglePageStorageDriverTest, getParameterNameSuccessfully) {
    size_t expected_param_name_length;

    ParamIndex_t int_param_idx = NODE_ID;
    auto int_param_expected_name = "uavcan.node.id";
    expected_param_name_length = strlen(int_param_expected_name);
    auto read_param_name = paramsGetName(int_param_idx);
    ASSERT_EQ(memcmp(int_param_expected_name, read_param_name, expected_param_name_length), 0);

    auto str_param_expected_name = "name";
    expected_param_name_length = strlen(str_param_expected_name);
    read_param_name = paramsGetName(NODE_NAME);
    ASSERT_EQ(memcmp(str_param_expected_name, read_param_name, expected_param_name_length), 0);
}
// Test 5.2: Get Parameter Name with Invalid Index
TEST_F(SinglePageStorageDriverTest, getParameterNameWithInvalidIndex) {
    auto read_param_name = paramsGetName(NUMBER_OF_PARAMS);
    ASSERT_EQ(read_param_name, nullptr);
}


// Test Case 6: Find Parameter by Name
// Test 6.1: Find Parameter by Valid Name
TEST_F(SinglePageStorageDriverTest, findParameterByValidName) {
    // Integer
    ASSERT_EQ(MAGNETOMETER_ID, paramsFind((const uint8_t*)"uavcan.pub.mag.id", 18));

    // String
    ASSERT_EQ(INTEGER_PARAMS_AMOUNT + NODE_ID, paramsFind((const uint8_t*)"name", 4));
}
// Test 6.2: Find Parameter by Invalid Name
TEST_F(SinglePageStorageDriverTest, findParameterByInvalidName) {
    ASSERT_EQ(NUMBER_OF_PARAMS, paramsFind((const uint8_t*)"none", 4));
}


// Test Case 7: Access Parameter Types
// Test 7.1: Get Parameter Type Successfully
TEST_F(SinglePageStorageDriverTest, getParameterTypeSuccessfully) {
    ASSERT_EQ(PARAM_TYPE_INTEGER,   paramsGetType(NODE_ID));
    ASSERT_EQ(PARAM_TYPE_STRING,    paramsGetType(NODE_NAME));
}
// Test 7.2: Get Parameter Type with Invalid Index
TEST_F(SinglePageStorageDriverTest, getParameterTypeWithInvalidIndex) {
    ASSERT_EQ(PARAM_TYPE_UNDEFINED, paramsGetType(NUMBER_OF_PARAMS));
}


// Test Case 8: Access Parameter Descriptions
// Test 8.1: Get Integer Parameter Description Successfully
TEST_F(SinglePageStorageDriverTest, getIntegerParameterDescriptionSuccessfully) {
    const IntegerDesc_t* desc = paramsGetIntegerDesc(NODE_ID);
    ASSERT_EQ(desc->def, 50);
    ASSERT_EQ(desc->min, 0);
    ASSERT_EQ(desc->max, 127);
}
// Test 8.2: Get Integer Parameter Description with Invalid Index
TEST_F(SinglePageStorageDriverTest, getIntegerParameterDescriptionWithInvalidIndex) {
    ASSERT_EQ(nullptr, paramsGetIntegerDesc(INTEGER_PARAMS_AMOUNT));
}
// Test 8.3: Get String Parameter Description Successfully
TEST_F(SinglePageStorageDriverTest, getStringParameterDescriptionSuccessfully) {
    auto node_name_desc = paramsGetStringDesc(NODE_NAME);
    EXPECT_TRUE(std::string("Unknown") == std::string((const char*)node_name_desc->def));
}
// Test 8.4: Get String Parameter Description with Invalid Index
TEST_F(SinglePageStorageDriverTest, getStringParameterDescriptionWithInvalidIndex) {
    ASSERT_EQ(nullptr, paramsGetStringDesc(NODE_ID));
}


// Test Case 9: Access Parameter Values
// Test 9.1: Get Integer Parameter Value Successfully
TEST_F(SinglePageStorageDriverTest, getIntegerParameterValueSuccessfully) {
    int32_t node_id = rand() % 127;
    paramsSetIntegerValue(NODE_ID, node_id);
    ASSERT_EQ(paramsGetIntegerValue(NODE_ID), node_id);
}
// Test 9.2: Get Integer Parameter Value with Invalid Index
TEST_F(SinglePageStorageDriverTest, getIntegerParameterValueWithInvalidIndex) {
    ASSERT_EQ(paramsGetIntegerValue(INTEGER_PARAMS_AMOUNT), -1);
}

void mutable_string_write_read_check(ParamIndex_t param_idx, const char* str) {
    size_t string_length = strlen(str);
    paramsSetStringValue(param_idx, string_length, (const uint8_t*)str);
    StringParamValue_t* read_str_param = paramsGetStringValue(param_idx);

    ASSERT_NE((size_t)read_str_param, 0);
    ASSERT_EQ(memcmp(str, read_str_param, string_length), 0);
}

// Test 9.3: Get String Parameter Value Successfully
TEST_F(SinglePageStorageDriverTest, getStringParameterValueSuccessfully) {
    const size_t STR_SIZE = 10;
    char origin_str[STR_SIZE];
    generateRandomCString(origin_str, STR_SIZE);

    paramsSetStringValue(NODE_NAME, STR_SIZE, (const uint8_t*)origin_str);
    StringParamValue_t* read_str = paramsGetStringValue(NODE_NAME);
    ASSERT_NE((size_t)read_str, 0);
    ASSERT_EQ(memcmp(origin_str, read_str, STR_SIZE), 0);
}
// Test 9.4: Get String Parameter Value with Invalid Index
TEST_F(SinglePageStorageDriverTest, getStringParameterValueWithInvalidIndex) {
    ParamIndex_t less_than_need_idx = INTEGER_PARAMS_AMOUNT - 1;
    ASSERT_EQ(nullptr, paramsGetStringValue(less_than_need_idx));

    ParamIndex_t more_than_need_idx = NUMBER_OF_PARAMS;
    ASSERT_EQ(nullptr, paramsGetStringValue(more_than_need_idx));
}


// Test Case 10: Set Parameter Values
// Test 10.1: Set Integer Parameter Value Successfully
TEST_F(SinglePageStorageDriverTest, test_paramsSetIntegerValue_immutable_param) {
    // Mutable integer was tested in the previous tests

    // Immutable integer
    paramsSetIntegerValue(IMMUTABLE_INTEGER_ID, 0);
    ASSERT_EQ(paramsGetIntegerValue(IMMUTABLE_INTEGER_ID), 1000000);
}
// Test 10.2: Set Integer Parameter Value with Invalid Index
// No way to check it
TEST_F(SinglePageStorageDriverTest, setIntegerParameterValueWithInvalidIndex) {
    paramsSetIntegerValue(INTEGER_PARAMS_AMOUNT, -1);
    paramsSetIntegerValue(INTEGER_PARAMS_AMOUNT, 128);
}
// Test 10.3: Set String Parameter Value Successfully
TEST_F(SinglePageStorageDriverTest, setStringParameterValueSuccessfully) {
    // Mutable string was tested in the previous tests

    // Immutable string
    auto mag_type = (const uint8_t*)"uavcan.si.sample.magnetic_field_strength.Vector3";
    paramsSetStringValue(MAGNETOMETER_TYPE, 12, (const uint8_t*)"some_string");
    StringParamValue_t* read_str_param = paramsGetStringValue(MAGNETOMETER_TYPE);
    ASSERT_EQ(memcmp(mag_type, read_str_param, sizeof(mag_type)), 0);
}
// Test 10.4: Set String Parameter Value with Invalid Index
TEST_F(SinglePageStorageDriverTest, test_paramsSetStringValue) {
    auto test_string = (const uint8_t*)"test_string";

    // Not a string yet (the index is less then need)
    ASSERT_EQ(0, paramsSetStringValue(INTEGER_PARAMS_AMOUNT - 1, 12, test_string));

    // No longer a string (the index is more then need)
    ASSERT_EQ(0, paramsSetStringValue(NUMBER_OF_PARAMS, 12, test_string));

    // String size is more than max
    ASSERT_EQ(0, paramsSetStringValue(NODE_NAME, MAX_STRING_LENGTH + 1, test_string));

    // Writing nullptr
    ASSERT_EQ(0, paramsSetStringValue(NODE_NAME, MAX_STRING_LENGTH + 1, nullptr));
}

TEST_F(RedundantRomStorageDriverTest, pageSwitchAfterSave) {
    ASSERT_EQ(active_rom->addr, primary_rom_addr);
    ASSERT_EQ(standby_rom->addr, redundant_rom_addr);

    paramsSave();
    ASSERT_EQ(active_rom->addr, redundant_rom_addr);
    ASSERT_EQ(standby_rom->addr, primary_rom_addr);

    ASSERT_TRUE(standby_rom->erased);
}


TEST_F(RedundantRomStorageDriverTest, pageEraseAfterSave) {
    ASSERT_EQ(active_rom->addr, primary_rom_addr);
    ASSERT_EQ(standby_rom->addr, redundant_rom_addr);

    paramsSave();
    ASSERT_EQ(active_rom->addr, redundant_rom_addr);
    ASSERT_EQ(standby_rom->addr, primary_rom_addr);
    // standby rom has to be erased
    ASSERT_TRUE(standby_rom->erased);
    romRead(standby_rom, 0, (uint8_t*)integer_values_pool, 4);
    ASSERT_EQ(integer_values_pool[0], 0);

    romRead(active_rom, 0, (uint8_t*)integer_values_pool, 4);
    ASSERT_NE(integer_values_pool[0], 0);
}

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
