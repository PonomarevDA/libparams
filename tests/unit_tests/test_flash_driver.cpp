/*
 * Copyright (c) 2022-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <gtest/gtest.h>
#include <iostream>
#include "flash_driver.h"
#include "libparams_error_codes.h"
#include "common/algorithms.hpp"

// Test Case 1. Initialize flash driver
TEST(TestFlashDriver, initializeFlashDriver) {
    flashUnlock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
}


// Test case 2. Erase
TEST(TestFlashDriver, test_erase_ok) {
    flashUnlock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
}
TEST(TestFlashDriver, test_erase_error_locked) {
    flashLock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_TRUE(res < 0);
}

TEST(TestFlashDriver, test_erase_error_bad_first_arg) {
    flashUnlock();
    auto res = flashErase(1, 1);
    flashLock();
    ASSERT_TRUE(res < 0);
}

TEST(TestFlashDriver, test_erase_error_bad_second_arg) {
    flashUnlock();
    auto res = flashErase(0, 0);
    flashLock();
    ASSERT_TRUE(res < 0);
}

// Test case 3. flashRead
TEST(TestFlashDriver, test_read_flash_ok) {
    flashUnlock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
    uint8_t val;
    res = flashRead(&val, 0, 1);
    ASSERT_EQ(res, 1);
    ASSERT_EQ(0, val);
}

// Test case 4. flashWriteU64
TEST(TestFlashDriver, test_write64_flash_ok) {
    flashUnlock();
    auto res = flashWriteU64(FLASH_START_ADDR, (uint64_t)42);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
}


// Test case 5. flashWrite
TEST(TestFlashDriver, test_write_flash_wrong_addr) {
    flashUnlock();
    auto res = flashWrite((uint8_t*)42, 0, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_WRONG_ARGS);
}

TEST(TestFlashDriver, test_write_flash_ok) {
    flashUnlock();
    uint8_t val = 42;
    auto res = flashWrite(&val, FLASH_START_ADDR, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
}

// Test case 6. Check values`
TEST(TestFlashDriver, test_flash_check_numeric_ok) {
    flashUnlock();
    uint8_t val = 42;
    auto res = flashWrite(&val, FLASH_START_ADDR, 1);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
    uint8_t read_val = 0;
    res = flashRead(&read_val, 0, 1);
    ASSERT_EQ(res, 1);
    ASSERT_EQ(val, read_val);
}

TEST(TestFlashDriver, test_flash_check_string_ok) {
    flashUnlock();
    char val[56];
    generateRandomCString(val, 56);
    auto res = flashWrite((uint8_t*)val, FLASH_START_ADDR, 56);
    flashLock();
    ASSERT_EQ(res, LIBPARAMS_OK);
    uint8_t read_val[56];
    res = flashRead(read_val, 0, 56);
    ASSERT_EQ(res, 56);
    ASSERT_STREQ(val, (char*)read_val);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
