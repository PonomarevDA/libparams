/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <experimental/random>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <fstream>

#include "YamlParameters.hpp"
#include "common/algorithms.hpp"
#include "flash_driver.h"

std::string dir(LIBPARAMS_PARAMS_DIR);
uint8_t flash_memory[100];

int8_t delete_file_from_dir(const char* path) {
    std::string full_path = dir;
    full_path.append("/").append(path);
    try {
        if (std::filesystem::remove(full_path)) {
            std::cout << "file " << full_path << " deleted.\n";
        } else {
            std::cout << "file " << full_path << " not found.\n";
            return -1;
        }
    } catch (const std::filesystem::filesystem_error& err) {
        std::cout << "filesystem error: " << full_path << err.what() << '\n';
        return -1;
    }
    return 0;
}


class YamlParamsTestBase : public ::testing::Test{
public:
    YamlParameters * yaml_params;
    FlashMemoryLayout_t flash;
    ParametersLayout_t params;

    void generate_params_pools() {
        IntegerDesc_t integer_desc_pool[512];
        StringDesc_t string_desc_pool[512];
        char int_names[params.num_int_params][10];
        char str_names[params.num_str_params][10];
        params.integer_desc_pool = {};
        for (int i = 0; i < params.num_int_params; i++) {
            generateRandomCString(int_names[i], 10);
            integer_desc_pool[i].name = int_names[i];
            integer_desc_pool[i].def = std::experimental::randint(0, 10000);
        }
        for (int i = 0; i < params.num_str_params; i++) {
            generateRandomCString(str_names[i], 10);
            string_desc_pool[i].name = str_names[i];
            *string_desc_pool[i].def = *(uint8_t*)str_names[i];
        }
        params.integer_desc_pool = integer_desc_pool;
        params.string_desc_pool = string_desc_pool;
    }

    void compare_flash_with_pool(){
        for (uint8_t idx = 0; idx < params.num_int_params; idx ++) {
            int32_t int_val = 0;
            memcpy(&int_val, flash.memory_ptr + 4 * idx, 4);
            ASSERT_EQ(int_val, params.integer_desc_pool[idx].def);
        }
        for (uint8_t idx = 0; idx < params.num_str_params; idx ++) {
            auto offset = flash.flash_size - MAX_STRING_LENGTH * (params.num_str_params - idx);
            std::string str_val((char*)(flash.memory_ptr + offset), MAX_STRING_LENGTH);
            auto def = (char*)(params.string_desc_pool[idx].def);
            ASSERT_STREQ(str_val.c_str(), def);
        }
    }
    int8_t fill_flash_with_default() {
    for (int i = 0; i < params.num_int_params; i++) {
        memcpy((void*)(flash.memory_ptr + 4 * i), &params.integer_desc_pool[i].def, 4);
    }
    for (int i = 0; i < params.num_str_params; i++) {
        auto str_value = (char*)params.string_desc_pool[i].def;
        int offset = flash.num_pages * flash.page_size - MAX_STRING_LENGTH *
                         (params.num_str_params - i);
        memcpy((void*)(flash.memory_ptr + offset), str_value, strlen(str_value));
    }
    return 0;
}
};

// Case 1. Check YamlParameters initialization with different parameters sets

struct InitParameters {
    uint8_t* flash_ptr;
    int num_pages;
    int page_size;
    int num_int_params;
    int num_str_parms;
    bool expected;
};

class YamlParametersParametrizedInitialization : public YamlParamsTestBase, public
                ::testing::WithParamInterface<InitParameters> {};

TEST_P(YamlParametersParametrizedInitialization, CheckYamlInitialization) {
    auto parameters = GetParam();
    flash.memory_ptr = parameters.flash_ptr;
    flash.num_pages = parameters.num_pages;
    flash.page_size = parameters.page_size;
    flash.flash_size = flash.num_pages * flash.page_size;

    params.num_int_params = parameters.num_int_params;
    params.num_str_params = parameters.num_str_parms;
    bool expected = parameters.expected;
    generate_params_pools();
    if (expected) {
        YamlParameters(flash, params);  // Assuming constructor does not throw
    } else {
        EXPECT_THROW(yaml_params = new YamlParameters(flash, params),
                                                            std::invalid_argument);
    }
}

// Flash nullptr
INSTANTIATE_TEST_CASE_P(
        YamlParametersInitFlashPtrTests,
        YamlParametersParametrizedInitialization,
        ::testing::Values(
                (InitParameters){flash_memory,   1,  100,    1,  1,  true},
                (InitParameters){nullptr,        1,  100,    1,  1, false}));

