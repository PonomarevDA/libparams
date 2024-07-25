/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include "YamlParameters.hpp"
#include "common/algorithms.hpp"
#include "flash_driver.h"
#include "libparams_error_codes.h"
#include "params.hpp"
#include "storage.h"

#define F_NAME_LEN strlen(LIBPARAMS_INITIAL_PARAMS_FILE) + 10
#define SIM_F_NAME_LEN strlen(LIBPARAMS_TEMP_PARAMS_FILE) + 10

extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

int8_t delete_file(const char* path) {
    try {
        if (std::filesystem::remove(path)) {
            std::cout << "file " << path << " deleted.\n";
        } else {
            std::cout << "file " << path << " not found.\n";
            return -1;
        }
    } catch (const std::filesystem::filesystem_error& err) {
        std::cout << "filesystem error: " << path << err.what() << '\n';
        return -1;
    }
    return 0;
}

// Test Case 1. Initialize YamlParameters
TEST(TestYamlParameters, yaml_init_okay) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
}

TEST(TestYamlParameters, flash_null_ptr) {
    uint8_t* flash = 0;
    EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0),
                                                            std::invalid_argument);
}

TEST(TestYamlParameters, zero_page_size) {
    uint8_t flash[100];
    EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 0, 1, 0, 0),
                                                            std::invalid_argument);
}

TEST(TestYamlParameters, page_n_zero) {
    uint8_t flash[100];
    EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 100, 0, 0, 0),
                                                            std::invalid_argument);
}

// Case 2. Set empty file names
TEST(TestYamlParameters, set_file_names_ok) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
    auto res = yaml_params.set_init_file_name("smth");
    ASSERT_EQ(res, LIBPARAMS_OK);
    res = yaml_params.set_temp_file_name("smth");
    ASSERT_EQ(res, LIBPARAMS_OK);
}

TEST(TestYamlParameters, set_empty_init_file_name) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
    auto res = yaml_params.set_init_file_name("");
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, set_empty_temp_file_name) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
    auto res = yaml_params.set_temp_file_name("");
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

// Case 3. Write to file
TEST(TestYamlParameters, write_empty_path) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 1, 1);
    auto res = yaml_params.write_to_files("");
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, write_wrong_params_num) {
    uint8_t flash[200];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 1);
    char file_name[9];
    std::string dir = LIBPARAMS_INITIAL_PARAMS_DIR;
    generateRandomCString(file_name, 9);
    yaml_params.set_temp_file_name(std::string(file_name));
    auto res = yaml_params.write_to_files(dir);

    // EXPECT_THROW(yaml_params.write_to_files(LIBPARAMS_INITIAL_PARAMS_DIR),
    //              std::invalid_argument);
    auto path = dir;
    delete_file((dir.append("/").append(file_name).append("_0.yml")).c_str());
}

TEST(TestYamlParameters, write_ok) {
    uint8_t flash[100];
    auto num_int_params = IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
    auto num_str_params = NUM_OF_STR_PARAMS;
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, num_str_params, num_int_params);
    char file_name[9];
    std::string dir = LIBPARAMS_INITIAL_PARAMS_DIR;
    generateRandomCString(file_name, 9);
    yaml_params.set_temp_file_name(std::string(file_name));
    auto res = yaml_params.write_to_files(dir);
    auto path = dir;
    delete_file((dir.append("/").append(file_name).append("_0.yml")).c_str());
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

// Test Case 4 Read from file
TEST(TestYamlParameters, read_no_such_file) {
    uint8_t flash[100];
    YamlParameters yaml_params = YamlParameters(flash, 100, 1, 1, 1);
    yaml_params.set_init_file_name("smth");
    auto res = yaml_params.read_from_dir(LIBPARAMS_INITIAL_PARAMS_DIR);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_not_enought_page_size) {
    auto req_flash_size = IntParamsIndexes::INTEGER_PARAMS_AMOUNT * 4 + NUM_OF_STR_PARAMS * 56;
    auto flash_size = std::max(1, req_flash_size - 100);
    uint8_t flash[flash_size];
    auto num_int_params = IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
    auto num_str_params = NUM_OF_STR_PARAMS;
    YamlParameters yaml_params = YamlParameters(flash, flash_size, 1,
                                                                num_int_params, num_str_params);
    EXPECT_THROW(yaml_params.read_from_dir(LIBPARAMS_INITIAL_PARAMS_DIR),
                 std::invalid_argument);
}

// Test Case 5. Check if reading is right with default_params file
TEST(TestYamlParameters, read_comparison) {
    auto num_int_params = IntParamsIndexes::INTEGER_PARAMS_AMOUNT;
    auto num_str_params = NUM_OF_STR_PARAMS;

    auto req_flash_size = num_int_params * 4 + (2 + num_str_params) * 56;
    uint8_t flash[req_flash_size];

    YamlParameters yaml_params = YamlParameters(flash, req_flash_size, 1,
                                                        num_str_params, num_int_params);
    auto res = yaml_params.read_from_dir(LIBPARAMS_INITIAL_PARAMS_DIR);
    ASSERT_EQ(res, LIBPARAMS_OK);
    for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx ++) {
        int32_t int_val = 0;
        memcpy(&int_val, flash + 4 * idx, 4);
        ASSERT_EQ(int_val, integer_desc_pool[idx].def);
    }
    for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
        auto offset = req_flash_size - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
        std::string str_val(reinterpret_cast<char*>(flash + offset),
        MAX_STRING_LENGTH);
        // size_t quote_pos = str_val.find('"');
        // size_t quote_end_pos = str_val.find('"', quote_pos + 1);
        // std::string str_value = str_val.substr(quote_pos + 1, quote_end_pos - quote_pos - 1);
        auto def = (char*)(string_desc_pool[idx].def);
        ASSERT_STREQ(str_val.c_str(), def);
    }
}

// // Test Case 4. Check if writing is right
// TEST(TestYamlParameters, write_comparison) {
//     uint8_t flash[2048];
//     for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx
//     ++) {
//         memcpy(flash + 4 * idx, &integer_desc_pool[idx].def, 4);
//     }
//     for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
//         auto offset = 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
//         memcpy(flash + offset, &string_desc_pool[idx].def,
//         MAX_STRING_LENGTH);
//     }

//     std::string path = LIBPARAMS_INITIAL_PARAMS_FILE;
//     auto last = path.find_last_of('.');
//     char file_name[F_NAME_LEN];
//     snprintf(file_name, F_NAME_LEN, "%s_%d%s",
//                                 path.substr(0, last).c_str(), 0,
//                                 path.substr(last).c_str());
//     auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
//     auto res = YamlParameters::write_to_files(file_name, flash, 1, &idxs,
//     2048); ASSERT_EQ(res, LIBPARAMS_OK); uint8_t flash_read[2048]; auto
//     idxs_read = std::tuple<uint8_t, uint8_t>(0, 0); res =
//     YamlParameters::read_from_dir(file_name, flash_read, 1, &idxs_read,
//     2048); ASSERT_EQ(res, LIBPARAMS_OK);

//     int32_t int_val = 0;
//     for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx
//     ++) {
//         memcpy(&int_val, flash_read + 4 * idx, 4);
//         ASSERT_EQ(int_val, integer_desc_pool[idx].def);
//     }
//     for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
//         auto offset = 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
//         std::string str_val(reinterpret_cast<char*>(flash_read + offset),
//         MAX_STRING_LENGTH); auto def = (char*)(string_desc_pool[idx].def);
//         ASSERT_STREQ(str_val.c_str(), def);
//     }
// }

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
