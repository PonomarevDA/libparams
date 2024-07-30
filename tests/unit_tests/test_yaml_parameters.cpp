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
#include <fstream>

#include "YamlParameters.hpp"
#include "common/algorithms.hpp"
#include "flash_driver.h"
#include "libparams_error_codes.h"

// #define NUM_INT_PARAMS          IntParamsIndexes::INTEGER_PARAMS_AMOUNT
// #define NUM_STR_PARAMS          NUM_OF_STR_PARAMS
// #define REQ_FLASH_SIZE          NUM_INT_PARAMS * 4 + NUM_STR_PARAMS * 56
// extern IntegerDesc_t integer_desc_pool[];
// extern StringDesc_t string_desc_pool[];
std::string dir(LIBPARAMS_PARAMS_DIR);

int8_t delete_file_from_dir(const char* path) {
    std::string full_path = dir;
    full_path.append(path);
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


class YamlParamsTest{
public:
    YamlParameters * yaml_params;
    IntegerDesc_t* integer_desc_pool_ptr;
    StringDesc_t* string_desc_pool_ptr;
    void SetUp() {
        FlashMemoryLayout_t flash = {};
        ParametersLayout_t params = {
            .integer_desc_pool = integer_desc_pool_ptr,
            .string_desc_pool = string_desc_pool_ptr
        };
        yaml_params = new YamlParameters(flash, params);
    }
    void TearDown() {
        delete yaml_params;
    }
};

class YamlParamsStandardPoolTest : public ::testing::Test {
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
    YamlParameters * yaml_params;
    FlashMemoryLayout_t flash;
    ParametersLayout_t params;
    void SetUp() {
        flash = {
            .flash_memory = flash_memory,
            .flash_pages_num = 1,
            .flash_size = 124
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
    ASSERT_NE(yaml_params, nullptr);

    ASSERT_EQ(yaml_params->set_temp_file_name(""), LIBPARAMS_WRONG_ARGS);
    ASSERT_EQ(yaml_params->set_init_file_name(""), LIBPARAMS_WRONG_ARGS);

    ASSERT_EQ(yaml_params->set_init_file_name("file1"), LIBPARAMS_OK);
    ASSERT_EQ(yaml_params->set_temp_file_name("file2"), LIBPARAMS_OK);

    delete_file_from_dir("file1");
    delete_file_from_dir("file2");

    ASSERT_FALSE(std::filesystem::exists(dir + "file1"));
    ASSERT_FALSE(std::filesystem::exists(dir + "file2"));
    ASSERT_EQ(yaml_params->read_from_dir(dir), LIBPARAMS_WRONG_ARGS);

    ASSERT_EQ(yaml_params->write_to_dir(dir), LIBPARAMS_OK);
    ASSERT_FALSE(std::filesystem::exists(dir + "file1"));
    ASSERT_TRUE(std::filesystem::exists(dir + "file2"));

    ASSERT_EQ(yaml_params->read_from_dir(dir), LIBPARAMS_OK);
    ASSERT_EQ(delete_file_from_dir("file2"), 0);
}

TEST_F(YamlParamsStandardPoolTest, ComparePool) {
    ASSERT_NE(yaml_params, nullptr);

    ASSERT_EQ(yaml_params->write_to_dir(dir), LIBPARAMS_OK);
    ASSERT_TRUE(std::filesystem::exists(dir + "file2"));
    for (uint8_t idx = 0; idx < params.num_int_params; idx ++) {
        int32_t int_val = 0;
        memcpy(&int_val, flash_memory + 4 * idx, 4);
        ASSERT_EQ(int_val, integer_desc_pool[idx].def);
    }
    for (uint8_t idx = 0; idx < params.num_str_params; idx ++) {
        auto offset = flash.flash_size - MAX_STRING_LENGTH * (params.num_int_params - idx);
        std::string str_val(reinterpret_cast<char*>(flash_memory + offset),
        MAX_STRING_LENGTH);
        auto def = (char*)(string_desc_pool[idx].def);
        ASSERT_STREQ(str_val.c_str(), def);
    }
}

// TEST_F(YamlParamsStandardPoolTest, Base) {
//     res = yaml_params.write_to_dir(dir);
//     yaml_params->write_to_dir();
// }
// // Test Case 1. Initialize YamlParameters
// TEST(TestYamlParameters, yaml_init_okay) {
//     FlashMemoryLayout_t flash = {
//         .flash_memory = {},
//         .flash_size = 100
//     };
//     ParametersLayout_t params = {
//         .integer_desc_pool = 
//     }
//     ParametersLayout_t params = {};
//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1, NUM_STR_PARAMS,
//                                                                         NUM_INT_PARAMS);
// }

// // Test Case 1. Initialize YamlParameters
// TEST(TestYamlParameters, yaml_init_okay_2_pages) {
//     uint8_t flash[100];
//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE/2, 2, NUM_STR_PARAMS,
//                                                                         NUM_INT_PARAMS);
// }

// TEST(TestYamlParameters, flash_null_ptr) {
//     uint8_t* flash = 0;
//     EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0),
//                                                             std::invalid_argument);
// }

// TEST(TestYamlParameters, zero_page_size) {
//     uint8_t flash[100];
//     EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 0, 1, 0, 0),
//                                                             std::invalid_argument);
// }

// TEST(TestYamlParameters, page_n_zero) {
//     uint8_t flash[100];
//     EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, 100, 0, 0, 0),
//                                                             std::invalid_argument);
// }

// TEST(TestYamlParameters, not_enought_page_size) {
//     uint8_t flash[REQ_FLASH_SIZE];
//     EXPECT_THROW(YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE - 100, 1,
//                         NUM_OF_STR_PARAMS, NUM_STR_PARAMS), std::invalid_argument);
// }

// // Case 2. Set empty file names
// TEST(TestYamlParameters, set_file_names_ok) {
//     uint8_t flash[100];
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
//     auto res = yaml_params.set_init_file_name("random");
//     ASSERT_EQ(res, LIBPARAMS_OK);
//     res = yaml_params.set_temp_file_name("random");
//     ASSERT_EQ(res, LIBPARAMS_OK);
// }

// TEST(TestYamlParameters, set_empty_init_file_name) {
//     uint8_t flash[100];
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
//     auto res = yaml_params.set_init_file_name("");
//     ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
// }

// TEST(TestYamlParameters, set_empty_temp_file_name) {
//     uint8_t flash[100];
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 0);
//     auto res = yaml_params.set_temp_file_name("");
//     ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
// }


// // Case 3. Write to file
// TEST(TestYamlParameters, write_ok) {
//     uint8_t flash[REQ_FLASH_SIZE];
//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1,
//                                                                 NUM_STR_PARAMS, NUM_INT_PARAMS);
//     auto res = yaml_params.write_to_dir(dir);
//     delete_file((dir + "/temp_params_0.yaml").c_str());
//     ASSERT_EQ(res, LIBPARAMS_OK);
// }

// TEST(TestYamlParameters, write_empty_path) {
//     uint8_t flash[100];
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 1, 1);
//     auto res = yaml_params.write_to_dir("");
//     ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
// }

// TEST(TestYamlParameters, write_wrong_params_num) {
//     uint8_t flash[200];
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 0, 1);
//     auto res = yaml_params.write_to_dir(dir);