// Flash number of pages
INSTANTIATE_TEST_CASE_P(
        YamlParametersInitNumPagesTests,
        YamlParametersParametrizedInitialization,
        ::testing::Values(
                // Total flash size equals to required for params num
                (InitParameters){flash_memory,   1,  100,    1,  1,  true},
                (InitParameters){flash_memory,  25,    4,   25,  0,  true},
                // Page size equals to required for params num, but n pages = 0
                (InitParameters){flash_memory,   0,  100,    1,  1, false}));

INSTANTIATE_TEST_CASE_P(
        YamlParametersInitPageSizeTests,
        YamlParametersParametrizedInitialization,
        ::testing::Values(
                // Total flash size equals to required to store params
                (InitParameters){flash_memory,   1,  112,    0,  2,  true},
                (InitParameters){flash_memory,   1,  100,   25,  0,  true},
                // Total flash size smaller than required to store params
                (InitParameters){flash_memory,   1,  99,    25,  0,  false},
                (InitParameters){flash_memory,   1,  55,     0,  1,  false},
                // Page size smaller than int params size (4 bytes), total flash size is ok
                (InitParameters){flash_memory,   4,    1,    1,  0,  false},
                (InitParameters){flash_memory,   30,   3,    1,  1,  false}));


class YamlParamsStandardPoolTest : public YamlParamsTestBase {
protected:
    IntegerDesc_t integer_desc_pool[512] = {
        {"uavcan.node.id",        0,      127,     50,        MUTABLE},
        {"uavcan.pub.mag.id",     0,      65535,   65535,     MUTABLE},
        {"uavcan.can.baudrate",   100000, 8000000, 1000000,   IMMUTABLE},
    };

    StringDesc_t string_desc_pool[512] = {
        {"name", "Unknown", MUTABLE},
        {"uavcan.pub.mag.type", "uavcan.si.sample.magnetic_field_strength.Vector3", IMMUTABLE},
    };
    uint8_t flash_memory[124];
    void SetUp() {
        flash = {
            .memory_ptr = flash_memory,
            .page_size = 124,
            .num_pages = 1,
        };
        params = {
            .integer_desc_pool = integer_desc_pool,
            .string_desc_pool = string_desc_pool,
            .num_int_params = 3,
            .num_str_params = 2
        };
        yaml_params = new YamlParameters(flash, params);
    }
};

TEST_F(YamlParamsStandardPoolTest, FileNamesConsistency) {
    ASSERT_EQ(yaml_params->set_temp_file_name(""), LIBPARAMS_WRONG_ARGS);
    ASSERT_EQ(yaml_params->set_init_file_name(""), LIBPARAMS_WRONG_ARGS);

    ASSERT_EQ(yaml_params->set_init_file_name("file1"), LIBPARAMS_OK);
    ASSERT_EQ(yaml_params->set_temp_file_name("file2"), LIBPARAMS_OK);

    delete_file_from_dir("file1_0.yaml");
    delete_file_from_dir("file2_0.yaml");

    // Reading non-existent files
    ASSERT_FALSE(std::filesystem::exists(dir + "/file1_0.yaml"));
    ASSERT_FALSE(std::filesystem::exists(dir + "/file2_0.yaml"));
    ASSERT_EQ(yaml_params->read_from_dir(dir), LIBPARAMS_WRONG_ARGS);

    // Creation of file "/file1_0.yaml"
    ASSERT_EQ(yaml_params->write_to_dir(dir), LIBPARAMS_OK);
    ASSERT_FALSE(std::filesystem::exists(dir + "/file1_0.yaml"));
    ASSERT_TRUE(std::filesystem::exists(dir + "/file2_0.yaml"));

    // Reading file "/file1_0.yaml"
    ASSERT_EQ(yaml_params->read_from_dir(dir), LIBPARAMS_OK);
    ASSERT_EQ(delete_file_from_dir("file2_0.yaml"), 0);
}

TEST_F(YamlParamsStandardPoolTest, ComparePool) {
    // writing temp file with default values of pools entries
    fill_flash_with_default();
    compare_flash_with_pool();
    ASSERT_EQ(yaml_params->write_to_dir(dir), LIBPARAMS_OK);
    ASSERT_TRUE(std::filesystem::exists(dir + "/temp_params_0.yaml"));

    // reading the values from the file into flash
    ASSERT_EQ(yaml_params->set_init_file_name("temp_params"), LIBPARAMS_OK);
    ASSERT_EQ(yaml_params->read_from_dir(dir), LIBPARAMS_OK);

    compare_flash_with_pool();
    delete_file_from_dir("temp_params_0.yaml");
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
