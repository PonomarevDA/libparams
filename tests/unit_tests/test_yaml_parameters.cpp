/*
 * Copyright (c) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "YamlParameters.hpp"
#include "libparams_error_codes.h"


#define F_NAME_LEN strlen(FLASH_DRIVER_STORAGE_FILE) + 10
#define SIM_F_NAME_LEN strlen(FLASH_DRIVER_SIM_STORAGE_FILE) + 10

extern IntegerDesc_t integer_desc_pool[];
extern StringDesc_t string_desc_pool[];

// Source: https://stackoverflow.com/a/12468109
std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

int8_t delete_file(char* path) {
    try {
        if (std::filesystem::remove(path)) {
        std::cout << "file " << path << " deleted.\n";
        } else {
        std::cout << "file " << path << " not found.\n";
            return -1;
        }
    } catch(const std::filesystem::filesystem_error& err) {
        std::cout << "filesystem error: " << err.what() << '\n';
        return -1;
    }
    return 0;
}

// Test Case 1. Write to file
TEST(TestYamlParameters, write_zero_page_size) {
    uint8_t* flash = 0;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file(std::string("smth"), flash, 1, &idxs, 0);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, write_flash_null_ptr) {
    uint8_t* flash = 0;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file(std::string("smth"), flash, 1, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, write_page_n_zero) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file(std::string("smth"), flash, 0, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, write_empty_path) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file("", flash, 0, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, write_non_existing_path) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    char path[100];
    snprintf(path, 100, "%s/%s", FLASH_DRIVER_STORAGE_DIR, random_string(3).c_str());
    auto res = YamlParameters::write_to_file(path, flash, 1, &idxs, 100);
    delete_file(path);
    ASSERT_EQ(res, LIBPARAMS_OK);
}

// Test Case 2. Read from file
TEST(TestYamlParameters, read_zero_page_size) {
    uint8_t* flash = 0;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file(std::string("smth"), flash, 1, &idxs, 0);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_flash_null_ptr) {
    uint8_t* flash = 0;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::read_from_file(std::string("smth"), flash, 1, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_page_n_zero) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::read_from_file(std::string("smth"), flash, 0, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_empty_path) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::read_from_file("", flash, 0, &idxs, 100);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_non_existing_path) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    char path[100];
    snprintf(path, sizeof(path), "%s/%s", FLASH_DRIVER_STORAGE_DIR, random_string(3).c_str());
    auto res = YamlParameters::read_from_file(path, flash, 1, &idxs, 100);
    delete_file(path);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestYamlParameters, read_not_enought_page_size) {
    uint8_t* flash;
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    char path[100];
    snprintf(path, sizeof(path), "%s/%s", FLASH_DRIVER_STORAGE_DIR, random_string(3).c_str());
    auto res = YamlParameters::read_from_file(path, flash, 1, &idxs, 100);
    delete_file(path);
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

// Test Case 3. Check if reading is right
TEST(TestYamlParameters, read_comparison) {
    std::string path = FLASH_DRIVER_STORAGE_FILE;
    auto last = path.find_last_of('.');
    char file_name[F_NAME_LEN];
    snprintf(file_name, F_NAME_LEN, "%s_%d%s",
                                path.substr(0, last).c_str(), 0, path.substr(last).c_str());
    uint8_t flash[2048];
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::read_from_file(file_name, flash, 1, &idxs, 2048);
    ASSERT_EQ(res, LIBPARAMS_OK);
    int32_t int_val = 0;
    for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx ++) {
        memcpy(&int_val, flash + 4 * idx, 4);
        ASSERT_EQ(int_val, integer_desc_pool[idx].def);
    }
    for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
        auto offset = 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
        std::string str_val(reinterpret_cast<char*>(flash + offset), MAX_STRING_LENGTH);
        auto def = (char*)(string_desc_pool[idx].def);
        ASSERT_STREQ(str_val.c_str(), def);
    }
}


// Test Case 4. Check if writing is right
TEST(TestYamlParameters, write_comparison) {
    uint8_t flash[2048];
    for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx ++) {
        memcpy(flash + 4 * idx, &integer_desc_pool[idx].def, 4);
    }
    for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
        auto offset = 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
        memcpy(flash + offset, &string_desc_pool[idx].def, MAX_STRING_LENGTH);
    }

    std::string path = FLASH_DRIVER_STORAGE_FILE;
    auto last = path.find_last_of('.');
    char file_name[F_NAME_LEN];
    snprintf(file_name, F_NAME_LEN, "%s_%d%s",
                                path.substr(0, last).c_str(), 0, path.substr(last).c_str());
    auto idxs = std::tuple<uint8_t, uint8_t>(0, 0);
    auto res = YamlParameters::write_to_file(file_name, flash, 1, &idxs, 2048);
    ASSERT_EQ(res, LIBPARAMS_OK);
    uint8_t flash_read[2048];
    auto idxs_read = std::tuple<uint8_t, uint8_t>(0, 0);
    res = YamlParameters::read_from_file(file_name, flash_read, 1, &idxs_read, 2048);
    ASSERT_EQ(res, LIBPARAMS_OK);

    int32_t int_val = 0;
    for (uint8_t idx = 0; idx < IntParamsIndexes::INTEGER_PARAMS_AMOUNT; idx ++) {
        memcpy(&int_val, flash_read + 4 * idx, 4);
        ASSERT_EQ(int_val, integer_desc_pool[idx].def);
    }
    for (uint8_t idx = 0; idx < NUM_OF_STR_PARAMS; idx ++) {
        auto offset = 2048 - MAX_STRING_LENGTH * (NUM_OF_STR_PARAMS - idx);
        std::string str_val(reinterpret_cast<char*>(flash_read + offset), MAX_STRING_LENGTH);
        auto def = (char*)(string_desc_pool[idx].def);
        ASSERT_STREQ(str_val.c_str(), def);
    }
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
