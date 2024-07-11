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

// Test case 4. flashWrite
TEST(TestFlashDriver, test_flash_ok) {
    flashUnlock();
    flashErase(0, 0);
    auto res = flashWriteU64(FLASH_START_ADDR, (uint64_t)42);
    flashLock();

    ASSERT_EQ(res, LIBPARAMS_OK);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
