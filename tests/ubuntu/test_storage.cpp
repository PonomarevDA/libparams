/**
 * @file test_stm32f1xx.cpp
 * @author d.ponomarev
 * @date Aug 01, 2022
 */

#include <gtest/gtest.h>
#include <iostream>
#include "storage.h"

void init() {
    romInit(0, 1);
    paramsInit(1, 1);
}

void write_integer_test_param() {
    paramsSetIntegerValue(0, 42);
    IntegerParamValue_t param_value = paramsGetValue(0);
    ASSERT_EQ(param_value, 42);
}

void write_string_test_param() {
    const StringParamValue_t WRITE_STR_PARAM = "this_is_node_name";
    paramsSetStringValue(1, MAX_STRING_LENGTH, WRITE_STR_PARAM);
    StringParamValue_t* read_str_param = paramsGetStringValue(1);
    ASSERT_NE((size_t)read_str_param, 0);
    int compare_res = memcmp(WRITE_STR_PARAM, read_str_param, MAX_STRING_LENGTH);
    ASSERT_EQ(compare_res, 0);
}


TEST(TestStorage, test_write_read_integers) {
    init();
    write_integer_test_param();
}

TEST(TestStorage, test_write_read_strings) {
    init();
    write_string_test_param();
}

TEST(TestStorage, test_load_to_and_from) {
    init();

    // write empty parameters
    paramsSetIntegerValue(0, 0);
    const StringParamValue_t WRITE_EMPTY_STR_PARAM = {};
    paramsSetStringValue(1, MAX_STRING_LENGTH, WRITE_EMPTY_STR_PARAM);

    // load empty params to flash
    int8_t load_res = paramsLoadToFlash();
    ASSERT_EQ(load_res, 0);

    // write non empty params
    write_integer_test_param();
    write_string_test_param();

    // load empty empty params from flash
    paramsLoadFromFlash();

    // check that now params are empty
    IntegerParamValue_t read_int_param_value = paramsGetValue(0);
    ASSERT_EQ(read_int_param_value, 0);

    StringParamValue_t* read_str_param = paramsGetStringValue(1);
    ASSERT_NE((size_t)read_str_param, 0);
    int compare_str_res = memcmp(WRITE_EMPTY_STR_PARAM, read_str_param, MAX_STRING_LENGTH);
    ASSERT_EQ(compare_str_res, 0);
}


int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