//     delete_file((dir + "/temp_params_0.yaml").c_str());
// }


// // Test Case 4 Read from file
// TEST(TestYamlParameters, read_ok) {
//     uint8_t flash[REQ_FLASH_SIZE];

//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1,
//                                                             NUM_STR_PARAMS, NUM_INT_PARAMS);
//     auto res = yaml_params.read_from_dir(dir);
//     ASSERT_EQ(res, LIBPARAMS_OK);
// }

// TEST(TestYamlParameters, read_no_such_file) {
//     uint8_t flash[100];
//     char file_name[10];
//     generateRandomCString(file_name, 10);
//     YamlParameters yaml_params = YamlParameters(flash, 100, 1, 1, 1);
//     yaml_params.set_init_file_name(file_name);
//     auto res = yaml_params.read_from_dir(dir);
//     ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
// }


// TEST(TestYamlParameters, read_empty_file) {
//     uint8_t flash[REQ_FLASH_SIZE];
//     char file_name[10];
//     generateRandomCString(file_name, 10);
//     std::ofstream params_storage_file;
//     std::string file_path = dir;
//     file_path.append("/").append(file_name) + ".yaml";
//     params_storage_file.open(file_path, std::ios_base::out);
//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1,
//                                                             NUM_STR_PARAMS, NUM_INT_PARAMS);
//     yaml_params.set_init_file_name(file_name);
//     auto res = yaml_params.read_from_dir(dir);
//     delete_file(file_path.c_str());
//     ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
// }


// // Test Case 5. Check if reading is right compare to the generated params
// // description params.cpp file.
// TEST(TestYamlParameters, read_comparison) {
//     uint8_t flash[REQ_FLASH_SIZE];

//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1,
//                                                         NUM_STR_PARAMS, NUM_INT_PARAMS);
//     auto res = yaml_params.read_from_dir(dir);
//     ASSERT_EQ(res, LIBPARAMS_OK);
//     for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx ++) {
//         int32_t int_val = 0;
//         memcpy(&int_val, flash + 4 * idx, 4);
//         ASSERT_EQ(int_val, integer_desc_pool[idx].def);
//     }
//     for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
//         auto offset = REQ_FLASH_SIZE - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
//         std::string str_val(reinterpret_cast<char*>(flash + offset),
//         MAX_STRING_LENGTH);
//         auto def = (char*)(string_desc_pool[idx].def);
//         ASSERT_STREQ(str_val.c_str(), def);
//     }
// }


// // Test Case 6. Check if created file is the same as initial_params yaml file generated
// // by python script (/scripts/generate_random_params.py).
// TEST(TestYamlParameters, write_comparison) {
//     uint8_t flash[REQ_FLASH_SIZE];

//     YamlParameters yaml_params = YamlParameters(flash, REQ_FLASH_SIZE, 1,
//                                                         NUM_STR_PARAMS, NUM_INT_PARAMS);
//     auto res = yaml_params.read_from_dir(dir);
//     ASSERT_EQ(res, LIBPARAMS_OK);
//     yaml_params.set_temp_file_name("written");
//     res = yaml_params.write_to_dir(dir);
//     ASSERT_EQ(res, LIBPARAMS_OK);

//     auto file_name = dir + "/" + "init_params_0.yaml";
//     std::ifstream init_storage_file;
//     init_storage_file.open(file_name, std::ios_base::in);

//     file_name = dir + "/" + "written_0.yaml";
//     std::ifstream written_storage_file;
//     written_storage_file.open(file_name, std::ios_base::in);


//     std::string line;
//     std::string written_line;
//     while (std::getline(init_storage_file, line)) {
//         std::getline(written_storage_file, written_line);
//         ASSERT_STREQ(line.c_str(), written_line.c_str());
//     }
//     init_storage_file.close();
//     written_storage_file.close();
//     delete_file(file_name.c_str());
// }

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
