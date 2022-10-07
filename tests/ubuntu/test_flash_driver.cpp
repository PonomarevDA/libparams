/**
 * @file test_stm32f1xx.cpp
 * @author d.ponomarev
 * @date Aug 01, 2022
 */

#include <gtest/gtest.h>
#include <iostream>
#include "flash_driver.h"

TEST(TestFlashDriver, test_erase_ok) {
    flashUnlock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, 0);
}

TEST(TestFlashDriver, test_erase_error_locked) {
    flashLock();
    auto res = flashErase(0, 1);
    flashLock();
    ASSERT_EQ(res, -1);
}

TEST(TestFlashDriver, test_erase_error_bad_first_arg) {
    flashUnlock();
    auto res = flashErase(1, 1);
    flashLock();
    ASSERT_EQ(res, -1);
}

TEST(TestFlashDriver, test_erase_error_bad_second_arg) {
    flashUnlock();
    auto res = flashErase(0, 0);
    flashLock();
    ASSERT_EQ(res, -1);
}

TEST(TestFlashDriver, test_flash_ok) {
    flashUnlock();
    flashErase(0, 0);
    auto res = flashWriteU64(FLASH_START_ADDR, (uint64_t)42);
    flashLock();

    ASSERT_EQ(res, 0);
}

int main (int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
